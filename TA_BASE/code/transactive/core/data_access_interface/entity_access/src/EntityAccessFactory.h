/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:37:35 $
  * Last modified by: $Author: huirong.luo $
  * 
  * EntityAccessFactory is a singleton class that is used to retrieve entities, and entity strings
  * from the database. Entities are returned as IEntityData objects, while entity strings are
  * contian the CORBA IOR for locating the entity in the naming service.
  */

#if !defined(EntityAccessFactory_BE8A9418_08C4_44ff_A791_B7082300A43B__INCLUDED_)
#define EntityAccessFactory_BE8A9418_08C4_44ff_A791_B7082300A43B__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250)
#endif // defined _MSC_VER


#include <map>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/ConfigEntity.h"
#include "core/naming/src/INamedObject.h"

namespace TA_Base_Core
{
    typedef std::map<unsigned long, IEntityData*> IEntityKeyToEntityMap;

    class EntityAccessFactory
    {
        typedef std::map< unsigned long, EntityData* >		EntityKeyToEntityMap;
        typedef EntityKeyToEntityMap::iterator				EntityKeyToEntityMapIter;

    private:

        EntityAccessFactory() ;
        EntityAccessFactory& operator=(const EntityAccessFactory &);
        EntityAccessFactory( const EntityAccessFactory& theEntityAccessFactory);

    public:
        /**
         * ~EntityAccessFactory
         *
         * Destruct the object by deleting the concrete classes used to determine which
         * entity object to create.
         */
        virtual ~EntityAccessFactory();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an EntityAccessFactory object.
         */
        static EntityAccessFactory& getInstance();


        /**
         * getEntity
         *
         * Retrieves the specified entity, and returns the data as an object conforming
         * to the IEntityData interface.
         *
         * @param name The name of the entity to retrieve.
         *
         * @param readWrite A boolean parameter specifying if the returned object should be 
         * a "standard" entity object or a configuration object.
         *
         * @return A pointer to the IEntityData object representing the specified entity.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityData* getEntity(const std::string& name, const bool readWrite = false);
		IEntityData* getEntity2(const std::string& name, const bool readWrite = false);
		//liqiang++
		IEntityDataList getEntityWithNameInToken(const std::string& name, const bool readWrite = false);

        /**
         * getEntity
         *
         * Retrieves the specified entity, and returns the data as an object conforming
         * to the IEntityData interface.
         *
         * @param key The pkey into the ENTITY table of the entity to retrieve.
         *
         * @param readWrite A boolean parameter specifying if the returned object should be 
         * a "standard" entity object or a configuration object.
         *
         * @return A pointer to the IEntityData object representing the specified entity.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityData* getEntity(const unsigned long key, const bool readWrite = false);

        /**
         * getCorbaNameOfEntity
         *
         * Retrieves the TDNS id for the specified entity. This can be used to locate the 
         * entity in the TDNS service (useful for GUIs that retrieve entity settings from
         * the CORBA object instead of from the database).
         *
         * @param name The name of the entity to retrieve.
         *
         * @return A CorbaName struct representing the specified entities location in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaName getCorbaNameOfEntity( const std::string& name, const bool isAgentName );	

        /**
         * getCorbaNameOfEntity
         *
         * Retrieves the TDNS id for the specified entity. This can be used to locate the 
         * entity in the TDNS service (useful for GUIs that retrieve entity settings from
         * the CORBA object instead of from the database).
         *
         * @param key The pkey into the ENTITY table of the entity to retrieve.
         *
         * @return A CorbaName struct representing the specified entities location in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaName getCorbaNameOfEntity( const unsigned long key, const bool isAgentKey );

		//Mintao++ TD16762
        /**
         * getAllChildEntitiesOfType
         *
         * Retrieves all related child entities of the specified type.
         *
         * @param type The type name of entity to retrieve.
         *
         * @param readWrite A boolean parameter specifying if the returned objects should be 
         * "standard" entity objects or configuration objects.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getAllChildEntitiesOfType(const std::string& type, const bool readWrite /*=false*/);


