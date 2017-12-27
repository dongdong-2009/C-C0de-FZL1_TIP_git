#pragma once


class DemoKeygen
{
public:

    DemoKeygen( const std::string& serial_number = "" )
        : m_serial_number( serial_number)
    {
    }

    std::string generate_key();
    std::string get_physical_drive_serial_number( int drive_number = 0 );
    std::string create_machine_code( const std::string& serial_number );
    std::string md5( const std::string& str );
    std::string get_key_from_file( const std::string& file_name = "c:\\transactive\\bin\\demo_key" );
    void save_key_to_file( const std::string& key, const std::string& file_name = "c:\\transactive\\bin\\demo_key" );

private :

    std::string m_serial_number;
};
