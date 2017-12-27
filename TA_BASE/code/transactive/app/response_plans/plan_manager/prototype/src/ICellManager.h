/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ICellManager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef ___ICellManager_h__
#define ___ICellManager_h__

#pragma warning(disable : 4786)

#include <vector>
#include <string>


struct ICellManager
{
    typedef enum
    {
        CT_NOTEDITABLE,
        CT_TEXT,
        CT_NUMBER,
        CT_SELECTION,
        CT_DECISION
    } CellType;

    typedef std::vector<std::string> CellData;


    virtual CellType GetCellType(int listCtrlId, int item, int subItem) = 0;
    virtual CellData GetCellData(int listCtrlId, int item, int subItem) = 0;
    virtual void UpdateCellValue(int listCtrlId, int item, int subItem, CString value) = 0;
};

#endif
