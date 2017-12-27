/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/NewReport.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which encapsulates the dialog box and is used to create a new report or a report
 * based on an existing report
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "NewReport.h"
#include "AuditMessageUtility.h"

// for action control related stuff
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

// NOTE: for displaying message boxes
#include "bus/generic_gui/src/TransActiveMessage.h"

// NOTE : for auditing
#include "core/message/types/ESVAudit_MessageTypes.h"

// NOTE : Location related
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// NOTE : Equipment Report related
#include "core/data_access_interface/src/EquipReport.h"
#include "core/data_access_interface/src/EquipReportAccessFactory.h"
#include "core/data_access_interface/src/ConfigEquipReport.h"
#include "core/data_access_interface/src/EquipReportData.h"
#include "core/data_access_interface/src/EquipReportDataAccessFactory.h"
#include "core/data_access_interface/src/ConfigEquipReportData.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

// NOTE : Debug related
#include "core/utilities/src/DebugUtil.h"

// NOTE: Database query related
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/Session.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"

#include "progressbar.h"
#include "CustomDefines.h"

using namespace TA_Base_Core;
using namespace TA_Base_App;

namespace TA_Base_App
{

	CNewReport::CNewReport(CWnd* pParent /*=NULL*/)
		: CDialog(CNewReport::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CNewReport)
	//}}AFX_DATA_INIT
		m_newLocationKey = 0L;
		m_newReportKey = "";
        m_isAllowedToViewAllReports = false;
		m_reportKeyStringToIdMap.clear();
	}


	void CNewReport::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CNewReport)
		DDX_Control(pDX, IDOK, m_BtnOk);
		DDX_Control(pDX, IDC_REPORT_NAME, m_ReportName);
		DDX_Control(pDX, IDC_LOCATION, m_Location);
		DDX_Control(pDX, IDC_BASE_REPORT, m_BaseReport);
		DDX_Control(pDX, IDC_BASE_LOCATION, m_BaseLocation);
		DDX_Control(pDX, IDC_CHECK1, m_BasedOnCheck);
	//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CNewReport, CDialog)
		//{{AFX_MSG_MAP(CNewReport)
		ON_BN_CLICKED(IDC_CHECK1, OnBasedOnCheckClick)
		ON_CBN_SELCHANGE(IDC_LOCATION, OnSelchangeLocation)
		ON_EN_CHANGE(IDC_REPORT_NAME, OnChangeReportName)
		ON_CBN_SELCHANGE(IDC_BASE_LOCATION, OnSelchangeBaseLocation)
		ON_CBN_SELCHANGE(IDC_BASE_REPORT, OnSelchangeBaseReport)
		ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	void CNewReport::OnBasedOnCheckClick() 
	{
		int checked = 1;
		bool isChecked = ( m_BasedOnCheck.GetCheck() == checked );

		if (isChecked == FALSE)
		{
			m_BaseLocation.SetCurSel(-1);
			m_BaseReport.SetCurSel(-1);
		}
        else
        {
            if ( m_Location.GetCurSel() != CB_ERR )
            {
                RePopulateBaseLocation();
                m_BaseLocation.SetCurSel(0);
            }
        }
		
		m_BaseLocation.EnableWindow(isChecked);
		m_BaseReport.EnableWindow(isChecked);

		CheckInputValidity();
        //TD16889 Mintao 
        OnSelchangeBaseLocation();
	}


	void CNewReport::CheckInputValidity()
	{
		CString ReportName;
		BOOL isValid = FALSE;

		m_ReportName.GetWindowText(ReportName);
        ReportName.TrimLeft();
        ReportName.TrimRight();

		// Check if new location has been selected

        // TD 12813: Allow only 80 characters to be entered.
        // Watch out for apostrophes because these are expanded to two apostrophes
        // when stored so that they don't break the SQL statement.

        if ( !(ReportName.IsEmpty()) && (m_Location.GetCurSel() != CB_ERR))
		{
            CString newReportName( ReportName);
		    newReportName.Replace("'","''");

			if( newReportName.GetLength() <= 80)
			{
				isValid = TRUE;
			}
			else
			{
				TA_Base_Bus::TransActiveMessage userMsg;
				UINT selectedButton = userMsg.showMsgBox(IDS_UE_083010);
				m_ReportName.SetSel(0, -1, TRUE);
			}
		}

		// check if based-on settings are valid
		int checked = 1;
		if ( isValid && ( m_BasedOnCheck.GetCheck() == checked ) )
		{
			if ((m_BaseLocation.GetCurSel() == CB_ERR) || (m_BaseReport.GetCurSel() == CB_ERR))
				isValid = FALSE;
		}
		
		m_BtnOk.EnableWindow(isValid);
	}


	void CNewReport::OnSelchangeLocation() 
	{
		int checked = 1;
		bool isChecked = ( m_BasedOnCheck.GetCheck() == checked );

		if (isChecked == TRUE)
        {
            if ( m_Location.GetCurSel() != CB_ERR )
            {
                RePopulateBaseLocation();
                OnSelchangeBaseLocation();
                return;
            }
        }
		CheckInputValidity();	
	}


	void CNewReport::OnChangeReportName() 
	{
		CheckInputValidity();	
	}


	void CNewReport::OnSelchangeBaseLocation() 
	{
		unsigned long locationKey = m_BaseLocation.GetItemData(m_BaseLocation.GetCurSel());

		PopulateBaseReports(locationKey);
		CheckInputValidity();
	}


	void CNewReport::OnSelchangeBaseReport() 
	{
		CheckInputValidity();
	}


    BOOL CNewReport::OnInitDialog() 
	{	
		CDialog::OnInitDialog();
		
		PopulateAllLocations();		
		
		m_Location.SetFocus();
        //Maximum 80 characters can be entred into the name field
        m_ReportName.LimitText(80);

		return TRUE;  
	}

	void CNewReport::PopulateBaseReports(const unsigned long locationKey)
	{
		using TA_Base_Core::IEquipReport;
		using TA_Base_Core::DatabaseException;
		using TA_Base_Core::DataException;

		m_BaseReport.ResetContent();
		m_reportKeyStringToIdMap.clear();

		unsigned long profileLocation = 0;
		std::vector<unsigned long> profileKeys;
		bool central = false;

		// This bit is expensive - don't do it if m_isAllowedToViewAllReports is true
		if (false == m_isAllowedToViewAllReports)
		{
			TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
			profileLocation = session.getLocationKey();

			// Need to get all the profiles for this session
			try
			{
                TA_Base_Bus::AuthenticationLibrary m_authenticationLibrary(true);

				profileKeys =
					m_authenticationLibrary.getProfiles(
						TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

				// Still not enough, because we need to know if there is a central profile
                TA_Base_Core::ProfileAccessFactory& profileAccessFactory =
                    TA_Base_Core::ProfileAccessFactory::getInstance();

				std::vector<unsigned long>::iterator profileIt;
				for (profileIt = profileKeys.begin(); profileIt != profileKeys.end(); profileIt++)
				{
					TA_Base_Core::IProfile* profile = profileAccessFactory.getProfile(*profileIt);

					if (!profile->requiresLocationToBeSelected())
					{
						central = true;
					}

					delete profile;
				}
			}
			catch (...)
			{
			}
		}

        // OK to populate if we have a central profile, local profile with matching location or
        // we have the right to view all reports
        if (central || profileLocation == locationKey || m_isAllowedToViewAllReports)
        {
            std::vector<IEquipReport*> aEquipReports;
            
            try
            {
                aEquipReports = TA_Base_Core::EquipReportAccessFactory::getInstance().getAllEquipReportsByLocationKey(locationKey);
            }
            catch (...)
            {
                return;
            }
            
            for (std::vector<IEquipReport *>::iterator iter = aEquipReports.begin(); iter != aEquipReports.end(); ++iter)
            {
                try
                {
                    // Must have same profile to access reports.
                    CString baseReportName((*iter)->getName().c_str());
                    // Only put report in list if profiles match or has right to view all reports
                    unsigned long tempProfile = (*iter)->getProfile();
                    bool showReport = m_isAllowedToViewAllReports;

                    std::vector<unsigned long>::iterator profileIt;
                    for (profileIt = profileKeys.begin(); 
                         profileIt != profileKeys.end() && showReport == false; 
                         profileIt++)
                    {
                        //TD16889 Mintao 
                        if ((*profileIt) == tempProfile)
                        {
                            showReport = true;
                            break;
                        }
                    }
						
					if(showReport)
					{
                        baseReportName.Replace("''","'");

						int index = m_BaseReport.AddString(baseReportName);
						m_reportKeyStringToIdMap.insert(std::pair<int, std::string>(index, (*iter)->getId()));
						m_BaseReport.SetItemData(index, index);
				    }
                }
				catch (...)
				{
				}

                delete *iter;
                *iter = NULL;
            }
        }

        if (m_BaseReport.GetCount() == 1)
			m_BaseReport.SetCurSel(0);
	}

	void CNewReport::PopulateAllLocations()
	{
		using TA_Base_Core::ILocation;			
		using TA_Base_Core::IEquipReport;		
		using TA_Base_Core::DatabaseException;
		using TA_Base_Core::DataException;

		m_Location.ResetContent();
		m_BaseLocation.ResetContent();

		std::vector<ILocation*> aLocations;

		TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
        unsigned long locationKey = session.getLocationKey();
        unsigned long nProfileKey = session.getProfileKey(); //by Veld TD CL_17480

		TA_Base_Core::IProfile* profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(nProfileKey); //by Veld TD CL_17480
        if((0 == locationKey) || (! profile->requiresLocationToBeSelected()))     //by Veld TD CL_17480		
        //if (locationKey == 0) //locationkey 0 means at all location
        {
		    try
		    {
			    aLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations(false);
		    }
		    catch (...)
		    {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_010001);
			    return;
		    }
        }
        else
        {
            ILocation* currentloc = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
            aLocations.push_back(currentloc);
        }
		
		for (std::vector<ILocation*>::iterator iter2 = aLocations.begin(); iter2 != aLocations.end(); ++iter2)
		{
			try
			{
				// TES #759 - 'Unallocated' location should be filtered out
				if ( (*iter2)->getKey() == 0)
				{
					continue;
				}

				int index = m_Location.AddString((*iter2)->getName().c_str());
				m_Location.SetItemData(index,(*iter2)->getKey());
				index = m_BaseLocation.AddString((*iter2)->getName().c_str());
				m_BaseLocation.SetItemData(index, (*iter2)->getKey());
			}
			catch (...)
			{				
				return;
			}
			
			delete *iter2;
			*iter2 = NULL;
		}
	}

    void CNewReport::RePopulateBaseLocation()
    {
        m_BaseLocation.ResetContent();
		int checked = 1;
        if ( m_Location.GetCurSel() != CB_ERR )
        {
            CString newlocation;
            m_Location.GetLBText(m_Location.GetCurSel(),newlocation);
		    int index = m_BaseLocation.AddString(newlocation);
		    m_BaseLocation.SetItemData(index, m_Location.GetItemData(m_Location.GetCurSel()) );
        }

    }

	/*
	*	Only called when all input satisfies basic input checks as per CheckInputValidity() function
	*	Here we have 2 scenarios
	*		1. Create a brand new report 
	*		2. Create a brand new report based on an existing report
	*/ 
	void CNewReport::OnOK() 
	{
		int checked = 1;
		if ( m_BasedOnCheck.GetCheck() == checked )
		{
			// check if Location and BaseLocation are the same 
			// we cant base a report on equipment from another location as 
			// it cannot be guaranteed to be the same
			if (m_Location.GetItemData(m_Location.GetCurSel()) != m_BaseLocation.GetItemData(m_BaseLocation.GetCurSel()))
			{
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_083008);
				return;
			}

			// get the location id of the new report
			unsigned long locationKey = m_Location.GetItemData(m_Location.GetCurSel());
			
			// get location name
			CString locationName("");			
			m_Location.GetLBText(m_Location.GetCurSel(), locationName);
			std::string locName(locationName.GetBuffer(locationName.GetLength()));

			// get the equip report key of the base report
			std::map<int, std::string>::iterator itr;
			itr = m_reportKeyStringToIdMap.find(m_BaseReport.GetItemData(m_BaseReport.GetCurSel()));
			std::string baseReportKey;
			if (itr != m_reportKeyStringToIdMap.end())
			{
				baseReportKey = itr->second;
			}
			else
			{
				return;
			}

			CString newReportName;
			
			// get our report name
			m_ReportName.GetWindowText(newReportName);
            newReportName.TrimLeft();
            newReportName.TrimRight();
			newReportName.Replace("'","''");

			std::string newRepName(newReportName.GetBuffer(newReportName.GetLength()));

			if (CreateNewReportBasedOnExisting(baseReportKey, locationKey, newRepName, m_newReportKey) == TRUE)
			{
				m_newLocationKey	= locationKey;				

				newReportName.Replace("''","'");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << newReportName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_083010);
				
				// write audit message
				CAuditMessageUtility auditLogWriter;	
				auditLogWriter.LogAuditMessage(newRepName, locName, ESVAudit::CreateReportLog);
				
			} else
			return;
		
		} else
		{
			using TA_Base_Core::IEquipReport;
			using TA_Base_Core::IConfigEquipReport;
			using TA_Base_Core::DatabaseException;
			using TA_Base_Core::DataException;
			
			// get the location key for the new report
			unsigned long locationKey = m_Location.GetItemData(m_Location.GetCurSel());

			// get location name
			CString locationName("");
			m_Location.GetLBText(m_Location.GetCurSel(), locationName);

			// get the profile for the new report from the user's current Session
            TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

			// get the report name
			CString newReportName;
			m_ReportName.GetWindowText(newReportName);
			newReportName.Replace("'","''");
			std::string repName(newReportName.GetBuffer(newReportName.GetLength()));

			// check if a report with newReportName already exists in the database
			
			try
			{
				if (TA_Base_Core::EquipReportAccessFactory::getInstance().equipReportAlreadyExistsAtLocationByName(
																					 locationKey, repName) == TRUE)
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << newReportName;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UW_083007);
					m_ReportName.SetSel(0, -1, TRUE);
					return;
				}
			} 
			catch (...)
			{			
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << newReportName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_083001);
				return;
			}

			IConfigEquipReport* newReport = NULL;
			
			try
			{
				// create new report
				newReport = TA_Base_Core::EquipReportAccessFactory::getInstance().createEquipReport();
			}
			catch (...)
			{
				return;
			}

			newReport->setName(repName);
			newReport->setLocationId(locationKey);
			newReport->setProfile(session.getProfileKey());
			newReport->applyChanges();

			m_newLocationKey	= locationKey;
			m_newReportKey		= newReport->getId();
			
			delete newReport;
			newReport = NULL;

			newReportName.Replace("''","'");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << newReportName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_083010);

			std::string locName(locationName.GetBuffer(locationName.GetLength()));

			// write audit message
			CAuditMessageUtility auditLogWriter;
			auditLogWriter.LogAuditMessage(repName, locName, ESVAudit::CreateReportLog);
		}
		
		CDialog::OnOK();
	}


	void CNewReport::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		CDialog::OnShowWindow(bShow, nStatus);
		
		m_Location.SetFocus();
		
		if (m_Location.GetCount() == 1)
		{
			m_Location.SetCurSel(0);
			m_BaseLocation.SetCurSel(0);
		}
		
	}


	/*	Function attempts to create a new Equipment Report based on the existing one and 
	*	proceeds to copy all the datapoints from the base report to the newly created one.
	*	Basic checking is done to make sure the report with the name at the location does not 
	*	already exist to prevent duplication.
	*/
	bool CNewReport::CreateNewReportBasedOnExisting(std::string& baseReportKey, 
													const unsigned long locationKey, 
													const std::string &newReportName, 
													std::string& aNewReportKey)
	{
		using TA_Base_Core::IEquipReport;
		using TA_Base_Core::IConfigEquipReport;
		using TA_Base_Core::IEquipReportData;
		using TA_Base_Core::IConfigEquipReportData;
		
		using TA_Base_Core::DatabaseException;
		using TA_Base_Core::DataException;

		try
		{
			// check if a report with newReportName exists in the database
			if (TA_Base_Core::EquipReportAccessFactory::getInstance().equipReportAlreadyExistsAtLocationByName(
																				locationKey,newReportName) == TRUE)
			{
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << newReportName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UW_083007);
				m_ReportName.SetSel(0, -1, TRUE);
				return false;
			}
		}
		catch (...)
		{
			return false;
		}
			

		// get an instance of the base report object
		IConfigEquipReport* baseReport = dynamic_cast<IConfigEquipReport*>(TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(baseReportKey, true));

		// copy the base equipment report to a new equipment report
		IConfigEquipReport* copiedReport = TA_Base_Core::EquipReportAccessFactory::getInstance().copyEquipReport(baseReport);
	
		delete baseReport; 
		baseReport = NULL;
		
		// set the new name for this newly copied report
		copiedReport->setName(newReportName);

		// get the profile for the new report from the user's current Session (TD 13290)
        TA_Base_Core::Session session(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
		copiedReport->setProfile(session.getProfileKey());

		copiedReport->applyChanges();

		// save the new report key for further reference
        std::string newReportKey = copiedReport->getId();
		
		aNewReportKey = newReportKey;

		delete copiedReport;
		copiedReport = NULL;

#define USING_STORED_PROCEDURE

		BeginWaitCursor();
		CString msg;
		msg.Format("Creating %s", newReportName.c_str());

#ifndef USING_STORED_PROCEDURE

		// Now copy all datanode(s)/datapoint(s) from the base report to the newly created one
		std::vector<IEquipReportData*> aEquipRepDatum;
		try
		{
			aEquipRepDatum = TA_Base_Core::EquipReportDataAccessFactory::getInstance().getAllEquipReportDatumByReportKey(baseReportKey);
		}
		catch (...)
		{
			return false;
		}

		CProgressBar progressBar(msg, 30, aEquipRepDatum.size());
		progressBar.SetBarColour(PROGRESS_BAR_COLOR);
		
		for (std::vector<IEquipReportData*>::iterator iter = aEquipRepDatum.begin(); iter != aEquipRepDatum.end(); ++iter)
		{
			unsigned long dataNodeKey = (*iter)->getDataNodeKey();
			unsigned long dataPointKey = (*iter)->getDataPointKey();

			// create new equipment report data record for each record found in the original equipment report
			IConfigEquipReportData* newReportData = TA_Base_Core::EquipReportDataAccessFactory::getInstance().createEquipReportData();
			
			// set data fields for this new record
			newReportData->setReportKey(newReportKey);
			newReportData->setDataNodeKey(dataNodeKey);
			newReportData->setDataPointKey(dataPointKey);
			newReportData->applyChanges();
			
			delete newReportData;
			newReportData = NULL;

			delete *iter;
			*iter = NULL;

			progressBar.StepIt();
		}		
#else
		CProgressBar progressBar(msg, 30, 1);
		progressBar.SetBarColour(PROGRESS_BAR_COLOR);

		try
		{
			TA_Base_Core::EquipReportDataAccessFactory::getInstance().copyEquipReportData(baseReportKey, newReportKey);		
		}
		catch (DatabaseException &)
		{
			// failed copying records
			progressBar.StepIt();

			// delete the new report completely then			
			using TA_Base_Core::IConfigEquipReport;
		
			IConfigEquipReport *reportToDelete = NULL;

			try
			{
				reportToDelete = dynamic_cast<IConfigEquipReport *>(EquipReportAccessFactory::getInstance().getEquipReportByKey(newReportKey, true));
				reportToDelete->deleteThisEquipReport();
			}
			catch (...)
			{		
				if (reportToDelete)
					delete reportToDelete;
				return false;
			}			

			if (reportToDelete)
				delete reportToDelete;
			reportToDelete = NULL;		
			
			EndWaitCursor();

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << newReportName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_083001);
			return false;
		}
		progressBar.StepIt();

#endif
		EndWaitCursor();
		return true;
	}



} // TA_Base_App

