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

# Check MakeMaker.pm for more good ideas
package BuildSystem;

require Exporter;
use Carp ();
use strict;
use Cwd;

use vars qw(
        @ISA @EXPORT @EXPORT_OK $AUTOLOAD
        $ISA_TTY
        %makelist $hashsize %targethash %dirvarnames %buildenvlist $tagName $rootDir
        );

#
# Set up the inheritance before we pull in the MM_* packages, because they
# import variables and functions from here
#
@ISA = qw( Exporter );
@EXPORT = qw( &Make &BuildEnvironment &ProjectInformation);
@EXPORT_OK = qw( %makelist $hashsize %targethash %dirvarnames %buildenvlist $tagName $rootDir);

sub BuildSystem::Make;
sub BuildSystem::BuildEnvironment;
sub BuildSystem::ProjectInformation;
$hashsize = 1;

sub warnhandler {
    $_[0] =~ /^Use of uninitialized value/ && return;
    $_[0] =~ /used only once/ && return;
    $_[0] =~ /^Subroutine\s+[\w:]+\s+redefined/ && return;
    warn @_;
}

sub ProjectInformation
{
    local $SIG{__WARN__} = \&warnhandler;
    # FIXME - Surely all of this can be done better?

    my %templist = @_;
    # Get the name and base directory
    $tagName = $templist{TagName};
    $rootDir = $templist{BaseDirectory};
    
    # Get an absolute path out of a possibly relative one
    my $runDir = cwd();
    chdir $rootDir;
    $rootDir = cwd();
    chdir $runDir;
    
}


sub Make
{
    Carp::croak "BuildSystem::Make: Need even number of args" if @_ % 2;
    local $SIG{__WARN__} = \&warnhandler;
    # FIXME - Surely all of this can be done better?

    my %templist = @_;
    my $component = $templist{Component};

    my $key;
    foreach $key ( keys %templist )
    {
        # The BuildOn key is special
        if( $key eq "BuildOn" )
        {
            if( ref( \$templist{$key} ) eq "SCALAR" )
            {
                # If it's just a SCALAR (string), turn it into a hash.
                $makelist{$component}{$key} = { $templist{$key} => "" };
            }
            else
            {
                $makelist{$component}{$key} = $templist{$key};
            }
        }
        else
        {
            $makelist{$component}{$key} = $templist{$key}
                if ( $key ne "Component" ); # Exclude Component
        }
    }
    $makelist{$component}{Order} = $hashsize;
    $hashsize++;
}

sub BuildEnvironment
{
    Carp::croak "BuildSystem::BuildEnvironment: Need even number of args"
        if @_ % 2;
    local $SIG{__WARN__} = \&warnhandler;

    my %templist = @_;
    my $name = $templist{Name};

    my $key;
    foreach $key ( keys %templist )
    {
        $buildenvlist{$name}{$key} = $templist{$key};
    }
}

1;