        /** 
          * getAllChildEntitiesOfAgentDataNode
          *
          * From the datanodes owned by the given agent,
          * Get the child entities of those datanodes
          * optionally exclude entities owned by the given agent
          * as within the agent these will already be loaded
          *
          * this should really be in DataNodeAccessFactory,
          * but it seems the structure of EntityAccessFactory prevents it
          * as it needs to be here to take advantage of the bulk loading
          *
          * @param agentKey the agent to load for
          * @param excludeAgentEntities if true, entities with agent key matching the given key wont be loaded
          *
          * @return a list of shared pointers to entities
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *            4) A PKEY is not a number
          */
		SharedIEntityDataList getAllChildEntitiesOfAgentDataNode(unsigned long agentKey, bool excludeAgentEntities);

		//Mintao++ TD16762

		//Mintao++ TD16609
		/**
         * getEquipmentsAtLocation
         *
         * Retrieves the equipment id for all entities of the specified type at the specified location.
		 * This can be used to locate the entities in the TDNS (useful for GUIs that retrieve 
         * entity settings from the CORBA objects instead of from the database).
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
		 * @param locationKey The location to find entities of the specific type
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
	    IEntityDataList getEquipmentsAtLocation( const unsigned long locationKey );
		//Mintao++ TD16609

        //TD16889 Mintao 
	    /**
	      * isEquipmentDataNode
	      * 
	      * check whether it is equipment datanode
	      * 
	      * @return bool 
	      * @param : unsigned long entitykey
	      * 
	      * @exception <exceptions> Optional
	      */
	    bool isEquipmentDataNode( unsigned long entitykey);
        //TD16889 Mintao 
        
