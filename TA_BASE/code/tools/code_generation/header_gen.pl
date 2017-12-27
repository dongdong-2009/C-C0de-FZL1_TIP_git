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
# This script generates a basic c++ header file for a giving class 
#
# Usage: header_gen.pl <class name> [<namespace1>]
#
#

use CPlusPlusTools;

GenerateIncludeFile(@ARGV);
