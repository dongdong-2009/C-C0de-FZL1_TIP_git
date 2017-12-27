#!perl

# This should converted into C++ code so that it can be
# automatically entered into the database.  It can also
# remove the assumptions on message contexts, message
# types and locations since these can be programmatically
# generated as needs require.

my $MAX_CONSUMERS = 100;
my $MAX_SUPPLIERS = 20;
my @msgContexts = ( "TestCommsContext1", "TestCommsContext2" );
my @locations = ( "5", "8" );


open OUTPUT, ">data.sql";

# print the consumer data.
my $consumerID = 0;
my $entityKey = 0;
do
{
    foreach $location (@locations)
    {
        foreach $context (@msgContexts)
        {
            print OUTPUT "insert into NST_CONSUMERCONFIG ";
            print OUTPUT "( consumerID, entityKey, locationKey, ";
            print OUTPUT "maxMessages, msgContext, msgType )";
            print OUTPUT " values ";
            print OUTPUT "( $consumerID, $entityKey, $location, 0, '$context'";
            if( $context =~ "TestCommsContext1" )
            {
                print OUTPUT ", 999997);\n";
            }
            elsif( $context =~ "TestCommsContext2" )
            {
                print OUTPUT ", 999998);\n";
            }
            else
            {
                print OUTPUT ", 999997);\n";
            }
            $consumerID = $consumerID + 1;
        }
    }
    $entityKey = $entityKey + 1;
}
while( $MAX_CONSUMERS > $consumerID );

# print the supplier data.
my $supplierID = 0;
do
{
    foreach $location (@locations)
    {
        foreach $context (@msgContexts)
        {
            print OUTPUT "insert into NST_SUPPLIERCONFIG ";
            print OUTPUT "( supplierID, startEntityKey, entityKeyCount, ";
            print OUTPUT "targetLocationKey, runTime, msgContext, msgType )";
            print OUTPUT " values ( $supplierID, 0, $entityKey, $location, 0, '$context'";
            if( $context =~ "TestCommsContext1" )
            {
                print OUTPUT ", 999997);\n";
            }
            elsif( $context =~ "TestCommsContext2" )
            {
                print OUTPUT ", 999998);\n";
            }
            else
            {
                print OUTPUT ", 999997);\n";
            }
            $supplierID = $supplierID + 1;
        }
    }
}
while( $MAX_SUPPLIERS > $supplierID );    

close OUTPUT;
