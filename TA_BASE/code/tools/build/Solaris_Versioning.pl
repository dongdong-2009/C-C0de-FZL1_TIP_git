#!perl -w
#
# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Karen Sheh
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
# This is to revert the VersionGen.cpp files to having #buildVersion and
# #headerArray after a build so that these files do not have to be checked out
# again. This was requested by Sean L. for Combuilder use.
#

use File::Find;

#
# Check commandline
#
my $path;
my $version;
my $projectname;

if(@ARGV == 3)
{
    $path = $ARGV[0];
	$version=$ARGV[1];
	$projectname=$ARGV[2];
	
	print "\nPath: $path\n";
	print "\nVersion: $version\n";
	print "\nProjectName: $projectname\n";
}
else
{
    print "Usage: perl Versioning.pl <path>";
    exit;
}

#
# Opens the VersionGen.cpp files.
#
	&File::Find::find(\&wanted, $path);

    exit;

sub wanted
{
    /^VersionGen.cpp$/ &&
	&revert($File::Find::name);
}

sub revert($)
{
    my $newfile = "";
	my $newfile2 = "";
	my $newheader="\t\t\"$projectname\"\n";
	my $lines="";
    my $file = shift @_;

	$file =~ s////\//g;

    print "\nFile: $file\n";

    open(FILE, "$file") || die "No such file $file\n";

    while(<FILE>)
    {
        if(/^(.*char buildVersion\[\]).*$/ || /^(.*char componentVersion\[\]).*$/)
        {
            $newfile .= $1." = \"$version\";\n";
        }
        else
        {
            $newfile .= $_;
        }
    }

    close(FILE);

    # Open file to write
    open(FILE, ">$file") || die "Unable to open file for writting $file\n";
    print FILE "$newfile";
    close(FILE);
    
	open(FILE, "$file") || die "No such file $file\n";

	@lines=<FILE>;
	
	while(@lines)
	{
		$line=shift(@lines);

		if($line =~ /char \* headerArray\[\] =.*$/)
		{
			$newfile2 .= $line;
			$line=shift(@lines);
			while($line !~ /[a-zA-Z]/)
			{
				$newfile2 .= $line;
				$line=shift(@lines);
			}

			$newfile2 .= $newheader;
		}
		else
		{
			$newfile2 .= $line;
		}
	}

    close(FILE);

	open(FILE, ">$file") || die "Unable to open file for writting $file\n";
		print FILE "$newfile2";
    close(FILE);

    return 1;
}
