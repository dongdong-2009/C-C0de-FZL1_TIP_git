/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ScadaHistoryConfigData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "ScadaHistoryConfigData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"




namespace TA_Base_Core
{

    const std::string ScadaHistoryConfigData::ENTITY_TYPE       = "HistoryConfig";
    const std::string ScadaHistoryConfigData::TIMEPERIOD        = "TimePeriod";
    const std::string ScadaHistoryConfigData::BASEDATA          = "BaseData";
    const std::string ScadaHistoryConfigData::DATATYPE          = "DataType";
    const std::string ScadaHistoryConfigData::SHORTDESCRIPTION  = "ShortDescription";
    const std::string ScadaHistoryConfigData::LONGDESCRIPTION   = "LongDescription";
    const std::string ScadaHistoryConfigData::ARCHIVEFLAG       = "ArchiveFlag";
    const std::string ScadaHistoryConfigData::STORAGETIME       = "StorageTime";
    const std::string ScadaHistoryConfigData::STARTOFDAY        = "StartOfDay";
    //const std::string ScadaHistoryConfigData::QUALITYMASK("QualityMask");
    const std::string ScadaHistoryConfigData::TABLENAME         = "TableName";
    const std::string ScadaHistoryConfigData::PROCEDURENAME     = "ProcedureName";

    //const int DEFAULT_QUALITY_MASK = 240;


    ScadaHistoryConfigData::ScadaHistoryConfigData()
    {
    }


    ScadaHistoryConfigData::ScadaHistoryConfigData(unsigned long key)
        : EntityData(key, getStaticType())
    {
    }


    std::string ScadaHistoryConfigData::getType() 
    {
        return getStaticType();
    }


    std::string ScadaHistoryConfigData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    unsigned long ScadaHistoryConfigData::getTimePeriod() 
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );

		std::string value = getHelper()->getParameter(TIMEPERIOD);
		unsigned long timePeriod = 0;
		if (!value.empty())
		{
			timePeriod = EntityHelper::getUnsignedLongData(value);
		}

        return timePeriod;
    }


    std::string ScadaHistoryConfigData::getBaseData()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(BASEDATA);
    }


    std::string ScadaHistoryConfigData::getDataType()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(DATATYPE);
    }


    std::string ScadaHistoryConfigData::getShortDescription()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(SHORTDESCRIPTION);
    }


    std::string ScadaHistoryConfigData::getLongDescription()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(LONGDESCRIPTION);
    }


    bool ScadaHistoryConfigData::getArchiveFlag()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
		std::string value = getHelper()->getParameter(ARCHIVEFLAG);
		bool archiveFlag = false; 
		if (!value.empty())
		{
			archiveFlag = EntityHelper::getBooleanData(value);
		}

		return archiveFlag;
    }


    unsigned long ScadaHistoryConfigData::getStorageTime()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );

		std::string value = getHelper()->getParameter(STORAGETIME);
		unsigned long storageTime = 0;
		if (!value.empty())
		{
			storageTime = EntityHelper::getUnsignedLongData(value);
		}

		return storageTime;
    }


    std::string ScadaHistoryConfigData::getStartOfDay()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(STARTOFDAY);
    }

    std::string ScadaHistoryConfigData::getTableName()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(TABLENAME);
    }


    std::string ScadaHistoryConfigData::getProcedureName()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        return getHelper()->getParameter(PROCEDURENAME);
    }

    void ScadaHistoryConfigData::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "Entity Helper is NULL" );
        getHelper()->invalidate();
    }


} // TA_Base_Core
