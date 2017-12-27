/**
* The source code in this file is the property of 
* ST Electronics and is not for redistribution
* in any form.
*
* Source:    
* @author:  Noel R. Tajanlangit
* @version: $Revision: #0.9.0.0 $
*
* This file provides the interface definition for C952-C955 Alarm interface 
* specific to show related schematic from C952 library to C955 bridge module.
*  
*/

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ISCSALARMACTIONBRIDGE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ISCSALARMACTIONBRIDGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef __ISCSALARMACTIONBRIDGE_H__
#define __ISCSALARMACTIONBRIDGE_H__

#ifdef ISCSALARMACTIONBRIDGE_EXPORTS
#define ISCSALARMACTIONBRIDGE_API __declspec(dllexport)
#else
#define ISCSALARMACTIONBRIDGE_API __declspec(dllimport)
#endif

#include <string>

namespace ISCSAlarmActionBridge
{
	//this struct includes alarm related schematic information.
	struct  SchematicDisplayInfo
	{
		// AlarmID for the alarm with related schematic
		std::string alarmID;

		// the graphic element
		std::string schematic;    

		// The x co-ordinated of the left side of alarm manager/Banner
		// -1 means ignore the parameter
		long leftPosition;        
	};

	// These are the values return by the call to showSchematic 
	typedef enum ShowSchematicResult
	{
		// Success with no Errors
		RESULT_OK = 0,
		
		// Error Invalid arguments passed
		RESULT_INVALID_ARG = -1,

		// Error Failed to load schematic diagram
		RESULT_FAILED_TO_LOAD = -2

	}ShowSchematicResult;
	/**
	* showSchematic
	*
	* This function calls to show schematic. 
	**
	* @param	SchematicDisplayInfo & schematic display information
	*
	*/   
	ISCSALARMACTIONBRIDGE_API ShowSchematicResult showSchematic(const SchematicDisplayInfo& schematicInfo);
}

#endif // __ISCSALARMACTIONBRIDGE_H__