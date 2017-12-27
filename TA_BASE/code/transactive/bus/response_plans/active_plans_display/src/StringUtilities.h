// StringUtilities.h : header file
//

#if !defined(AFX_STRINGUTILITIES_H__76D94DE4_8AF3_4A46_837C_AA8C51E52725__INCLUDED_)
#define AFX_STRINGUTILITIES_H__76D94DE4_8AF3_4A46_837C_AA8C51E52725__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/StringUtilities.h $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 18:34:29 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Collection of Static String related functions related to the display of Active
  * Plan Instances.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // StringUtilities

    class StringUtilities
    {
    public:
        // Support initialization language switch and support part of dynamic language switch
        static std::string RUNNING_PLAN_EXEC_STATE;
        static std::string OPERATOR_PAUSED_EXEC_STATE;
        static std::string STOPPED_PLAN_EXEC_STATE;
        static std::string PAUSED_PLAN_EXEC_STATE;
        static std::string DELAYED_PLAN_EXEC_STATE;
        static std::string COMPLETED_PLAN_EXEC_STATE;
        static std::string ABORTED_PLAN_EXEC_STATE;
        static std::string FAILED_PLAN_EXEC_STATE;
        static std::string PENDING_PLAN_EXEC_STATE;
        static std::string UNDEFINED_PLAN_EXEC_STATE;

    // Operations
    public:
		/** 
		  * extractPlanName
		  *
		  * Static method which accepts a string coresponding to full plan path and
		  * parses it to return string of just plan name
		  *
		  * @param const CString &planPath  -	Full plan path
		  *
		  *	@return CString			    	-	The Plan Name
		  *
		  */
        static CString extractPlanName(const CString &planPath);

		/** 
		  * extractCategoryPath
		  *
		  * Static method which accepts a string coresponding to full plan path and
		  * parses it to return string of just Category Path.
		  *
		  * @param const CString &planPath  -	Full plan path
		  *
		  *	@return CString				    -	The Category Path
		  *
		  */
        static CString extractCategoryPath(const CString &planPath);

		/** 
		  * convertToString [int]
		  *
		  * Static method which accepts an integer and returns the corresponding string
		  *
		  * @param const int value		-	integer to be converted
		  *
		  *	@return CString				-	The CString corresponding to the integer
		  *
		  */
        static CString convertToString(const int value);

		/** 
		  * convertToString [unsigned long]
		  *
		  * Static method which accepts an integer and returns the corresponding string
		  *
		  * @param const unsigned long value	-	unsigned long to be converted
		  *
		  *	@return CString			        	-	The CString corresponding to the unsigned long
		  *
		  */
        static CString convertToString(const unsigned long value);

		/** 
		  * convertToString [double]
		  *
		  * Static method which accepts a double and returns the corresponding string (to a 
		  * parameter specifed precision)
		  *
		  * @param const double value				-	double to be converted 
		  *
		  * @param const unsigned long precision	-	precision (No of DP) (default value = 0) 
		  *
		  *	@return CString				        	-	The CString corresponding to the unsigned long
		  *
		  */
        static CString convertToString(const double value, const unsigned long precision = 0);

		/** 
		  * convertToString [EActivePlanState]
		  *
		  * Static method which accepts an EActivePlanState (enum) and returns the corresponding 
		  * string
		  *
		  * @param const TA_Base_Core::EActivePlanState execState	-	EActivePlanState (enum) to be 
		  *											        		converted
		  *
		  *	@return CString							        	-	The CString corresponding to the 
		  *												        	EActivePlanState (enum)
		  *     
		  */
		static CString convertToString(const TA_Base_Core::EActivePlanState execState);

		/** 
		  * convertToString [time_t]
		  *
		  * Static method which accepts an EActivePlanState (enum) and returns the corresponding 
		  * string
		  *
		  * @param const time_t tmSrc	-	time to be 
		  *											        		converted
		  *
		  *	@return string							        	-	The string corresponding to the 
		  *												        	tmSrc
		  *     
		  */
        static std::string timeToString(const time_t tmSrc);

 		/** 
		  * compareNumbers
		  *
		  * Static method which compares two numbers represented as strings. It returns 0 if the numbers
          * are equal, -1 if the first number if less than the second number and +1 if the first number is
          * greater than the second number.
		  *
		  * @param const CString &str1	-	the first number string to compare 
		  *
		  * @param const CString &str2	-	the second number string to compare 
		  *
		  *	@return int			       	-	comparison result
		  *
		  */      
        static int compareNumbers(const CString &str1, const CString &str2);

 		/** 
		  * compareTimestamps
		  *
		  * Static method which compares two timestamps represented as strings. It returns 0 if the timestamps
          * are equal, -1 if the first timestamp if less than the second timestamp and +1 if the first timestamp
          * is greater than the second timestamp.
		  *
		  * @param const CString &str1	-	the first timestamp string to compare 
		  *
		  * @param const CString &str2	-	the second timestamp string to compare 
		  *
		  *	@return int			       	-	comparison result
		  *
		  */
        static int compareTimestamps(const CString &str1, const CString &str2);

 		/** 
		  * compareExecutionStates
		  *
		  * Static method which compares two plan execution states represented as strings. It returns 0 if 
          * the states are equal, -1 if the first state has priority over the second state, and +1 if the 
          * second state has priority over the first state. Execution state are prioritised as follows, from
          * highest to lowest: Running, Paused, Pending, Completed, Stopped, Aborted.
		  *
		  * @param const CString &str1	-	the first execution state string to compare 
		  *
		  * @param const CString &str2	-	the second execution state string to compare 
		  *
		  *	@return int			       	-	comparison result
		  *
		  */
        static int compareExecutionStates(const CString &str1, const CString &str2);

    
    // Attributes
    private:
        static std::map<std::string, int> s_executionStateMap; // map execution state string to priority 
    };
}

#endif // !defined(AFX_STRINGUTILITIES_H__76D94DE4_8AF3_4A46_837C_AA8C51E52725__INCLUDED_)
