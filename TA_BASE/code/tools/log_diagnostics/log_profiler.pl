#!/usr/bin/perl
# log_profiler.pl
# Functionality:
# 1. Reads trough all the transactive log files on the command line (or from STDIN).
# 2. Selects the Function Entry and Function Exit logs
# 3. Orders them by time
# 4. Attempts to fix bad logs (and prints a warning)
#	- Removes entry logs that are don't have an exit.
#	- Removes exit logs that appear before any entry logs.
# 5. Attempts to match the Entries with the Exits
# 6. Prints the result
#
# The results can be printed as a tree using the -t option before the list
# of log files.
#
# DOES NOT WORK FOR LOGS THAT SPAN MIDNIGHT

use strict;

my $use_tree = 0;

# process optional -t flag
if (@ARGV && $ARGV[0] eq '-t')
{
	$use_tree = 1;
	shift;
}

my @unsorted_lines;
my @lines;
my @unsorted_entries;
my @unsorted_exits;
my @entries;
my @exits;

# find all the entry and exit logs
while (<>)
{
	if( /Function Ent/ || /Function Ex/)
	{
		push( @unsorted_lines, $_ );
	}
}

# sort by time
@lines = sort bytime @unsorted_lines;

# fix missing exits
fixmissingexits(\@lines);

foreach (@lines)
{
	if( /Function Ent/ )
	{
		push( @entries, $_ );
	}

	if( /Function Ex/ )
	{
		push( @exits, $_ );
	}	
}

my $nidx = 0;
my $found_exit = 0;
my @fn_level;
my @exit_line;

while( $nidx < @entries )
{
	if( $entries[$nidx] eq "" )
	{
		$nidx++;
		next;
	}

	my $xidx = 0;
	while( ($found_exit == 0) && ($xidx < @exits) )
	{
		if( $exits[$xidx] eq "" )
		{
			$xidx++;
			next;
		}

		if( logsmatch($entries[$nidx],$exits[$xidx]) )
		{
			my $i = $#exit_line;
			my $is_sub = 0;
			while ( ($is_sub == 0) && ($i >= 0) )
			{
				if( $xidx < $exit_line[$i] )
				{
					push(@fn_level, $fn_level[$i]+1);
					$is_sub = 1;
				}
				$i--;
			}

			if( $is_sub == 0 )
			{
				push(@fn_level,0);
			}

			push( @exit_line, $xidx );

			$found_exit = 1;
		}
		else
		{
			$xidx++;
		}
	}
		
	if( $found_exit == 0 )
	{
		warn "unmatched: $entries[$nidx]";
	}
	else
	{
		my $fn_name = getname($entries[$nidx]);
		my $entry_time = gettime($entries[$nidx]);
		my $exit_time  = gettime($exits[$xidx]);

		chomp($fn_name);
		chomp($entry_time);
		chomp($exit_time);

		my $elapsed_time = diff_time($entry_time,$exit_time);
		my $i;

		if( $use_tree )
		{
			for( $i = 0; $i < $fn_level[$#fn_level]; $i++ )
			{
				print "|\t";
			}
			print "$fn_name\t($elapsed_time)\n";
		}
		else
		{
			print "$fn_name\t$elapsed_time\t($entry_time -> $exit_time)\n";
		}

		$found_exit = 0;
		$exits[$xidx] = "";
	}
	$nidx++;
}

sub diff_time
{
	my $h1  = $_[0];
	my $h2  = $_[1];
	my $m1  = $_[0];
	my $m2  = $_[1];
	my $s1  = $_[0];
	my $s2  = $_[1];
	my $ms1 = $_[0];
	my $ms2 = $_[1];

	$h1 =~ s/(\d+):.*/\1/;
	$h2 =~ s/(\d+):.*/\1/;

	$m1 =~ s/\d+:(\d+):.*/\1/;
	$m2 =~ s/\d+:(\d+):.*/\1/;

	$s1 =~ s/\d+:\d+:(\d+):.*/\1/;
	$s2 =~ s/\d+:\d+:(\d+):.*/\1/;

	$ms1 =~ s/\d+:\d+:\d+:(\d+).*/\1/;
	$ms2 =~ s/\d+:\d+:\d+:(\d+).*/\1/;

	my $diff;
	$diff = 60*($h2 - $h1);
	$diff = 60*($diff + $m2 - $m1);
	$diff = 1000*($diff + $s2 - $s1);
	$diff = $diff + $ms2 - $ms1;
	return $diff;
}

sub bytime
{
	my $ta = gettime($a);
	my $tb = gettime($b);
	return diff_time($tb,$ta)
}

sub logsmatch
{
	my $log;
	my @name;
	my @thread;
	
	foreach $log (@_)
	{
		push( @name  , getname($log) );
		push( @thread, getthread($log) );
	}
		
	if( ($thread[0] == $thread[1]) &&
		((lc($name[0]) eq lc($name[1])) || (substr(lc($name[0]),3) eq lc($name[1]))) )
	{
		return 1;
	}
	return 0;
}

sub getname
{
	my $rval = $_[0];
	$rval =~ s/(.*Function E\w+:)\s+(.*)/\2/;
	return $rval;
}

sub gettime
{
	my $rval = $_[0];
	$rval =~ s/(.* )(\d+:\d+:\d+:\d+)(.*)/\2/;
	return $rval;
}

sub getthread
{
	my $rval = $_[0];
	$rval =~ s/.*\[Thrd: (\d+)\].*/\1/;
	return $rval;
}

sub fixmissingexits
{
	my $lnref = $_[0];
	my $i;
	my $entry_found = 0;
	my $first_entry_found = 0;
	for( $i = 0; $i < @$lnref; $i++ )
	{
		if( !$first_entry_found )
		{
			if( $lnref->[$i] =~ /Function Ex/ )
			{
				$lnref->[$i] = "";
			}
		}
		if( $entry_found == 0 )
		{
			if( $lnref->[$i] =~ /Function Ent/ )
			{
				$entry_found = 1;
				$first_entry_found = 1;
			}
		}

		if( $entry_found == 1 )
		{
			my $j = $i+1;
			my $exit_found = 0;
			my $dup_entry_found = 0;

			while ( ($j < @$lnref) && ($exit_found == 0) && ($dup_entry_found == 0) )
			{
				if( $lnref->[$j] =~ /Function Ex/ )
				{

					if( logsmatch($lnref->[$i],$lnref->[$j]) )
					{
						$exit_found = 1;
						next;
					}
				}

				if( $lnref->[$j] =~ /Function Ent/ )
				{
					if( logsmatch($lnref->[$i],$lnref->[$j]) )
					{
						$dup_entry_found = 1;
						next;
					}
				}
				$j++;
			}

			if( ($dup_entry_found == 1) || ($dup_entry_found == 0 && $exit_found == 0) )
			{
				warn "bad log: $lnref->[$i]";
				$lnref->[$i] = "";
			}
			
			$entry_found = 0;
		}
	}
}
