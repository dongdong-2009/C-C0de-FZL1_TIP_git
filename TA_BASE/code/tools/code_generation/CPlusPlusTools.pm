#
# C++Tools
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
#
# This module contains routines for automatically generating c++ include and source file stubs
#

package CPlusPlusTools;
require Exporter;

our @ISA = qw(Exporter);
our @EXPORT = qw(GenerateIncludeFile AddInterface);
our @VERSION = 1.00;

#
# GenerateIncludeFile
#
# Arguments: class name
# Return Value: 1 on correct execution, 0 on error
#
sub GenerateIncludeFile
{
# get name of include file / class to create
    my $className;
    if(@_ > 0)
    {
        $className = $_[0];
    }
    else
    {
        print "No class name specified!\n";
        return 0;
    }

# get namespaces (if they exist)
    my $namespace1 = "";
    my $namespace2 = "";
    if(@_ > 1)
    {
        $namespace1 = $_[1];
    }

# create new file
    if(open(file, "$className.h"))
    {
        print "Cannot create new file with name $className.h. File already exists\n";
        return 0;
    }
    open(file, ">> $className.h");

# add include guard
    my $guard = "$className" . "_H";
    print file "#ifndef $guard\n";
    print file "#define $guard\n";
    print file "\n";

# add file comment
    print file "/**\n";
    print file "  * The source code in this file is the property of \n";
    print file "  * Ripple Systems and is not for redistribution\n";
    print file "  * in any form.\n";
    print file "  *\n";
    print file "  * Source:   \$Source\$\n";
    print file "  * \@author:  <your name>\n";
    print file "  * \@version: \$Revision\$\n";
    print file "  *\n";
    print file "  * Last modification: \$Date\$\n";
    print file "  * Last modified by:  \$Author\$\n";
    print file "  * \n";
    print file "  * <description>\n";
    print file "  */\n\n";


    
# add class body (declaration, default constructor, destructor etc)
    $indent = "";
    if($namespace1 ne "")
    {
        print file "namespace $namespace1\n";
        print file "{\n";
        $indent = "\t";
    }
    print file "$indent" . "class $className\n";
    print file "$indent" . "{\n";
    print file "$indent" . "public:\n";
    PrintMethodHeader(file, $indent, "Constructor");
    print file "$indent" . "    $className();\n\n";
    PrintMethodHeader(file, $indent, "Destructor");
    print file "$indent" . "    virtual ~$className();\n\n";
    print file "$indent" . "\n";
    print file "$indent" . "    // Insert public methods here\n";
    print file "$indent" . "\n";
    print file "$indent" . "\n";
    print file "$indent" . "private:\n";
    print file "$indent" . "    // Insert private methods and variables here\n";
    print file "$indent" . "};\n";
    print file "$indent" . "\n";

    if($namespace1 ne "")
    {
        print file "}\n";
    }

# end include guard
    print file "#endif // $guard";

    close file;

    return 1;
}

