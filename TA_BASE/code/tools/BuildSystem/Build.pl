#!/usr/local/bin/perl
#
# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Lennart Goedhart
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
#-=-=-=-=-=-=-=-=- Copyright (c) 1998-2001 Lennart Goedhart -=-=-=-=-=-=-=-=-=-
#
my $PROGRAM_VERSION = "0.1";
#
# **** About this Program ****
#
#    Build.pl is a build system. FIXME - maybe I should POD this?
#
# Usage:
  my $PROGRAM_USAGE = "
  Build.pl [ Build File ] [ --action=ACTION ] [ options ]
  Actions:
  -a ACTION, --Action==ACTION    What to do. Default action is 'release';
  release        Rebuild everything from scratch. This
                 inlcudes cleaning everything first.
  develop        Build everything. This will NOT clean
                 everything first. It will build over the
                 top of what you currently have.
  debug          Debug release. Same as 'release', but 
                 debug binaries are built.
  clean          Clean up everything. Revert to 'virgin' state.
  makefiles      Generate makefiles. Future option... =)

  Options:
  -d LEVEL, --Debug==LEVEL   Print debug from LEVEL 1 to 6 to STDERR.
  -h, --help     Display this banner.
  -v, --version  Print the version number and exit.
  ";
  my $PROGRAM_HELP = "
  Build.pl buildFle.txt \"Word add-in\" --action=release --version=4.0.0
  Build.pl buildFle.txt --debug=4

  Build output will be generated to STDOUT. Debug will be generated to
  STDERR. In addition, the following files are generated.
  xx_Buildout.txt          All output from build process (excluding debug).
  xx_Builddebug.txt        Build process debug information.
  xx_Builderr.txt          Errors/Warnings get printed to here as well.
  xx_Buildsummary.txt      Summary information (success/fail of all
                           components).
  ";
#
# Return Codes:
#    0   - Success
#    1   - Fail
#
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# TODO LIST
# - Add a common STDOUT/STDERR output handler. Logger would be good instead
#   of 'print'.
# - Catch signals and kill children when cancelled.
# - Clean this thing up into functions, into BuildSystem.pm - like you should.
# - Common match symbols - #, %, / - you've got them all!
# - Check for ALL Makefiles, etc before building any components
# - Standardise all your variable names!!! Keep track of them!!!
# - Fix up Buildfile.txt so that 'TargetDir' doesn't need full qualification.
# - 'tee' to STDOUT AND file
# - Should really do dependencies (deps file) instead of relying on user to
#   set build order. Perhaps a 'warning message' if order is not correct.
#   This means that the user should know the build order. It's useful info.
# - Buildoutput.txt should show all warnings, etc.
#-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

# FIXME - Can I get around including these to make the build system smaller?
use strict;
use Cwd;
use File::Copy;
use File::Path;
use File::Find;

use lib 'BuildSystem';
use BuildSystem;
use vars qw( %opt_linkage $opt_version $opt_help $opt_action
             $glbDebugLevel $glbBuildAction $glbBuildComponents);
use Getopt::Long;

#
#==============================================================================
#
#   Program Globals
#
#==============================================================================

use vars qw( $glbBuildEnv $glbTargetFile $glbBuildFile $glbSyscallVerbose
             $glbBuildSuffix $glbTargetDir $glbOutputFile $glbSummaryFile $glbLogDir);
$glbBuildFile       = "Buildfile.txt";
$glbSyscallVerbose  = 0;
$glbBuildSuffix     = "xx_";
$glbTargetDir       = "${glbBuildSuffix}target"; # The start of my target tree
$glbLogDir			= "Log";
$glbOutputFile      = "${glbLogDir}/${glbBuildSuffix}Buildoutput.txt";
$glbSummaryFile     = "${glbLogDir}/${glbBuildSuffix}Buildsummary.txt";

use vars qw( @bufMakeOutput %bufMakeStats @glbBuildOrder $glbBaseDir );
use vars qw( $progMake );

#
#==============================================================================
#
#   Main
#
#==============================================================================
sub Log;
sub ProcessCommandLine;
sub Initialise;
sub ProcessBuildfile;
sub SetBuildOrder;
sub CheckEnvironment;
sub CleanTargetTree;
sub MakeTargetTree;
sub Finale;

