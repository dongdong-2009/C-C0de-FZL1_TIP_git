#
# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  adamr
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
# This script checks the transactive paths app core and bus 
# to find makefiles and creates the configuration file for solaris build
#

my $USAGE = "usage: perl createSolarisConfig.pl";
$USAGE ."must be run where 'code' is in the local directory";

#default header for solaris build configuration files
my $CONFIGHEADER = "# Things we need to know about this project
ProjectInformation(
	# Base directory should be 'code' 
	# relative from here, or absolute both work, although absolute must be reconfigured more often
	BaseDirectory	=> \"code\",
);

# Compilation environments
BuildEnvironment (
    Name            => 'unixsh',
    OperatingSystem => 'solaris',
    Makefile        => 'Makefile',
    Processor       => 'sparc4u',
    MakeProg        => 'gmake 2>&1',
#    CleanCommand    => 'gmake clean 2>&1',
    DebugCommand    => 'gmake DEBUG=1 2>&1',
    InstallCommand  => 'gmake install 2>&1',
    ErrorHandling   => 'Output:
       \$Warnings   = grep( /.* warning: .*/, split( /\\n/, \$Output ) );
       \$Errors     = grep( /(.*:\d+: [abcdefghijklmnopqrstuvxyz].*)|(.* \\*\\*\\* .*)/, split( /\\n/, \$Output ) ); ',
);

# Actual Components to make";

my $CORE="
# ===================================  Core ==================================

";
my $BUS="
# ===================================  Bus ===================================

";
my $APP="
# ===================================  App ===================================

";

#Global variables needed by most sub routines
my $environment=$^O;
my $directory="";
my $count=0;
my $find = "";
my $cwd=cwd();
my $COMPONENTA="Make (
    Component       => \"";
my $COMPONENTB="\",
    SourceDir       => \"";
my $COMPONENTC="\",
    BuildOn         => \"unixsh\",
);";


#command line argument vars
use vars qw($opt_s $opt_f);
#use File::NCopy qw(copy);

#Modules required
use Getopt::Std;
use File::Find;
use File::Path;
use Cwd;

# globals
use vars qw(@makeList);

#############################################################################
#                                                                           #
#                             Main Program                                  #
#                                                                           #
#############################################################################

# make sure we can write the config file
if ( !(-d "BuildFiles") )
{
	mkdir "BuildFiles";
}

open( CONFIGFILE, ">BuildFiles/solarisNightly.txt" ) or die "Can't open BuildFiles/solarisNightly.txt\n";


print CONFIGFILE $CONFIGHEADER;

print CONFIGFILE $CORE;

my $runningdir = cwd();
chdir "code";

# find all makefiles in core
recursivelyFindMakefiles("core");

# add the targets to the file
my $makefile = shift(@makeList);
while(defined $makefile)
{
	print CONFIGFILE $COMPONENTA.$makefile.$COMPONENTB.$makefile.$COMPONENTC."\n";
	$makefile = shift(@makeList);
}

print CONFIGFILE $BUS;

# find all makefiles in bus
recursivelyFindMakefiles("bus");

# add the targets to the file
$makefile = shift(@makeList);
while(defined $makefile)
{
	print CONFIGFILE $COMPONENTA.$makefile.$COMPONENTB.$makefile.$COMPONENTC."\n";
	$makefile = shift(@makeList);
}

print CONFIGFILE $APP;
# find all makefiles in app
recursivelyFindMakefiles("app");

# add the targets to the file
$makefile = shift(@makeList);
while(defined $makefile)
{
	print CONFIGFILE $COMPONENTA.$makefile.$COMPONENTB.$makefile.$COMPONENTC."\n";
	$makefile = shift(@makeList);
}

chdir $runningdir;

close (CONFIGFILE);


#############################################################################
#                                                                           #
#		         Sub Routines                                       #
#                                                                           #
#############################################################################
sub recursivelyFindMakefiles
#
# all directories are gone through recursively
# looking for makefiles
#
#	PARAMETERS:
#	$_[0] the directory to traverse
#
#   GLOBALS:
#	@makeList
{
	# open the directory
	my $dir = $_[0];
	opendir(DIR, $dir);
	
	# get the list of makefiles and directories
	# Ok, it matches 'Makefile', and any directory but NOT . or .. !
	my @files = grep { (/Makefile/) || ( (-d "$dir/$_") && !(/^\.$/ || /^\.\.$/) )  } readdir DIR;
	
	# close it
	closedir DIR; 
	
	my $makedir;
	
	# for each file
	foreach my $fileName (@files)
	{
		# if its a directory, recurse
		if ( -d "$dir/$fileName")
		{
			recursivelyFindMakefiles("$dir/$fileName");
		}# or if its a make file, add it to the list
		elsif ( -f "$dir/$fileName")
		{
			$makedir = "$dir";
		}
	}
	
	if ( (defined $makedir) && (filterMakeTarget($makedir) != 0) )
	{
		#add it to the list
		push (@makeList, $makedir);
	}
}

#==============================================================================

sub filterMakeTarget
#
#	If it returns 1, the target passed can be added to the make list
#
#	PARAMETERS:
#	$_[0] the directory to traverse
#
#   RETURNS:
#	0 if the target cant be added, 1 if it can
{
	if(($_[0] !~ /\/test/) && ($_[0] !~ /\/entity_config/) && ($_[0] !~ /\/network_monitor/) && ($_[0] !~ /\/acids_ann/) && ($_[0] !~ /\/traffic_detector_agent/) && ($_[0] !~ /\/entity_control/))
	{
		return 1;
	}
	return 0;
}