        /**
         * getEntitiesOfType
         *
         * Retrieves all entities of the specified type.
         *
         * @param type The type name of entity to retrieve.
         *
         * @param readWrite A boolean parameter specifying if the returned objects should be 
         * "standard" entity objects or configuration objects.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getEntitiesOfType(const std::string& type, const bool readWrite = false, const bool loadParam = true, const bool isOnly = false );
		//jinhua++
		IEntityDataList getTTISLibraryVersionEntityFromNamelike( const std::string& entiyName );

        /**
         * getEntitiesOfTypeWithNameLikeToken
         *
         * Retrieves all entities of the specified type whose entity name matches the given
         * token search string.  The implementation will execute an SQL statement on the lines of:
         *
         *    select ... where NAME like '<token>';
         *
         * @param type The type name of entity to retrieve.
         *
         * @param token The token search string used to match datapoint entities.  The token 
         *              should be compatible with the SQL like statement.
         *
         * @param readWrite A boolean parameter specifying if the returned objects should be 
         * "standard" entity objects or configuration objects.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getEntitiesOfTypeWithNameLikeToken(const std::string& type, const std::string& token,
														   const bool readWrite = false);

		// Same with getEntitiesOfTypeWithNameLikeToken but not convert token to %token%
        IEntityDataList getEntitiesOfTypeWithNameStrictLikeToken(const std::string& type, const std::string& token, const bool readWrite = false);

		IEntityDataList getEntitiesOfTypeWithAddressLikeToken(const std::string& type, const std::string& token, const bool readWrite =false);

        /**
         * getCorbaNamesOfType
         *
         * Retrieves the TDNS id for all entities of the specified type whose entity name 
         * matches the given token search string. This can be used to locate the entities 
         * in the TDNS (useful for GUIs that retrieve entity settings from the CORBA objects 
         * instead of from the database).  The implementation will execute an SQL statement 
         * on the lines of:
         *
         *    select ... where NAME like '<token>';
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
         *
         * @param token The token search string used to match entity names.  The token 
         *              should be compatible with the SQL like statement.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaNameList getCorbaNamesOfType( const std::string& type, const bool isAgentType );

        
        /**
         * getCorbaNamesOfType
         *
         * Retrieves the TDNS id for all entities of the specified type. This can be used 
         * to locate the entities in the TDNS (useful for GUIs that retrieve 
         * entity settings from the CORBA objects instead of from the database).
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaNameList getCorbaNamesOfTypeWithNameLikeToken(const std::string& type, const std::string& token, const bool isAgentType);

        /**
         * getEntitiesOfTypeAtLocation
         *
         * Retrieves all entities of the specified type at the specified location
         *
         * @param type The type name of the entities to retrieve.
		 * @param location The location key of the location
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type at the specified location
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getEntitiesOfTypeAtLocation( const std::string& type, unsigned long locationKey, const bool loadParam = true);

		IEntityDataList getEntitiesOfTypeAtLocation( const std::string& type, std::vector<unsigned long> locationKeys, const bool loadParam = true);

        IEntityDataList getFasAlarmEntities();

        /**
         * getEntitiesOfTypeAtLocation
         *
         * Retrieves all entities of the specified type at the specified location
         *
         * @param type The type name of the entities to retrieve.
		 * @param location The location name
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type at the specified location
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getEntitiesOfTypeAtLocation( const std::string& type, const std::string& locationName);


		IEntityDataList getEntitiesOfTypeAtLocationWithNameLikeToken(const std::string& type, 
             const unsigned long locationKey,
             const std::string& token);

        /**
         * getEntitiesOfTypeWithCustomToken
         *
         * Retrieves all entities of the specified type with additional tokens
		 * e.g. and pkey in (select entity_id_dp from sc_opc_tag_alias)
         *
         * @param type The type name of the entities to retrieve.
		 * @param customToken The custom token used to provide flexible entity filter.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type at the specified location
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
		IEntityDataList getEntitiesOfTypeWithCustomToken(const std::string& type, 
             const std::string& customToken, const bool loadParam = true);

		/**
         * getEntitiesOfTypeAtLocationWithCustomToken
         *
         * Retrieves all entities of the specified type in location with additional tokens
		 * e.g. and e.pkey in (select entity_id_dp from sc_opc_tag_alias)
         *
         * @param type The type name of the entities to retrieve.
		 * @param locationKey The location which the entities to retrieve belong to.
		 * @param customToken The custom token used to provide flexible entity filter.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type at the specified location
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
		IEntityDataList getEntitiesOfTypeAtLocationWithCustomToken(
			 const std::string& type, 
             const unsigned long locationKey,
			 const std::string& customToken,
			 const bool loadParam = true);
		/**
         * getCorbaNamesOfTypeAtLocation
         *
         * Retrieves the TDNS id for all entities of the specified type.
		 * This can be used to locate the entities in the TDNS (useful for GUIs that retrieve 
         * entity settings from the CORBA objects instead of from the database).
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfTypeAtAllLocation( const std::string& type, const bool isAgentType );

		/**
         * getCorbaNamesOfTypeAtLocation
         *
         * Retrieves the TDNS id for all entities of the specified type at the specified location.
		 * This can be used to locate the entities in the TDNS (useful for GUIs that retrieve 
         * entity settings from the CORBA objects instead of from the database).
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
		 * @param locationKey The location to find entities of the specific type
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfTypeAtLocation( const std::string& type,
			const unsigned long locationKey, const bool isAgentType );

		/**
         * getCorbaNamesOfTypeAtLocation
         *
         * Retrieves the TDNS id for all entities of the specified type at the specified location.
		 * This can be used to locate the entities in the TDNS (useful for GUIs that retrieve 
         * entity settings from the CORBA objects instead of from the database).
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
		 * @param locationName The location to find entities of the specific type
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfTypeAtLocation( const std::string& type,
			const std::string& locationName, const bool isAgentType );

		/**
         * getCorbaNamesOfTypeAtLocationWithNameLikeToken
         *
         * Retrieves the TDNS id for all entities of the specified type whose entity name 
         * matches the given token search string and whose location matches the given 
         * locationKey. This can be used to locate the entities in the TDNS (useful for 
         * GUIs that retrieve entity settings from the CORBA objects instead of from the 
         * database).  The implementation will execute an SQL statement 
         * on the lines of:
         *
         *    select ... where NAME like '<token>'
         *                 and location = <locationKey>
         *                 and typekey = 
         *                 (select pkey from entitytype where name = '<type>')
         *
         * @param type The name of the type of entity for which to retrieve CorbaNames.
		 * @param locationKey The location to find entities of the specific type
         * @param token The token search string used to match entity names.  The token 
         *              should be compatible with the SQL like statement.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            3) Entity type is not recognised
         */
        CorbaNameList getCorbaNamesOfTypeAtLocationWithNameLikeToken
            (const std::string& type, 
             const unsigned long locationKey,
             const std::string& token, const bool isAgentType);