#
# AddInterface
#
# Changes the header file <class name>.h so that the class inherits from the interface and declares all of its pure 
# virtual methods.
#
# Arguments: class name
#            interface name
# Return Value: 1 on correct execution, 0 on error
# 
sub AddInterface
{
    # globals. Should be members variables of a class
    @source_file_contents; 

    my $dest_class = @_[0];
    # separate the path from the class name for the source argument
    my @source_class_arguments = SeparatePathFromClass(@_[1]);
    my $source_directory = $source_class_arguments[0];
    my $source_class = $source_class_arguments[1];
    print "AddInterface() - Adding $source_class to $dest_class\n";

    # open interface source file
    my $source_file_name = "@_[1].h";
    unless(open(source_file, "$source_file_name"))
    {
        print "Cannot open destination file $source_file_name. Exiting";
        return 0;
    }
    #'slurp' in the source file
    @source_file_contents = <source_file>;

    # open the destination file
    my $destination_file_name = "$dest_class.h";
    unless(open(destination_file, "$destination_file_name"))
    {
        print "Cannot open file $destination_file_name. File does not exist";
        return 0; 
    }
    # 'slurp' in the destination file
    @destination_file_contents = <destination_file>;
    open(destination_file, "> $dest_class.h");

    # find the first non-comment, non-#include statement. put #include there
    my $i = 0;
    my $found_it = 0;
    while($i < @destination_file_contents and $found_it != 1)
    {
        if(length("@destination_file_contents[$i]") != 0 and 
           index("@destination_file_contents[$i]", "#") == -1 and 
           index("@destination_file_contents[$i]", "*") == -1 and
           index("@destination_file_contents[$i]", "*") == -1 and
           @destination_file_contents[$i] =~ "\s")
        {
            $found_it = 1;
            $i += -1;
        }
        $i += 1;
    }

    # add a #include 2 lines before the class declaration
    my $hash_include = "#include \"$source_class.h\"\n\n";
    print "hash_include = $hash_include\n";
    my @rest_of_dest_file = splice(@destination_file_contents, $i);
    push(@destination_file_contents, $hash_include);
    push(@destination_file_contents, @rest_of_dest_file);

    # find the class body
    $found_it = 0;
    while($i < @destination_file_contents and $found_it != 1)
    {
        if(index("@destination_file_contents[$i]", "class $dest_class") != -1)
        {
            my $j = index "@destination_file_contents[$i]", "class $dest_class";
            $found_it = 1;
            $i += -1;
        }
        $i += 1;
    }


    
    # find any namespaces that the interface belongs to
    my $namespace = GetNamespace(@source_file_contents);
    my $qualified_source_class = "$namespace" . "$source_class";
    # make the dest_class inherit from this interface
    AddInheritance(@destination_file_contents[$i], $qualified_source_class);

    # find destructor (will add interface's methods after this)
    $found_it = 0;
    while($i < @destination_file_contents and $found_it != 1)
    {
        if(index("@destination_file_contents[$i]", "virtual ~") != -1)
        {
            $found_it = 1;
        }
        $i += 1;
    }

    # add method signitures to dest
    if($found_it == 1)
    {
        my @interface_methods = GetVirtualMethods(@source_file_contents);
        my @rest_of_dest_file = splice(@destination_file_contents, $i);

        push(@destination_file_contents, "\n");
#        push(@destination_file_contents, "    // $source_class methods\n");
        push(@destination_file_contents, @interface_methods);
        push(@destination_file_contents, @rest_of_dest_file);
        print destination_file @destination_file_contents;    
    } 

    close destination_file;
    return 1;
}


#
# AddInheritance
#
# Modifies a class declaration such that the class inherits from another class
#
# Parameter: inheriter's class declaration (eg class X\n) [In/Out]
#            inheritee's name
# Return: none
#
sub AddInheritance
{
    $class_declaration = substr($_[0], 0, -1);
    if(index($class_declaration, ":") == -1)
    {
        $_[0] = "$class_declaration : public $_[1]\n";
    }
    else
    {
        $_[0] = "$class_declaration,\n        public $_[1]\n";
    }
}


#
# GetVirtualMethods
#
# Parses an global variable "@source_file_contents" and returns the signitures
# of all virtual methods in the variable. An array entry that contains a virtual
# method has the form: "virtual <method signiture> =0;"
#
# Parameter: none 
# Return: array {method_signiture1, method_signiture2, ... ,method_signitureN}
#
sub GetVirtualMethods
{
    my $i = 0;
    my $found_it = 0;
    while($i < @source_file_contents and $found_it != 1)
    {
        if(index("@source_file_contents[$i]", "~") != -1)
        {
            $found_it = 1;
        }
        $i += 1;
    }

    if($found_it == 0)
    {
        print "Couldn't find destructor\n";
        return;
    }

    my @virtual_methods;
    while($i < @source_file_contents)
    {
        # get all lines that are of the form "virtual <method sig> = 0;"
        if(index("@source_file_contents[$i]", "virtual") != -1 and index("@source_file_contents[$i]", "0;") != -1)
        {
            # get method header comment
            my @method_header = GetPrecedingComments($i);
            push(@virtual_methods, @method_header);

            # remove "= 0"
            my $equal_sign_index = index("@source_file_contents[$i]", " =");
            my $method = substr("@source_file_contents[$i]", 0, $equal_sign_index);
            $method = $method . ";\n\n";
            
            push(@virtual_methods, $method);
        }
        $i += 1;
    }

    return @virtual_methods;
}


