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
# This script moves all of the files from the staging area
# and deploys them in their appropriate place on the specified host
#

my $USAGE = "usage: perl deploy.pl <staging area> <host>\n";
$USAGE .= "Output will be to the screen, and to files_found.out\n";
$USAGE .= "where    <staging area> => is the directory of the staging area\n";
$USAGE .= "         <host>         => is the host to deploy to";

#Global variables needed by most sub routines
my $environment=$^O;
my $staging_area="";
my $directory="";
my $count=0;
my $glbIntegrationFile;

#Modules required
use Getopt::Std;
use File::NCopy qw (copy);
use File::Path;
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
$staging_area = shift(@ARGV);

if($staging_area eq"")
{
	die $USAGE;
}

$staging_area =~ s/\\/\//g;
print "copying files from $staging_area to on $host\n";

$host = shift(@ARGV);
if($host eq"")
{
	die $USAGE;
}
#make sure all paths exsist and if not create them



if($environment eq "cygwin")
{
	die "This script should be run using a Command Prompt not a cygwin shell.";
}
if($environment eq "MSWin32")
{
	if (-d "$staging_area")
	{
		if(-d "//$host/Transactive")
		{
			my $file = File::NCopy->new(recursive => 1);
			check_directory("//$host/Transactive/test");
			if(!$file->copy("$staging_area/*","//$host/Transactive/"))
			{
				print "failed $_\n";
			}	
			#$file->copy "$staging_area/*","//$host/Transactive";
		}
		else 
		{
			die "//$host/Transactive doesn't exsist\n";
		}
	}
	else 
	{
		die "$staging_area doesn't exsist\n";
	}
}
else
{
	$directory = getcwd();
	chdir $staging_area;
	system("tar cf - bin lib | rsh -l transactive $host \"cd /u01/transactive; tar xf -\"");
	chdir $directory;
}





#############################################################################
#                                                                           #
#		         Sub Routines                                       #
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
    my $file = "";
    my $line;

    open( INTEGRATEFILE, "$glbIntegrationFile" )
        or die "I can't open your $glbIntegrationFile!\n";
    
    while( $line = <INTEGRATEFILE> )
    {
    
        $file = $file . "$line";
    }
    
    # FIXME - This does funky stuff if you leave a comma off the Make component
    # list. e.g. Leave a comma off between SourceDir and TargetDir.
    eval $file;
    
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
		    print getcwd()."/";
		    print "$_\n";
		    copy("$_","$staging_area/$DeployList{$count}{CopyTo}/");
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
	print "$copyFrom $copyTo\n";
	#my $file = File::NCopy->new(recursive => 1);
	check_directory("$staging_area/$copyTo");
	#if(!$file->copy($copyFrom,"$staging_area/$copyTo/"))
	#{
	#	print "failed $_\n";
	#}
	system("cp -r $copyFrom $staging_area/$copyTo");
}