		/**
         * getEntitiesOfTypeInPhysicalSubsystem
         *
         * Retrieves all entities of the specified type in the specified physical subsystem
         *
         * @param type The type name of the entities to retrieve.
		 * @param physicalSubsystemKey The physical subsystem of entities to retrieve
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the entities of the specified type at the specified location
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getEntitiesOfTypeInPhysicalSubsystem( const std::string& type, unsigned long physicalSubsystemKey, 
															  const bool readWrite = false );
		
		/**
         * getCorbaNamesOfTypeInPhysicalSubsystem
         *
         * Retrieves the TDNS id for all entities of the specified type in the specified physical subsystem
		 * This can be used to locate the entities in the TDNS (useful for GUIs that retrieve 
         * entity settings from the CORBA objects instead of from the database).
         *
         * @param type The type name of the entities to retrieve.
		 * @param physicalSubsystemKey The physical subsystem of entities to retrieve
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfTypeInPhysicalSubsystem( const std::string& type, unsigned long physicalSubsystemKey, const bool isAgentType );

        /**
         * getChildEntitiesOf
         *
         * Retrieves all entities that have the specified entity as their parent.
         *
         * @param name The name of the entity for which to retrieve the children.
         *
         * @param readWrite A boolean parameter specifying if the returned objects should be 
         * "standard" entity objects or configuration objects.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the children of the specified entity.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getChildEntitiesOf(const std::string& name, const bool readWrite = false);

        /**
         * getChildEntitiesOf
         *
         * Retrieves all entities that have the specified entity as their parent.
         *
         * @param key The pkey into the ENTITY table of the entity for which to retrieve the children.
         *
         * @param readWrite A boolean parameter specifying if the returned objects should be 
         * "standard" entity objects or configuration objects.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the children of the specified entity.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getChildEntitiesOf(const unsigned long key, const bool readWrite = false);
        std::vector<IEntityDataList> getChildEntitiesOf(const std::vector<unsigned long> keys, const bool readWrite = false);

        /**
         * getCorbaNamesOfChildren
         *
         * Retrieves the TDNS ID for all child entieies of the specified entity. 
         * This can be used to locate the entities in the naming service (useful for 
         * GUIs that retrieve entity settings from the CORBA objects instead of from 
         * the database).
         *
         * @param name The name of the parent entity for which to retrieve child TDNS ids.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfChildren( const std::string& name );

        /**
         * getCorbaNamesOfChildren
         *
         * Retrieves the TDNS ID for all child entieies of the specified entity. 
         * This can be used to locate the entities in the naming service (useful for 
         * GUIs that retrieve entity settings from the CORBA objects instead of from 
         * the database).
         *
         * @param key The pkey of the parent entity for which to retrieve child TDNS ids.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaNameList getCorbaNamesOfChildren( const unsigned long key );

		/**
         * getCorbaNameOfParent
         *
         * Retrieves the TDNS ID for the parent of the specified entity. 
         * This can be used to locate the entities in the naming service (useful for 
         * GUIs that retrieve entity settings from the CORBA objects instead of from 
         * the database).
         *
         * @param name The name of a child entity for which to retrieve parent TDNS id.
         *
         * @return  A CorbaName struct representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaName getCorbaNameOfParent( const std::string& name );

        /**
         * getCorbaNameOfParent
         *
         * Retrieves the TDNS ID for the parent the specified entity. 
         * This can be used to locate the entities in the naming service (useful for 
         * GUIs that retrieve entity settings from the CORBA objects instead of from 
         * the database).
         *
         * @param key The pkey of a child entity for which to retrieve child TDNS ids.
         *
         * @return A CorbaName struct representing the specified entity location in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaName getCorbaNameOfParent( const unsigned long key );

        /**
         * getChildEntitiesOfType
         *
         * Retrieves all entities that have parents of the specified type.
         *
         * @param type The name of the type of entities for which to retrieve the children.
         *
         * @param readWrite A boolean parameter specifying if the returned objects should be 
         * "standard" entity objects or configuration objects.
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the children of all entities of the specified type.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getChildEntitiesOfType(const std::string& type, const bool readWrite = false);

        /**
         * getCorbaNamesOfChildrenOfType
         *
         * Retrieves the TDNS ID for all child entieies of all entities of the specified type. 
         * This can be used to locate the entities in the naming service (useful for 
         * GUIs that retrieve entity settings from the CORBA objects instead of from 
         * the database).
         *
         * @param type The name of the type of entities for which to retrieve the children.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaNameList getCorbaNamesOfChildrenOfType( const std::string& type );

        /**
         * getCorbaNamesOfChildrenOfTypeAtLocation
         *
         * Retrieves the TDNS ID for all child entities of all entities of the specified type,
		 * At the specified location. This can be used to locate the entities in the naming
		 * service (useful for GUIs that retrieve entity settings from the CORBA objects 
		 * instead of from the database).
         *
         * @param type The name of the type of entities for which to retrieve the children.
		 * @param locationKey the location of the children
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
        CorbaNameList getCorbaNamesOfChildrenOfTypeAtLocation( const std::string& type,
			unsigned long locationKey );

		/**
         * getChildEntitiesOfEntityOfType
         *
         * Retrieves all entities that have the specified entity as their parent that are of the specified type
         *
         * @param name The name of the entity for which to retrieve the children.
		 * @param type the type of child entities to retrieve
         * @param readWrite whether or not write access to the data is required
         *
         * @return A vector of pointers to the IEntityData objects representing 
         * the children of the specified type of the specified entity

         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IEntityData objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         *            4) A PKEY is not a number
         */
        IEntityDataList getChildEntitiesOfEntityOfType(
            const std::string& name,
            const std::string& type,
            bool readWrite = false);

