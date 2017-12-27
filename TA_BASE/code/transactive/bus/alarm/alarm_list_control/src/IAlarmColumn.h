/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/IAlarmColumn.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This interface is used to define an alarm list column. This interface is used by the alarm list control
 * and provides a flexible choice of columns shown and their order
 */

#if !defined(AFX_IAlarmColumn_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_IAlarmColumn_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AlarmVectorWrap.h"

// namespace TA_Base_Core
// {
//     struct AlarmDetailCorbaDef;
// }

namespace TA_Base_Bus
{
    // Forward declare
    class Sort;
    class IAlarmAction;
    class DatabaseCache;

    class IAlarmColumn
	{
	public:

        virtual ~IAlarmColumn(){};

        /**
         * getColumnIdentifier
         *
         * @return long - A unique identifier for this column. This is defined in the file ColumnIdentifiers.h
         */
        virtual long getColumnIdentifier() const =0;


        /**
         * getColumnHeaderText
         *
         * @return CString - Returns the column header text
         */
        virtual CString getColumnHeaderText() const =0;


        /**
         * getColumnHeaderBitmapId
         *
         * @return UINT - Some columns require a bitmap in the header instead of text. This method should
         *                return the resource id of a bitmap if the header should contain a bitmap. If not it
         *                should return 0.
         */
        virtual UINT getColumnHeaderBitmapId() const =0;
        

        /**
         * getColumnWidth
         *
         * @return long - The width of the column
         */
        virtual long getColumnWidth() const =0;
        

        /**
         * isColumnEnabled
         *
         * @return bool - Returns a boolean indicating whether the column is enabled or not
         */
        virtual bool isColumnEnabled() const =0;
        

        /**
         * getColumnAscendingSort
         *
         * @return const Sort& - The sort to use when the column is ascending
         */
        virtual const Sort& getColumnAscendingSort() const =0;
        
		virtual const SORT_COLUMN getSortColumn() const =0;

        /**
         * getColumnDescendingSort
         *
         * @return const Sort& - The sort to use when the column is descending
         */
        virtual const Sort& getColumnDescendingSort() const =0;
        

        /**
         * getColumnDoubleClickAction
         *
         * @return IAlarmAction* - The action to perform when the column is double-clicked.
         *                         This can be NULL indicating no action.
         */
        virtual IAlarmAction* getColumnDoubleClickAction() const =0;
        

        /**
         * setColumnWidth
         *
         * @param long - Set the width of the column.
         */
        virtual void setColumnWidth(long width) =0;
        
        
        /**
         * setIsColumnEnabled
         *
         * @param bool - set whether the column is enabled or not. true = enabled
         */
        virtual void setIsColumnEnabled(bool isEnabled) =0;
        

        /**
         * setColumnAscendingSort
         *
         * @param Sort - The sort to use when sorting the column ascending
         */
        virtual void setColumnAscendingSort(const Sort& sort) =0;
        

        /**
         * setColumnDescendingSort
         *
         * @param Sort - The sort to use when sorting the column descending
         */
        virtual void setColumnDescendingSort(const Sort& sort) =0;
        

        /**
         * setColumnDoubleClickAction
         *
         * @param IAlarmAction* - The action to use when the user double clicks. This can be NULL indication
         *                        no action.
         */
        virtual void setColumnDoubleClickAction(IAlarmAction* action) =0;


        /**
         * addIconsToImageList
         *
         * This allows the column to create and add any icons to the list control image list.
         *
         * @param CImageList& - The image list used by the list control
         */
        virtual void addIconsToImageList(CImageList& imageList) =0;


        /**
         * getItemText
         *
         * @param AlarmDetailCorbaDef* - The alarm to retrieve information for
         * @param DatabaseCache - This object can be used to retrieve database information if necessary
         *
         * @return CString - This returns the text to display in the column for the specified alarm.
         */
        //virtual const char* getItemText(TA_Base_Core::AlarmItem* displayItem) =0;
        virtual const std::string getItemText(AlarmVectorWrap* pAlarmWrap, unsigned long ulIndex) =0; //++Noel


        /**
         * getItemIcon
         *
         * @param AlarmDetailCorbaDef* - The alarm to retrieve information for
         *
         * @return long - An index into the list control image list. This should be -1 if no icon is required
         */
		virtual long getItemIcon(TA_Base_Bus::AlarmDataItem* alarm) =0;

	};
} //End of TA_Base_Bus


#endif // !defined(AFX_IAlarmColumn_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