#
# GetPrecedingComments
#
# Gets the all the commented lines preceding a particular line in the global @source_file_contents variable
#
# Parameter: index into the array to the particular line 
# Return: array of lines
#
sub GetPrecedingComments
{
    my $i = $_[0] - 1;
    my @comments;

#print "\ni = $i\n"; 
#print "source_file_contents[i] = @source_file_contents[$i]"; 
#    print "test1a = " . ((index "@source_file_contents[$i]", "/") != -1) ."\n";
#    print "test1b = " . ((index "@source_file_contents[$i]", "*") != -1) ."\n";
    while($i > -1 and (((index "@source_file_contents[$i]", "/") != -1) or ((index "@source_file_contents[$i]", "*") != -1)))
    {
        push(@comments, @source_file_contents[$i]);
        $i = $i - 1;
#        print "\ni = $i\n"; 
#        print "source_file_contents[i] = @source_file_contents[$i]"; 
#        print "test1a = " . ((index "@source_file_contents[$i]", "/") != -1) ."\n";
#        print "test1b = " . ((index "@source_file_contents[$i]", "*") != -1) ."\n";
    }

    # return a reverse order of the comments
    return reverse(@comments);
}


#
# SeparatePathFromClass
#
# Converts from <path><classname> to {<path>, <classname>} where the path is 
# delimited by forward slashes (unix style).
#
# Parameter: path/classname string
# Return: array {path, classname}
#
sub SeparatePathFromClass
{
    # arg = <path>/<classname>
    $delimitor_position = rindex($_[0], '/');
    my $path = substr($_[0], 0, $delimitor_position+1);
    my $class = substr($_[0], $delimitor_position+1);
    return "$path", "$class";
}


#
# PrintMethodHeader
#
# Adds method header to destination file
#
# Parameter: destination file, method name
# Return: none
#
sub PrintMethodHeader
{
    my $file = $_[0];
    my $indent = $_[1];
    my $method_name = $_[2];

    print file "$indent" . "  /**\n";
    print file "$indent" . "    * $method_name\n";
    print file "$indent" . "    * \n";
    print file "$indent" . "    * <description>\n";
    print file "$indent" . "    *\n";
    print file "$indent" . "    * \@return <return value> Optional\n";
    print file "$indent" . "    *\n";
    print file "$indent" . "    * \@param <parameters> Optional\n";
    print file "$indent" . "    *\n";
    print file "$indent" . "    * \@exception <exceptions> Optional ( C++ only )\n";
    print file "$indent" . "    *\n";
    print file "$indent" . "    */\n";
}


#
# GetNamespaces
#
# Gets the namespaces from the file
#
# Parameter: file
# Return: namespace string (i.e. "<namespace1>::<namespace2>::")
#
sub GetNamespace
{
    my @file = @_;
    my $i = 0;
    my $namespace = "";
    while($i < @file)
    {
        my $index = index("@file[$i]", "namespace");
        print "index = $index\n";

        if($index != -1 and (index "@file[$i]", "using") == -1)
        {
            my $length = length(@file[$i]) - $index - 11;
            print "length = $length\n";
            my $the_namespace = substr(@file[$i], $index+10, $length); 
            print "the_namespace = $the_namespace\n";
            $namespace .= "$the_namespace";
            $namespace .= "::";
        }
        $i += 1;
    }
    print "namespace = $namespace\n";
    return $namespace;
}

                
