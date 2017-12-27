/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/SNMP_library/src/DataTranslater.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/21 16:51:05 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "DataTranslater.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	DataTranslater::DataTranslater()
	{       
	}

	DataTranslater::~DataTranslater()
	{
    }

	void DataTranslater::translateToDpValue(DpValue& dpValue, const /*Snmp_pp::*/Vb& vb)
	{
		const char* pCharValue = vb.get_printable_value();
		if (0 == pCharValue)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "get_printable_value from vb is zero.");
			return ;
		}
		
		switch(dpValue.getType())
		{
		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			{
				double value = 0;
				int i = 0;
				long l = 0;
				unsigned long ul = 0;
				if (SNMP_CLASS_SUCCESS == vb.get_value(i))
				{
					value = i;
				}
				else if (SNMP_CLASS_SUCCESS == vb.get_value(l))
				{
					value = l;
				}
				else if (SNMP_CLASS_SUCCESS == vb.get_value(ul))
				{
					value = ul;
				}
				else
				{
					value = atof(pCharValue);
				}
				dpValue.setFloat(value);
			}
			break;
		case DPT_ENUM_TYPE:
		case DPT_BOOL_TYPE:
			{
				long value = 0;
				int i = 0;
				long l = 0;
				unsigned long ul = 0;
				if (SNMP_CLASS_SUCCESS == vb.get_value(i))
				{
					value = i;
				}
				else if (SNMP_CLASS_SUCCESS == vb.get_value(l))
				{
					value = l;
				}
				else if (SNMP_CLASS_SUCCESS == vb.get_value(ul))
				{
					value = ul;
				}
				else
				{
					value = atol(pCharValue);
				}

				if (DPT_ENUM_TYPE == dpValue.getType())
				{
					dpValue.setEnum(value);
				}
				else
				{
					dpValue.setBoolean(value != 0);
				}
			}
			break;

		case DPT_TEXT_TYPE:
			{
				std::string value("");
				char buf[2048] = {0};
				unsigned char* pStr = (unsigned char*)buf;
				unsigned long len = 0, maxLen = 2048;
				if (SNMP_CLASS_SUCCESS == vb.get_value(pStr, len, maxLen))
				{
					if (maxLen == len)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "the value length is big then the max length:%d", maxLen);
						buf[maxLen - 1] = 0;
					}
					value = buf;
				}
				else
				{
					value = pCharValue;
				}
				dpValue.setText(value);
			}
			break;
		case DPT_NULL_TYPE:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "datapoint value type is null type, failed to set value");
			break;
		}
	}

	void DataTranslater::translateToSNMPVb(const DpValue& dpValue, /*Snmp_pp::*/Vb& vb)
	{
		switch(dpValue.getType())
		{
		case DPT_INT16_TYPE:
		case DPT_INT32_TYPE:
		case DPT_INT16S_TYPE:
		case DPT_INT32S_TYPE:
			{
				int i = int(dpValue.getFloat());
				vb.set_value(i);
				break;
			}
		case DPT_UINT16_TYPE:
		case DPT_UINT32_TYPE:
		case DPT_IEEE32_TYPE:
			{
				unsigned long ul = (unsigned long) (dpValue.getFloat());
				vb.set_value(ul);
				break;
			}
			break;
		case DPT_ENUM_TYPE:
			{
				long e = dpValue.getEnum();
				vb.set_value(e);
				break;
			}
			break;

		case DPT_BOOL_TYPE:
			{
				int b = dpValue.getBoolean();
				vb.set_value(b);
				break;
			}
			break;

		case DPT_TEXT_TYPE:
			{
				std::string s = dpValue.getText();
				vb.set_value(s.c_str());
				break;
			}
			break;
		case DPT_NULL_TYPE:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "datapoint value type is null type, failed to set value");
			break;
		}
	}
}
