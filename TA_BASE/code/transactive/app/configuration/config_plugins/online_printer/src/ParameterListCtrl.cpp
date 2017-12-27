/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/ParameterListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the entity parameters in the EntityView class
  */

#include "app/configuration/config_plugins/online_printer/src/stdafx.h"
#include "app/configuration/config_plugins/online_printer/src/ChineseInfo.h"
#include "app/configuration/config_plugins/online_printer/src/ParameterListCtrl.h"
#include "app/configuration/config_plugins/online_printer/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/OnlinePrinterAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // These are the set up details for the attribute list control
    const CString ParameterListCtrl::TYPE_COLUMN_NAME   = TYPE_MSG;
    const int     ParameterListCtrl::TYPE_COLUMN_WIDTH  = 100;
    const int     ParameterListCtrl::TYPE_COLUMN        = 0;
    const CString ParameterListCtrl::SUBSYSTEM_COLUMN_NAME  = SUBSYSTEM_MSG;
    const int     ParameterListCtrl::SUBSYSTEM_COLUMN_WIDTH = 200;
    const int     ParameterListCtrl::SUBSYSTEM_COLUMN       = 1;
    const CString ParameterListCtrl::PRINTER_COLUMN_NAME = PRINTER_MSG;
    const int     ParameterListCtrl::PRINTER_COLUMN_WIDTH= 200;
    const int     ParameterListCtrl::PRINTER_COLUMN      = 2;

    static CString TranslateSubsystem(unsigned long pSubsystem)
    {
        std::vector<TA_Base_Core::OnlinePrinterAccessFactory::SubsystemRecord> subsystems =
                    TA_Base_Core::OnlinePrinterAccessFactory::getInstance().getSubsystems();

        for (int i = 0; i < subsystems.size(); ++i)
        {
            if (subsystems[i].key == pSubsystem)
            {
                return subsystems[i].name.c_str();
            }
        }
        return "";
    }


    ParameterListCtrl::ParameterListCtrl() :
                                             m_maximumCharactersInField(0),
                                             CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    ParameterListCtrl::~ParameterListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(ParameterListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(ParameterListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ParameterListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void ParameterListCtrl::populate(unsigned long pLocation)
    {
        std::vector<TA_Base_Core::IOnlinePrinter*> rec = TA_Base_Core::OnlinePrinterAccessFactory::getInstance().getAllOnlinePrinters(pLocation, true);

        unsigned int i;

        for (i = 0; i < m_records.size(); ++i)
        {
            delete m_records[i];
        }
        m_records.clear();
        DeleteAllItems();

        for (i = 0; i < rec.size(); ++i)
        {
            insertRecord(dynamic_cast<TA_Base_Core::IConfigOnlinePrinter*>(rec[i])); 
        }
    }

    void ParameterListCtrl::insertRecord(TA_Base_Core::IConfigOnlinePrinter* rec)
    {
        m_records.push_back(rec);
        int index = GetItemCount();
        int pos = InsertItem(index, "");
        SetItemText(pos, TYPE_COLUMN, rec->getIsAlarm() ? "ALARM" : "EVENT");
        SetItemText(pos, SUBSYSTEM_COLUMN, TranslateSubsystem(rec->getSubsystem()));
        SetItemText(pos, PRINTER_COLUMN, rec->getPrinter().c_str());
    }

    void ParameterListCtrl::deleteCurrentRecord()
    {
        int index = GetItemCount() - 1;

        if (index >= 0)
        {
            do
            {
                if (GetItemState(index, LVIS_SELECTED) == LVIS_SELECTED)
                    break;
                --index;
            } while (index >= 0);
        }

        if (index >= 0)
        {
            m_records[index]->deleteThisObject(true);
            m_records.erase(m_records.begin() + index);
            DeleteItem(index);
        }
        else
        {
            MessageBox("No record currently selected");
        }
    }

    bool ParameterListCtrl::hasChanged()
    {
        unsigned int i;
        for (i = 0; i < m_records.size(); ++i)
        {
            if (m_records[i]->hasChanged())
            {
                return true;
            }
        }
        return false;
    }

    void ParameterListCtrl::applyChanges()
    {
        CString locStr;
        unsigned int i;

        // check the required value
        for (i = 0; i < m_records.size(); ++i)
        {
            if (m_records[i]->hasChanged())
            {
                locStr = m_records[i]->getPrinter().c_str();
                locStr.TrimRight();
                if (locStr == "")
                {
                    CString alarm = GetItemText(i, 0);
                    CString subsystem = GetItemText(i, 1);
                    std::ostringstream ostr;
                    ostr << "Missing printer value for " << alarm.GetBuffer(0)
                        << " " << subsystem.GetBuffer(0);
                    MessageBox(ostr.str().c_str());
                    return;
                }
            }
        }

        for (i = 0; i < m_records.size(); ++i)
        {
            if (m_records[i]->hasChanged())
            {
                m_records[i]->applyChanges();
            }
        }
    }

    void ParameterListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(TYPE_COLUMN,   TYPE_COLUMN_NAME,   LVCFMT_LEFT, TYPE_COLUMN_WIDTH);
        InsertColumn(SUBSYSTEM_COLUMN,  SUBSYSTEM_COLUMN_NAME,  LVCFMT_LEFT ,SUBSYSTEM_COLUMN_WIDTH);
        InsertColumn(PRINTER_COLUMN, PRINTER_COLUMN_NAME, LVCFMT_LEFT ,PRINTER_COLUMN_WIDTH);

        FUNCTION_EXIT;
    }



    ICellManager::CellType ParameterListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        switch (subItem)
        {
        case PRINTER_COLUMN:
            return CT_TEXT;
        }

        return CT_NOTEDITABLE;
    }


    unsigned long ParameterListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return m_maximumCharactersInField;
    }


    ICellManager::CellData ParameterListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

	    ICellManager::CellData data;

        return data;
    }


    CString ParameterListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char ParameterListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool ParameterListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        try
        {
            if (subItem == PRINTER_COLUMN)
            {
                m_records[item]->setPrinter(value.GetBuffer(0));

                AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
                if (detailView != NULL)
                {
                    detailView->refreshButtonAvailability();
                }
                return true;
            }
        }
        catch( const TA_Base_Core::DatabaseException& e)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, e.what());
        }

        return false;
    }

}

