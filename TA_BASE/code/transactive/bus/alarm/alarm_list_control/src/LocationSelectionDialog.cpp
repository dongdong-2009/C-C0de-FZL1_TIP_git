/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/alarm/alarm_list_control/src/LocationSelectionDialog.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_Bus
{

    LocationSelectionDialog::LocationSelectionDialog( CWnd* pParent )
        : CDialog( LocationSelectionDialog::IDD, pParent ),
          m_initialLocation( 0 ),
          m_selectedLocation( 0 )
    {
        FUNCTION_ENTRY( "LocationSelectionDialog( initialLocation, pParent )" );

        // get a list of locations,
        std::vector<TA_Base_Core::ILocation*> allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

        // the customer is going to expect locations sorted by order ID
       
        for ( std::vector<TA_Base_Core::ILocation*>::iterator locationIter = allLocations.begin();
              locationIter != allLocations.end(); ++locationIter )
        {
            // filter out the list to only station locations.
            if ( ( 0 != (*locationIter)->getKey() ) &&
                 ( (*locationIter)->getLocationType() == TA_Base_Core::ILocation::STATION ) )
            {
                LocationInfo newLocation;
                newLocation.key = (*locationIter)->getKey();
                newLocation.name = (*locationIter)->getName();

                m_locationInfoMap.insert( LocationInfoMap::value_type( (*locationIter)->getOrderId(), newLocation ) );
            }

            delete *locationIter;
            *locationIter = NULL;
        }

        allLocations.clear();

        FUNCTION_EXIT;
    }


    LocationSelectionDialog::~LocationSelectionDialog()
    {
        FUNCTION_ENTRY( "~LocationSelectionDialog()" );
        FUNCTION_EXIT;
    }


    void LocationSelectionDialog::setInitialSelection( unsigned long initialLocation )
    {
        m_initialLocation = initialLocation;
    }


    unsigned long LocationSelectionDialog::getSelectedLocation()
    {
        FUNCTION_ENTRY( "getSelectedLocation()" );
        FUNCTION_EXIT;
        return m_selectedLocation;
    }


    BOOL LocationSelectionDialog::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        // Calls the parent implementation of this method
        CDialog::OnInitDialog();

        m_locationList.Clear();

        int itemToSelect = CB_ERR;

        // now its sorted, insert each item
        for ( LocationInfoMap::iterator orderedLocationIter = m_locationInfoMap.begin();
              orderedLocationIter != m_locationInfoMap.end(); ++orderedLocationIter )
        {
            // populates m_locationList with all the station locations. Use the
            // location name for the text, the location key for the item data.
            int index = m_locationList.AddString( orderedLocationIter->second.name.c_str() );
            m_locationList.SetItemData( index, orderedLocationIter->second.key );

            // if this matches m_initialLocation, then store the item to select
            if ( orderedLocationIter->second.key == m_initialLocation)
            {
                itemToSelect = index;
            }
        }

        // if m_initialLocation is in m_locationList
        // select that item in m_locationList
        if ( ( itemToSelect > CB_ERR ) &&
             ( itemToSelect < m_locationList.GetCount() ) )
        {
            m_locationList.SetCurSel( itemToSelect );
            
            // set m_selectedLocation to this location key
            m_selectedLocation = m_initialLocation;
        }
        else
        {
            // if not, set m_selectedLocation to 0
            m_selectedLocation = 0;
        }

        FUNCTION_EXIT;
        return TRUE;
    }


    void LocationSelectionDialog::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(LocationSelectionDialog)
        DDX_Control( pDX, IDC_LOCSEL_COMBOBOX_LOCATIONS, m_locationList );
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP( LocationSelectionDialog, CDialog )
        //{{AFX_MSG_MAP(LocationSelectionDialog)
        ON_CBN_SELCHANGE( IDC_LOCSEL_COMBOBOX_LOCATIONS, onSelectionChangeLocationList )
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void LocationSelectionDialog::onSelectionChangeLocationList()
    {
        FUNCTION_ENTRY( "onSelectionChangeLocationList" );

        // Get the selected item, and its item data.
        // set m_selectedLocation to the item data
        int curItemIdx = m_locationList.GetCurSel();

        if ( CB_ERR != curItemIdx )
        {
            m_selectedLocation = m_locationList.GetItemData( curItemIdx );
        }

        FUNCTION_EXIT;
    }
}
