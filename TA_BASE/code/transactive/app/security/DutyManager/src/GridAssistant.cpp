/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/GridAssistant.cpp $
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
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace 
{
    /**
      * DutyComparator is used to arrange duties based on associated profile keys, according to the
      * session's profile order (i.e. base profile first, followed by subsequent override profiles).
      *
      */
    template <typename DutyType> struct DutyComparator
    {
        DutyComparator(const TA_Base_Bus::SessionInfoEx& sessionInfoEx) : m_sessionInfoEx(sessionInfoEx) {}
            
        bool operator()(const DutyType& duty1, const DutyType& duty2) const
        {
            TA_ASSERT(m_sessionInfoEx.SessionId == duty1.session.in() && 
                      m_sessionInfoEx.SessionId == duty2.session.in(), "Session ID mismatch");

            for (TA_Base_Bus::ProfileKeyList::const_iterator profileIt = m_sessionInfoEx.ProfileId.begin();
                 profileIt != m_sessionInfoEx.ProfileId.end(); profileIt++)
            {
                if (*profileIt == duty1.profileKey)
                {
                    return true;
                }

                if (*profileIt == duty2.profileKey)
                {
                    return false;
                }
            }

            return false;
        }

        const TA_Base_Bus::SessionInfoEx& m_sessionInfoEx;
    };


}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::RegionDutyMatrix;
using TA_Base_Bus::RegionDutyMatrixElement;
using TA_Base_Bus::SubsystemDutyMatrix;
using TA_Base_Bus::SubsystemDutyMatrixElement;
using TA_Base_Bus::RegionDutyList;
using TA_Base_Bus::SubsystemDutyList;
using TA_Base_Bus::SubsystemKey;
using TA_Base_Bus::SubsystemKeyList;
using TA_Base_Bus::ProfileKeyList;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::SessionKey;
using TA_Base_Bus::SessionInfoEx;
using namespace TA_Base_App;
using namespace TA_Base_Bus;

const int GridAssistant::INVALID_GRID_ROW(-1);
const int GridAssistant::INVALID_GRID_COLUMN(-1);
const unsigned long GridAssistant::NO_ACTION_GROUP(0xFFFFFFFF); //azenitha //This is also defined in DutyAgentState.h


CImageList& GridAssistant::getGridImages()
{
    static CImageList images;

    if (images.GetSafeHandle() == NULL)
    {
        images.Create(MAKEINTRESOURCE(IDB_GRID_IMAGES_BMP), 16, 1, RGB(255,255,255));
    }

    return images;
}


bool GridAssistant::isCentralProfileInList(const TA_Base_Bus::ProfileKeyList& profileKeys)
{
    DataCache& dataCache = DutyDataStore::getDataCache();

    for (ProfileKeyList::const_iterator profileIt = profileKeys.begin(); profileIt != profileKeys.end(); profileIt++)
    {
        if (dataCache.isCentralProfile(*profileIt))
        {
            return true;
        }
    }

    return false;
}


std::string GridAssistant::generateOperatorLoginString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx)
{
    DataCache& dataCache = DutyDataStore::getDataCache();
    std::string operatr(dataCache.getOperatorDescription(sessionInfoEx.UserId));
    std::string profile("");

    for (ProfileKeyList::const_iterator profileIt = sessionInfoEx.ProfileId.begin(); 
         profileIt != sessionInfoEx.ProfileId.end(); profileIt++)
    {
        if (!profile.empty())
        {
            profile += " / ";
        }

        profile += dataCache.getProfileName(*profileIt)
                +  (dataCache.isCentralProfile(*profileIt)? " [C]" : " [L]");
    }

    std::string login(operatr + " : " + profile);

    return login;
}


