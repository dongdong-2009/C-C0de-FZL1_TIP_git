#if !defined(AFX_GRIDASSISTANT_H__CBA8F0AB_F7E9_4D95_9E01_BF924FC07BD4__INCLUDED_)
#define AFX_GRIDASSISTANT_H__CBA8F0AB_F7E9_4D95_9E01_BF924FC07BD4__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/GridAssistant.h $
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
            GI_TICK = 0, 
            GI_CROSS 
        } EGridImageType;


    // Operations
    public:
        /**
          * Loads from a bitmap and returns a list of images used by the region and subsystem grids.
          *
          */
        static CImageList& getGridImages();

        /**
          * Builds and returns a string containing operator login details as per the specified session.
          * Format:  <base profile>[/<override profile 1>[/<override profile N>]]@<location>
          * Throws a DutyMonitorFramework exception if profile or location name cannot be resolved.
          *
          */
        static std::string generateOperatorLoginString(const TA_Base_Bus::SessionInfoEx& sessionInfoEx);

        /**
          * Builds and returns a string containing operator name + login details as per the specified session.
          * Format:  <operator> (<base profile>[/<override profile 1>[/<override profile N>]]@<location>)
          * Throws a DutyMonitorFramework exception if operator, profile or location name cannot be resolved.
          *
          */
        static std::string generateOperatorNameAndLoginString(const TA_Base_Bus::SessionId& sessionId);

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


    // Attributes
    public:
        static const int INVALID_GRID_ROW;
        static const int INVALID_GRID_COLUMN;
    };
}

#endif // !defined(AFX_GRIDASSISTANT_H__CBA8F0AB_F7E9_4D95_9E01_BF924FC07BD4__INCLUDED_)
