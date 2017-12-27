/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/ColumnSourceTimestamp.h $
 * @author:  Karen Graham
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2015/01/22 17:50:12 $
 * Last modified by:  $Author: Noel $
 * 
 * This class provides all the information for the source timestamp column.
 */

#if !defined(AFX_ColumnSourceTimestamp_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_ColumnSourceTimestamp_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/alarm_list_control/src/columns/AbstractAlarmColumn.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnIdentifiers.h"

namespace TA_Base_Bus
{

    class ColumnSourceTimestamp : public AbstractAlarmColumn
	{
	public:

        
        /**
         * Constructor
         *
         * @param int - The width of the column
         * @param bool - Whether the column is currently enabled
         * @param Sort* - The ascending sort for the column. If the sort is NULL then the default sort will be used
         * @param Sort* - The descending sort for the column. If the sort is NULL then the default sort will be used
         * @param IAlarmAction* - The action to perform when the user double-clicks on the column. This can be NULL
         *                        signifying no action.
         */
        ColumnSourceTimestamp(int width,
                         bool isEnabled,
                         SORT_COLUMN sortCol,
                         IAlarmAction* doubleClickAction);


        virtual ~ColumnSourceTimestamp()
        {
        };


        /**
         * getColumnIdentifier
         *
         * @return long - A unique identifier for this column. This is defined in the file ColumnIdentifiers.h
         */
        virtual long getColumnIdentifier() const
        {
            return COL_SOURCETIMESTAMP;
        }


        /**
         * getColumnHeaderText
         *
         * @return CString - Returns the column header text
         */
        virtual CString getColumnHeaderText() const
        {
            return "日期/时间";
        }


        /**
         * getItemText
         *
         * @param AlarmDetailCorbaDef* - The alarm to retrieve information for
         * @param DatabaseCache - This object can be used to retrieve database information if necessary
         *
         * @return CString - This returns the text to display in the column for the specified alarm.
         */
		//++Noel
        //virtual const char* getItemText(TA_Base_Core::AlarmItem* displayItem);
		virtual const std::string getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex);

        /**
         * setTimeFormat
		 *
		 * Sets the time format used by this column. This function asserts that the time
		 * format is not an empty string, but otherwise it is up to the client to ensure
		 * that the format passed is valid. The default format is "%d-%b-%y %H:%M:%S".
         *
         * @param const CString& - The format string as per the strftime command
         */
        void setTimeFormat(const CString& formatStr);


    private:
        /**
         * operator= - Hidden as not used
         */
		const char* timeFromater(unsigned long tm);

        ColumnSourceTimestamp& operator=(const ColumnSourceTimestamp& rhs);

        /**
         * Copy constructor - Hidden as not used
         */
        ColumnSourceTimestamp(const ColumnSourceTimestamp&);

		// String used to format the date/time as per the strftime command
		CString m_formatStr;
		
		char m_timebuf[32];
	};
} //End of TA_Base_Bus


#endif // !defined(AFX_ColumnSourceTimestamp_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