        /**
         * getCorbaNamesOfChildrenOfEntityOfType
         *
         * Retrieves the CorbaName objects for all children of an entity that have the specified type.
         *
         * @param name The name of the parent for which to retrieve the children.
		 * @param type The type of child entities
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfChildrenOfEntityOfType( const std::string& name, const std::string& type );

		/**
         * getDescendantsOfAgent
         *
         * Retrieves the CorbaNames for all descendants of an agent. Descendants
		 * of an agent are identified by having the same agentKey as the pkey of the agent itself.
		 * Retrieves the TDNS ID for all child entities of all entities of the specified type,
		 * At the specified location. This can be used to locate the entities in the naming
		 * service (useful for GUIs that retrieve entity settings from the CORBA objects 
		 * instead of from the database).
         *
         * @param agentName The name of the agent for which to retrieve the entities.
         *
         * @return vector of IEntityData* representing the descendants of the agent
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		SharedIEntityDataList getDescendantsOfAgent( const unsigned long key );
		SharedIEntityDataList getDescendantsOfAgent( const std::string& agentName );

		/**
         * getDataNodeNotInAgentButChildDPInAgent
         *
         * Retrieves the DataNode entities which DataPoint children's agent key is @param agentKey, but their agent key
		 * are not @param agentKey
         *
         * @param agentKey The key of the agent for which to retrieve the entities.
         *
		 * @param fetchRowNo user defined row no fetch back from database each time,default FETCH_DEFAULT_ROWS
		 *
         * @return vector of IEntityData*
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		IEntityDataList getDataNodeNotInAgentButChildDPInAgent(unsigned long agentKey);
        /**
         * getCorbaNamesOfDescendantsOfAgent
         *
         * Retrieves the CorbaName objects for all descendants of an agent. Descendants
		 * of an agent are identified by having the same agentKey as the pkey of the agent itself.
         *
         * @param agentName The name of the agent for which to retrieve the entities.
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfDescendantsOfAgent( const std::string& agentName );

		/**
         * getDescendantsOfAgentOfType
         *
         * Retrieves the IEntityData objects for all descendants of an agent it they are
		 * of the specificed type. Descendants of an agent are identified by having the 
		 * same agentKey as the pkey of the agent itself.
         *
         * @param agentName The name of the agent for which to retrieve the entities.
		 * @param typeName The name of the type of entities you want to retrieve
         *
         * @return vector of IEntityData* representing the descendants of the agent of
		 *				the required type
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		IEntityDataList getDescendantsOfAgentOfType( const std::string& agentName,
			const std::string& typeName);

		/**
         * getCorbaNamesOfDescendantsOfAgentOfType
         *
         * Retrieves the CorbaName objects for all descendants of an agent. Descendants
		 * of an agent are identified by having the same agentKey as the pkey of the agent itself.
         *
         * @param agentName The name of the agent for which to retrieve the CorbaNames.
		 * @param typeName	The type of the entity to load
         *
         * @return vector of CorbaName structs representing the specified entity locations in the TDNS.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
         */
		CorbaNameList getCorbaNamesOfDescendantsOfAgentOfType( const std::string& agentName, 
			const std::string& typeName );

