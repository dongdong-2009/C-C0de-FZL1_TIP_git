/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/ListColumnsBase.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma once

#include <string>

/// NOTE (to avoid compiler errors): 
/// 1) The derived class must defined the 'EColumn' enum
/// where COL_MAX defines max column index
/// 2) Must define a static function getFieldFromRecord
/// which extracts the row data from the objects of type 'RecordType'
template <class RecordType, class DerivedClass>
class ListColumnsBase
{
public:    
    
    enum ECompareType
    {
        COMPARE_STRING,
        COMPARE_INTEGER
    };

    virtual ~ListColumnsBase() {}

    /**
     * setupListControlColumns
     *
     * Set up the input list control to contain all the locally contained columns
     *
     * @param list the list to be set up with the columns
     *
     * @pre verifyInternalSetup passes all checks
     *
     */
    void setupListControlColumns(CListCtrl& list);


    /**
     * insertRowAtHead
     *
     * Inserts a new row at the head of the input list
     * @return the row index where insertion took place
     * @param list the list to be set up with the columns
     * @param details the data to be used to fill out contents of row
     * @param itemData the value to apply to the itemData field of this row in the list control
     */
    int insertRowAtHead(CListCtrl& list, 
                const RecordType& details, 
                DWORD itemData);

    /**
     * updateRow
     *
     * Updates the specified row contents
     * @param list the list to be set up with the columns
     * @param index the index of the row to be updated
     * @param details the data to be used to fill out contents of row
     */
    void updateRow(CListCtrl& list, DWORD index, const RecordType& details);

    /**
     * compareItems
     *
     * Compares the two input records based on the state of the element defined by input column
     * @param columnId defines for which column we are performing the comparison for
     *          each column maps to a unique element of the 'RecordType' properties
     * @param item1, item2 the items to compare (against each other)
     */
    int compareItems(DWORD columnId, 
        const RecordType& item1, 
        const RecordType& item2);

protected:

    /**
     * setColumnWidth (simple setter)
     *
     * @param columnId index into column
     * @param width, label width and label of column
     * @param compareType specifies, for sorting operations, what type
     *          of comparison should be performed on elements in this column
     */
    void setColumnParams(DWORD columnId, 
                            DWORD width,  
                            const std::string& label,
                            ECompareType compareType);

private:
    
    /**
     * verifyInternalSetup
     *
     * This is called to verify some basic parameters of the internal setup
     *      (to verify it's correctly structured, no data missing)
     */
    void verifyInternalSetup();

    struct ColumnElement
    {
        DWORD width;
        std::string label;
        ECompareType compareType;
    };

    typedef std::map<DWORD, ColumnElement> ColumnData;
    /// Map defining all the column information
    ColumnData m_columnData;
};


#include "ListColumnsBase.inl"
