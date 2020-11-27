#include "Tuple.h"
#include "NetworkSubsystem.h"

#include "rutil/DnsUtil.hxx"
#include "rutil/ResipAssert.h"
#include "rutil/Logger.hxx"
using namespace resip;

#define RESIPROCATE_SUBSYSTEM NetworkSubsystem::NETWORK

Tuple::Tuple(const resip::Data& printableAddr, int port, resip::IpVersion ipVer, resip::TransportType type /*= UNKNOWN_TRANSPORT*/) : mTransportType(type)
{
    if (ipVer == resip::V4)
    {
        memset(&m_anonv4, 0, sizeof(m_anonv4));
        m_anonv4.sin_family = AF_INET;
        m_anonv4.sin_port = htons(port);

        if (printableAddr.empty())
        {
            m_anonv4.sin_addr.s_addr = htonl(INADDR_ANY);
        }
        else
        {
            DnsUtil::inet_pton(printableAddr, m_anonv4.sin_addr);
        }
    }
    else
    {
#ifdef USE_IPV6
        memset(&m_anonv6, 0, sizeof(m_anonv6));
        m_anonv6.sin6_family = AF_INET6;
        m_anonv6.sin6_port = htons(port);
        if (printableAddr.empty())
        {
            m_anonv6.sin6_addr = in6addr_any;
        }
        else
        {
            DnsUtil::inet_pton(printableAddr, m_anonv6.sin6_addr);
        }
#else
        // avoid asserts since Tuples created via printableAddr can be created from items 
        // received from the wire or from configuration settings.  Just create an IPV4 inaddr_any tuple.
        //assert(0);  
        memset(&m_anonv4, 0, sizeof(m_anonv4));
        m_anonv4.sin_family = AF_INET;
        m_anonv4.sin_port = htons(port);
        m_anonv4.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
    }
}

socklen_t Tuple::length() const // of sockaddr
{
    if (mSockaddr.sa_family == AF_INET) // v4
    {
        return sizeof(sockaddr_in);
    }
#ifdef USE_IPV6
    else  if (mSockaddr.sa_family == AF_INET6) // v6
    {
        return sizeof(sockaddr_in6);
    }
#endif

    resip_assert(0);
    return 0;
}

void Tuple::setPort(int port)
{
    if (mSockaddr.sa_family == AF_INET) // v4   
    {
        m_anonv4.sin_port = htons(port);
    }
    else
    {
#ifdef USE_IPV6
        m_anonv6.sin6_port = htons(port);
#else
        resip_assert(0);
#endif
    }
}

int Tuple::getPort() const
{
    if (mSockaddr.sa_family == AF_INET) // v4   
    {
        return ntohs(m_anonv4.sin_port);
    }
    else
    {
#ifdef USE_IPV6
        return ntohs(m_anonv6.sin6_port);
#else
        resip_assert(0);
#endif
    }

    return -1;
}

resip::Data Tuple::inet_ntop(const Tuple& tuple)
{
#ifdef USE_IPV6
    if (!tuple.isV4())
    {
        const sockaddr_in6& addr = reinterpret_cast<const sockaddr_in6&>(tuple.getSockaddr());
        return DnsUtil::inet_ntop(addr.sin6_addr);
    }
    else
#endif
    {
        const sockaddr_in& addr = reinterpret_cast<const sockaddr_in&>(tuple.getSockaddr());
        return DnsUtil::inet_ntop(addr.sin_addr);
    }
}

resip::Data Tuple::presentationFormat() const
{
#ifdef USE_IPV6
    if (isV4())
    {
        return Tuple::inet_ntop(*this);
    }
    else if (IN6_IS_ADDR_V4MAPPED(&m_anonv6.sin6_addr))
    {
        return DnsUtil::inet_ntop(*(reinterpret_cast<const in_addr*>(
            (reinterpret_cast<const unsigned char*>(&m_anonv6.sin6_addr) + 12))));
    }
    else
    {
        return Tuple::inet_ntop(*this);
    }
#else
    return Tuple::inet_ntop(*this);
#endif
}

bool Tuple::isV4() const
{
    return mSockaddr.sa_family == AF_INET;
}

resip::IpVersion Tuple::ipVersion() const
{
    return mSockaddr.sa_family == AF_INET ? resip::V4 : resip::V6;
}

const resip::Data& Tuple::toData(resip::TransportType type)
{
    return resip::toData(type);  // TransportTypes.hxx
}

EncodeStream& operator<<(EncodeStream& ostrm, const Tuple& tuple)
{
    ostrm << "[ ";

#ifdef USE_IPV6
    if (tuple.mSockaddr.sa_family == AF_INET6)
    {
        ostrm << "V6 " << DnsUtil::inet_ntop(tuple.m_anonv6.sin6_addr) << " port=" << tuple.getPort();
    }
    else
#endif
        if (tuple.mSockaddr.sa_family == AF_INET)
        {
            ostrm << "V4 " << Tuple::inet_ntop(tuple) << ":" << tuple.getPort();
        }
        else
        {
            resip_assert(0);
        }

    ostrm << " " << Tuple::toData(tuple.mTransportType);

    ostrm << " ]";

    return ostrm;
}
