#!/bin/sh

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

perl -I /cygdrive/c/9999_Sandbox/code/tools/code_generation c:/9999_Sandbox/code/tools/code_generation/source_gen.pl $1

