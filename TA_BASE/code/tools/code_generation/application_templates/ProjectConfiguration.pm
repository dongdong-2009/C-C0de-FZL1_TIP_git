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
# Example config file
###################################################################
#[LOCATION]
## location relative to base directory
#ProjectLocation=app/test-dir/
## where the template project is (relative to the base directory)
#TemplateProject=code/bus/generic_gui/templates/TemplateDialog
#
#[REPLACE]
## replace this in filenames and in files
## eg TemplateDialog.dsw=AppName.dsw will rename the file,
## and wont watch anything within the file
#TemplateDialog=AppName
#TEMPLATEDIALOG=APPNAME
#Test Property Sheet=Application Name
## fix location of 'code' from the new project location
#..\..\..\..=..\..\..
#../../../..=../../..
###################################################################

require Exporter;
use Carp ();
use strict;
use Cwd;

use vars qw(
        @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS
        $ISA_TTY
		$COMMENT $LOCATION $REPLACE
        %ProjectDefinitions
        );

@ISA = qw( Exporter );
@EXPORT = qw(&parseConfigFile %ProjectDefinitions $COMMENT $LOCATION $REPLACE);
@EXPORT_OK = qw();
%EXPORT_TAGS = ( DEFAULT => [qw( &parseConfigFile %ProjectDefinitions $COMMENT $LOCATION $REPLACE)]);

$COMMENT = "#";
$LOCATION = "[LOCATION]";
$REPLACE = "[REPLACE]";

sub parseConfigFile;

# Parses the given configuration file, and puts the information in
# %ProjectDefinitions
# The input is a config file name
# returns 0 on success, 1 on failure
sub parseConfigFile
{
	my $fileName = $_[0];
	print "Processing $fileName...";
	if ( open( CONFIG_FILE, "$fileName" ) )
	{
		my $line;
        

		my $section = "NONE";
		while($line = <CONFIG_FILE>)
		{
			if ($line =~ /#/)
			{
				# Ignore
			}# search for "[LOCATION]" line
			elsif ($line =~ /\[LOCATION]/)
			{
				$section = "$LOCATION";
			}# search for "[REPLACE]" line
			elsif ($line =~ /\[REPLACE]/)
			{
				$section = "$REPLACE";
			}# something = something else
			elsif ($line =~ /^(.*)=(.*)$/)
			{
				$ProjectDefinitions{$section}{$1} = $2;
			}
			elsif ($line =~ /.*\S.*/)
			{
				print "ERROR unrecognised line in $section:\n$line\n";
			}
		}
		
		# close the template file
		close( CONFIG_FILE );

	}
	else
	{
		print "I can't open your configuration file $fileName\n";
		return 1;
	}
	print "Done\n";
	return 0;
}

1;
