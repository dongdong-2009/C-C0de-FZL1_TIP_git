#!/usr/local/bin/perl -w
#
# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Katherine Thomson
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
#

#############################################################################
# autobuild.pl                                                              #
#                                                                           #
# This script is for automatically building the ITS system.                 #
#                                                                           #
# USAGE: perl autobuild.pl [-r] [-c] [-b] [-s] <version> <Build File>       #
#                                                                           #
# where            -r => Remove the existing code directory (leave cots)    #
#                  -R => Remove the existing code directory (completely)    #
#        -c <version> => Checkout the <version> branch.                     #
#  -b <version> <Build File> => Build the system using the <Build File>     #
#                  -d => Build the system in debug mode                     #
#                       -s => Summarise failures from xx_Buildsummary.txt   #
#    -m => Mail the results to the addresses specified in autobuild.pl      #
#                                                                           #
#                                                                           #
# Author: Katherine Thomson                                                 #
# Date: 5th February 2002                                                   #
#                                                                           #
# Last Modified by: Adam Radics                                             #
# Date: 21 March 2003                                                       #
# Quick Modification to work with Build.pl's new parameters                 #
# Mail::Sender didnt seem to work, replaced with MIME::Lite                 #
#############################################################################

use strict;
use vars qw($opt_r $opt_R $opt_c $opt_b $opt_s $opt_m $opt_d);
use Getopt::Std;
use MIME::Lite;

# Global variables to be used in most functions.
my $emailTo = "ameerp\@ripple-systems.com, andrewdc\@ripple-systems.com, adamr\@ripple-systems.com";
my $version = "";
my $buildFile = "";
my $environment = $^O;
my $DEBUG = "";
my $logDir = "Log";
my $logFile = "$logDir/autobuild.log";
my $buildOutputFile = "$logDir/xx_Buildoutput.txt";

#############################################################################
#                                                                           #
#                             Main Program                                  #
#                                                                           #
#############################################################################

# make sure we can write the log file
if ( !(-d $logDir) )
{
print "$logDir"; exit 1; # DEBUG #########	
	system "mkdir $logDir";
}
open( LOGFILE, ">$logFile" ) or die "Can't open logfile\n";

# Get the command line options.

my $commandLine = "perl $0";
foreach my $arg (@ARGV)
{
    $commandLine .= " $arg";
}

my $usage = "USAGE: perl autobuild.pl [-r|-R] [-c] [-b] [-s] [-m] <version> <Build File>\n";
$usage   .= "where                 -r => Remove the existing code directory (leave cots).\n";
$usage   .= "                      -R => Remove the existing code directory (completely).\n";
$usage   .= "            -c <version> => Checkout the <version> branch.\n";
$usage   .= " -b <version> <Build File> => Build the system using the <Build File>\n";
$usage	 .= "                      -d => Build the system in debug mode\n";
$usage   .= "                      -s => Summarise failures from xx_Buildsummary.txt\n";
$usage   .= "                      -m => Mail the results to the addresses specified in autobuild.pl\n";

$opt_r = 0;
$opt_R = 0;
$opt_c = 0;
$opt_b = 0;
$opt_s = 0;
$opt_m = 0;
$opt_d = 0;
getopts('rRcbsmd') or die $usage;

Log("Command line: $commandLine\n");

# If none of the options are set, print out the usage banner.
if (($opt_r == 0)&& ($opt_R == 0) && ($opt_c == 0) && ($opt_b == 0) && 
    ($opt_s == 0))
{ 
    die $usage;
}

if ($environment eq "cygwin")
{
    die "This script should be run using a Command Prompt not a cygwin shell.";
}
elsif ($environment eq "MSWin32")
{
    #chdir "f:\\";
}
else
{
    #chdir "";
}

# Remove the existing code directory.
if ($opt_r)
{
    Log("=== Removing code directory, Leaving cots...\n\n");
    system("rm -rf code/app");
    system("rm -rf code/bus");
    system("rm -rf code/core");
    system("rm -rf code/tools");
}
elsif ($opt_R)
{
    Log("=== Removing code directory completely...\n\n");
    system("rm -rf code");
}


# Checkout the branch specified by <version>.
if ($opt_c)
{
    CheckoutFiles();
}

# For the next options we need to be in the ITS directory.
#chdir "code";

# Build the branch specified by <version> and <buildFile>.
if ($opt_b)
{
    BuildITS();
    my $summaryMessage = SummariseErrors();
	if ($opt_m)
	{
		MailBuilders($summaryMessage);
	}
}

# Print out a summary of the failures and email them to someone.
if (($opt_s) && (!$opt_b))
{
    my $summaryMessage = SummariseErrors();
    if ($opt_m)
	{
		MailBuilders($summaryMessage);
	}
}


#############################################################################
#                                                                           #
#                             Subroutines                                   #
#                                                                           #
#############################################################################

#############################################################################
# CheckoutFiles                                                             #
#                                                                           #
# Checkout the <version> files from CVS required for the build.             # 
#                                                                           #
# ARGS:    None                                                             #
# RETURNS: None                                                             #
#############################################################################

