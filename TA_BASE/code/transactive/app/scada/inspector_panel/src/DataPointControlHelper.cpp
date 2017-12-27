/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/DataPointControlHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/11/18 14:31:43 $
  * Last modified by:  $Author: jinmin.huang $
  *
  */
#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/DataPointControlHelper.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"

using namespace TA_Base_Bus;

namespace TA_Base_App
{
    bool DataPointControlHelper::isControlValueValid(
        TA_Base_Bus::DataPointProxySmartPtr &dataPoint, 
        const std::string& strValue)
    {
        try
		{	
			if (dataPoint->isDerived())
			{
				try
				{
					// This will throw an exception if the value is unknown
					dataPoint->getDerivedStateAsUnsignedLong(strValue);
				}
				catch (TA_Base_Core::ScadaProxyException&)
				{
					// Invalid selection
					std::stringstream message;
					message << dataPoint->getDescription() << ": 值 \"" << strValue << "\" 不合法.";

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

					// wenching++
					// bug 811 (TD12603)
					std::string* statusLineStr = new std::string(message.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
					// ++wenching
					// bug 811 (TD12603)
					//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );

					return false;
				}

				
			}
			else if (dataPoint->isNumber())
			{
				// Do range checking if this datapoint is numeric

				double doubleValue = 0;
				std::istringstream valueStream(strValue);

				if (valueStream >> doubleValue)
     			{
					if (doubleValue > dataPoint->getLimit(false)
						|| doubleValue < dataPoint->getLimit(true))
					{
						std::stringstream message;

						//  [3/14/2010 benxun]
						//message << dataPoint->getDescription() << ": Value \"" << strValue << "\" is not in range.";
						message << dataPoint->getDescription() << ": 值 \"" << strValue << "\" 不在有效范围内.";
						//  [3/14/2010 benxun]

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

						std::string* statusLineStr = new std::string(message.str());
						AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
						//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );

						std::stringstream message2;
						//  [3/14/2010 benxun]
						/*
						message2 << dataPoint->getDescription() << ": Valid range is from ";
						message2 << dataPoint->getLimit(true) << " to ";
						message2 << dataPoint->getLimit(false) << " ";
						message2 << dataPoint->getUnit() << ".";
						*/

						message2 << dataPoint->getDescription() << ": 有效范围从 ";
						message2 << dataPoint->getLimit(true) << " 到 ";
						message2 << dataPoint->getLimit(false) << " ";
						message2 << dataPoint->getUnit() << ".";

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message2.str().c_str());

						std::string* statusLineStr2 = new std::string(message2.str());
						AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr2), 0);
						//CInspectorPanelDlg::addStatusLine(message2.str().c_str()); 

						return false;
					}
				}
				else
				{
					std::stringstream message;
					//  [3/14/2010 benxun]
					//message << dataPoint->getDescription() << ": Value \"" << strValue << "\" is not a valid number.";
					message << dataPoint->getDescription() << ": 值 \"" << strValue << "\" 不是一个有效数字.";
					//  [3/14/2010 benxun]

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str());

					std::string* statusLineStr = new std::string(message.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
					//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );
				
					return false;
				}
			}
			// Make sure this is a valid value for boolean
			else if (dataPoint->isBoolean())
			{
				std::string trueLabel = "";
				std::string falseLabel = "";

				try
				{
					trueLabel = dataPoint->getBooleanLabel(true);
				}
				catch (...)
				{
				}

				try
				{
					falseLabel = dataPoint->getBooleanLabel(false);
				}
				catch (...)
				{
				}

				if (strValue == "" || (strValue != trueLabel && strValue != falseLabel))
				{
					// Invalid selection
					std::stringstream message;
					message << dataPoint->getDescription() << ": 值 \"" << strValue << "\" 不合法.";

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

					// wenching++
					// bug 811 (TD12603)
					std::string* statusLineStr = new std::string(message.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

					// wenching++
					// bug 811 (TD12603)
					//std::string* statusLineStr = new std::string(message.str());
					//AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
					// ++wenching
					// bug 811 (TD12603)
					//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );

					return false;
				}
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());

			// Not always available
			std::string message = "DataPoint \"" + dataPoint->getDescription() + "\" is not available.";
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", message.c_str());			

			//CInspectorPanelDlg::addStatusLine(message.c_str());

			return false;
		}

        return true;
    }

} // namespace
