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
# This script archives the staging area to ntserver
#

my $USAGE = "usage: perl archiveStaging.pl <staging area> <iteration number> <smbclient user>\n";
$USAGE .= "where    <staging area> is the staging area eg /u01/staging/C830_Iteration3_030\n";
$USAGE .= "         <iteration number> is the iteration number, eg 3, 3A etc\n";
$USAGE .= "         <smbclient user> is the user to log on as, itsbuild is default, but doesnt have permissions yet\n";
$USAGE .= "         Note that on Solaris the smbclient tool must be in your path\n";

#Global variables needed by most sub routines
my $environment=$^O;
my $staging_area="";
my $iteration_number="";
my $user="";

#Modules required
use Getopt::Std;
#use File::Find;
use File::Copy;
#use File::NCopy qw (copy);
use File::Path;
use Cwd;
#############################################################################
#                                                                           #
#                             Main Program                                  #
#                                                                           #
#############################################################################

#get command line args
$staging_area = shift(@ARGV);
if($staging_area eq "")
{
	die $USAGE;
}
$staging_area =~ s/\\/\//g;

$iteration_number = shift(@ARGV);
if($iteration_number eq "")
{
	die $USAGE;
}

$user = shift(@ARGV);
if($user eq "")
{
	$user = "itsbuild";
}

if($environment eq "cygwin")
{
	die "This script should be run from Solaris\n";
}
elsif ($environment eq "MSWin32")
{
	# create zip files
	system("pushd \"\\\\ntserver\\projects\\1TS-4669-F\\Iteration Release\"& mkdir Iteration_$iteration_number& popd");
	system ("pushd $staging_area& zip \"\\\\ntserver\\projects\\1TS-4669-F\\Iteration Release\\Iteration_$iteration_number\\Windows.zip\" -r Windows& popd");
	system ("pushd $staging_area& zip \"\\\\ntserver\\projects\\1TS-4669-F\\Iteration Release\\Iteration_$iteration_number\\Solaris.zip\" -r Solaris& popd");
}
else
{
	# get where we are
	my $runningDir = cwd();
	
	# go to staging area
	chdir "$staging_area" or die "Cant change to $staging_area $_\n";
	
	# create zip files
	print "==>Creating archives\n";
	system ("zip Windows.zip -r Windows/");
	system ("zip Solaris.zip -r Solaris/");
	
	# copy zip files
	print "==>Copying zip files to:\n";
	print "==> //ntserver/projects/1TS-4669-F/Iteration Release/Iteration_$iteration_number\n";
	print "==> you may be prompted for a password\n";
	my $command = "smbclient //ntserver/projects -U $user -D \"1TS-4669-F/Iteration Release\" -c \"mkdir Iteration_$iteration_number; cd Iteration_$iteration_number; put Windows.zip; put Solaris.zip;\" ";
	system ($command);
	
	# remove zip files
	print "==>Removing archives\n";
	system ("rm Windows.zip");
	system ("rm Solaris.zip");
	
	# change back
	chdir $runningDir;
}

