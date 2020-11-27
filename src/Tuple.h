
#include "rutil/Data.hxx"
#include "rutil/TransportType.hxx"
#include "rutil/resipfaststreams.hxx"

#include "rutil/Socket.hxx"
#include "rutil/compat.hxx"

#if defined(WIN32)
#include <Ws2tcpip.h>
#else
#include <netinet/in.h>
#endif

class Tuple
{
public:
    Tuple(const resip::Data& printableAddr, int port, resip::IpVersion ipVer, resip::TransportType type = resip::UNKNOWN_TRANSPORT);
    /// @brief Retrieve a const binary representation of the socket address
      /// for this tuple.
    const sockaddr& getSockaddr() const { return mSockaddr; }
    sockaddr& getMutableSockaddr() { return mSockaddr; }
    socklen_t length() const; // of sockaddr
    resip::TransportType getType() const { return mTransportType; }
    void setPort(int port);
    int getPort() const;
    ///  @brief Converts the binary socket address to presentation format,
      /// via the DnsUtil::inet_ntop() method.
    static resip::Data inet_ntop(const Tuple& tuple);
    /// Wrapper around the inet_top() method.
    resip::Data presentationFormat() const;

    /// @deprecated use ipVersion()
      /// @todo !dcm! -- should deprecate asap
    bool isV4() const;
    resip::IpVersion ipVersion() const;
    ///  @brief Converts the TransportType to a string representation of the
      /// transport type, e.g. "TCP"
    static const resip::Data& toData(resip::TransportType type);

    friend EncodeStream& operator<<(EncodeStream& ostrm, const Tuple& tuple);
private:
    union
    {
        sockaddr mSockaddr;
        sockaddr_in m_anonv4;
#ifdef IPPROTO_IPV6
        // enable this if the current platform supports IPV6
        // ?bwc? Is there a more standard preprocessor macro for this?
        sockaddr_in6 m_anonv6;
#endif
        char pad[RESIP_MAX_SOCKADDR_SIZE]; //< this make union same size if v6 is in or out
    };
    resip::TransportType mTransportType;
};
