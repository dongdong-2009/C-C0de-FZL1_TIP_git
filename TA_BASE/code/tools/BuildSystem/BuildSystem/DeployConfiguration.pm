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

# deploy configuration module
#
#
#
#

require Exporter;
use Carp ();
use strict;
use Cwd;

use vars qw(
        @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS
        $ISA_TTY
        %DeployList
	$counter
        );

@ISA = qw( Exporter );
@EXPORT = qw(&Deploy %DeployList);
@EXPORT_OK = qw();
%EXPORT_TAGS = ( DEFAULT => [qw( &Deploy %DeployList)]);
$counter=0;

sub DeployConfiguration::Deploy;

sub warnhandler {
    $_[0] =~ /^Use of uninitialized value/ && return;
    $_[0] =~ /used only once/ && return;
    $_[0] =~ /^Subroutine\s+[\w:]+\s+redefined/ && return;
    warn @_;
}

sub Deploy
{
	# Dont ask me what this does
	# something to do with suppressing warnings maybe?
	local $SIG{__WARN__} = \&warnhandler;
	
	# parameters
	my %templist = @_;

	# get the values
	$DeployList{$counter}{Search} = $templist{Search};
	$DeployList{$counter}{FileNameContains} = $templist{FileNameContains};
	$DeployList{$counter}{DirectoryNameContains}=$templist{DirectoryNameContains};
	$DeployList{$counter}{CopyTo} = $templist{CopyTo};
	$counter++;
}


1;
