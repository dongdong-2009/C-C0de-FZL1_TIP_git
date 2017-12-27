/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/columns/AbstractAlarmColumn.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This interface is used to define an alarm list column. This interface is used by the alarm list control
 * and provides a flexible choice of columns shown and their order. This class is abstract and only implements
 * some of the common functionality of the alarm column.
 */

#if !defined(AFX_AbstractAlarmColumn_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
#define AFX_AbstractColumn_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/alarm_list_control/src/IAlarmColumn.h"
#include "bus/alarm/alarm_list_control/src/Sort.h"
#include "bus/alarm/alarm_list_control/src/IAlarmAction.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

namespace TA_Base_Bus
{
    // Forward declare
    class Sort;
    class IAlarmAction;

    class AbstractAlarmColumn : public IAlarmColumn
	{
	public:

        /**
         * Constructor
         *
         * @param int - The default width of the column
         * @param bool - Whether the column is currently enabled
         * @param Sort* - The ascending sort for the column. If the sort is NULL then the default sort will be used
         * @param Sort* - The descending sort for the column. If the sort is NULL then the default sort will be used
         * @param IAlarmAction* - The action to perform when the user double-clicks on the column. This can be NULL
         *                        signifying no action.
         */
        AbstractAlarmColumn(int width,
							SORT_COLUMN sortCol,
                            bool isEnabled = true,
                            IAlarmAction* doubleClickAction = NULL)
        : m_width(width),
          m_isEnabled(isEnabled),
          m_doubleClickAction(doubleClickAction),
		  m_sortCol(sortCol)
        {
           /* if (ascendingSort != NULL)
            {
                m_ascendingSort = *ascendingSort;
            }
            if (descendingSort != NULL)
            {
                m_descendingSort = *descendingSort;
            }
			*/
        }


        virtual ~AbstractAlarmColumn()
        {
        }


        /**
         * getColumnHeaderBitmapId
         *
         * Most columns don't use a bitmap so we'll implement some default behaviour here that can be
         * overwritten
         *
         * @return UINT - Some columns require a bitmap in the header instead of text. This method should
         *                return the resource id of a bitmap if the header should contain a bitmap. If not it
         *                should return 0.
         */
        virtual UINT getColumnHeaderBitmapId() const
        {
            return 0;
        }


       /**
         * addIconsToImageList
         *
         * This allows the column to create and add any icons to the list control image list. Most columns
         * will not have any images so this will do nothing. It can be overwritten if necessary.
         *
         * @param CImageList& - The image list used by the list control
         */
        virtual void addIconsToImageList(CImageList& imageList)
        {
        }


        /**
         * getColumnWidth
         *
         * @return long - The width of the column
         */
        virtual long getColumnWidth() const
        {
            return m_width;
        }


        /**
         * isColumnEnabled
         *
         * @return bool - Returns a boolean indicating whether the column is enabled or not
         */
        virtual bool isColumnEnabled() const
        {
            return m_isEnabled;
        }


        /**
         * getColumnAscendingSort
         *
         * @return const Sort& - The sort to use when the column is ascending
         */
        virtual const Sort& getColumnAscendingSort() const
        {
            return m_ascendingSort;
        }
		

        /**
         * getColumnDescendingSort
         *
         * @return const Sort& - The sort to use when the column is descending
         */
        virtual const Sort& getColumnDescendingSort() const
        {
            return m_descendingSort;
        }

        virtual const SORT_COLUMN getSortColumn() const
		{
			return m_sortCol;
		}


        /**
         * getColumnDoubleClickAction
         *
         * @return IAlarmAction* - The action to perform when the column is double-clicked.
         *                         This can be NULL indicating no action.
         */
        virtual IAlarmAction* getColumnDoubleClickAction() const
        {
            return m_doubleClickAction;
        }


        /**
         * setColumnWidth
         *
         * @param long - Set the width of the column.
         */
        virtual void setColumnWidth(long width)
        {
            m_width = width;
        }


        /**
         * setIsColumnEnabled
         *
         * @param bool - set whether the column is enabled or not. true = enabled
         */
        virtual void setIsColumnEnabled(bool isEnabled)
        {
            m_isEnabled = isEnabled;
        }


        /**
         * setColumnAscendingSort
         *
         * @param Sort - The sort to use when sorting the column ascending
         */
        virtual void setColumnAscendingSort(const Sort& sort)
        {
            m_ascendingSort = sort;
        }


        /**
         * setColumnDescendingSort
         *
         * @param Sort - The sort to use when sorting the column descending
         */
        virtual void setColumnDescendingSort(const Sort& sort)
        {
            m_descendingSort = sort;
        }


        /**
         * setColumnDoubleClickAction
         *
         * @param IAlarmAction* - The action to use when the user double clicks. This can be NULL indication
         *                        no action.
         */
        virtual void setColumnDoubleClickAction(IAlarmAction* action)
        {
            m_doubleClickAction = action;
        }


        /**
         * getItemIcon
         *
         * Most items will not have an icon so this returns -1. It can be overwritten if necessary.
         * 
         * @param AlarmDetailCorbaDef* - The alarm to retrieve information for
         *
         * @return long - An index into the list control image list. This should be -1 if no icon is required
         */
        virtual long getItemIcon(TA_Base_Bus::AlarmDataItem* alarm)
        {
            return -1;
        }


    private:
        /**
         * operator= - Hidden as not used
         */
        AbstractAlarmColumn& operator=(const AbstractAlarmColumn& rhs);

        /**
         * Copy constructor - Hidden as not used
         */
        AbstractAlarmColumn(const AbstractAlarmColumn&);


        int            m_width;             // This is the default width of the column
		bool		   m_isEnabled;         // This indicates if the column is enabled
        Sort           m_ascendingSort;     // This is the sort to use when this column is sorted ascending
        Sort           m_descendingSort;    // This is the sort ot use when this column is sorted descending
        IAlarmAction*  m_doubleClickAction; // This action is the one that will be performed when the user
                                            // double-clicks on an alarm in this column. If this is NULL
                                            // then no action is required to be performed.

		SORT_COLUMN m_sortCol;

	};
} //End of TA_Base_Bus


#endif // !defined(AFX_AbstractAlarmColumn_H__26C75118_4AC8_4594_BBE3_266FC53666A0__INCLUDED_)
