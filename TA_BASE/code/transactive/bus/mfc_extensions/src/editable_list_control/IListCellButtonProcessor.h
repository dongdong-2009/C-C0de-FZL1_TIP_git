/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/IListCellButtonProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#ifndef ___IListCellButtonProcessorh__
#define ___IListCellButtonProcessorh__

#pragma warning(disable : 4786)

#include "ICellManager.h"

// This guard is so that this MFC extension can be used in another
// MFC extension DLL
//

class CEditableListCtrl;

//class IListCellButtonProcessor
#ifdef IMPORT_EXTENSIONS
struct __declspec(dllimport) IListCellButtonProcessor
#else
struct AFX_EXT_CLASS IListCellButtonProcessor
#endif
{
public: 

    virtual ~IListCellButtonProcessor() {}

    // Process the button click from the ListCellButton
    virtual void processButtonClick(int item, int subItem, CEditableListCtrl* parent, ICellManager* manager) = 0;

};

#endif