sub CheckoutFiles
{
    if (@ARGV == 0)
    {
        Log("### No checkout version specified.");
        die $usage;
    }
    $version = shift(@ARGV);

    Log("=== Checking out $version...\n\n");
    if ( (-e "code/CVS/Repository") && (-e "code/CVS/Entries") && (-e "code/CVS/Root") )
    {
	system("cvs -q update -d -C -A -r $version code");
	}
    else
    {
	system("cvs -q checkout -r $version code");
    }
}

#############################################################################
# BuildITS                                                                  #
#                                                                           #
# Run the Build.pl script.                                                  #
#                                                                           #
# ARGS:    None                                                             #
# RETURNS: None                                                             #
#############################################################################

sub BuildITS
{
    Log("=== Building system...\n\n");

#FIX THIS
#    system("\"c:\\Program Files\\Microsoft Visual Studio\\VC98\\bin\\VCVARS32.bat\"");

    if ($version eq "")
    {
        if (@ARGV == 0)
        {
            Log("### No version specified.");
            die $usage;
        }
    $version = shift(@ARGV);
    }
        
    if (@ARGV == 0)
    {
	    if ($environment eq "MSWin32")
	    {
		    system("perl createWindowsBuildFile.pl code");
		    Log("### No build file specified, auto generating one instead\n");
		    #set the build file to what is autogenerated by createWindowsConfig.pl
		    $buildFile = "BuildFiles/windowsNightly.txt";
	    }
	    else
	    {
		    system("perl createSolarisBuildFile.pl");
		    Log("### No build file specified, auto generating one instead\n");
		    #set the build file to what is autogenerated by createSolarisConfig.pl
		    $buildFile = "BuildFiles/solarisNightly.txt";
	    }
    }
    else 
    {
	    $buildFile = shift(@ARGV);
    }

    # Run the Build script.

    my $cmd = "perl Build.pl $buildFile";
	if ($opt_d)
	{
		$cmd = $cmd . " --action=debug";
	}
	else
	{
		$cmd = $cmd . " --action=release";
	}

    print "Running the Build.pl script with build file: $buildFile\n" if ($DEBUG);
    system ($cmd);

}


#############################################################################
# SummariseErrors                                                           #
#                                                                           #
# Parse the xx_Buildsummary.txt file and list all the projects              #
# that did not build.                                                       #
#                                                                           #
# ARGS:    None                                                             #
# RETURNS: A message about the files that failed.                           #
#############################################################################

sub SummariseErrors
{
    open(SUMMARY_FILE,"$buildOutputFile") or
        die "Could not open $buildOutputFile";

    my $summaryMessage = "";
	my $checkMessage = "";
    my $num = 1;
	my $check = 1;

    while(<SUMMARY_FILE>)
    {
        if( /.*\'(.*)\'.*failed.*/ )
        {
            $summaryMessage .= "$num\. $1\n";    
			$num++;
        }
		elsif( /.*\'(.*)\'.*ok\. But with errors.*/ )
        {
            $checkMessage .= "$check\. $1\n";    
			$check++;
        }
    }
    
    if( $summaryMessage ne "" )
    {
        $summaryMessage = "### The following projects failed to build. " .
                          "See $buildOutputFile.\n\n" . 
                          $summaryMessage;
    }
    	
	if( $checkMessage ne "" )
    {
        $checkMessage = "\n### The following projects may have failed to build. " .
                          "See $buildOutputFile.\n\n" . 
                          $checkMessage;
		$summaryMessage = $summaryMessage . $checkMessage . "\n";
    }
    
	if( $summaryMessage eq "" )
    {
        $summaryMessage = "===There were no build failures.\n";
    }
	
	Log("$summaryMessage");
    return $summaryMessage;
}


#############################################################################
# MailBuilders                                                              #
#                                                                           #
# Mail the builders the results of the build.                               #
#                                                                           #
# ARGS:    The summary message                                              #
# RETURNS: None                                                             #
#############################################################################

sub MailBuilders
{
    my $message = shift(@_);    

    # Set the send type to smtp, rather than sendmail.
    # Set the mail server to exchange
    MIME::Lite->send('smtp', "envelope", Timeout=>60);

    # new message
    my $msg = MIME::Lite->new(
                 To      =>$emailTo,
                 Subject =>"$environment $version build for $buildFile",
                 Type    =>'TEXT',
		 Data    =>$message
                 );
    # attach the buildoutput text file
    $msg->attach(Type     =>'text/plain',   
                 Path     =>"$buildOutputFile"
		 );
		 
    # finally send it
    $msg->send; 
}

#############################################################################
# Log                                                                       #
#                                                                           #
# Log to the file "autobuild.log" and to the command line if -v is specifed.#
#                                                                           #
# ARGS:    The message to log.                                              #
# RETURNS: None                                                             #
#############################################################################

sub Log 
{
    my $log = shift(@_);
    print LOGFILE $log;
    print $log;
}
