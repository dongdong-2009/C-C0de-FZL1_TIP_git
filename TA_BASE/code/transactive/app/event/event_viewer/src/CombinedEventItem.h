/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventItem.h $
 * @author:  Bradley Cleaver
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2014/07/18 15:34:16 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * CombinedEventItem is an implementation of the DisplayItem abstract class. It contains the data for one row in
 * the list, and defines the standard ordering operator (<) for list sorting.
 */

#if !defined(_COMBINED_EVENT_ITEM_H_)
#define _COMBINED_EVENT_ITEM_H_

#include <map>

#include "app/event/event_viewer/EventListComponent/src/DisplayItem.h"
#include "core/data_access_interface/src/ICombinedEventData.h"

namespace EventConstants
{
	static const std::string RPARAM_MILLISECOND = "Millisecond";
};


class CombinedEventItem : public DisplayItem
{
public:

    /**
     * Constructor
     *
     * @param message The database object represented by this display object.
     */
    CombinedEventItem( TA_Base_Core::ICombinedEventData* const eventData );

	CombinedEventItem();

	std::string getCreateTimeStr();

	virtual std::string getEventID();
	virtual timeb getCreateTime();
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
    virtual const std::string getColumnData(const std::string& columnName);


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
    virtual const std::string getCompleteColumnData( const std::string& columnName );

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
/*    virtual bool operator<(DisplayItem& rhs);*/

	virtual bool compareTo(DisplayItem* rhs, std::string & sortColumn, bool sortAscending);

	/** 
	 * setEventCategory
	 *
	 * set m_eventCategory by the given value.
	 *
	 * @param ec, the value to be set to m_eventCategory
	 */
	virtual void setEventCategory( TA_Base_Core::EventCategory ec );

	/** 
	 * getEventCategory
	 * 
	 * return the value of m_eventCategory
	 *
	 */ 
	virtual TA_Base_Core::EventCategory getEventCategory();

	/** 
	 * getKey
	 *
	 * return the value of m_key
	 */ 
	virtual unsigned long getKey ();




private:
    // Disable copy constructor and assignment operator to stop copying.
    CombinedEventItem( const CombinedEventItem& );
    CombinedEventItem& operator=( const CombinedEventItem& );

	// These functions are to convert the various mms, dss and avl status from string stored in database
	// to defined identifier (ie. letter) for display
	const std::string getMmsStatusIdentifier(const std::string mmsStatus);
	const std::string getDssPlanStatusIdentifier(const std::string dssStatus);
	const std::string getAvlStatusIdentifier(const std::string avlStatus);
	const std::string getSevIdentifier(const std::string severityName);

    // Map containing column data. Maps ColumnName->Data
   // std::map<std::string,std::string> m_columnData;

    // Variables to store the time_t values of the time columns for sorting purposes
    timeb m_createTime;

	//std::string m_strCreateTime;

	TA_Base_Core::EventCategory m_eventCategory;
	static std::string dateFormatStr;
	static bool is_set_millisec;
	// to store pkey value (For ISCS, it's real pkey from DB; for ATS, it's a dinamic value set after each loading.)
	// This is for sorting purpose, when m_createtime has the same value, then check this 
	unsigned long m_key ; 

	std::string m_atsEventID;
	
	std::string m_eventId;

	TA_Base_Core::ICombinedEventData*  m_eventData;

};

#endif // !defined(_COMBINED_EVENT_ITEM_H_)
