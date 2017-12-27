# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Adam Radics
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
# Quick hack to autmoatically create a build file
# This grabs the projects defined in the global workspace
# It uses the names to create build targets, and writes a config file
# It removes anything with agent or system controller in the name
#
# this will be cleaned up and made more robust when
# the build script gets an overhaul (read non-dodgy rewrite)

use Carp ();
use strict;

my $USAGE = "Usage: createWindowsBuildFile.pl [location of code]
Some options are hard coded in the script, since they arent likely to change often"; 


# constants
use vars qw ($START_PROJECT $globalWorkspace $globalWorkspaceLocation $buildFileName);
$globalWorkspace = "Build_Everything.dsw";
$globalWorkspaceLocation = "tools/build/";
$buildFileName = "BuildFiles/windowsNightly.txt";

# make sure we can write the config file
if ( !(-d "BuildFiles") )
{
	mkdir "BuildFiles";
}

# used for parsing
$START_PROJECT = "Project:";

###############################################################################

# globals
use vars qw($buildFile %ProjectList $codeDirectory);

$buildFile = "# Things we need to know about this project
ProjectInformation(
	# Base directory should be 'code' 
	# relative from here, or absolute both work, although absolute must be reconfigured more often
	BaseDirectory	=> \"code\",
);

# Compilation environments
BuildEnvironment (
    Name            => 'MSWin32',
    OperatingSystem => 'MSWin32',
    Makefile        => '*.dsw',
    Processor       => 'intel',
    MakeProg        => 'msdev',
    DebugCommand    => '\$MakeProg \"\$Makefile\" /MAKE \"\$Target\" /BUILD',
    ReleaseCommand  => '\$MakeProg \"\$Makefile\" /MAKE \"ALL - RELEASE\" /BUILD',
    CleanCommand    => '\$MakeProg \"\$Makefile\" /MAKE \"ALL - ALL\" /CLEAN',
    ErrorHandling   => 'Output:
       \$Warnings   = grep( /.*warning .*/, split( /\\n/, \$Output ) );
       \$Errors     = grep( /.*error .*/, split( /\\n/, \$Output ) ); ',

);

# Actual Components to make\n";


# prototypes
sub processGlobalDSWFile;
sub buildMakeDefinitions;

#==============================================================================

if ($#ARGV < 0)
{
	#die $USAGE;
	print "$USAGE";
	print "Assuming code diectory is ./code";
	$codeDirectory = "code";
}
else
{
	$codeDirectory = shift(@ARGV);
}


# Open global workspace file
if (processGlobalDSWFile() == 0)
{
	buildMakeDefinitions();

	# write the new file
	print "Writing $buildFileName...";
	open( BUILD_FILE, ">$buildFileName" )
		or die "Cant open $buildFileName for writing";
	print BUILD_FILE "$buildFile";
	close(BUILD_FILE);
	print "Done\n";
}


#==============================================================================

sub processGlobalDSWFile

{
	# Open Template file
	print "Processing $codeDirectory/$globalWorkspaceLocation/$globalWorkspace...";
	
	if ( open( GLOBAL_WORKSPACE, "$codeDirectory/$globalWorkspaceLocation/$globalWorkspace" ) )
	{
		my $line;

		my $name = "empty";
		while($line = <GLOBAL_WORKSPACE>)
		{
			
			# search for "Project:" line
			if ($line =~ /.*$START_PROJECT.*/)
			{
				# get project info
				# The name is whats inbetween the two quotes
				$line =~ /\"(.*)\"=/;
				$name = $1;
				
				# The location starts at = and ends with .dsp
				$line =~ /.*=(.*)\\.*\.dsp.*/;
				my $location = $1;
				$ProjectList{$name}{Location} = $location;
			 }
		}
		
		# close the workspace file
		close( GLOBAL_WORKSPACE );

	}
	else
	{
		print "I can't open your global DSW file $codeDirectory/$globalWorkspaceLocation/$globalWorkspace\n";
		return 1;
	}
	print "Done\n";
	return 0;
}

#==============================================================================

sub buildMakeDefinitions
{
	my $coreList;
	my $busList;
	my $appList;
	
	for my $component (keys %ProjectList)
	{
		if ( ($component !~ /Agent/) && ($component !~ /Everything/) && ($component !~ /SystemController/) )
		{
			my $makedef = "\nMake (
   Component       => \"$component\",
   SourceDir       => \"$globalWorkspaceLocation\",
   BuildOn         => \"MSWin32\",
   Makefile        => \"$globalWorkspace\",
   Target          => \"$component - Win32 Debug\",
);\n";
			if ($ProjectList{$component}{Location} =~ /\\app\\/)
			{
				$appList .= $makedef;
			}
			elsif ($ProjectList{$component}{Location} =~ /\\bus\\/)
			{
				$busList .= $makedef;
			}
			elsif ($ProjectList{$component}{Location} =~ /\\core\\/)
			{
				$coreList .= $makedef;
			}
		}
	}
	
	$buildFile .= "#========================== CORE =============================\n";
	$buildFile .= $coreList;
	$buildFile .= "#========================== BUS =============================\n";
	$buildFile .= $busList;
	$buildFile .= "#========================== APP =============================\n";
	$buildFile .= $appList;
}
#==============================================================================
