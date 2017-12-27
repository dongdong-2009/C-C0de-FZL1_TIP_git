/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/OnlinePrinterDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure the details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/online_printer/src/StdAfx.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterDetailView.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrintCfgDatabaseAccessor.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterListView.h"
#include "app/configuration/config_plugins/online_printer/src/OnlinePrinterComponent.h"
#include "app/configuration/config_plugins/online_printer/src/NewItem.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "core/data_access_interface/src/IConfigOnlinePrintCfg.h"
#include "core/data_access_interface/src/OnlinePrintCfg.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"


#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    const int OnlinePrinterDetailView::PRINTER_FIELD_LENGTH = 50;

    static const int ALARM_INDEX = 0;
    static const int NON_ALARM_INDEX = 1;
    
    IMPLEMENT_DYNCREATE(OnlinePrinterDetailView, CFormView)

    OnlinePrinterDetailView::OnlinePrinterDetailView() : 
        m_configItem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(OnlinePrinterDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(OnlinePrinterDetailView)
	//}}AFX_DATA_INIT

    }


    OnlinePrinterDetailView::~OnlinePrinterDetailView()
    {
    }


    void OnlinePrinterDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(OnlinePrinterDetailView)
	//DDX_Control(pDX, IDC_EDIT_DEFAULT_PRINTER, m_defaultPrinterEdit);
	DDX_Control(pDX, IDC_LIST1, m_recordList);
    DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyBtn);
	//TD17780 marc_bugfix
	DDX_Control(pDX, IDC_COMBO_DEFAULT_PRINTER, m_defaultPrinterCombo);

	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(OnlinePrinterDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(OnlinePrinterDetailView)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_ADD, onButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, onButtonDelete)
	//ON_EN_KILLFOCUS(IDC_EDIT_DEFAULT_PRINTER, onKillfocusEditDefaultPrinter)
	ON_CBN_SELCHANGE(IDC_COMBO_DEFAULT_PRINTER, onSelChangeComboDefaultPrinter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void OnlinePrinterDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    

        // Hide all the details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void OnlinePrinterDetailView::showItemDetails(bool isShown)
    {
        int cmdShow = isShown ? SW_SHOW : SW_HIDE;
        // Show or hide all the controls from the view

        //m_defaultPrinterEdit.ShowWindow(cmdShow);
        m_recordList.ShowWindow(cmdShow);
		//TD17780 marc_bugfix
		m_defaultPrinterCombo.ShowWindow(cmdShow);

		GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(cmdShow);
		GetDlgItem(IDC_DEFAULT_PRINTER_STATIC)->ShowWindow(cmdShow);
		GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(cmdShow);
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(cmdShow);
		GetDlgItem(IDC_BUTTON_DELETE)->ShowWindow(cmdShow);
		GetDlgItem(IDC_DEFAULT_PRINTER_BITMAP)->ShowWindow(cmdShow);

		
    }


    void OnlinePrinterDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<OnlinePrintCfgDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<OnlinePrinterListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper OnlinePrinterDetailView");
        }


        m_configItem = dynamic_cast<TA_Base_Core::IConfigOnlinePrintCfg*>(currentItem);

        if (m_configItem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
    		//GetDlgItem(IDC_STATIC_LOCATION)->SetWindowText(m_configItem->getLocationName().c_str());
			GetDlgItem(IDC_STATIC_LOCATION)->SetWindowText(m_configItem->getLocationDisplayName().c_str());
            //m_defaultPrinterEdit.SetWindowText( m_configItem->getDefaultPrinter().c_str() );
			//TD17780 marc_bugfix find first string occurence specified by getDefaultPrinter()
			int iLocation = m_defaultPrinterCombo.FindStringExact(0, m_configItem->getDefaultPrinter().c_str());
			m_defaultPrinterCombo.SetCurSel(iLocation == CB_ERR ? 0 : iLocation);
		

            m_recordList.populate(m_configItem->getLocation());

            showItemDetails(true);
            refreshButtonAvailability();

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve details to display");
            
            //UE-030001
            MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",
                        RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve details to display");

            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
            MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
        }

        FUNCTION_EXIT;
    }

    void OnlinePrinterDetailView::refreshButtonAvailability()
    {
        if (m_configItem == NULL)
        {
            return;
        }

        if (m_configItem->isNew() )
        {
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        bool hasChanged = m_configItem->hasChanged();

        if (!hasChanged)
        {
            hasChanged = m_recordList.hasChanged();
        }

        if (hasChanged)
        {
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }

//TD 17780 marc_bugfix unnecessary code
/*
    void OnlinePrinterDetailView::onKillfocusEditDefaultPrinter() 
    {
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }

        CString printerString;
        m_defaultPrinterEdit.GetWindowText(printerString);
		std::string printer (printerString);

        try
        {
            if (m_configItem->getDefaultPrinter() == printer)
            {
                m_defaultPrinterEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                return;
            }

            m_configItem->setDefaultPrinter( printer );
            m_defaultPrinterEdit.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        }
        catch( ... )
        {
            m_defaultPrinterEdit.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            return;
        }

        refreshButtonAvailability();
    }
*/
	//TD17780 marc_bugfix
	  void OnlinePrinterDetailView::onSelChangeComboDefaultPrinter() 
	  {

		CString printerString;

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if (!m_configItem) 
        {
            // For the odd occasion that the operator has clicked new item straight after modifying
            // this field
            return;
        }
		m_defaultPrinterCombo.GetLBText(m_defaultPrinterCombo.GetCurSel(), printerString);

		if (m_configItem->getDefaultPrinter().c_str() == printerString)
        {
			return;
		}
		else
		{
			std::string printer (printerString);

			m_configItem->setDefaultPrinter( printer );
		}

		
		refreshButtonAvailability();
	}

    bool OnlinePrinterDetailView::hasChanges()
    {
        if (m_configItem == NULL) return false;

        bool hasChanged = m_configItem->hasChanged();

        if (!hasChanged)
        {
            hasChanged = m_recordList.hasChanged();
        }
        return hasChanged;
    }

    bool OnlinePrinterDetailView::applyChanges()
    {
		//TD 17780 marc_bugfix unnecessary code
		/*
        if ( !m_defaultPrinterEdit.isTextValid() )
        {
            //UE-030060
            MessageBox("The item could not be applied because invalid names still exist in the fields.",
                       RunParams::getInstance().get(RPARAM_APPNAME).c_str(), MB_OK|MB_ICONSTOP);
            return false;
        }
	

        CString defPrinter;
        m_defaultPrinterEdit.GetWindowText(defPrinter);
        defPrinter.TrimRight();
        if (defPrinter == "")
        {
            MessageBox("Preset printer cannot be blank");
            return false;
        }
		*/

        try
        {
            m_recordList.applyChanges();
            applyButtonPressed(m_configItem);
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            MessageBox(e.what());
            return false;
        }

        return true;
    }



    void OnlinePrinterDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Under normal circumstances, the onKillfocus*() methods should be called to update the underlying 
        // IConfig objects.  However, when the operator presses the 'Enter' key, this method is called without
        // first calling onKillfocus*() for the previous widget.  This following line should force the implicit
        // call to the onKillfocus*() method.  
        m_applyBtn.SetFocus();

        applyChanges();

        FUNCTION_EXIT;
    }

    void OnlinePrinterDetailView::onButtonAdd() 
    {
        CNewItem dlg(m_configItem->getLocation(), this);
        if (dlg.DoModal() == IDOK)
        {
            m_recordList.insertRecord(dlg.m_newItem);
        }
    }

    void OnlinePrinterDetailView::onButtonDelete() 
    {
        try
        {
            m_recordList.deleteCurrentRecord();
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            MessageBox(e.what());
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // OnlinePrinterDetailView diagnostics

    #ifdef _DEBUG
    void OnlinePrinterDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void OnlinePrinterDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
