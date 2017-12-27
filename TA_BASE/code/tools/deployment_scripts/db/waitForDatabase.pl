#!/usr/local/bin/perl

my $success="false";
my $count = 0;
my $command = shift(@ARGV);


if($command eq "start")
{
	while($success ne "true")
	{
		open( SQLPLUS, "/u01/app/oracle/OraHome920/bin/sqlplus -L its/its\@tra_stat <<quit|" );

		my @sqlplus = <SQLPLUS>;
		foreach $line (@sqlplus)
		{
			if($line =~ /.*Connected to:.*/)
			{
				$success="true";
			}
		}
		print "waiting $count seconds for database to start up\n";
		$count++;
		sleep(1);
	
	}
	print "Database initialised\n";
}
else
{
	print "Database Stopped\n";
}




