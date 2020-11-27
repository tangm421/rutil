#include "rutil/Data.hxx"
#include "rutil/Subsystem.hxx"

class NetworkSubsystem : public resip::Subsystem
{
public:
    static NetworkSubsystem NETWORK;
private:
    explicit NetworkSubsystem(const char* rhs);;
    explicit NetworkSubsystem(const resip::Data& rhs);
    NetworkSubsystem& operator=(const resip::Data& rhs);
};