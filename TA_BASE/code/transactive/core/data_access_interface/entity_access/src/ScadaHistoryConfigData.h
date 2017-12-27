#ifndef SCADAHISTORYCONFIGDATA_H_INCLUDED
#define SCADAHISTORYCONFIGDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ScadaHistoryConfigData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class ScadaHistoryConfigData : public EntityData
    {
    public:

        ScadaHistoryConfigData();

        ScadaHistoryConfigData(unsigned long key);

        static IEntityData* clone(unsigned long key)
        {
            return new ScadaHistoryConfigData(key);        
        }

        unsigned long getTimePeriod(); 
        
        std::string getBaseData();
        
        std::string getDataType();
        
        std::string getShortDescription();
        
        std::string getLongDescription();
        
        bool getArchiveFlag();
        
        unsigned long getStorageTime();
        
        std::string getStartOfDay();
       
        //int getQualityMask();
        
        std::string getTableName();

        std::string getProcedureName();

        std::string getType();
		static std::string getStaticType();
        
        
    /**
     * implement methods as required by the interfaces from
     * which this class inherits
     */
    private:

        ScadaHistoryConfigData(ScadaHistoryConfigData&);

        ScadaHistoryConfigData& operator=(ScadaHistoryConfigData&);

        void invalidate();

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string TIMEPERIOD;
        static const std::string BASEDATA;
        static const std::string DATATYPE;
        static const std::string SHORTDESCRIPTION;
        static const std::string LONGDESCRIPTION;
        static const std::string ARCHIVEFLAG;
        static const std::string STORAGETIME;
        static const std::string STARTOFDAY;
        //static const std::string QUALITYMASK;
        static const std::string TABLENAME;
        static const std::string PROCEDURENAME;
    };
    
    typedef boost::shared_ptr<ScadaHistoryConfigData> ScadaHistoryConfigDataPtr;
}

#endif // SCADAHISTORYCONFIGDATA_H_INCLUDED
