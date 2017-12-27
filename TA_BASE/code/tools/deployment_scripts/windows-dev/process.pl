#!perl


MAIN:
{
    @lines = ();         # Array that will store the lines

    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_CONSOLE_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_SYSTEM_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_SERVER_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_ALARM_STORE_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_SN_SYSTEM_CONTROLLER_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_SN_MANAGED_PROCESS_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file

    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_ALARM_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_AUTHENTICATION_AGENT_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_DUTY_AGENT_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_NOTIFICATIONSERVICE_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_RIGHTS_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_SCHEDULING_AGENT_ENTITY_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file
    open( FILE, "c:\\transactive\\agents\\scripts\\ITS_SYSTEM_STATUS_AGENT_DATA.SQL" ); # Open the file
    @lines = (@lines, <FILE>);     # Read it into an array
    close( FILE );       # Close the file

    $alllines = join '', @lines;
    $alllines =~ s/HOSTNAME/@ARGV[0]/g;

    $alllines.= "\n/\nquit;";
    
    open( FILE, ">c:\\transactive\\agents\\scripts\\PROCESSED.SQL" );
    print FILE "$alllines";
    close( FILE );    
}

