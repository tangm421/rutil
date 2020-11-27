#include "TransportSocket.h"
#include "NetworkSubsystem.h"

#include "rutil/Logger.hxx"
using namespace resip;

#define RESIPROCATE_SUBSYSTEM NetworkSubsystem::NETWORK

TransportSocket::TransportSocket(const resip::Data& printableAddr, int port, resip::TransportType type, resip::IpVersion ipVer)
    : mValid(false)
    , mTuple(printableAddr, port, ipVer, type)
    , mAddress(printableAddr)
    , mPortOri(port)
    , mSock(TransportSocket::socket(type, ipVer))
{
    if (mSock != INVALID_SOCKET)
    {
        mValid = true;
    }
}

TransportSocket::~TransportSocket()
{
    if (mSock != INVALID_SOCKET)
    {
        //DebugLog (<< "Closing " << mFd);
        closeSocket(mSock);
    }
}

bool TransportSocket::bind()
{
    bool ok = false;
    DebugLog(<< "Binding to " << mAddress << ":" << mPortOri);

    if (::bind(mSock, &mTuple.getMutableSockaddr(), mTuple.length()) == SOCKET_ERROR)
    {
        int e = getErrno();
        if (e == EADDRINUSE)
        {
            error(e);
            ErrLog(<< mTuple << " already in use ");
            return ok;
        }
        else
        {
            error(e);
            ErrLog(<< "Could not bind to " << mTuple);
            return ok;
        }
    }

    // If we bound to port 0, then query OS for assigned port number
    if (mTuple.getPort() == 0)
    {
        socklen_t len = mTuple.length();
        if (::getsockname(mSock, &mTuple.getMutableSockaddr(), &len) == SOCKET_ERROR)
        {
            int e = getErrno();
            ErrLog(<< "getsockname failed, error=" << e);
            return ok;
        }
    }

    ok = makeSocketNonBlocking(mSock);
    if (!ok)
    {
        ErrLog(<< "Could not make socket non-blocking " << mTuple.getPort());
        return ok;
    }

    return ok;
}

resip::Socket TransportSocket::socket(resip::TransportType type, resip::IpVersion ipVer)
{
    Socket fd = INVALID_SOCKET;
    switch (type)
    {
    case UDP:
#ifdef USE_IPV6
        fd = ::socket(ipVer == V4 ? PF_INET : PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
#else
        fd = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif
        break;
    case TCP:
    case TLS:
#ifdef USE_IPV6
        fd = ::socket(ipVer == V4 ? PF_INET : PF_INET6, SOCK_STREAM, 0);
#else
        fd = ::socket(PF_INET, SOCK_STREAM, 0);
#endif
        break;
    default:
        InfoLog(<< "Try to create an unsupported socket type: " << Tuple::toData(type));
    }

    if (fd == INVALID_SOCKET)
    {
        int e = getErrno();
        ErrLog(<< "Failed to create socket: " << strerror(e));
    }

#ifdef USE_IPV6
#ifdef __linux__
    int on = 1;
    if (ipVer == V6)
    {
        if (::setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)))
        {
            int e = getErrno();
            InfoLog(<< "Couldn't set sockoptions IPV6_V6ONLY: " << strerror(e));
            error(e);
        }
    }
#endif
#endif

    DebugLog(<< "Creating fd=" << fd << (ipVer == V4 ? " V4/" : " V6/") << (type == UDP ? "UDP" : "TCP"));

    return fd;
}

