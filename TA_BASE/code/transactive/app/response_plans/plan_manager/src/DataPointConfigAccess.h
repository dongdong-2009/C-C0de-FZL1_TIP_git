//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointConfigAccess.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// This is a utility class. It provides methods for determining a data point's type and,
// where applicable, configured states.


#if !defined(AFX_DATAPOINTCONFIGACCESS_H__CC39C293_505A_4303_AC2B_D32B48FD27BA__INCLUDED_)
#define AFX_DATAPOINTCONFIGACCESS_H__CC39C293_505A_4303_AC2B_D32B48FD27BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointConfigAccess

    class DataPointConfigAccess
    {
    // Types
    public:
        typedef enum
                {
                    DPT_UNDEFINED,
                    DPT_UNSIGNED_INT16,    // 16-bit unsigned integer
                    DPT_SIGNED_INT16_2C,   // 16-bit signed integer (2's complement)
                    DPT_SIGNED_INT16_SM,   // 16-bit signed integer (sign + magnitude)
                    DPT_UNSIGNED_INT32,    // 32-bit unsigned integer
                    DPT_SIGNED_INT32_2C,   // 32-bit signed integer (2's complement)
                    DPT_SIGNED_INT32_SM,   // 32-bit signed integer (sign + magnitude)
                    DPT_IEEE_FLOAT32,      // Single precision float
                    DPT_BOOLEAN,           // Digital
                    DPT_TEXT,              // Text
                    DPT_DERIVED_IN,        // Derived input
                    DPT_COMBINED_OUT       // Combined output
                }
                EDataPointType;

        typedef struct StateValue
                {
                    StateValue() {}
                    StateValue(const StateValue &sv)
                    {
                        operator=(sv);
                    }

                    StateValue &operator=(const StateValue &sv)
                    {
                        if (this != &sv)
                        {
                            state = sv.state;
                            description = sv.description;
                        }
                        return *this;
                    }

                    int state;
                    std::string description;
                };

        typedef std::vector<StateValue> StateValueList;


    // Operations
    public:
        static EDataPointType getDataPointType(std::string dataPointName);
        static StateValueList getDataPointStates(std::string dataPointName);
    };
}

#endif // !defined(AFX_DATAPOINTCONFIGACCESS_H__CC39C293_505A_4303_AC2B_D32B48FD27BA__INCLUDED_)
