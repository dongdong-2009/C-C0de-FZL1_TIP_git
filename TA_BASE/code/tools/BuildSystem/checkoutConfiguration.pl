#!/usr/local/bin/perl -w
#
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
# checkoutConfiguration.pl
# checks out the configuration branch from the 9999 and 4669 repositories
# then removes all CVS folders
#
use strict;

# Global variables to be used in most functions.
my $environment = $^O;


my $usage = "USAGE: perl checkoutConfiguration.pl <Tag>\n";
$usage   .= "<Tag> => Checkout the <Tag> branch.\n";

#############################################################################
#                                                                           #
#                             Main Program                                  #
#                                                                           #
#############################################################################
if (@ARGV == 0)
{
	Log("### No checkout version specified.");
	die $usage;
}
my $version = shift(@ARGV);

# Remove the existing config directory
print "==> Removing existing config directory\n";
system("rm -rf config_4669 config_9999");

# set environment variables
# checkout from 9999
{
	print "==> Checking out config from 9999 with tag $version\n";
	local $ENV{CVSROOT}=":pserver:itsbuild\@cvs:/cvs/1TS-9999-T";
	system("cvs -q checkout -d config_9999 -r $version config");
}

# set environment variables
# checkout from 4669
{
	print "==> Checking out config from 4669 with tag $version\n";
	local $ENV{CVSROOT}=":pserver:itsbuild\@cvs:/cvs/1TS-4669-F";
	system("cvs -q checkout -d config_4669 -r $version config");
}

