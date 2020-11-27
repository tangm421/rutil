
#include "header.h"
#include "TransportSocket.h"

#include "rutil/Time.hxx"
#include "rutil/MD5Stream.hxx"
using namespace resip;

#include <memory>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;


// void Base::process() {
// 	std::auto_ptr<Controler> ptr(new Controler(*this));
// 	std::cout << ptr.get() << std::endl;
// }

void testSocket(int argc, char* argv[])
{
    // Initialize network
    resip::initNetwork();

    TransportSocket s("", 0, resip::TransportType::UDP, resip::IpVersion::V4);
    if (s)
    {
        s.bind();
        cout << "bind origin port:" << s.oriport() << ", bind port:" << s.port() << endl;
    }
}


const UInt64 bytes4K = 4096;
const UInt64 bytes8K = 2 * bytes4K;
void FileMD5(int argc, char* argv[])
{
    string filepath;
    char* szfile = NULL;
    if (argc > 1)
    {
        szfile = argv[1];
    }
    else
    {
        cout << "file path: ";
        std::getline(cin, filepath);
        szfile = &filepath.front();
    }

    ifstream file(szfile, ios_base::binary|ios_base::in);
    if(!file.is_open())
    {
        cout << "can't open file:" << szfile << endl;
        return;
    }

    file.seekg(0, ios_base::end);
    UInt64 len = file.tellg();
    cout << "file length:" << len << endl;
    if (len < bytes8K)
    {
        //cout << "a video file less then 8k? impossible!" << endl;
        char szBuf[bytes8K];
        file.seekg(0, ios_base::beg);
        file.read(szBuf, bytes4K);
        UInt64 readlen = file.gcount();
        szBuf[readlen] = 0;

        MD5Stream md5;
        md5 << szBuf;
        md5.flush();
        cout << "whole file hash:" << md5.getHex() << endl;
    }
    else
    {
        int err = 0;
        UInt64 offset[4] = { bytes4K, len / 3, len / 3 * 2, len - bytes8K };
        Data result[4];

        UInt64 timecost = ResipClock::getTimeMicroSec();
        for (int i = 0; i < sizeof(offset) / sizeof(offset[0]); ++i)
        {
            char szBuf[bytes4K + 1];
            file.seekg(offset[i]);
            file.read(szBuf, bytes4K);
            UInt64 readlen = file.gcount();
            szBuf[readlen] = 0;

            MD5Stream md5;
            md5 << szBuf;
            md5.flush();
            result[i] = md5.getHex();
        }
        timecost = ResipClock::getTimeMicroSec() - timecost;
        for (int i = 0; i < sizeof(offset) / sizeof(offset[0]); ++i)
        {
            cout << "position at:" << setiosflags(ios::left) << setw(20) << offset[i] << " bytes calc hash:" << result[i] << endl;
        }
        cout << "timecost: " << timecost / 1000000LL << " s " << timecost / 1000LL << " ms " << timecost % 1000 << " us" << endl;
    }

    file.close();
}

int main(int argc, char* argv[])
{
	//std::auto_ptr<Base> ptr(new Derive);
	//ptr->process();

    //testSocket();

    FileMD5(argc, argv);

	return 0;
}