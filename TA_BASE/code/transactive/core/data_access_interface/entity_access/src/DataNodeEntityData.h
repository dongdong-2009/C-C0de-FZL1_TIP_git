/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DataNodeEntityData.h $
  * @author Darren Sampson
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/22 10:48:00 $
  * Last modified by: $Author: haijun.li $
  * 
  * DataNodeEntityData provides access to DataNode entity defined in
  * the database.
  */

#ifndef DATANODE_ENTITY_DATA_H
#define DATANODE_ENTITY_DATA_H

#include <map>
#include <time.h>

#include "boost/shared_ptr.hpp"

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_Base_Core
{
    class DataNodeEntityData : public EntityData
    {
    
    public:

        enum ESynthesisType
        {
            Hierarchy,
            List,
            Both,
            None
        };

        DataNodeEntityData();
        DataNodeEntityData( unsigned long key );

        virtual ~DataNodeEntityData();

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

        //
        // operations specific to DataNodeEntityData
        //
        const std::string & getLabel();
        bool setLabel ( const std::string & newShortLabel );

        const std::string & getShortLabel();
        bool setShortLabel ( const std::string & newShortLabel );

        const std::string & getAssetName();
        bool setAssetName ( const std::string & newAssetName );

        int getDisplayOrder();
        bool setDisplayOrder ( int orderNumber );

        const std::string & getListOfChildEntities();
        std::vector < std::string > getParsedListOfChildEntities();
        bool setListOfChildEntities ( const std::string & newListOfChildEntities );

        const std::string & getSynthesisType();
        ESynthesisType getSynthesisTypeEnum();
        bool setSynthesisType ( const std::string & newSynthesisType );

		SharedIEntityDataList getChildDataPoints();
		SharedIEntityDataList getChildDataNodes();

		//Using for IEC104 Protocol 
		bool getPScadaFlag();
		bool setPScadaFlag ( bool newPScadaFlag );

    protected:

        /**
         * setParameterValues
         *
         * Set the specified parameters of the DataNode entity to the specified value
         *
         * @param parameters The map of new values for specified parameters
         *           The parameters map is in the form of Name->Value
         *
         * @return True if the operation is successfull, false otherwise
         */
        bool setParameterValues ( std::map < std::string, std::string > & parameters );

        std::string getStringData ( const std::string & parameterName );
        bool getBooleanData ( const std::string & parameterName );
        int getIntegerData ( const std::string & parameterName );

    private:
        DataNodeEntityData ( const DataNodeEntityData & obj );
        DataNodeEntityData & operator= ( const DataNodeEntityData &);

		void createSynthesisChild();
		void createHierarchyChild();
		void initChildEntity();
		void parseTheListForEntityName( const std::string & listOfChildEntities,
                                        std::vector < std::string > & namesOfChildEntities );
    
    protected:

        int m_displayOrder;
        std::string m_label;
        std::string m_shortLabel;
        std::string m_assetName;  
        std::string m_listOfChildEntities;
        std::string m_synthesisType;
		
		bool m_isChildInit;
		TA_Base_Core::ReEntrantThreadLockable	m_childInitLock;

		std::vector< boost::shared_ptr<IEntityData> > m_childDataPoints;
		std::vector< boost::shared_ptr<IEntityData> > m_childDataNodes;

    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
        static const std::string DATANODE_LABEL;
        static const std::string DATANODE_SHORT_LABEL;
        static const std::string DATANODE_ASSET_NAME;
        static const std::string DATANODE_DISPLAY_ORDER;
        static const std::string DATANODE_LIST_OF_CHILD_ENTITIES;
        static const std::string DATANODE_SYNTHESIS_TYPE;
        static const std::string DATANODE_SYNTHESIS_TYPE_HIERARCHY;
        static const std::string DATANODE_SYNTHESIS_TYPE_LIST;
        static const std::string DATANODE_SYNTHESIS_TYPE_BOTH;
        static const std::string DATANODE_SYNTHESIS_TYPE_NONE;
		static const std::string DATANODE_PSCADA_FLAG;   // Using for IEC 104 Protocol
    };

    typedef boost::shared_ptr<DataNodeEntityData> DataNodeEntityDataPtr;

} //close namespace TA_Base_Core

#endif
