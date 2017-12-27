#ifndef DATA_NODE_STATES_H
#define DATA_NODE_STATES_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datanode_library/src/DataNodeStates.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/21 14:56:06 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Common defines and functions for handling DataNode
 * tag/inhibit states
 *
 */

#include <string>

namespace TA_Base_Bus
{
    namespace
    {
        //
        // State parameter value constants
        //

        static const std::string DN_NOT_TAGGED =            "NOT_TAGGED";
        static const std::string DN_TAGGED =                "TAGGED";
        static const std::string DN_TAGGED_PTW =            "TAGGED_PTW";
        static const std::string DN_NOT_OUTPUT_INHIBITED =  "DN_NOT_OUTPUT_INHIBITED";
        static const std::string DN_CONTROL_INHIBIT =       "DN_CONTROL_INHIBIT";
        static const std::string DN_NOT_INPUT_INHIBITED =   "DN_NOT_INPUT_INHIBITED";
        static const std::string DN_SCAN_INHIBIT =          "DN_SCAN_INHIBIT";
        static const std::string DN_ALARM_INHIBIT =         "DN_ALARM_INHIBIT";
        static const std::string DN_MMS_INHIBIT =           "DN_MMS_INHIBIT";

        
        //
        // Conversion functions for parameter string <--> CORBA struct member
        //

        static TA_Base_Bus::ETagState stringToTagState(const std::string& tagString)
        {
            if (tagString == DN_TAGGED)
            {
                return TA_Base_Bus::Tagged;
            }
            if (tagString == DN_TAGGED_PTW)
            {
                return TA_Base_Bus::TaggedPtw;
            }
            if (tagString == DN_NOT_TAGGED)
            {
                return TA_Base_Bus::NotTagged;
            }

            return TA_Base_Bus::NotTagged;
        }


        static std::string tagStateToString(TA_Base_Bus::ETagState tagState)
        {
            switch (tagState)
            {
                case TA_Base_Bus::Tagged:
                    return DN_TAGGED;

                case TA_Base_Bus::TaggedPtw:
                    return DN_TAGGED_PTW;

                case TA_Base_Bus::NotTagged:

                default:
                    return DN_NOT_TAGGED;
            }
        }


        static TA_Base_Bus::EOutputInhibitState stringToTagOutputInhibitState(const std::string& tagInhibitString)
        {
            if (tagInhibitString == DN_NOT_OUTPUT_INHIBITED)
            {
                return TA_Base_Bus::NoOutputInhibit;
            }
            if (tagInhibitString == DN_CONTROL_INHIBIT)
            {
                return TA_Base_Bus::InhibitControl;
            }

            return TA_Base_Bus::NoOutputInhibit;
        }


        static TA_Base_Bus::EInputInhibitState stringToTagInputInhibitState(std::string tagInhibitString)
        {
            if (tagInhibitString == DN_NOT_INPUT_INHIBITED)
            {
                return TA_Base_Bus::NoInputInhibit;
            }
            if (tagInhibitString == DN_SCAN_INHIBIT)
            {
                return TA_Base_Bus::InhibitScan;
            }
            if (tagInhibitString == DN_ALARM_INHIBIT)
            {
                return TA_Base_Bus::InhibitAlarm;
            }
            if (tagInhibitString == DN_MMS_INHIBIT)
            {
                return TA_Base_Bus::InhibitMmsAlarm;
            }

            return TA_Base_Bus::NoInputInhibit;
        }
        

        static std::string tagOutputInhibitStateToString(TA_Base_Bus::EOutputInhibitState tagInhibitState)
        {
            switch (tagInhibitState)
            {
                case TA_Base_Bus::InhibitControl:
                    return DN_CONTROL_INHIBIT;

                case TA_Base_Bus::NoOutputInhibit:
                default:
                    return DN_NOT_OUTPUT_INHIBITED;
            }
        }


        static std::string tagInputInhibitStateToString(TA_Base_Bus::EInputInhibitState tagInhibitState)
        {
            switch (tagInhibitState)
            {
                case TA_Base_Bus::InhibitScan:
                    return DN_SCAN_INHIBIT;

                case TA_Base_Bus::InhibitAlarm:
                    return DN_ALARM_INHIBIT;

                case TA_Base_Bus::InhibitMmsAlarm:
                    return DN_MMS_INHIBIT;

                case TA_Base_Bus::NoInputInhibit:
                default:
                    return DN_NOT_INPUT_INHIBITED;
            }
        }
        
    }
}

#endif // DATA_NODE_STATES_H
