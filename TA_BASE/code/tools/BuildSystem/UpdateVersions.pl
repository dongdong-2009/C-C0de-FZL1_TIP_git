# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  andrewdc
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
# This program automatically creates each application and library's
# VersionGen.cpp file which contains information about the version of the
# application / library.
#
# It receives its data from the specified XML definitions file.
#

use XML::Simple;
use Data::Dumper;
require Time::Local;
# version file for DrWatson.pl and its.sh scripts to use
open( VERSIONINFO, ">./versions" ) or die "Can't open ./versions\n";
sub GenerateArrayData
{
    my ( $fileHandle, $arrayName, @arrayData ) = @_;

    if( -1 == $#arrayData )
    {
        printf $fileHandle "\t\tint %sSize = 1;\n", $arrayName;
        print $fileHandle "\t\tchar * $arrayName\[\] =\n";
        print $fileHandle "\t\t{\n\t\t\t\" \"\n";
        print $fileHandle "\t\t};\n";
    }
    else
    {
        printf $fileHandle "\t\tint %sSize = %d;\n", $arrayName, $#arrayData + 1;
        print $fileHandle "\t\tchar * $arrayName\[\] =\n";
        print $fileHandle "\t\t{\n";
        for $value ( @arrayData )
        {
            print $fileHandle "\t\t\t\"$value\"";
            if( $value ne $arrayData[ $#arrayData ] )
            {
                print $fileHandle ",";
            }
            print $fileHandle "\n"
        }
        print $fileHandle "\t\t};\n";
    }
}

sub GenerateVersionFile
{
    my ( $versionFile,
         $componentName,
	 $buildNumber,
         $buildVersion,
         $patchLevel,
         $libraries,
         $headers,
         $other ) = @_;

    my $fileHandle;
    if (open( $fileHandle, ">$versionFile" ))
	{
		print "Generating version file for component '$componentName'\n";
		
		print $fileHandle "/*\n";
		printf $fileHandle " * \$%s:\$\n", "Source";
		printf $fileHandle " * \$%s:\$\n", "Revision";
		printf $fileHandle " * \$%s:\$\n *\n", "Date";
		print $fileHandle " * DO NOT MANUALLY EDIT THIS FILE.  THIS FILE IS\n";
		print $fileHandle " * CREATED AUTOMATICALLY BY THE BUILD SYSTEM\n";
		print $fileHandle " */\n";
	
		# insert namespace
		
		print $fileHandle "namespace TA_Core\n";
		print $fileHandle "{\n";
		
		print $fileHandle "\tchar componentName[] = \"$componentName\";\n";
		# RY : removed keyword substitution for cvs tag as was not working correctly
		# printf $fileHandle "char buildVersion[] = \"$buildVersion TAG: \$%s:\$\";\n", "Name";
		printf $fileHandle "\tchar buildVersion[] = \"$buildVersion\";\n";
		print $fileHandle "\tchar componentVersion[] = \"$buildVersion";
		if ( $patchLevel ne "0" )
		{
			print $fileHandle " - Application Patch $patchLevel";
		}
		print $fileHandle "\";\n";
	
		print $fileHandle "\tchar buildDate[] = \"$buildNumber\";\n";
	
		print $fileHandle "\tchar identString[] = \"\@(\#)Uncontrolled\";\n";
	
		GenerateArrayData( $fileHandle, "libraryArray", @$libraries );
		GenerateArrayData( $fileHandle, "headerArray", @{ $headers } );
		GenerateArrayData( $fileHandle, "otherArray", @{ $other } );
		
		# end namespace
		print $fileHandle "}\n";
		
		close($fileHandle);
	}
	else
	{
		print "Cant write version file for component '$componentName'\n";
	}
}

#
# Make sure that there are sufficient arguments.
#
if( $#ARGV != 0 )
{
    die "usage: ./UpdateVersions.pl <versiondata>\n";
}

#
# NOTE: suppressempty must be set to 1, otherwise XMS::Simple returns empty
# attributes as empty hashes and this confuses the application.
#
my $xmlParser = new XML::Simple( suppressempty => 1 );
my $xmlDefinition = $xmlParser->XMLin( $ARGV[0] );

#
# Extract the project specific values.
#
my $projectTitle = $xmlDefinition->{DETAILS}->{TITLE};
my $projectVersion = $xmlDefinition->{DETAILS}->{VERSION};
print VERSIONINFO "Version:$projectVersion\n";

#
# Generate the VersionGen.cxx file for each component.
#
for $component ( keys %{ $xmlDefinition->{COMPONENTS} } )
{
    my @componentLibraries = ( split / /, $xmlDefinition->{COMPONENTS}->{$component}->{LIBS} );
    my @componentHeaders = ( $projectTitle );
    my @componentOther = ( split /\n/, $xmlDefinition->{COMPONENTS}->{$component}->{OTHER} );
    my $componentPatchLevel = $xmlDefinition->{COMPONENTS}->{$component}->{PATCH_LEVEL};
    my $componentName = $xmlDefinition->{COMPONENTS}->{$component}->{NAME};
    my $componentBuildNumber = $xmlDefinition->{COMPONENTS}->{$component}->{BUILDNUMBER};
    my $componentLocation = $xmlDefinition->{COMPONENTS}->{$component}->{LOCATION};

    print VERSIONINFO "$componentName:$componentBuildNumber\n";
    #
    # Now generate the file.
    #
    GenerateVersionFile( $componentLocation . "/VersionGen.cpp",
                         $componentName,
			 $componentBuildNumber,
                         $projectVersion,
                         $componentPatchLevel,
                         \@componentLibraries,
                         \@componentHeaders,
                         \@componentOther );
}


