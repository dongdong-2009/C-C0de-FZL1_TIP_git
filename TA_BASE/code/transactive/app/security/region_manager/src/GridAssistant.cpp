/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/GridAssistant.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Generic grid helper methods.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Bus;

const int GridAssistant::INVALID_GRID_ROW(-1);
const int GridAssistant::INVALID_GRID_COLUMN(-1);


CImageList& GridAssistant::getGridImages()
{
    static CImageList images;

    if (images.GetSafeHandle() == NULL)
    {
        images.Create(MAKEINTRESOURCE(IDB_GRID_IMAGES_BMP), 16, 1, RGB(255,255,255));
    }

    return images;
}


std::string GridAssistant::generateOperatorLoginString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx)
{
    TA_Base_Bus::DataCache& dataCache = TA_Base_Bus::DutyDataStore::getDataCache();
    std::string login("");

    for (TA_Base_Bus::ProfileKeyList::const_iterator profileIt = sessionInfoEx.ProfileId.begin(); 
         profileIt != sessionInfoEx.ProfileId.end(); profileIt++)
    {
        if (!login.empty())
        {
            login += "/";
        }

        login += dataCache.getProfileName(*profileIt);
    }

    login += "@" + dataCache.getLocationName(sessionInfoEx.LocationId);

    return login;
}


std::string GridAssistant::generateOperatorNameAndLoginString(const TA_Base_Bus::SessionId& sessionId)
{
    TA_Base_Bus::SessionInfoEx sessionInfoEx(DutyDataStore::getSessionCache().getSessionInfo(sessionId));

    std::ostringstream targetOperator;
    targetOperator << DutyDataStore::getDataCache().getOperatorDescription(sessionInfoEx.UserId)
                   << " (" << generateOperatorLoginString(sessionInfoEx) << ")";

    return targetOperator.str();
}


int GridAssistant::findGridRow(const CGridCtrl& grid, unsigned long key)
{
    for (int row = grid.GetFixedRowCount(); row < grid.GetRowCount(); row++)
    {
        if (key == static_cast<unsigned long>(grid.GetItemData(row, 0)))
        {
            return row;
        }
    }

    return INVALID_GRID_ROW;
}


int GridAssistant::findGridColumn(const CGridCtrl& grid, unsigned long key)
{
    for (int column = grid.GetFixedColumnCount(); column < grid.GetColumnCount(); column++)
    {
        if (key == static_cast<unsigned long>(grid.GetItemData(0, column)))
        {
            return column;
        }
    }

    return INVALID_GRID_COLUMN;
}

