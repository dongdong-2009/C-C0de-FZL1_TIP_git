/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $Workfile:$
  * @author Jade Lee
  * @version $Revision: 1.1.2.3 $
  *
  * Last modification: $Modtime: $
  * Last modified by: $Author: jadel $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */


#pragma warning (disable : 4284)

template <class RecordType, class DerivedClass>
void ListColumnsBase<RecordType, DerivedClass>::setupListControlColumns(CListCtrl& list)
{   
    // Some verification code (should really be in a unit test)
    verifyInternalSetup();

    // Insert the columns to display the data
    for (int i = 0; i < DerivedClass::COL_MAX; i ++)
    {
        ColumnData::iterator itr = m_columnData.find(i);
        TA_ASSERT(itr != m_columnData.end(), "Column data unset");
        
        list.InsertColumn(i, itr->second.label.c_str(), LVCFMT_LEFT, itr->second.width);
    }
}


template <class RecordType, class DerivedClass>
void ListColumnsBase<RecordType, DerivedClass>::verifyInternalSetup()
{    
    for (int i = 0; i < DerivedClass::COL_MAX; i ++)
    {
        // Confirm all widths, names and compare types set
        TA_ASSERT (m_columnData.end() != m_columnData.find(i), "Column data unset");
        
        RecordType dummyItem;

        // Additionally confirm the sorting function behaves (ie doesn't assert on any column sort)
        compareItems(i, dummyItem, dummyItem);

        // Confirm all fields have a correct mapping
        DerivedClass::getFieldFromRecord(i, dummyItem);
    }
}


template <class RecordType, class DerivedClass>
int ListColumnsBase<RecordType, DerivedClass>::insertRowAtHead
(
    CListCtrl& list, 
    const RecordType& details, 
    DWORD itemData
)
{
    int position = list.InsertItem(0,"",0);

    updateRow(list, position, details);    
    
    list.SetItemData(position, itemData);

    return position;
}


template <class RecordType, class DerivedClass>
void ListColumnsBase<RecordType, DerivedClass>::updateRow
(
 CListCtrl& list, 
 DWORD index, 
 const RecordType& details
)
{        
    for (int i = 0; i < DerivedClass::COL_MAX; i ++)
    {
        list.SetItemText(index, i, DerivedClass::getFieldFromRecord(i, details));
    }
}   


template <class RecordType, class DerivedClass>
int ListColumnsBase<RecordType, DerivedClass>::compareItems(
    DWORD columnId, 
    const RecordType& item1, 
    const RecordType& item2)
{
    int returnVal = 0;
        
    const CString field1 = DerivedClass::getFieldFromRecord(columnId, item1);
    const CString field2 = DerivedClass::getFieldFromRecord(columnId, item2);

    ColumnData::iterator itr = m_columnData.find(columnId);
    TA_ASSERT(itr != m_columnData.end(), "Couldn't find comparison type");

    switch (itr->second.compareType)
    {
        case COMPARE_STRING:
            returnVal = field1.CompareNoCase(field2);

            break;
        case COMPARE_INTEGER:
            {
                int value1 = atoi((LPCTSTR)field1);
                int value2 = atoi((LPCTSTR)field2);
                returnVal = value1 - value2;
            }

            break;
        default:
            TA_ASSERT(FALSE, "Unknown comparison type");
    }

    return returnVal;
}
         

template <class RecordType, class DerivedClass>
void ListColumnsBase<RecordType, DerivedClass>::setColumnParams
(
 DWORD columnId, 
 DWORD width, 
 const std::string& label,
 ECompareType compareType
)
{
    ColumnElement elem;
    elem.width = width;
    elem.label = label;
    elem.compareType = compareType;

    m_columnData[columnId] = elem;   
}
