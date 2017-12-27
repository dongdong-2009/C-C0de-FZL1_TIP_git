/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/StatusDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the dialog which displays the equiment information tab shown
  * by the inspector panel.  It allows the operator to view the values of the data points.
  */

#include "stdafx.h"

#include <sstream>
#include "ace/OS.h"

#include "app/scada/inspector_panel/src/StatusDialog.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CStatusDialog dialog


    CStatusDialog::CStatusDialog(CWnd* pParent /*=NULL*/)
        : CDialog(CStatusDialog::IDD, pParent), m_equipment (NULL), m_accessAllowed(false)
    {
        FUNCTION_ENTRY ("CStatusDialog()");

        //{{AFX_DATA_INIT(CStatusDialog)
    //}}AFX_DATA_INIT

        FUNCTION_EXIT;
    }

    void CStatusDialog::setEquipment(CEquipment* equipment)
    {
        FUNCTION_ENTRY ("setEquipment()");

        m_equipment = equipment;

        if (m_equipment != NULL)
        {
            updateDataNode(TA_Base_Bus::ConfigStateUpdate);
        }

        FUNCTION_EXIT;
    }
	void CStatusDialog::displayDataPoints()
	{
		FUNCTION_ENTRY ("setEquipment()");
	/*	if (m_equipment != NULL)
        {
            updateDataNode(TA_Base_Bus::ConfigStateUpdate);
        }*/
		FUNCTION_EXIT;
	}
    void CStatusDialog::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY ("DoDataExchange()");

        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CStatusDialog)
    DDX_Control(pDX, IDC_STATUS_INFO, m_statusInfo);
    //}}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CStatusDialog, CDialog)
        //{{AFX_MSG_MAP(CStatusDialog)
    ON_WM_SHOWWINDOW()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CStatusDialog message handlers

    BOOL CStatusDialog::OnInitDialog() 
    {
        FUNCTION_ENTRY ("OnInitDialog()");

        CDialog::OnInitDialog();

        FUNCTION_EXIT;
        
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CStatusDialog::updateDataPoint(unsigned long key, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        FUNCTION_ENTRY ("updateDataPoints()");

        updateDataNode(updateType);

        FUNCTION_EXIT;
    }

    void CStatusDialog::updateDataNode(TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        FUNCTION_ENTRY ("updateDataNode()");

        if (m_hWnd==0 || m_equipment == NULL)
        {
            return;
        }

        // Just update anyway - this is just a test tab
        try
        {
            TA_Base_Bus::DataNodeProxySmartPtr &dataNode = m_equipment->getDataNode();

            // Clear the edit box
            m_statusInfo.SetWindowText("");

            std::ostringstream text;

            text << "Diagnostic test information for Equipment Status\r\n\r\n";

            text << "DataNode CorbaName:\t" << dataNode->getAgentName() << ":" << dataNode->getEntityName() << "\r\n";
            text << "Proxy Config Valid:\t\t" << (dataNode->isConfigValid() ? "Yes" : "No") << "\r\n";
            text << "Entity Key:\t\t" << dataNode->getEntityKey() << "\r\n";

            if ( true == dataNode->isConfigValid())
            {
                text << "Description:\t\t"  << dataNode->getDescription() << "\r\n";
                text << "Label:\t\t\t"      << dataNode->getLabel() << "\r\n";
                text << "Short Label:\t\t"  << dataNode->getShortLabel() << "\r\n";
                text << "Asset Name:\t\t"   << dataNode->getAssetName() << "\r\n";
            }

            TA_Base_Bus::BooleanTag boolTag;
            TA_Base_Bus::EnumTag enumTag;

            text << "Quality Summary:\t\t";
            enumTag = dataNode->getQualitySummary();
            switch( ((long)enumTag.value) & TA_Base_Bus::QUALITY_MASK)
            {
            case TA_Base_Bus::QUALITY_BAD:
                text << "bad (" << TA_Base_Bus::QUALITY_BAD << ")";
                break;
            
            case TA_Base_Bus::QUALITY_UNCERTAIN:
                text << "uncertain (" << TA_Base_Bus::QUALITY_UNCERTAIN << ")";
                break;
            
            case TA_Base_Bus::QUALITY_GOOD:
                text << "good (" << TA_Base_Bus::QUALITY_GOOD << ")";
                break;
            }
            text << " - " << getQuality(enumTag.quality) << "\r\n";                

            text << "Alarm Summary:\t\t";
            boolTag = dataNode->getAlarmSummary();
            text << (const char *)( boolTag.value ? "alarmed" : "no alarm" )
                << " - " << getQuality(boolTag.quality) << "\r\n";

            text << "Alarm Ack Summary:\t";
            enumTag = dataNode->getAlarmAckStateSummary();
            switch (enumTag.value)
            {
            case TA_Base_Bus::ScadaAlarmNack:
                text << "not acknowledged";
                break;
                
            case TA_Base_Bus::ScadaAlarmAck:
                text << "acknowledged";
                break;

            case TA_Base_Bus::ScadaAlarmNotPresent:
                text << "not present";
                break;

            case TA_Base_Bus::ScadaAlarmUnknown:
                text << "unknown";
                break;

            default:
                text << "undefined";
            }
            text << " (" << enumTag.value << ") - " << getQuality(enumTag.quality) << "\r\n";

            text << "Scan Inhibit Summary:\t";
            boolTag = dataNode->getScanInhibitSummary();
            text << ( boolTag.value ? "inhibit" : "not inhibited" )
                << " - " << getQuality(boolTag.quality) << "\r\n";

            text << "Alarm Inhibit Summary:\t";
            boolTag = dataNode->getAlarmInhibitSummary();
            text << ( boolTag.value ? "inhibit" : "not inhibited" )
                << " - " << getQuality(boolTag.quality) << "\r\n";

            text << "MMS Alarm Inhibit Summary:\t";
            boolTag = dataNode->getMmsAlarmInhibitSummary();
            text << ( boolTag.value ? "inhibit" : "not inhibited" )
                << " - " << getQuality(boolTag.quality) << "\r\n";
            
            text << "Control Inhibit Summary:\t";
            boolTag = dataNode->getControlInhibitSummary();
            text << ( boolTag.value ? "inhibit" : "not inhibited" )
                << " - " << getQuality(boolTag.quality) << "\r\n";

            text << "Override Summary:\t\t";
            boolTag = dataNode->getOverrideSummary();
            text << ( boolTag.value ? "override" : "not overridden" )
                << " - " << getQuality(boolTag.quality) << "\r\n";

            text << "Control Pending Summary:\t";
            boolTag = dataNode->getControlPendingSummary();
            text << ( boolTag.value ? "pending" : "not pending" )
                << " - " << getQuality(boolTag.quality) << "\r\n";

            if ( true == dataNode->isConfigValid())
            {
                try
                {
                    std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> > childDataPoints = dataNode->getChildDataPointEntities();

                    if (childDataPoints.size() > 0)
                    {
                        text << "\r\n" << "Child DataPoints:" << "\r\n";

                        std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> >::iterator itDp;

                        for (itDp = childDataPoints.begin(); itDp != childDataPoints.end(); itDp++)
                        {
                            text << (*itDp)->getAgentName() << ":" << (*itDp)->getName() << "\r\n";
                        }
                    }
                }
                catch (...)
                {
                    text << "Child DataPoints: *** not available ***" << "\r\n";
                }

                try
                {
                    std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> > childDataNodes = dataNode->getChildDataNodeEntities();

                    if (childDataNodes.size() > 0)
                    {
                        text << "\r\n" << "Child DataNodes:" << "\r\n";

                        std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> >::iterator itDn;

                        for (itDn = childDataNodes.begin(); itDn != childDataNodes.end(); itDn++)
                        {
                            text << (*itDn)->getAgentName() << ":" << (*itDn)->getName() << "\r\n";
                        }
                    }
                }
                catch (...)
                {
                    text << "Child DataNodes: *** not available ***" << "\r\n";
                }
            
                text << "\r\n" << "Diagnostic test information for DataPoint Status" << "\r\n";

                std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();
                std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator dpIt;
            
                for (dpIt = dataPoints.begin(); dpIt != dataPoints.end(); dpIt++)
                {
					TA_Base_Bus::DataPointProxySmartPtr* pDataPointPtr = *dpIt;
                    text << "DataPoint CorbaName:\t" << "\r\n";
                    text << (*pDataPointPtr)->getAgentName() << ":" << (*pDataPointPtr)->getEntityName() << "\r\n";

                    text << "Proxy Config Valid:\t\t" << ((*pDataPointPtr)->isConfigValid()?"Yes":"No") << "\r\n";

                    if ((*pDataPointPtr)->isConfigValid())
                    {
                        text << "Description:\t\t" << (*pDataPointPtr)->getDescription() << "\r\n";

                        text << "DataPoint Type\t\t" << (*pDataPointPtr)->getShortNameOfType()
                             << " (" << (*pDataPointPtr)->getTypeAsString() << " - ";

                        if ((*pDataPointPtr)->isWriteable())
                        {
                            text << "Output";
                        }
                        else
                        {
                            text << "Input";
                        }
                        text << ")" << "\r\n";

                        text << "Display Order:\t\t" << (*pDataPointPtr)->getDisplayOrder() << "\r\n";

                        if ((*pDataPointPtr)->isWriteable())
                        {
                            text << "Last Control:\t\t";
                        }
                        else
                        {
                            text << "Value:\t\t\t";
                        }

                        try
                        {
                            text << (*pDataPointPtr)->getValueAsString();
                            if ((*pDataPointPtr)->isNumber())
                            {
                                text << " (";
                                try
                                {
                                    text << (*pDataPointPtr)->getValueAsDouble();
                                }
                                catch (TA_Base_Core::ScadaProxyException&)
                                {
                                    text << "**";
                                }
                                text << ")";
                            }
                            else if ((*pDataPointPtr)->isBoolean())
                            {
                                text << " (";
                                try
                                {
                                    bool value = (*pDataPointPtr)->getValueAsBool();
                                    if (value)
                                    {
                                        text << "True";
                                    }
                                    else
                                    {
                                        text << "False";
                                    }
                                }
                                catch (TA_Base_Core::ScadaProxyException&)
                                {
                                    text << "**";
                                }
                                text << ")";
                            }
                        }
                        catch (TA_Base_Core::ScadaProxyException&)
                        {
                            text << "*** not available ***";
                        }
                        text << "\r\n";

                        if (!(*pDataPointPtr)->isWriteable())
                        {
                            text << "Timestamp:\t\t";

                            try
                            {
                                const unsigned int bufLen = 27;
                                char buf[bufLen];

                                time_t timestamp = (*pDataPointPtr)->getTimestamp();
                                struct tm newtime;
                                ACE_OS::localtime_r( &timestamp, &newtime ); 
                                text << ACE_OS::asctime_r( &newtime, buf, bufLen );
                            }
                            catch (TA_Base_Core::ScadaProxyException&)
                            {
                                text << "*** not available ***";
                            }
                        }
                        text << "\r\n";

                        text << "Data Type:\t\t";
                        if ((*pDataPointPtr)->isBoolean())
                        {
                            text << "Boolean";
                        }
                        else if ((*pDataPointPtr)->isDerived())
                        {
                            text << "Derived";
                        }
                        else if ((*pDataPointPtr)->isNumber())
                        {
                            text << "Numeric";
                        }
                        else if ((*pDataPointPtr)->isText())
                        {
                            text << "Text";
                        }
                        else
                        {
                            text << "Unknown";
                        }
                        text << "\r\n";
                            
                        if (!(*pDataPointPtr)->isWriteable())
                        {
                            text << "Quality:\t\t\t" << (*pDataPointPtr)->getQualityAsString()
                                 << " (" << (*pDataPointPtr)->getStatus() << ")";
                        }
                        else
                        {
                            try
                            {
                                text << "Control State:\t\t";
                                TA_Base_Bus::EDataPointControlState controlState = (*pDataPointPtr)->getControlState();

                                switch (controlState)
                                {
                                case TA_Base_Bus::noControl:
                                    text << "noControl";
                                    break;

                                case TA_Base_Bus::written:
                                    text << "written";
                                    break;

                                case TA_Base_Bus::pending:
                                    text << "pending";
                                    break;

                                case TA_Base_Bus::conditionalSuccess:
                                    text << "conditionalSuccess";
                                    break;

                                case TA_Base_Bus::success:
                                    text << "success";
                                    break;

                                case TA_Base_Bus::failed:
                                    text << "failed";
                                    break;

                                case TA_Base_Bus::LCC_failed:
                                    text << "LCC_failed";
                                    break;

                                case TA_Base_Bus::RCC_failed:
                                    text << "RCC_failed";
                                    break;

                                case TA_Base_Bus::comms_failed:
                                    text << "comms_failed";
                                    break;
                                }
                            }
                            catch (TA_Base_Core::ScadaProxyException&)
                            {
                                text << "*** not available ***";
                            }
                        }
                        text << "\r\n";
                    }
                    text << "\r\n";
                }
            }
            m_statusInfo.SetWindowText(text.str().c_str());

        }
        catch (...)
        {
        }

        FUNCTION_EXIT;
    }

    bool CStatusDialog::isUnsavedChangeOnTab()
    {
        FUNCTION_ENTRY ("isUnsavedChangeOnTab()");
        FUNCTION_EXIT;

        return false;
    }

    bool CStatusDialog::isOkToChangeTab()
    {
        FUNCTION_ENTRY ("isOkToChangeTab()");
        FUNCTION_EXIT;

        return true;
    }


    void CStatusDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
    {
        CDialog::OnShowWindow(bShow, nStatus);
        
        if (bShow)
        {
            updateDataNode(TA_Base_Bus::ConfigStateUpdate);
        }
    }

    void CStatusDialog::getWindowRect(CRect* rect)
    {
        GetWindowRect(rect);
    }

    unsigned int CStatusDialog::getMinimumHeight()
    {
        // No real minimum height - one of the other dialogs will be bigger.
        return 0;
    }       

    void CStatusDialog::resize(unsigned int height)
    {
        CRect editRect, dialogRect;
        m_statusInfo.GetWindowRect(&editRect);
        GetWindowRect(&dialogRect);

        editRect.NormalizeRect();
        dialogRect.NormalizeRect();

        int diff = height - dialogRect.Height();

        if (diff > 0)
        {
            editRect.InflateRect(0, 0, 0, diff);
            dialogRect.InflateRect(0, 0, 0, diff);
        }
        else
        {
            editRect.DeflateRect(0, 0, 0, -diff);
            dialogRect.DeflateRect(0, 0, 0, -diff);
        }

        m_statusInfo.SetWindowPos(NULL, 0, 0, editRect.Width(), editRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
        SetWindowPos(NULL, 0, 0, dialogRect.Width(), dialogRect.Height(), SWP_NOMOVE|SWP_NOZORDER);
    }

    void CStatusDialog::updatePermissions(/*TA_Base_Bus::RightsLibrary* rightsMgr*/)
    {
/*      m_accessAllowed = false;

        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        try
        {
            m_accessAllowed = rightsMgr->isActionPermittedOnResource(
                                            sessionId, 
                                            resourceId, 
                                            TA_Base_Bus::aca_IP_STATUS_TAB_ACCESS, 
                                            reason, 
                                            decisionModule);     
        }
        catch (...)
        {
        }
*/
    }


    const char * const CStatusDialog::getQuality( TA_Base_Bus::EQualityStatus quality ) const
    {
        switch (quality)
        {
        case TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR:
            return TA_Base_Bus::QUALITY_BAD_CONFIGURATION_ERROR_STRING;

        case TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED:
            return TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED_STRING;

        case TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE:
            return TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE_STRING;

        case TA_Base_Bus::QUALITY_BAD_SENSOR_FAILURE:
            return TA_Base_Bus::QUALITY_BAD_SENSOR_FAILURE_STRING;
            
        case TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE:
            return TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE_STRING;

        case TA_Base_Bus::QUALITY_BAD_COMM_FAILURE:
            return TA_Base_Bus::QUALITY_BAD_COMM_FAILURE_STRING;

        case TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE:
            return TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE_STRING;

        case TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON:
            return TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON_STRING;

        case TA_Base_Bus::QUALITY_UNCERTAINTY_LAST_USABLE_VALUE:
            return TA_Base_Bus::QUALITY_UNCERTAINTY_LAST_USABLE_VALUE_STRING;

        case TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE:
            return TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE_STRING;
            break;

        case TA_Base_Bus::QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED:
            return TA_Base_Bus::QUALITY_UNCERTAINTY_ENG_UNIT_EXCEEDED_STRING;

        case TA_Base_Bus::QUALITY_UNCERTAINTY_SUB_NORMAL:
            return TA_Base_Bus::QUALITY_UNCERTAINTY_SUB_NORMAL_STRING;

        case TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON:
            return TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON_STRING;

        case TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE:
            return TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE_STRING;

        case TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON:
        default:
            return TA_Base_Bus::QUALITY_BAD_NO_SPECIFIC_REASON_STRING;
        }
    }


    bool CStatusDialog::isAnyDataPointsDisplayedInTab() const
    {
        // no data point on the Notes page, default to true
        return true;
    }

	void CStatusDialog::resetList()
	{

	}
}
