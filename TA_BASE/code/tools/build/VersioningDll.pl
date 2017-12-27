# perl c:\versioningDll.pl 00 27 08 10
#!/usr/bin/perl

use File::Find;
# @ARGS -- command line arguments.

my $Path;
my $Mode;
my $Phase;
my $Major;
my $Minor;
my $Patch="";

if((@ARGV == 6)  || (@ARGV == 5))
{
    $Path = $ARGV[0];
	$Mode=$ARGV[1];
	$Phase=$ARGV[2];
	$Major=$ARGV[3];
	$Minor=$ARGV[4];
	if(@ARGV == 6)
	{
		$Patch=$ARGV[5];
	}

}
else
{
    print "Usage: perl -f 1.pl <path> mode phase major minor [patch]";
    exit;
}

find(\&edits, $Path);

sub edits() {

    if ( -f and /\.rc$/ ) {

        $file = $_;
		$f=$File::Find::name;
		print "\nFile: $f\n";

        open FILE, $file;
        @lines = <FILE>;
        close FILE;

        for $line ( @lines ) {
          
		  if ( $line =~ /FILEVERSION/  ) 
		  {
				
				$line = "FILEVERSION " . "$Mode, $Phase, $Major, $Minor";
				if ($Patch) 
			    {
					$line .= ", $Patch\n";
				}
				else
			    {
					$line .= "\n";
			    }
          }
		
		  if ($line =~ /VALUE \"FileVersion\"/) 
		  {
			  $line = "            VALUE \"FileVersion\", " . "\"$Mode, $Phase, $Major, $Minor";
			  if($Patch)
			  {
				  $line .= ", $Patch\\0\"\n";
			  }
			  else
			  {
				  $line .= "\\0\"\n";
			  }
		  }

		  if ($line =~ /VALUE \"ProductVersion\"/) 
		  {
			  $line = "            VALUE \"ProductVersion\", " . "\"$Mode, $Phase, $Major, $Minor";
			  if($Patch)
			  {
				  $line .= ", $Patch\\0\"\n";
			  }
			  else
			  {
				  $line .= "\\0\"\n";
			  }
		  }
        }
       open FILE, ">$file";
       print FILE @lines;
       close FILE;

    }
}
