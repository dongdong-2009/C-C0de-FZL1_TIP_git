#!/usr/bin/perl
# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Grace Koh
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
# Lists files that have been modified as part of a fix for an issue
# or defect within a date range for a specified cvs user based on the
# file's cvs logs. Note that the defect or issue number must be present
# in the file's logs and must match the regex [TP][DW].{0,2}[0-9]+

use strict;

my $file;
my $date_range;
my $user;

if( @ARGV == 1 )
{
	($file) = @ARGV;
}
elsif( @ARGV == 3 )
{
	($file,$user,$date_range) = @ARGV;
	system "cvs log -N -w$user -d \"$date_range\" > $file";
	print "\n";
}
else
{
	die "usage trace_defets_and_issues FILE_NAME [USER CVS_COMPAT_DATE_RANGE]\n
         e.g. trace_defects_and_issues cvs-out.txt greggk 2004-02-14\\<2004-02-17\n
		 or   trace_defects_and_issues cvs-out.txt";

}

open(CVSOUT,"< $file") or die "Can't open $file for reading: $!\n";
my @section;
my $found_file = 0;
my @issue_list;

while (<CVSOUT>)
{
	if( $found_file == 1 )
	{
		push(@section,$_);
		
		if( /^=/ )
		{
			my $sline;
			foreach $sline (@section)
			{
				while ($sline =~ /([TP][DW].{0,2}[0-9]+)/g)
				{
					push(@issue_list, (join ',', $1,$section[0]) );
				}
			}
			$found_file = 0;
			@section = ();
		}
	
	}
	else
	{
		if (/Working file/)
		{
			s/(Working file: )(.*)(\r*\n)/\2/;
			push(@section,$_);
			$found_file = 1;
		}
	}
}
close(CVSOUT);

my %unique = map { $_ => 1 } @issue_list;
my @unique = keys %unique;


print join "\n", sort @unique;