		/** 
		  * getParameter
		  *
		  * This function will return the specified parameter as the zeroth row of the 
		  * IData object. It is the callers responsibility to delete memory and to
		  * know the type (unsigned long, string, bool, etc) of the return parameter.
		  *
		  * ie: data->getStringData( 0, "VALUE" ); // where "VALUE" is fixed
		  *
		  * @return IData* the data
		  *
		  * @param  std::string& entityName The name of the entity whose parameter is required
		  * @param std::string& parameterName The name of the parameter
		  *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException Thrown for one of the following reasons:
         *            1) More than one entity matches specification
         *            2) No entity matches specification
         *            3) Entity type is not recognised
		  */
		IData* getParameter( const std::string& entityName, const std::string& parameterName );

        /**
         * createEntity
         * 
         * Creates a new configuration entity object of the specified type.
         *
         * @param type The type of the entity to create. This will be used to read the
         * default parameters from the database
         *
         * @return The configuration entity object as a pointer to an IConfigEntity object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigEntity* createEntity(std::string type);


        /**
         * copyEntity
         * 
         * Creates a new configuration entity object using the entity passed in
         *
         * @param IConfigEntity* The entity to copy
         *
         * @return The configuration entity object as a pointer to an IConfigEntity object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigEntity* EntityAccessFactory::copyEntity(const IConfigEntity* entityToCopy);


        /**
         * getEntityTypes
         *
         * Retrieves the entity types from the database and returns them in a vector of names and keys
         *
         * @return map<unsigned long, string> - A list of all entity type names and keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException
         */
        std::map<unsigned long, std::string> getEntityTypes();


        /**
         * getChildTypesOf
         *
         * This returns all child entity types of the entity type passed in
         *
         * @param string - The name of the entity type to retrieve child types for
         *
         * @return map<unsigned long, string> - A list of all entity type names and keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException
         */
        std::map<unsigned long, std::string> getChildTypesOf(const std::string& entityType);

		/**
         * getTypeOf
         *
         * This returns the entity type of the entity passed in
         *
         * @param string - The name of the entity to retrieve type for
         *
         * @return string - the entity type name
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         * @exception DataException
         */
        std::string getTypeOf(const std::string& name);

		std::string getParameterDefaultValue(const std::string& name, unsigned long typeKey);


		/**
		  * getEntities
		  * 
		  * get the entities which entity key in a set
		  *
		  * Note: in SQL the in only support max size 256.
		  * so make sure the set's size is less than 256.
		  * 
		  * @return IEntityDataList 
		  * @param : const std::vector< std::string > entityKeySet
		  * 
		  * @exception <exceptions> Optional
		  */
        IEntityDataList getEntities(const std::vector< std::string >& entityNameSet);
        IEntityDataList getEntities(const std::vector<unsigned long>& entityKeySet);
        IEntityKeyToEntityMap getKeyToEntityMap(const std::vector<unsigned long>& entityNameSet);

