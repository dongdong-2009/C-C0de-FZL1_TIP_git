#
# The source code in this file is the property of
# Ripple Systems and is not for redistribution
# in any form.
#
# Source:  $File: $ 
# Author:  Daniel Hoey
# Version: $Revision: $
#
# Last modification: $DateTime: $
# Last modified by:  $Author: $
#
# This script generates method stubs for a c++ source file from a c++ header file.
#
# Usage: source_gen.pl <class name>
#    note: a header file with the name <class name>.h must exist
#


# global 'member' variables
@header_file_contents;
$class_name;


# get name of include file / class 
if(@ARGV > 0)
{
    $class_name = $ARGV[0];
}
else
{
    print "No class name specified! Exiting";
    exit;
}

# open .h file
my $header_file_name = "$class_name.h";
unless(open(header_file, "$header_file_name"))
{
    print "Cannot open header file $header_file_name. Exiting";
    exit;
}
#'slurp' in the file
@header_file_contents = <header_file>;

# create the new source file
my $source_file_name = "$class_name.cpp";
if(open(source_file, "$source_file_name"))
{
    print "Cannot create new file with name $source_file_name. File already exists";
    exit; 
}
open(source_file, ">> $source_file_name");

# add #include 
print source_file "\n";
print source_file "#include \"$header_file_name\"\n";
print source_file "\n";
print source_file "\n";

#find the class body
my $i = 0;
my $found_it = 0;
while($i < @header_file_contents and $found_it != 1)
{
    if((index "@header_file_contents[$i]", "class $class_name") != -1)
    {
        my $j = index "@header_file_contents[$i]", "class $class_name";
        $found_it = 1;
    }
    $i += 1;
}

#find {
$found_it = 0;
while($i < @header_file_contents and $found_it != 1)
{
    if(index "@header_file_contents[$i]", "{" != -1)
    {
        $found_it = 1;
    }
    $i += 1;
}

#find public:
$found_it = 0;
while($i < @header_file_contents and $found_it != 1)
{
    if(index "@header_file_contents[$i]", "public:" != -1)
    {
        $found_it = 1;
    }
    $i += 1;
}

# add method stubs to source file
if($found_it != 0)
{
    $i = AddMethodStubs($i);
}

#find private:
$found_it = 0;
while($i < @header_file_contents and $found_it != 1)
{
    if(index "@header_file_contents[$i]", "private:" != -1)
    {
        $found_it = 1;
    }
    else
    {
        $i += 1;
    }
}

# add method stubs to source file
if($found_it != 0)
{
    $i = AddMethodStubs($i);
}



#
# sub routines
#

sub AddMethodStubs
{
    my $i = @_[0];

    while($i < @header_file_contents)
    {
        if(IsFunctionSigniture($header_file_contents[$i]))
        {
            # strip off leading whitespace
            my $method_signiture = substr($header_file_contents[$i], 0);
            $method_signiture =~ s/^\s*//;

            # add class name (ie ClassName::) to method signiture
            $method_signiture = AddClassName($class_name, $method_signiture);
            
            # get the other lines if the method has a multi line signiture
            while(index("$header_file_contents[$i]", ";") == -1)
            {
                $i += 1;
                $method_signiture = "$method_signiture" . "$header_file_contents[$i]";
            }
                    
            # replace ; with {}
            $method_signiture = ReplaceSemiColon($method_signiture);
            print source_file "$method_signiture\n\n\n";
        }
        $i += 1;
    }
    return $i;
}

sub IsFunctionSigniture
{
# if a line:
#   - is blank 
#   - has comments
#   - does not start with 4 spaces
#   - does not have ( 
# then it is not a method signiture
   
    my $current_line = "@_[0]"; 
#print "\nCheck line - $current_line\n";
#print "test1a = " . ((index "$current_line", "/") == -1) ."\n";
#print "test1b = " . ((index "$current_line", "*") == -1) ."\n";
#print "test3  = " . ((index "$current_line", "\n") != 0) . "\n";
#print "test4  = " . ((index "$current_line", "(") != -1) . "\n";

    my $test = ((index "$current_line", "/") == -1) * 
               ((index "$current_line", "*") == -1) * 
               ((index "$current_line", "\n") != 0) *
               ((index "$current_line", "(") != -1);

    return $test;
}

sub AddClassName
{
    my $method_signiture = "@_[1]";
    my $class_name = "@_[0]";

    # if method is virtual, then remove this
    my $virutal_index = index("$method_signiture", "virtual");
    if($virutal_index != -1)
    {
       $method_signiture = substr($method_signiture, $virutal_index+8); 
    }
    
    # if method is static, then remove this
    my $static_index = index("$method_signiture", "static");
    if($static_index != -1)
    {
       $method_signiture = substr($method_signiture, $static_index+7); 
    }

    if(index("$method_signiture", "$class_name") == 0)
    {
        return "$class_name" . "::" . "$method_signiture";
    }
    else
    {
        my $i = index "$method_signiture", " ";
        my $return_code = substr($method_signiture, 0, $i+1);
        my $remainder = substr($method_signiture, $i+1);
        return "$return_code" . "$class_name" . "::" . "$remainder";
    }
}

sub ReplaceSemiColon
{
    my $tmp = "@_[0]";
    chop $tmp; # remove newline
    chop $tmp; # remove ;
    $tmp =~ s/;//;
    return "$tmp" . "\n{\n}";
}
