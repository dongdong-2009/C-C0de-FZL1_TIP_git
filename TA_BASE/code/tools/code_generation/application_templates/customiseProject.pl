#!/usr/local/bin/perl
#
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
# Usage:
  my $PROGRAM_USAGE = "customiseProject.pl [ Configuration File ] [ code path ]\n"
  					. "\tWhere [Configuration File] is the config file for the particular template project\n"
					. "\t      [code path] is the location of the 'code' repository from here\n";


use strict;
use Cwd;
use File::NCopy qw(copy);
use File::Find;
use File::Path;
use File::Spec;

use ProjectConfiguration qw(:DEFAULT);

#
#==============================================================================
#
#   Program Globals
#
#==============================================================================

use vars qw($glbConfigFile $glbCodePath );

#
#==============================================================================
#
#   Main
#
#==============================================================================
sub copyProject;
sub renameFiles;
sub replaceText;

# We must have a configuration file, and a path
die "Usage: $PROGRAM_USAGE" if ( $#ARGV < 1 );
my $glbConfigFile = $ARGV[0];
die "I can't open your $glbConfigFile!\n" if ( ! -e "$glbConfigFile" );
my $glbCodePath = $ARGV[1];


# Read in the configuration
if (parseConfigFile($glbConfigFile) == 0)
{
	# copy the files to the location
	my $location = copyProject();
	
	# rename files
	renameFiles($location);
	
	# replace text in each file
	recursivelyReplaceText($location);
}

exit 0;


#==============================================================================
#
#   Subroutines
#
#==============================================================================

sub copyProject
#
# Given the template project to use,
# copy it to the final location
# move the generated makefiles to the final location
# remove the template makefiles from the final location
#
#   GLOBALS:
#	$glbCodePath
#	$ProjectDefinitions
#	$glbConfigFile
#
#	RETURNS:
#	The absolute location created
{
	# get our location
	my $runningDir = cwd;

	# change to the code location and create the project directory
	chdir "$glbCodePath" or die "Can\'t change to the directory $glbCodePath";
	mkpath "$ProjectDefinitions{$LOCATION}{ProjectLocation}";
	
	# change to it, and get an absolute location
	# this makes sure it was created, and the absolute location
	# is printed in case the user entered something wrong,
	# there is no 'where did it go?'
	chdir "$ProjectDefinitions{$LOCATION}{ProjectLocation}";
	my $absoluteLocation = cwd;
	print "$absoluteLocation created\n";

	# change back to the code directory
	chdir "$runningDir";
	# we have now created the project location, and know where it is absolutely
	
    # get the absolute location of the template
    chdir("$glbCodePath/$ProjectDefinitions{$LOCATION}{TemplateProject}")
        or die "Can\'t change to the template directory $glbCodePath/$ProjectDefinitions{$LOCATION}{TemplateProject}";
    
    my $absoluteTemplate = cwd();
    
	# (recursively) copy the template to the project location
	print "Copying template project from $absoluteTemplate to $absoluteLocation...";
	my $file = File::NCopy->new(recursive => 1);
	
	if ($file->copy("\*","$absoluteLocation") )
	{
		print "Done\n";
	}
	else
	{
		print "Failed\n$!\n";
	}
	
	
	# change to the directory
	chdir "$absoluteLocation";
    
    # remove the copied config file
	# strip the path from the config file name
	my $abs_path = File::Spec->rel2abs($glbConfigFile);
	my ($volume,$directories,$file) = File::Spec->splitpath($abs_path);
	unlink $file;
    
    # remove the CVS directories
    # and the unneccesary visual studio option files
    # that crash visual studio if they were copied across
    &find(\&removeUnwanted,'.');
    
	
	chdir "$runningDir";
	return $absoluteLocation;
}

sub removeUnwanted
{
    my $dir = $_;
    if (-d $dir)
    {
        # list the files in this directory
        opendir THISDIR, $dir;
        my @allFiles = readdir THISDIR;
        closedir THISDIR;

        # if the cvs directory exists, delete it
        if ( "@allFiles" =~ /CVS/)
        {
            rmtree "$dir/CVS";
        }
        
        # if the ncb file exists, delete it
        if ( "@allFiles" =~ /\.ncb/)
        {
            unlink <$dir/*.ncb>;
        }
        
        # if the opt file exists, delete it
        if ( "@allFiles" =~ /\.opt/)
        {
            unlink <$dir/*.opt>;
        }
    }
    # return this to its original value
    # File::Find needs it like that
    $_ = $dir;
}
#==============================================================================

sub renameFiles
#
# rename the files specified in the config file
#
#	PARAMETERS:
#	The absolute location created
#
#   GLOBALS:
#	$glbCodePath
#	$ProjectInfo
{
	# get our location
	my $runningDir = cwd;
	my $location = $_[0];
    
    &find(\&renameFile, $location);
}

sub renameFile
{
    my $oldFileName = $_;
    my $newFileName = $oldFileName;
    
    # replace all of the occuring patterns in the filename
    foreach my $pattern (keys %{$ProjectDefinitions{$REPLACE}})
    {
        $newFileName =~ s/$pattern/$ProjectDefinitions{$REPLACE}{$pattern}/g;
    }
    
    if ($oldFileName ne $newFileName)
    {
        use File::Copy qw(move);
        
        # move the old file to the new file
        print "moving $oldFileName to $newFileName...";
        if (move("$oldFileName","$newFileName"))
        {
            print "success\n";
        }
        else
        {
            print "failed\n$_\n";
        }
    }
}
#==============================================================================

sub recursivelyReplaceText
#
# do the global find and replace on the strings specified in the config file
# all files and directories are gone throught recursively
#
#	PARAMETERS:
#	$_[0] the directory to traverse
#
#   GLOBALS:
#	$glbCodePath
#	$ProjectDefinitions
{
	# open the directory
	my $dir = $_[0];
	
    &find(\&replaceText, $dir);
}

#==============================================================================

sub replaceText
#
# do the global find and replace on the strings specified in the config file
# all files and directories are gone throught recursively
#
#	PARAMETERS:
#	$_[0] the file to replace text in
#
#   GLOBALS:
#	$ProjectDefinitions
{
	my $fileName = $_;
    
    # if its a plain file
    if ( -T $fileName )
    {
        my $fileContents;
        
        print "\nSetting write permissions on $fileName...";
        if (chmod(0644, $fileName) == 1)
        {
            print "Succeeded\n";
        }
        else
        {
            print "Failed\n";
        }
        
        print "Reading $fileName...";
        if (open (INPUT_FILE, $fileName))
        {
            # read it in
            my @text = <INPUT_FILE>;
            
            # close the template file
            close (INPUT_FILE);
    
            # put it in the output variable
            # the $" is the separator when the array is written to the string
            my $tmp = $";
            $" = "";
            $fileContents = "@text";
            my $oldFileContents = $fileContents;
            $" = $tmp;
            print "Done\n";
            
            # replace the text
            for my $target (keys %{$ProjectDefinitions{$REPLACE}})
            {
                my $replacement = $ProjectDefinitions{$REPLACE}{$target};
                # quote out special characters
                my $target = "\Q$target\E";
                $fileContents =~ s/$target/$replacement/g;
            }
            
            # write the modified text
            if ($fileContents ne $oldFileContents)
            {
                print "Writing modified $fileName...";
                if ( open( OUTPUT_FILE, ">$fileName" ) )
                {
                    print OUTPUT_FILE "$fileContents";
                    close(OUTPUT_FILE);
                    print "Done\n";
                }
                else
                {
                    print "Cant open for writing, check permissions\n";
                }
            }
            else
            {
                print "$fileName is unmodified\n";
            }
        }
        else
        {
            print "Can't open, skipping\n";
        }
    }
}