std::string GridAssistant::generateProfileString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx, TA_Base_Bus::RegionKey regionKey)
{
    DataCache& dataCache = DutyDataStore::getDataCache();
    RegionDutyMatrix& regionDutyMatrix = DutyDataStore::getRegionDutyMatrix();
    const RegionDutyMatrixElement& rdmElement = regionDutyMatrix.getElement(sessionInfoEx.SessionId, regionKey);
    RegionDutyList regionDuties(rdmElement.getRegionDuties());

    std::string profile("");

    if (!regionDuties.empty())
    {
        std::sort(regionDuties.begin(), regionDuties.end(), DutyComparator<TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDuty>(sessionInfoEx));

        for (RegionDutyList::const_iterator dutyIt = regionDuties.begin(); dutyIt != regionDuties.end(); dutyIt++)
        {
            if (!profile.empty())
            {
                profile += " / ";
            }

			std::string displayName =  dataCache.getProfileDisplayName(dutyIt->profileKey);
			if(displayName.empty() || displayName.length() == 0)
			{
				profile += dataCache.getProfileName(dutyIt->profileKey);
			}
			else
			{
				profile += displayName;
			}
        }
    }

    return profile;
}


std::string GridAssistant::generateActionGroupString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx, TA_Base_Bus::SubsystemKey subsystemKey, TA_Base_Bus::RegionKey regionKey)
{
    //DutyDataStore dutyDataStore;
    DataCache& dataCache = DutyDataStore::getDataCache();
    SubsystemDutyMatrix& subsystemDutyMatrix = DutyDataStore::getSubsystemDutyMatrix();
    const SubsystemDutyMatrixElement& sdmElement = subsystemDutyMatrix.getElement(subsystemKey, regionKey);
    SubsystemDutyList subsystemDuties(sdmElement.getSubsystemDuties(sessionInfoEx.SessionId));

    std::string actionGroup("");

    if (!subsystemDuties.empty())
    {
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"TD15346: NO_ACTION_GROUP:%lu", 
			NO_ACTION_GROUP);

        std::sort(subsystemDuties.begin(), subsystemDuties.end(), DutyComparator<TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDuty>(sessionInfoEx));

        for (SubsystemDutyList::const_iterator dutyIt = subsystemDuties.begin(); dutyIt != subsystemDuties.end(); dutyIt++)
        {
            if (!actionGroup.empty())
            {
                actionGroup += " / ";
            }

			try
			{
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"TD15346: dutyIt->actionGroupKey:%lu", 
					dutyIt->actionGroupKey);

				//azenitha++
				//Prevents error message box to appear if action group key is NO_ACTION_GROUP
				unsigned long actGroupKey = dutyIt->actionGroupKey;
				LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"TD15346: actGroupKey:%lu", 
					actGroupKey);

				if (actGroupKey != NO_ACTION_GROUP)
				{
					actionGroup += dataCache.getActionGroupName(dutyIt->actionGroupKey);
				}
				//++azenitha
			}
            catch (const TA_Base_Core::TransactiveException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DutyManager Exception", ex.what());
            }
			catch (...)
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception");
			}
        }
    }

    return actionGroup;
}


std::string GridAssistant::generateRegionString(const TA_Base_Bus::RegionKeyList& regionKeys)
{
    DataCache& dataCache = DutyDataStore::getDataCache();

    std::string region("");

    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
        if (!region.empty())
        {
            region += ", ";
        }

        region += dataCache.getRegionName(*regionIt);
    }    

    return region;
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


int CALLBACK GridAssistant::operatorCompareProc(LPARAM param1, LPARAM param2, LPARAM /*paramSort*/)
{
    // Central operators take precedence over local operators.
    // Operators within each category are ordered alphabetically by name.

    //DutyDataStore dutyDataStore;
    SessionCache& sessionCache = DutyDataStore::getSessionCache();

    SessionKey sessionKey1(param1);
    SessionKey sessionKey2(param2);

    SessionInfoEx sessionInfoEx1(sessionCache.getSessionInfo(sessionKey1));
    SessionInfoEx sessionInfoEx2(sessionCache.getSessionInfo(sessionKey2));

    if (isCentralProfileInList(sessionInfoEx1.ProfileId) && !isCentralProfileInList(sessionInfoEx2.ProfileId))
    {
        return -1;
    }

    if (!isCentralProfileInList(sessionInfoEx1.ProfileId) && isCentralProfileInList(sessionInfoEx2.ProfileId))
    {
        return 1;
    }

    DataCache& dataCache = DutyDataStore::getDataCache();

    CString operator1(dataCache.getOperatorDescription(sessionInfoEx1.UserId).c_str());
    CString operator2(dataCache.getOperatorDescription(sessionInfoEx2.UserId).c_str());

    return operator1.CompareNoCase(operator2);
}
