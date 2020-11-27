#include "Tuple.h"
#include "rutil/Data.hxx"
#include "rutil/Socket.hxx"
#include "rutil/TransportType.hxx"

class TransportSocket
{
public:
    TransportSocket(const resip::Data& printableAddr, int port, resip::TransportType type, resip::IpVersion ipVer);
    ~TransportSocket();
    operator bool() { return mValid; }
    int oriport() const { return mPortOri; }
    int port() const { return mTuple.getPort(); }
    bool bind();
    static resip::Socket socket(resip::TransportType type, resip::IpVersion ipVer);
    static void error(int e);
protected:
    bool            mValid;
    Tuple           mTuple;
    resip::Data     mAddress;
    int             mPortOri;
    resip::Socket   mSock;
};