        /** 
          * getEntitiesByName
          *
          * Given a list of entity names, load those entities
          * similar to above, but handles the name list > 256
          *
          * @param entityNameList the list of names.
          *                       If > 256, multiple queries will be executed
          *
          * @return the list of matching entities
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
          * @exception DataException Thrown for one of the following reasons:
          *            1) More than one entity matches specification
          *            2) No entity matches specification
          *            3) Entity type is not recognised
          *            4) A PKEY is not a number
          */
        SharedIEntityDataList getEntitiesByName(const std::vector<std::string>& entityNameList);

		 /** 
          * getAgentNamesOfTypeAtLocation
          *
          * Given a list of agent names, which hold entity of entityType = entityTypeName at specified location
          *
          * @param entityType : the entity type which need get its agent names
          *   
		  * @param locationKey : the location key which those entities belong
		  *                      if locationKey = 0 means all location
          *
          * @return the list of matching agent names
          *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          *
		  */

		std::vector<std::string> getAgentNamesOfTypeAtLocation(const std::string entityType, unsigned long locationKey);

    private:

        /**
         * setUpMap
         *
         * Creates the map of typename->IEntityData* used to determine which concrete entity
         * class to create. This method could be moved externally, or even altered to read
         * from a file if more dynamic behavior was desired.
         */
        void setUpMap();

        /**
         * createEntityFromType
         *
         * Creates a concrete entity class based upon the type name retrieved from the database
         *
         * @param type The type name of entity class to create
         *
         * @param key The key to the entity. Used to construct the entity with key data.
         *
         * @return A pointer to an IEntityData object that contains the concrete entity class
         * corresponding to the specified type.
         */
        IEntityData* createEntityFromType(std::string type,unsigned long key);

        /**
         * getSingularData
         *
         * Executes the sql statement, and checks the number of returned rows in the IData object.
         * If the number returned is not 1, an exception is raised.
         */
        IData* getSingularData(IDatabase* pDatabase, SQLStatement& sql,std::vector<std::string> columnNames,
            std::string itemSpecification);

		/** 
		  * getCorbaNames
		  *
		  * This function will execute the sql and return a vector of CorbaNames
		  *
		  * @param  std::string& sql
		  * @param CorbaNameList& corbaNames
		  */
		void getCorbaNames( IDatabase* pDatabase, const SQLStatement& sql, CorbaNameList& corbaNames );

		/**
		 * getEntityData
		 * 
		 * Retrieves entity data for a given list of entity keys.
		 * The sql must select all the default parameters, including pkey, and type name
		 *
		 * @param sql query to implement
		 * @param entityData	the resultant entity data is inserted into this vector	
		 * @param readWrite		whether or not write access to the data is required
		 */ 
		void getEntityData( IDatabase* pDatabase, const SQLStatement& sql, IEntityDataList& entityData, bool readWrite = false );


        /** 
          * getEntityDataWithParameterValues
          *
          * Load all the entities and the parameter values specified
          * by the given sql. 
          *
          * note : this function is only use for load entity data 
		  *		   currently don't support config entity data
          *
          * @param sql the sql to load the entities
          * @param parameterSQL the sql to load the parameters
          * @param entityData This MUST be a vector, either IEntityDataList or SharedIEntityDataList
          *
          */
        template<typename T>
        void getEntityDataWithParameterValues( IDatabase* pDatabase, const SQLStatement& sql, 
			const SQLStatement& parameterSQL, std::vector<T>& entityData);

		void createParameterCache();

	private:
		
		// Clone function typedef
		typedef IEntityData* ( *cloneFunc )( unsigned long key);

		typedef std::map<std::string, cloneFunc > EntityTypeMap;
		EntityTypeMap m_entityTypes;

		struct EntityParameter
		{
			unsigned long typeKey;
			std::string defaultValue;

			EntityParameter()
				: typeKey(0), defaultValue("")
			{
			};
		};
		std::multimap< std::string, EntityParameter > m_entityParameterCache;
        std::map<unsigned long, std::string>            m_entityTypesCache;

		TA_Base_Core::ReEntrantThreadLockable m_parameterLock;
        TA_Base_Core::ReEntrantThreadLockable m_entityTypesCacheLock;
		
        };

} // close namespace TA_Base_Core

#endif // !defined(EntityAccessFactory_BE8A9418_08C4_44ff_A791_B7082300A43B__INCLUDED_)
