/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/DisplayItem.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * DisplayItem is the base class for data items that can be displayed by the Event List Component. The application using
 * the Event List Component should define a sub-class of DisplayItem for each type of data it wishes to display in the 
 * Event List Component.
 */

#if !defined(AFX_DISPLAYITEM_H__8289FA18_7603_46C2_91AB_6A4BF743AEE9__INCLUDED_)
#define AFX_DISPLAYITEM_H__8289FA18_7603_46C2_91AB_6A4BF743AEE9__INCLUDED_

#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DisplayItem  
{
public:
    /**
     * constructor
     */
    DisplayItem();

    /**
     * destructor
     */
	virtual ~DisplayItem();

    /**
     * getColumnData
     * 
     * Retrieves the string representation of the data in the specified column in this object.
     * 
     * @param columnName The name of the column from which to retrieve the data
     *
     * @return The string representation of the data held in the specified column.
     *         This is often a summarised form of the real data.
     */
    virtual const std::string& getColumnData(const std::string& columnName) =0;

    /**
     * getCompleteColumnData
     *
     * getColumnData will return the text that should be displayed in a particular
     * column. However, the item still needs the ability to allow more details
     * information to be retrieved. For example, the 'C' column in the
     * CombinedEventFilter will only display a C in the list control. However the
     * double click action still needs the ability to get the entire comment.
     *
     * @param columnName The name of the column from which to retrieve the data.
     *
     * @return std::string& The real data related to the column.
     */
    virtual const std::string& getCompleteColumnData( const std::string& columnName );

    /**
     * operator<
     *
     * Determines if this display item is "dependently less than" the one passed to the function. What that means is
     * that the result of this operator is dependant upon a modifier set in the s_sortAscending variable. This modifier can 
     * reverse the meaning of this operator. This is done as this operator is used in a number of std:: functions for sorting.
     *
     * @param rhs The item to compare this one to
     *
     * @return True if the this object is less than the passed in object, or if it is greater than and s_sortAscending is false. 
     * False if this object is greater than the passed in object, or if it is less than and s_sortAscending is false.
     */
    virtual bool operator<(DisplayItem& rhs) =0;

    /**
     * setSortInfo
     *
     * Sets the details of any sort carried out, specifing which column to sort on, and the order in which to sort it.
     *
     * @param sortColumn The column on which to perform the sort operation
     * @param sortAscending Boolean set to true if the column is to be sorted in ascending order, and false if descending.
     */
    static void setSortInfo(const std::string& sortColumn,const bool sortAscending);

    // Public static variables to hold sort order. Allows access to the sorting data from any class as if a property.
    static std::string s_sortColumn;
    static bool s_sortAscending;

private:
    DisplayItem( const DisplayItem& );
    DisplayItem& operator=( const DisplayItem& );
};

#endif // !defined(AFX_DISPLAYITEM_H__8289FA18_7603_46C2_91AB_6A4BF743AEE9__INCLUDED_)