select((select(STDOUT), $| = 1)[$[]);

# guard against potential stupids
# NOTE: doing this means we can also report them in the log...
my $evalResult;
$evalResult = eval
{
    ProcessCommandLine();
    CheckEnvironment();
    Initialise();
    ProcessBuildfile();

	# make sure we can write the log files
	if ( !(-d $glbLogDir) )
	{
		mkdir $glbLogDir;
	}
	
    # Check if we have already built for this platform.
    # FIXME - make generic.
    my $builtSolaris = 0;
    my $builtWindows = 0;
    if( -e $glbSummaryFile )
    {
        open( CHECK, "$glbSummaryFile" ) or die "Cannot open $glbSummaryFile\n";
        my @summaryBuffer = <CHECK>;
        $builtSolaris =
            grep /Project for solaris environment/,
                @summaryBuffer;
        $builtWindows =
            grep /Project for MSWin32 environment/,
                @summaryBuffer;
        close( CHECK );
    }

    open( SUMMARY, ">$glbSummaryFile" ) or die "Cannot open $glbSummaryFile\n";
    open( OUTFILE, ">$glbOutputFile" ) or die "Cannot open $glbOutputFile\n";
    select( ( select( OUTFILE ), $| = 1 )[$[] );
    select( ( select( SUMMARY ), $| = 1 )[$[] );

    if( ( $builtSolaris > 0 and $builtWindows > 0
        and $glbBuildAction eq "release" ) or $glbBuildAction eq "clean" )
    {
        # I've built for everything, so I can start from scratch.
        unlink( "$glbSummaryFile" ); # FIXME - check if I did it.
        unlink( "$glbOutputFile" ); # FIXME - check if I did it.

        # TODO - Put this stuff into a VAR, write to a file at the end.
        CleanTargetTree();
    }
    elsif( ( $glbBuildEnv eq "solaris" and $builtSolaris > 0 ) or
           ( $glbBuildEnv eq "MSWin32" and $builtWindows > 0 ) )
    {
        Log "I've already built for $glbBuildEnv... ";
        Log " building anyway.\n";
    }

    Log "Building Project for $glbBuildEnv environment.\n";

    SetBuildOrder();
    
    if( $glbBuildAction eq "develop" or $glbBuildAction eq "release" )
    {
        MakeTargetTree();
    }
    
    
    my $project; # FIXME - This is here because it needs global scope
    my $dswFile; # FIXME - This is here because it needs global scope
    my $buildDir; # FIXME - This is here because it needs global scope
    my $exeName; # FIXME - This is here because it needs global scope
    my $makefile;
    my $findMakeregexp; # Here because the 'Find' sub needs it to be global.
    
    my $component;
    my $line;
    foreach $component ( @glbBuildOrder )
    {
		# added to build a specific target
		my $target = $BuildSystem::makelist{$component}{Target};
		if ( (!defined $target) || ($target eq "") )
		{
			$target = "ALL - ".uc($glbBuildAction);
		}
		
		
        @bufMakeOutput = "";
        # Make sure I'm in my glbBaseDir before I start
        chdir $glbBaseDir or die "Can't change directory to $glbBaseDir\n";
        my $arch = $BuildSystem::makelist{$component}{BuildOn};
        my $os;

        my ( $elem ) = scalar( %{ $arch } ) =~ m#(\d+)/#;
        if( $elem > 1 )
        {
            my( $key, $env );
            foreach $key ( keys( %{ $arch } ) )
            {
                $env = $BuildSystem::buildenvlist{$key}{OperatingSystem};
                if( $env eq $glbBuildEnv )
                {
                    $arch = $key;
                    $os = $env;
                    last;
                }
            }
        }
        else
        {
            my @key = keys( %{ $arch } );
            $arch = pop( @key );
            $os = $BuildSystem::buildenvlist{$arch}{OperatingSystem};
        }

        if( ref( $arch ) eq "HASH" )
        {
            Log "===| '$component' has a multiply defined directive\n" .
                "   | but there were no matches for '$glbBuildEnv'.\n"
        }
        elsif( not( defined $os ) )
        {
            Log "###| Can't find directive '$arch'.\n" .
                "   | Please add a .cfg file for it.\n";
        }
        elsif( $glbBuildComponents ne "" and
            not grep / $component / , $glbBuildComponents )
        {
            Log "===| Skipping '$component'. You asked me not to build it.\n";
        }
        elsif( $os ne $glbBuildEnv )
        {
            Log "===| Skipping '$component'. It's a '$os' directive.\n";
        }
        else
        {
            my $sourceDir = $BuildSystem::makelist{$component}{SourceDir};
            if ( ! -d $sourceDir )
            {
                Log "###| Can't build $component\n" .
                    "   | $sourceDir directory doesn't exist!\n";
            }
            else
            {
                chdir $sourceDir
                    or die "Can't change directory into $sourceDir\n";
            }

            if( $glbBuildAction eq "clean" or $glbBuildAction eq "release" or
                $glbBuildAction eq "debug" )
            {
                my $cleancmd =
                    $BuildSystem::buildenvlist{$arch}{CleanCommand};

                if( not( defined( $cleancmd ) ) )
                {
                    Log "===| Not cleaning '$component'.\n" .
                        "   | CleanCommand not defined.\n";
                }
                else
                {
                    Log "===> Cleaning '$component'... ";
                    my $cleanargs;
                    # FIXME - maybe the whole makefile thing should go first?
                    #$cleancmd =~
                    #    s%\$(Makefile)%$makefile%eg;

			  #$cleancmd =~
                    #    s%\$(Target)%$target%eg;
                    $cleancmd =~
                        s%\$(\w+)%$BuildSystem::buildenvlist{$arch}{$1}%eg;
	
                    # FIXME - do a check that the clean command can be run '-x'
                    # like ReleaseCommand
                    open( MAKEPROG, "$cleancmd |" )
                        or die "Can't start Make Program\n";
                    close( MAKEPROG );
                    Log "done.\n";
                }
            }
            if( $glbBuildAction eq "develop" or $glbBuildAction eq "release" or
                $glbBuildAction eq "debug" )
            {
                Log "===> Building '$component'...";
                my $makego = 0; # FIXME - Nasty little hack until later...
                my $findMakefile;
                if( defined( $BuildSystem::makelist{$component}{Makefile} ) )
                {
                    $findMakefile
                        = $BuildSystem::makelist{$component}{Makefile};
                }
                else
                {
                    $findMakefile
                        = $BuildSystem::buildenvlist{$arch}{Makefile};
                }
                my $foundcounter = 0;
                my @fileList;

                # TODO - if $findMakefile is not a regexp, don't do a Find.
                if( defined( $findMakefile ) )
                {
                    # Turn the string into a regular expression.
                    $findMakeregexp = $findMakefile;
                    $findMakeregexp =~ s#\.#\\\.#g;
                    $findMakeregexp =~ s#\*#\.\*#g;

                    File::Find::find({wanted => \&FindMakefile }, '.');

                    sub FindMakefile
                    {
                        # Only find the matching file in the current directory
                        /^$findMakeregexp\z/s &&
                            ( $makefile = "$File::Find::name" ) &&
                            ( $foundcounter++ )
                            if( $File::Find::dir eq "." );
                    }
                }

                if( not( defined( $makefile ) ) )
                {
                    Log "failed!\n###| Can't build $component \n" .
                          "   | Makefile not found. Can't find " .
                          "'$findMakefile'!\n";
                }
                elsif( $foundcounter > 1 )
                {
                    Log "failed!\n###| Can't build $component \n" .
                          "   | Found more than one match for " .
                          "'$findMakefile'!\n";
                }
                else
                {
                    $makego = 1;
                    # FIXME - still gotta fix this bit up
                    if ( $glbBuildEnv eq "MSWin32" )
                    {
                        ( $exeName ) = $makefile =~ m#\./(.*)\.\w+#;
                    }
                }

                if( $makego )
                {
                    # See if my make program is defined, or if I just have to
                    # inherit from MakeProg.
                    # FIXME - should do a 'test -x' at the beginning of the
                    #       script to make sure the makeprog is there before
                    #       getting this far...
                    #       Move this whole section to the beginning, in fact.
                    my $makeCommand = ucfirst( $glbBuildAction ) . "Command";
                    if( defined( $BuildSystem::buildenvlist{$arch}{$makeCommand} ) )
                    {
                        $progMake =
                            $BuildSystem::buildenvlist{$arch}{$makeCommand};
                    }
                    else
                    {
                        $progMake =
                            $BuildSystem::buildenvlist{$arch}{MakeProg};
                    }
                    # FIXME - can I do multiple recursion for this?
                    $progMake =~
                        s%\$(Makefile)%$makefile%eg;
					$progMake =~
                        s%\$(Target)%$target%eg;
                    $progMake =~
                        s%\$(\w+)%$BuildSystem::buildenvlist{$arch}{$1}%eg;
						

                    die "Make Program not defined in either Buildfile.txt or " .
                        ".cfg file!\n" if( not( defined( $progMake ) ) );

                    push( @bufMakeOutput,
                        "Built using '$progMake'\n" );
                    # FIXME - how can you check if the $progMake is blocking?
                    # eg. remember running just 'sh'? it opens a shell, and
                    # your app blocks? What you gonna do about it?
                    open( MAKEPROG, "$progMake |" )
                        or die "Can't start Make Program\n";
                    my $makeout;
                    while( $makeout = <MAKEPROG> )
                    {
                        chomp $makeout;
                        push( @bufMakeOutput, "$makeout\n" );
				
#				print "$makeout\n";
				
                    }
                    close( MAKEPROG );
					
					# return code checking
					my $exit_code=$? >> 8;
    
                    my $outputLine;
                    $bufMakeStats{$component}{Output} = "";
                    foreach $outputLine ( @bufMakeOutput )
                    {
                        $bufMakeStats{$component}{Output} =
                            $bufMakeStats{$component}{Output} . $outputLine;
                    }
    
                    my $evalcmd =
                        $BuildSystem::buildenvlist{$arch}{ErrorHandling};
                    if( $evalcmd =~ s#^Output:\s+## )
                    {
                        $evalcmd =~
                            s%\$(\w+)%\$bufMakeStats{\$component}{$1}%g;
                    }
                    eval( $evalcmd );

                    if( not( defined( $bufMakeStats{$component}{Errors} ) ) )
                    {
						#Log "can't determine success status.\n" .
						Log "Return code is $exit_code, but no error output was captured"
						    ."   | Check xx_Buildoutput for details.\n";
                        Log ( "=-" x 16 . "  BUILD OUTPUT  " . "-=" x 16
                            . "\n", 2 );
                        Log ( "### Output:\n" .
                            "$bufMakeStats{$component}{Output}\n", 2 );
                        Log ( "=-" x 40 . "\n\n", 2 );
                    }
                    elsif( $bufMakeStats{$component}{Errors} == 0 )
                    {
						Log "ok. ($bufMakeStats{$component}{Warnings} warnings)\n";
						if ($exit_code != 0)
						{
							Log "\t\tExited with abnormal code $exit_code\n";
						}
                        # Copy files to target.
                        # FIXME - Having it in here is a hack.
                        # Even the way it's all done is a dirty hack. =(
                        # Assumptions, assumptions...
                        my $targetdir =
                            $BuildSystem::makelist{$component}{BuildOn}{$arch};
                        if ( $glbBuildEnv eq "MSWin32" and
                            $targetdir ne "" )
                        {
                            copy( "$buildDir/${exeName}.exe",
                                "$glbBaseDir/$glbTargetDir/$targetdir" );
                        }
                        elsif ( $glbBuildEnv eq "solaris" and
                            $targetdir ne "" )
                        {
                            open( MAKEFILE, "Makefile" )
                                or die "Can't open Makefile";
                            my $projname = "";
                            while( $line = <MAKEFILE> )
                            {
                                ( $projname ) = $line =~ m%^PROJ_NAME\s+=\s+(\w+)%
                                    if( grep /^PROJ_NAME/, $line and
                                        $projname eq "" );
                            }
                            close( MAKEFILE );
                            my $rv = copy( "obj/$projname",
                                "$glbBaseDir/$glbTargetDir/$targetdir" );
                            Log "###| Failed to copy $component to target!\n" .
                                "   | '$!'\n"
                                if( $rv == 0 );
                        }
                    }
                    else
                    {
						if ($exit_code != 0)
						{
							Log "failed! ($bufMakeStats{$component}{Errors} errors,"
                            	." $bufMakeStats{$component}{Warnings} warnings)\n";
						}
						else
						{
							Log "ok. But with errors ($bufMakeStats{$component}{Errors} errors,"
                            	." $bufMakeStats{$component}{Warnings} warnings)\n";
						}
							
                        Log ( "=-" x 16 . " ERRORS OCCURED " . "-=" x 16
                            . "\n", 2 );
                        Log ( "### Errors: $bufMakeStats{$component}{Errors}\n"
                            , 2 );
                        Log ( "### Warnings: " .
                            "$bufMakeStats{$component}{Warnings}\n", 2 );
                        Log ( "### Output:\n" .
                            "$bufMakeStats{$component}{Output}\n", 2 );
                        Log ( "=-" x 40 . "\n\n", 2 );
                    }


                    # I've got to do a make install if it's defined.
                    if ( $BuildSystem::makelist{$component}{Install} and
                         $bufMakeStats{$component}{Errors} == 0 )
                    {
                        my $progInstall;
                        if( defined( $BuildSystem::makelist{$component}{InstallCommand} ) )
                        {
                            $progInstall =
                                $BuildSystem::makelist{$component}{InstallCommand};
                        }
                        else
                        {
                            $progInstall =
                                $BuildSystem::buildenvlist{$arch}{InstallCommand};
                        }

                        # I should only do the install if an InstallCommand
                        # is defined, otherwise, I'll skip it
                        if( not( defined( $progInstall ) ) )
                        {
                            Log "===| Not Installing '$component'.\n" .
                                "   | Could not find an InstallCommand " .
                                "directive in the *.cfg file\n";
                        }
                        else
                        {
                            # FIXME - can I do multiple recursion for this?
                            $progInstall =~
                                s%\$(Makefile)%$makefile%eg;
                            $progInstall =~
                                s%\$(\w+)%$BuildSystem::buildenvlist{$arch}{$1}%eg;

                            Log "===> Installing '$component'... ";
                            open( MAKEPROG, "$progInstall |" )
                                or die "Can't start Make Program\n";
                            my $makeout;
                            # FIXME - you are going to do something useful with
                            # this output aren't you? Good boy.
                            # And check for errors while you're at it? There's a
                            # good lad.
                            while( $makeout = <MAKEPROG> )
                            {
                                chomp $makeout;
                                push( @bufMakeOutput, "$makeout\n" );
                            }
                            close( MAKEPROG );
                            Log "done.\n";
                        }
                    }
    
                }
            }
        }
    }

    Finale();
    exit 0;
 
}; # $evalResult = eval ...
 
# how well did we do?
unless ( defined $evalResult )
{
     # not well at all it seems...
     die "Runtime Error: $@";
}


#==============================================================================
#
#   Subroutines
#
#==============================================================================


#==============================================================================
sub Log
#
#   Log to STDOUT/STDERR and the log files.
#
{
#   INPUTS:
    my $logmsg = $_[0];
    my $output = $_[1];
#
#   INIT:
    $output = 3 if ( not defined ( $output ) ); # Set default value.
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   none
#
#   NOTES:
#   This works using binary arithmetic.
#   Always prints to output file
#   1 = To STDOUT (and summary file)
#   2 = To some other file...
#   4 = To yet another file...
#
#==============================================================================
    # FIXME - open once. It's faster. Only need to handle SIGNALS, etc
    print OUTFILE "$logmsg";
    print STDOUT "$logmsg" if ( $output & 1 );
    print SUMMARY "$logmsg" if ( $output & 1 );
} 


#==============================================================================
sub ProcessCommandLine
#
#   Check out my command line arguments
#
{
#   INPUTS:
#   none
#
#   INIT:
    my $retval = 0;
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   $glbBuildFile
    $glbDebugLevel = 3;
    $glbBuildAction = "debug";
#
#   NOTES:
#   TODO
#
#==============================================================================
    %opt_linkage = (
        version => \$opt_version,
        help    => \$opt_help,
        action  => \$glbBuildAction,
        debug   => \$glbDebugLevel,
    );

    $retval = &GetOptions( \%opt_linkage, "version", "help", "action=s",
        "debug:i" );

    if( $opt_version )
    {
        print "Build.pl\n";
        print "Version $PROGRAM_VERSION\n";
    exit 0;
    }

    if( $opt_help or $retval == 0 )
    {
        print "Usage: $PROGRAM_USAGE\n";
        print "Help:  $PROGRAM_HELP\n";
        exit 0;
    }

    die "You haven't asked me what to build!\nUsage: $PROGRAM_USAGE"
        if ( $#ARGV < 0 );

    $glbBuildFile = $ARGV[0];
} 


#==============================================================================
sub Initialise
#
#   Check all the things I need to know to run properly.
#
{
#   INPUTS:
#   none
#
#   INIT:
#   none
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   $glbBuildEnv
#
#   NOTES:
#   TODO
#
#==============================================================================
    # FIXME - Hack for cygwin
    die "I don't like cygwin yet. Please install ActiveState Perl"
        if( $glbBuildEnv eq "cygwin" );

    die "I can't open your $glbBuildFile!\n"
        if ( ! -e "$glbBuildFile" );

} 


#==============================================================================
sub ProcessBuildfile
#
#   Process the Buildfile file
#
{
#   INPUTS:
#   none
#
#   INIT:
    my $file = "";
    my $line;
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   $glbBaseDir
#
#   NOTES:
#   TODO
#
#==============================================================================
    open( BUILDFILE, "$glbBuildFile" )
        or die "I can't open your $glbBuildFile!\n";
    
    while( $line = <BUILDFILE> )
    {
    
        $file = $file . "$line";
    }
    
    # FIXME - This does funky stuff if you leave a comma off the Make component
    # list. e.g. Leave a comma off between SourceDir and TargetDir.
    eval $file;
    
    close( BUILDFILE );
    $glbBaseDir = $BuildSystem::rootDir;
}

#==============================================================================
sub SetBuildOrder
#
#   Set which order to do the build in.
#
{
#   INPUTS:
#   none
#
#   INIT:
    my $component;
    my $element;
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   $glbBuildOrder %BuildSystem::makelist
#
#   NOTES:
#   TODO
#
#==============================================================================
    # Sorting. FIXME - do sorting in BuildSystem.pm?
    foreach $component ( keys %BuildSystem::makelist )
    {
        $element = $BuildSystem::makelist{$component}{Order} - 1;
        $glbBuildOrder[$element] = $component;
    }
}


#==============================================================================
sub CheckEnvironment
#
#   Check which environment I'm working under, and if it's ready to go.
#
{
#   INPUTS:
#   none
#
#   INIT:
#   none
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   none
#
#   NOTES:
#   TODO
#
#==============================================================================
    # Now some init starters... can I find my make program, etc?
    # FIXME - This gets updated by SetEnvironment later...
    # Note that I should have some system defaults also.
    $glbBuildEnv = $^O;
    
    if( $glbBuildEnv eq "MSWin32" )
    {
        # Check if vcvars32.bat has been run. For some reason, the
        # 'open MAKEPROG' below doesn't like two sets of double quotes
        # and there'd have to be two, one for the "msdev.exe", and one for
        # the /MAKE statement.
        if( ! defined( $ENV{MSVCDIR} ) )
        {
            Log "Run VCVARS32.BAT first\n";
            exit 0;
        }
        my $tempenv = $ENV{MSVCDIR};
        $tempenv =~ s%\\%/%g;
        $progMake = "$tempenv/../Common/MSDev98/bin/msdev.exe";
    }
    elsif( $glbBuildEnv eq "solaris" )
    {
        $progMake = `/usr/bin/which gmake`;
        chomp( $progMake );
    }
    else
    {
        die "\n###| Building on an unknown platform! \n\n";
    }
    
    die "Cannot execute $progMake!\n" if( ! -x "$progMake" );
}


#==============================================================================
sub CleanTargetTree
#
#   Make clean, and delete the target directory tree
#
{
#   INPUTS:
#   none
#
#   INIT:
#   none
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   none
#
#   NOTES:
#   TODO
#
#==============================================================================
    if( -d $glbTargetDir )
    {
        Log "Cleaning old build target directory... ";
        rmtree( ["$glbTargetDir"], $glbSyscallVerbose );
        Log "done.\n";
    }
}


#==============================================================================
sub MakeTargetTree
#
#   Create the target directory structure
#
{
#   INPUTS:
#   none
#
#   INIT:
    my $dir;
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   none
#
#   NOTES:
#   TODO
#
#==============================================================================
    # Now let's build my target directory structure. FIXME - In BuildSystem.pm
    Log "Building target directory structure... ";
    foreach $dir ( keys %BuildSystem::targethash )
    {
        mkpath( ["$glbTargetDir/$dir"], $glbSyscallVerbose, 0777 );
    }
    Log "done.\n";
}


#==============================================================================
sub Finale
#
#   Final clean up bits
#
{
#   INPUTS:
#   none
#
#   INIT:
#   none
#
#   OUTPUTS:
#   none
#
#   GLOBALS:
#   none
#
#   NOTES:
#   TODO
#
#==============================================================================
    close( OUTFILE );
    close( SUMMARY );
}

#
#==============================================================================
#
#   Doco
#
#==============================================================================

=head1 NAME

Build - build an entire software project

=for html <link href="lendoc.css" type="text/css" rel="stylesheet">

=head1 SYNOPSIS

  Build.pl [ --help ] [ --version ] [ project ] [ component ... ]
    [ --action=<action> ] [ --debug=<debug_level> ]

=head1 DESCRIPTION

Build builds the specified project on the current platform. You need to run
build once for every architecture you have. So, if your project has both
Solaris and Windows code, then you need to run Build twice, once on each
platform.

=head2 Building when you're already set up (for Developers/Release Officers)

The most useful command if you're starting out;

    Build.pl --help

If you want to build everything from scratch (a fresh and crusty full release);

    Build.pl ProjectX

See that you didn't have to type C<--action=release>? That's because it's the
default action.

If you want to build particular component(s), list them after your project:

    Build.pl ProjectX "Standard MegaMower" "ProjectX UberDrill"

If you want to do an incremental build (not from scratch);

    Build.pl ProjectX --action=develop

To clean up;

    Build.pl ProjectX --action=clean

=head2 Setting up your Build system (for Build Officers)

Build looks in the directory specified by F<project> for a subdirectory called
F<Build>.  In there, it expects to find two files, L<"Buildfile.txt"> and
L<"Buildtargets.txt">.

Before you can start anything though, you must first tell Build how your system
looks. Because it makes no assumptions about your environment, you have to set
up L<configuration files|"BuildSystem Configuration Files"> in your
F<Tools/BuildSystem> subdirectory.
So, the structure of your directories must look like this:

  - Build.pl
     +- Tools
     |   +- BuildSystem
     |       #  Your Configuration files go here. For example:
     |       +- solaris.cfg
     |       +- wingcc.cfg
     |       +- winmsdev.cfg
     +- ProjectX (whatever your project name is)
     |   +- Build
     |       +- Buildfile.txt
     |       +- Buildtargets.txt
     +- ProjectY (some other project)
         +- Build
             +- Buildfile.txt
             +- Buildtargets.txt

Other than that, it can look however you want. It's up to you, since you
just reference where the files are in your L<"Buildfile.txt">.
Let's make an example structure up. For the rest of this document, I'll pretend
I make power tools. See the example L<Buildfile.txt|"Example">.

  - Build.pl
     +- Tools
     |   +- BuildSystem
     |       #  Your BuildSystem config files go here. For example:
     |       +- solaris.cfg
     |       +- WinGcc.cfg
     |       +- WinMSDev.cfg
     +- ProjectX
     |   +- Build
     |   |   +- Buildfile.txt
     |   |   +- Buildtargets.txt
     |   +- UberDrill
     +- Base
         +- CommonComponents
         |   +- powertool
         +- MegaMower

Note that the C<Base> directory didn't have a C<Build> directory in it? That's
because we never actually build a "Base" Project. All of our other projects
are just using the Base components.
Of course, there's nothing stopping you from building a Base project... so long
as you put in a C<Build> directory and your config files, of course!

=head3 BuildSystem Configuration Files

These file live in the BuildSystem directory. They contain the following
directives.

=over 4

=item B<BuildEnvironment>

Specifies an environment that can be used in the L<"Buildfile.txt">. When Build
runs, it will match its current environment against this directive.

=back

The following attributes can be specified as arguments to C<BuildEnvironment>:

=over 4

=item B<Name>

What to call this environent. This is what is referenced in the
L<"Buildfile.txt">.

=item B<OperatingSystem>

Fairly obvious. But to be more specific, it's what perl returns in its C<$^O>
variable.

=item B<Processor>

The processor architecture of your machine. I'm not sure if this is going to
make a difference yet. It's here, just in case =).

=item B<MakeProg>

The system command to run to actually build on this environment - without
arguments (they come later).

=item B<CleanCommand>

When you do a B<Build> with an action of B<release> or B<clean>, run this
command.

=item B<DevelopCommand>

When you do a B<Build> with an action of B<develop>, run this command.

=item B<ReleaseCommand>

When you do a B<Build> with an action of B<release> (which is the default
action), run this command.

=item B<DebugCommand>

When you do a B<Build> with an action of B<debug>, run this command.

=item B<InstallCommand>

If your B<Make> directive in L<"Buildfile.txt"> has the B<Install> flag set to
true (non-zero) run this command as well, when the Develop/Release/Debug is
finished.

=back

Note that there is nothing stopping your from creating multiple environments
on the one OperatingSystem/Processor box. Your just have to set up two
environents.
For example, you may have both Visual Studio and gcc being used in your project.
No problem: create an environment called C<WinMSDev> and one called C<WinGcc>.
The OperatingSystem/Processor values can be the same, but the MakeCommand can
be different. Then, in your L<"Buildfile.txt">, you tell it which one you want
to use.

=head4 Examples

Let's have a file called C<solaris.cfg>. We'll build on a sparc box, using
gmake. I'm just calling it C<solaris> because it's the only solaris
configuration I need, plus I'm lazy.

    BuildEnvironment => {
        Name            => "solaris",
        OperatingSystem => "solaris",
        Processor       => "sparc4u",
        MakeProg        => "gmake",
        InstallCommand  => "$MakeProg install",
        CleanCommand    => "$MakeProg clean",
        #ReleaseCommand => "$MakeProg;test", # should test as well...
        # The following line is redundant. Only showing this for the manual.
        #DevelopCommand => "$MakeProg",
     };

You might notice I didn't specify DevelopCommand, ReleaseCommand or
DebugCommand. That's because if you don't, Build will just use C<MakeProg> on
it's own. And since gmake is nice and simple, it's all I need.

Since this project is writing code for Windows as well, let's make a few more 
environment directives.

First, we'll do the example for our gmake configuration (C<wingcc.cfg>):

    BuildEnvironment => {
        Name            => "WinGcc",
        OperatingSystem => "MSWin32",
        Processor       => "intel",
        MakeProg        => "gmake",
        InstallCommand  => "$MakeProg install",
        CleanCommand    => "$MakeProg clean",
    };

Ahh.. relatively painless. But now, let's try the big one - Visual Studio
(C<winmsdev.cfg>):

    BuildEnvironment => {
        Name            => "WinMSDev",
        OperatingSystem => "MSWin32",
        Processor       => "intel",
        MakeProg        => "msdev",
        makeCommand     => "$MakeProg \"$project\" /MAKE \"ALL - " . uc($action) . "\" /BUILD",
        DebugCommand    => "$makeCommand",
        ReleaseCommand  => "$makeCommand",
        CleanCommand    => "$MakeProg \"$project\" /MAKE \"ALL - ALL\" /CLEAN",
    };

What the..! So... Visual Studio is a I<little> more complex. To be fair, I
haven't set up B<nmake> on my machine. I'm just building the projects using
B<msdev> on the command line. It's still easier than keeping nmakefiles
synced with dsp's etc. Especially when you've got 30 developers.

Hey! You didn't mention makeCommand in the directive list! Well no... it's not a
reserved directive. It's really just a temporary value for the other directives
to use.

See! It's all just perl. This allows us to do the gratuitous variable
substitution and the use of functions like C<uc> (translate to upper case).
There's also character escaping.

Luckily for me, the msdev command line to make release is RELEASE, and debug is
DEBUG, so I just take my B<action> command line argument, and upper case it.
If it wasn't so easy, I could have used ye olde C<if> statment and make a
temporary variable.

=head3 Buildfile.txt

The following attributes can be specified as arguments to C<Make>:

=over 4

=item B<Component>

The Name of the Component. This is how this component gets referenced by other
files in the system. It can (and prehaps should for readability reasons)
contain spaces.

=item B<SourceDir>

Where to find the Makefile, or dsw/dsp file for the make. Depends on how you
set up your environment.

=item B<TargetDir>

Where the target binary gets put when it's built. At the moment it only copies
executables. It will be expanded to include libraries, etc.

=item B<BuildOn>

What you're building this component for. The value for this attribute should
be one of the environments described by the
L<BuildSystem Configuration Files>, which you've set up, of course =).

=item B<Version>

Describe me

=item B<TargetOwner>

Who the owner of the file should be when it is copied to the TargetDir.

=item B<TargetGroup>

Who the owning group of the file should be when it is copied to the TargetDir.

=back

=head4 Example

Okay! Using our above BuildSystem Configuration examples, let's see what the
Buildfile.txt might look like.

    Make (
        Component       => "PowerTool Libraries",
        SourceDir       => "Base/CommonComponents/powertool",
        BuildOn         => "BOTH",
        Install         => 1,   # run 'make install'
        # I don't have a TargetDir, since the Makefile sorts this out for me
    );

    Make (
        Component       => "Standard MegaMower",
        SourceDir       => "Base/MegaMower",
        BuildOn         => (
                solaris     => $BuildSystem::svrbindir,
                WinMSDev    => $BuildSystem::wksbindir,
        );
    );

    Make (
        Component       => "ProjectX Uber Drill",
        SourceDir       => "ProjectX/UberDrill",
        BuildOn         => "WinGcc",
    );

=head3 Buildtargets.txt

Blah blah blah

=head1 SYSTEM REQUIREMENTS

perl. Yes, Julien, this is written in Perl.

So, for UNIXes, download perl (version 6).

This has also been used with ActiveState Perl Build623.

Don't use cygwin's perl, because it can't read environment variables properly.
When I update the functionality to remove this dependence, you should be able
to use cygwin perl.

=head1 BUGS

None yet =).

=head1 TODO LIST

 - Create buildstamp.txt file
 - Create makefiles
 - Write a GUI to generate Buildfile.txt and Buildtargets.txt
 - Generate release notes

=head1 AUTHOR

Lennart Goedhart, lennart.goedhart@mi-services.com

=head1 SEE ALSO

perl(1).

=head1 COPYRIGHT

Copyright (c) 2000, Lennart Goedhart. All rights reserved.

Visual Studio and Windows are either registered trademarks or trademarks of 
Microsoft Corporation in the United States and/or other countries.

Solaris is a registered trademark of Sun Microsystems Inc.

=cut
