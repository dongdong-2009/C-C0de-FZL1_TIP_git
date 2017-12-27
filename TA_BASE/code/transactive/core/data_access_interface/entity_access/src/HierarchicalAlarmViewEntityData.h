/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/HierarchicalAlarmViewEntityData.h $
  * @author: Lizettte Lingo
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * DataNodeEntityData provides access to DataNode entity defined in
  * the database.
  */

#ifndef HIERARCHICALALARMVIEW_ENTITY_DATA_H
#define HIERARCHICALALARMVIEW_ENTITY_DATA_H

#include <map>
#include <time.h>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/naming/src/INamedObject.h"

 

namespace TA_Base_Core
{
    class HierarchicalAlarmViewEntityData : public EntityData
    {
    
    public:
        HierarchicalAlarmViewEntityData();
        HierarchicalAlarmViewEntityData( unsigned long key );

        virtual ~HierarchicalAlarmViewEntityData();

        //
        // operations required by IEntityData
        //
   
        /**
         * clone
         *
         * Returns a pointer to a new RTUEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the object.
         */
        static IEntityData* clone ( unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
        static std::string getStaticType();
		
		/**
		 * getXXXImplementerName()
		 * 
		 * Returns the name of the Implementer to be instantiated. 
		 * This must be fixed for each subsystem to be supported. 
		 * (e.g. Train subsystem will return "Train", Radio subsystem will return "Radio")
		 * This feature is for the future implementation of the HierarchicalAlarmViewer 
		 * configurable attributes to support more than one root nodes of different subsystems
		 *
		 */
		static std::string getTrainImplementer();
		static std::string getRadioImplementer();

        //
        // operations specific to HierarchicalAlarmViewEntityData
        //
        std::vector<std::string> getRootNodes();

		std::map<std::string, std::string> getRootNameNEndPoint(); 

		std::map<std::string,std::string> getRootNameNImplementer(); 

		std::vector<std::string> getColumnNames();  
		
		unsigned short getColumnSize(); 

       
    protected:

        /**
         * reload
         *
         * (Re)loads the parameter data from the database.
         *
         * @exception DatabaseException If there is a database error.
         * @exception DataException If a parameter value cannot be converted to the required
         *            type
         */
        void reload();

    private:
        HierarchicalAlarmViewEntityData ( const HierarchicalAlarmViewEntityData & obj ) {} 
        HierarchicalAlarmViewEntityData & operator= ( const HierarchicalAlarmViewEntityData &) {}

    
	    bool m_isValidData;
		unsigned short m_columnSize;
		unsigned short m_numberOfRoots;
		std::vector<std::string> m_columnNames;
		std::vector<std::string> m_rootNodes;
		std::map<std::string,std::string> m_rootsNImplementersMap;
		std::map<std::string,std::string> m_rootsNEndPointMap;  

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string COLUMN_SIZE;
		static const std::string COLUMN_NAME1;
		static const std::string COLUMN_NAME2;
		static const std::string COLUMN_NAME3;
		static const std::string NUMBER_OF_ROOTS;
		static const std::string ROOT_NODE_PREFIX;
		static const std::string IMPLEMENTER_PREFIX;
		static const std::string ENDPOINT_PREFIX;
        static const std::string TRAIN_IMPLEMENTER;
        static const std::string RADIO_IMPLEMENTER;
    };
    
    typedef boost::shared_ptr<HierarchicalAlarmViewEntityData> HierarchicalAlarmViewEntityDataPtr;
} //close namespace TA_Base_Core

#endif //HIERARCHICALALARMVIEW_ENTITY_DATA_H
