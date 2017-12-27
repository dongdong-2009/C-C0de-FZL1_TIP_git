#pragma once
#include <string>


class DemoKeygen
{
public:

    DemoKeygen( const std::string& serial_number = "" );
    ~DemoKeygen();

    std::string generateKey();
    std::string getPhysicalDriveSerialNumber( int drive_number = 0 );
    std::string createMachineCode( const std::string& serial_number );
    std::string md5( const std::string& str );
    std::string getKeyFromFile( const std::string& file_name = "c:\\transactive\\bin\\demo_key" );

private :

    std::string m_serial_number;
};
