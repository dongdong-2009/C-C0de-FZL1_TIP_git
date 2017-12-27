/**
* The source code in this file is the property of
* MI Consulting Group (Australia) and is not for
* redistribution in any form.
*
# Source:  $File: //depot/TA_Common_V1_TIP/transactive/core/versioning/src/Version.cpp $ 
# Author:  Ripple
# Version: $Revision: #2 $
#
# Last modification: $DateTime: 2015/02/06 17:22:20 $
# Last modified by:  $Author: limin.zhu $
*
* Main source file for Version information
*
* NOTE: The variables declared externall in Version.cpp
* are defined in the file VersionGen.cpp which is located in
* all component src directories. The variables are resolved
* at link time.
*/

#include "Version.h"
#include "SimpleIni.h" // TODO: <boost/property_tree/ini_parser.hpp>
#include <iostream>


namespace TA_Base_Core
{
    // These variables are all declared external so they
    // will be picked up on link time with individual applications
    extern char buildVersion[];
    extern char buildDate[];
    extern char componentName[];
    extern char componentVersion[];
    extern int libraryArraySize;
    extern char* libraryArray[];
    extern int headerArraySize;
    extern char* headerArray[];
    extern int otherArraySize;
    extern char* otherArray[];

    /**
      * displayVersionInfo()
      *
      * This is a static method that will print out versionInformation to stdout.
      */
    void Version::displayVersionInfo()
    {
        // try to get project name from a configuration file
        const char* projectName = NULL;
        CSimpleIniA ini;
        SI_Error rc = ini.LoadFile( "/u01/transactive/bin/transactive.ini" );

        if ( SI_OK == rc )
        {
            projectName = ini.GetValue( "Project", "name" );
        }

        if ( projectName != NULL )
        {
            std::cout << projectName << std::endl;
        }
        else
        {
            // Print out the header information (copyright information and such)
            for( int j = 0; j < headerArraySize; j++)
            {
                std::cout << headerArray[j] << std::endl;
            }
        }

        std::cout << std::endl;

        // Print out all the details relevant to this particular application
        std::cout << "Release Number:\t\t" << buildVersion << std::endl;
        std::cout << "Build Time:\t\t" << buildDate << std::endl;
        std::cout << "Component:\t\t" << componentName << std::endl;
        std::cout << "Component Version:\t" << componentVersion << std::endl << std::endl;

        // Print out all the relevant library information
        std::cout << "Component Library Information:" << std::endl;
        for (int i = 0; i < libraryArraySize; i++)
        {
            std::cout << "\t" << libraryArray[i] << std::endl;
        }
        std::cout << std::endl;

        // Print out any other information for this application
        std::cout << "Other Component Information:" << std::endl;
        for(  int k = 0; k < otherArraySize; k++)
        {
            std::cout << "\t" << otherArray[k] << std::endl;
        }
    }
}
