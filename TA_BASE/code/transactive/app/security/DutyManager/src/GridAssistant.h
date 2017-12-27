#if !defined(AFX_GRIDASSISTANT_H__DC3E3AAA_390D_407F_9C47_C2AB01554EA1__INCLUDED_)
#define AFX_GRIDASSISTANT_H__DC3E3AAA_390D_407F_9C47_C2AB01554EA1__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/GridAssistant.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Generic grid helper methods.
  *
  */

namespace TA_Base_App
{
    class GridAssistant
    {
    // Types
    public:
        typedef enum 
        { 
            GI_BLANK = -1,
            GI_CENTRAL = 0,
            GI_LOCAL,
            GI_TICK, 
            GI_CROSS ,
			GI_DELEGATE_TICK, // TD19076
        } EGridImageType;

    // Operations
    public:
        /**
          * Loads from a bitmap and returns the list of images used by the region and subsystem grids.
          *
          */
        static CImageList& getGridImages();

        /**
          * Returns true if the specified profile key list contains at least one key associated 
          * with a central profile.
          *
          */
        static bool isCentralProfileInList(const TA_Base_Bus::ProfileKeyList& profileKeys);

        /**
          * Builds and returns a string containing operator login details as per the specified session.
          * Format:  <operator name> : <base profile + type>[/<override profile + type 1>[/<override profile + type N>]]
          * where profile type = "[C]" for central profiles and "[L]" for local profiles.
          * Throws a DutyMonitorFramework exception if operator or profile name cannot be resolved.
          *
          */
        static std::string generateOperatorLoginString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx);

        /**
          * Builds and returns a string containing profile names associated with region duties allocated
          * to the specified session for the specified region. Empty string is returned if the session
          * has no duties for the region.
          * Format:  <profile 1>[/<profile 2>[/<profile N>]]
          * Throws a MatrixException if an invalid session-region combination is specified.
          * Throws a DutyMonitorFramework exception if a profile name cannot be resolved.
          *
          */
        static std::string generateProfileString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx, TA_Base_Bus::RegionKey regionKey);

        /**
          * Builds and returns a string containing action group names associated with subsystem duties
          * allocated to the specified session for the specified subsystem and region. Empty string is 
          * returned if the session has no duties for the subsystem.
          * Format:  <action grp 1>[/<action grp 2>[/<action grp N>]]
          * Throws a MatrixException if an invalid subsystem-region combination is specified.
          * Throws a DutyMonitorFramework exception if an action group name cannot be resolved.
          *
          */
        static std::string generateActionGroupString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx, TA_Base_Bus::SubsystemKey subsystemKey, TA_Base_Bus::RegionKey regionKey);

        /**
          * Builds and returns a string containing the names of regions matching the specified keys.
          * Format: <region 1>, <region 2>, <region N>
          * Throws a DutyMonitorFramework exception if a region name cannot be resolved.
          *
          */
        static std::string generateRegionString(const TA_Base_Bus::RegionKeyList& regionKeys);

        /**
          * Finds and returns 0-based index of the row matching the specified key. The key is compared
          * against item data of the first column's cells. -1 is returned when no matching row is found.
          *
          */
        static int findGridRow(const CGridCtrl& grid, unsigned long key);

        /**
          * Finds and returns 0-based index of the column matching the specified key. The key is compared
          * against item data of the first rows's cells. -1 is returned when no matching column is found.
          *
          */
        static int findGridColumn(const CGridCtrl& grid, unsigned long key);

        /**
          * Used for sorting rows in the region duty grid. Compares two operators to determine their
          * relative order in the grid. The ordering rules take into account operator profile and name.
          * param1 contains the session key for the first operator, while param2 contains the session key 
          * for the second operator. paramSort is unused. 
          * Returns -1 if the first operator takes precedence over the second operator, and +1 otherwise.
          *
          */
        static int CALLBACK operatorCompareProc(LPARAM param1, LPARAM param2, LPARAM paramSort);


    // Attributes
    public:
        static const int INVALID_GRID_ROW;
        static const int INVALID_GRID_COLUMN;
		static const unsigned long NO_ACTION_GROUP; //azenitha //This is also defined in DutyAgentState.h
    };
}

#endif // !defined(AFX_GRIDASSISTANT_H__DC3E3AAA_390D_407F_9C47_C2AB01554EA1__INCLUDED_)