void TransportSocket::error(int e)
{
    switch (e)
    {
    case EAGAIN:
        //InfoLog (<< "No data ready to read" << strerror(e));
        break;
    case EINTR:
        InfoLog(<< "The call was interrupted by a signal before any data was read : " << strerror(e));
        break;
    case EIO:
        InfoLog(<< "I/O error : " << strerror(e));
        break;
    case EBADF:
        InfoLog(<< "fd is not a valid file descriptor or is not open for reading : " << strerror(e));
        break;
    case EINVAL:
        InfoLog(<< "fd is attached to an object which is unsuitable for reading : " << strerror(e));
        break;
    case EFAULT:
        InfoLog(<< "buf is outside your accessible address space : " << strerror(e));
        break;

#if defined(WIN32)
    case WSAENETDOWN:
        InfoLog(<< " The network subsystem has failed.  ");
        break;
    case WSAEFAULT:
        InfoLog(<< " The buf or from parameters are not part of the user address space, "
            "or the fromlen parameter is too small to accommodate the peer address.  ");
        break;
    case WSAEINTR:
        InfoLog(<< " The (blocking) call was canceled through WSACancelBlockingCall.  ");
        break;
    case WSAEINPROGRESS:
        InfoLog(<< " A blocking Windows Sockets 1.1 call is in progress, or the "
            "service provider is still processing a callback function.  ");
        break;
    case WSAEINVAL:
        InfoLog(<< " The socket has not been bound with bind, or an unknown flag was specified, "
            "or MSG_OOB was specified for a socket with SO_OOBINLINE enabled, "
            "or (for byte stream-style sockets only) len was zero or negative.  ");
        break;
    case WSAEISCONN:
        InfoLog(<< "The socket is connected. This function is not permitted with a connected socket, "
            "whether the socket is connection-oriented or connectionless.  ");
        break;
    case WSAENETRESET:
        InfoLog(<< " The connection has been broken due to the keep-alive activity "
            "detecting a failure while the operation was in progress.  ");
        break;
    case WSAENOTSOCK:
        InfoLog(<< "The descriptor is not a socket.  ");
        break;
    case WSAEOPNOTSUPP:
        InfoLog(<< " MSG_OOB was specified, but the socket is not stream-style such as type "
            "SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, "
            "or the socket is unidirectional and supports only send operations.  ");
        break;
    case WSAESHUTDOWN:
        InfoLog(<< "The socket has been shut down; it is not possible to recvfrom on a socket after "
            "shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.  ");
        break;
    case WSAEMSGSIZE:
        InfoLog(<< " The message was too large to fit into the specified buffer and was truncated.  ");
        break;
    case WSAETIMEDOUT:
        InfoLog(<< " The connection has been dropped, because of a network failure or because the "
            "system on the other end went down without notice.  ");
        break;
    case WSAECONNRESET:
        InfoLog(<< "Connection reset ");
        break;

    case WSAEWOULDBLOCK:
        DebugLog(<< "Would Block ");
        break;

    case WSAEHOSTUNREACH:
        InfoLog(<< "A socket operation was attempted to an unreachable host ");
        break;
    case WSANOTINITIALISED:
        InfoLog(<< "Either the application has not called WSAStartup or WSAStartup failed. "
            "The application may be accessing a socket that the current active task does not own (that is, trying to share a socket between tasks),"
            "or WSACleanup has been called too many times.  ");
        break;
    case WSAEACCES:
        InfoLog(<< "An attempt was made to access a socket in a way forbidden by its access permissions ");
        break;
    case WSAENOBUFS:
        InfoLog(<< "An operation on a socket could not be performed because the system lacked sufficient "
            "buffer space or because a queue was full");
        break;
    case WSAENOTCONN:
        InfoLog(<< "A request to send or receive data was disallowed because the socket is not connected "
            "and (when sending on a datagram socket using sendto) no address was supplied");
        break;
    case WSAECONNABORTED:
        InfoLog(<< "An established connection was aborted by the software in your host computer, possibly "
            "due to a data transmission time-out or protocol error");
        break;
    case WSAEADDRNOTAVAIL:
        InfoLog(<< "The requested address is not valid in its context. This normally results from an attempt to "
            "bind to an address that is not valid for the local computer");
        break;
    case WSAEAFNOSUPPORT:
        InfoLog(<< "An address incompatible with the requested protocol was used");
        break;
    case WSAEDESTADDRREQ:
        InfoLog(<< "A required address was omitted from an operation on a socket");
        break;
    case WSAENETUNREACH:
        InfoLog(<< "A socket operation was attempted to an unreachable network");
        break;

#endif

    default:
        InfoLog(<< "Some other error (" << e << "): " << strerror(e));
        break;
    }
}

