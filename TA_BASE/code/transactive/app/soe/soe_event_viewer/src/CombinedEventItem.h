/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/CombinedEventItem.h $
 * @author:  Bradley Cleaver
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * CombinedEventItem is an implementation of the DisplayItem abstract class. It contains the data for one row in
 * the list, and defines the standard ordering operator (<) for list sorting.
 */

#if !defined(_COMBINED_EVENT_ITEM_H_)
#define _COMBINED_EVENT_ITEM_H_

#include <map>

#include "app/soe/soe_event_viewer/EventListComponent/src/DisplayItem.h"
#include "core/data_access_interface/src/ICombinedEventData.h"
#include "core/data_access_interface/soe/src/SOEEvent.h"

namespace EventConstants
{
	static const std::string RPARAM_MILLISECOND = "Millisecond";
};


class CombinedEventItem : public DisplayItem
{
public:
	 
	CombinedEventItem( TA_Base_Core::SOEEvent* const eventData );

	std::string getCreateTimeStr();
    /**
     * Destructor
     */
	virtual ~CombinedEventItem();

    /**
     * getColumnData
     * 
     * Retrieves the string representation of the data in the specified column in this object.
     * 
     * @param columnName The name of the column from which to retrieve the data
     *
     * @return The string representation of the data held in the specified column.
     */
    virtual const std::string& getColumnData(const std::string& columnName);

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
     * that the result of this operator is dependant upon a modifier set in a static variable. This modifier can reverse the
     * meaning of this operator. This is done as this operator is used in a number of std:: functions for sorting.
     *
     * @param rhs The item to compare this one to
     *
     * @return True if the this object is less than the passed in object, or if it is greater than and the modifier is set. False
     * if this object is greater than the passed in object, or if it is less than and the modifier is set.
     */
    virtual bool operator<(DisplayItem& rhs);

	//li Zhongyan add
	//virtual int compareitem(DisplayItem& rhs);
	void setrPKey(double rPKey);
	double getrPKey();

private:
    // Disable copy constructor and assignment operator to stop copying.
	CombinedEventItem( const CombinedEventItem& );
    CombinedEventItem& operator=( const CombinedEventItem& );

	 

    // Map containing column data. Maps ColumnName->Data
    std::map<std::string,std::string> m_columnData;

    // Variables to store the time_t values of the time columns for sorting purposes
    timeb m_createTime;

	//li Zhongyan add
	double m_rPKey;

	std::string m_strCreateTime;
};

#endif // !defined(_COMBINED_EVENT_ITEM_H_)
