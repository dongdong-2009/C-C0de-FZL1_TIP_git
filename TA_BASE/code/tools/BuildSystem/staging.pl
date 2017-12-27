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
# This script moves all of the built files from the build
# machine to its appropriate place in the staging area
#

my $USAGE = "usage: perl staging.pl [-f] <staging area> <staging setup file>\n";
$USAGE .= "Output will be to the screen, and to files_found.out\n";
$USAGE .= "where          -f  => forces overwriting and removal of discrepency (no questions asked)\n";

#Global variables needed by most sub routines
my $environment=$^O;
my $staging_area="";
my $input="";
my $directory="";
my $count=0;
my $glbIntegrationFile;
my $logDir = "Log";
my $logFile = "$logDir/staging.log";
my $file = File::NCopy->new(recursive => 1);

#command line argument vars
use vars qw($opt_f);

#Modules required
use Getopt::Std;
use File::Find;
#use File::Copy;
use File::NCopy qw (copy);
use File::Path;
use BuildSystem::DeployConfiguration;
use Cwd;
#############################################################################
#                                                                           #
#                             Main Program                                  #
#                                                                           #
#############################################################################

#get command line args

my $commandLine = "perl $0";
foreach my $arg (@ARGV)
{
    $commandLine .= " $arg";
}

#process all command line options
getopts('f') or die $USAGE;

$staging_area = shift(@ARGV);
if($staging_area eq"")
{
	die $USAGE;
}
$staging_area =~ s/\\/\//g;

print "$staging_area\n";

$glbIntegrationFile = shift(@ARGV);
if($glbIntegrationFile eq"")
{
	die $USAGE;
}


if($environment eq "cygwin")
{
	die "This script should be run using a Command Prompt not a cygwin shell.";
}

else
{
	if ( !(-d "$logDir") )
	{
		mkdir "$logDir";
	}
	
	open( LOGFILE, ">$logFile" ) or die "Can't open logfile\n";

	process_integration_file();
	
	foreach my $counting(keys%DeployList)
	{
		$count=$counting;
		check_directory("$staging_area/$DeployList{$count}{CopyTo}");
		find(\&sort_files, $DeployList{$count}{Search});
	}
	
	close(LOGFILE);
}





#############################################################################
#                                                                           #
#		                 Sub Routines                                       #
#                                                                           #
#############################################################################


#############################################################################
# Process integration file                                                  #
#   Processes the setup file                                                #
#                                                                           #
#   INPUTS: none                                                            #
#   OUTPUTS: none                                                           #
#                                                                           #
#############################################################################

sub process_integration_file
{
    my $ifile = "";
    my $line;

    open( INTEGRATEFILE, "$glbIntegrationFile" )
        or die "I can't open your $glbIntegrationFile!\n";
    
    while( $line = <INTEGRATEFILE> )
    {
    
        $ifile = $ifile . "$line";
    }
    
    # FIXME - This does funky stuff if you leave a comma off the Make component
    # list. e.g. Leave a comma off between SourceDir and TargetDir.
    eval $ifile;
    
    close( INTEGRATEFILE );
}


#############################################################################
# Sort Files                                                                #
#                                                                           #
# take files found by the 'find' routine and sort them into                 #
# their appropriate directory in the staging area                           #
#                                                                           #
# ARGS:    (run by File::Find)                                              #
# RETURNS: None                                                             #
#############################################################################

sub sort_files
{
    if(eval $DeployList{$count}{FileNameContains})
    {
	    if(getcwd() =~ $DeployList{$count}{DirectoryNameContains})
	    {
			my $fileName = $_;
			Log(getcwd()."/$fileName -> $staging_area/$DeployList{$count}{CopyTo}/\n");
		    if (!$file->copy("$fileName","$staging_area/$DeployList{$count}{CopyTo}/"))
			{
				Log("Failed $_\n");
			}
	    }
    }
}
#############################################################################
# Check directory                                                           #
#                                                                           #
# make sure the directory exists and if not will create it                  # 
# will not ask for conformation if $opt_f set true                          #
# ARGS:    directory that wants to be checked                               #
# RETURNS: None                                                             #
#############################################################################
sub check_directory
{
	$directory = shift(@_);
	print "$directory \n";
	if (-d $directory) 
	{
		# name exists and is a directory
	}
	elsif (-e _) {
		# $pathname exists, but is NOT a directory
		print "$directory is a file not a directory\n";
		if(!$opt_f)
		{
			print "do you wish to remove it? (y/n)\n";
			$input = <>;
		}
		#if answered yes to question or force suppression of questions
		if((lc($input) eq "y\n") || $opt_f)
		{
			#removing bogus file and
			#making directory
			system("rm -f \"$directory\"");
			mkpath($directory,1,0777);
		}
		else 
		{
			die "cant continue with this operation\n";
		}
	}
	else {
		# $pathname doesn't even exist
		if(!$opt_f)
		{
			print "$directory doesn't exist, do you want to create it (y/n)";
			$input = <>;
		}
		#if answered yes to question or force suppression of questions
		if((lc($input) eq "y\n") || $opt_f)
		{
			#making directory
			mkpath($directory,1,0777);
		}
		else 
		{
			die "cant continue with this operation\n";
		}
	}
}

sub fileCopy
{
	my $copyFrom = shift(@_);
	my $copyTo = shift(@_);
	Log("$copyFrom -> $copyTo\n");
	check_directory("$staging_area/$copyTo");
	if(!$file->copy($copyFrom,"$staging_area/$copyTo/"))
	{
		Log("failed $_\n");
	}
	#system("cp -r $copyFrom $staging_area/$copyTo");
}


sub Log
{
	print "$_[0]";
	print LOGFILE "$_[0]";
}
