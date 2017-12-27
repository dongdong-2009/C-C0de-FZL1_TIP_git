/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/opctag_alias/src/OpcTagAliasHelper.cpp $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * OpcTagAliasHelper is an object that is held by all type-specific OpcTagAlias objects, as well as by
  * the ConfigOpcTagAlias. It contains all data common across OpcTagAliass, and manipulation
  * methods for the data. It helps avoid re-writing code for each OpcTagAlias object.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "trucated to 255 character" warning
#pragma warning(disable:4284)
#endif

#include <ctime>
#include <vector>

#include "core/data_access_interface/opctag_alias/src/OpcTagAliasHelper.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    //table name 
    static const std::string SCOPCTAGALIAS_TBL    ("SC_OPC_TAG_ALIAS");
    //index column
    static const std::string INDEX_COL       ("SCOTAL_ID");
    //OPC alias column
    static const std::string ALIAS_COL   ("ALIAS");
    //entity id column
    static const std::string ENTITYIDDP_COL   ("ENTITY_ID_DP");
    //attribute column
    static const std::string ATTRIBUTE_COL   ("ATTRIBUTE_DP");
    //for initialized invalid key
    enum
    {
        INVALID_KEY     =  -1
    };
    
    OpcTagAliasHelper::OpcTagAliasHelper( )
        : m_key (INVALID_KEY),
          m_name(""),
          m_entitykey(INVALID_KEY),
          m_attribute(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper Default Constructor");
        FUNCTION_EXIT;
    }


    OpcTagAliasHelper::OpcTagAliasHelper( const unsigned long entitykey, const std::string& name )
        : m_name(name),
          m_entitykey(entitykey),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(false),
          m_isValidData(false)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper Constructor with entitykey and name");
        getPrimarykeyAndAttributeByEntitykeyAndAlias(entitykey, name);
        FUNCTION_EXIT;        
    }

    OpcTagAliasHelper::OpcTagAliasHelper( const unsigned long key, const unsigned long entitykey, const std::string& name , const std::string attribute)
        : m_key(key), 
          m_name(name),
          m_entitykey(entitykey),
          m_attribute(attribute),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(false),
          m_isValidData(true)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper Constructor with all the fields");
        FUNCTION_EXIT;        
    }

    OpcTagAliasHelper::OpcTagAliasHelper( const OpcTagAliasHelper& theOpcTagAlias)
        : m_key(theOpcTagAlias.m_key),
          m_name(theOpcTagAlias.m_name),
          m_entitykey(theOpcTagAlias.m_entitykey),
          m_attribute(theOpcTagAlias.m_attribute),
          m_dateCreated(0),
          m_dateModified(0),
          m_isNew(true),
          m_isValidData(false)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper Copy Constructor");
        FUNCTION_EXIT;
    }


    OpcTagAliasHelper::~OpcTagAliasHelper()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper Destructor");
        FUNCTION_EXIT;
    }


    unsigned long OpcTagAliasHelper::getKey()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper getKey");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    
    std::string OpcTagAliasHelper::getName()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper getName");
        // We don't want to call reload() if this is a new OpcTagAlias and it has not been written to the database
        if (!m_isValidData && !m_isNew && m_name.empty())  
        {            
            // This is an existing OpcTagAlias and the data needs to be loaded
            reload();
        }
        FUNCTION_EXIT;
        return m_name;
    }

    
    unsigned long OpcTagAliasHelper::getEntitykey()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper getEntitykey");
        // We don't want to call reload() if this is a new OpcTagAlias and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing OpcTagAlias and the data needs to be loaded
            reload();
        }
        FUNCTION_EXIT;
        return m_entitykey;
    }
    
    std::string OpcTagAliasHelper::getAttributeName()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper getAttributeName");
        // We don't want to call reload() if this is a new OpcTagAlias and it has not been written to the database
        if (!m_isValidData && !m_isNew && m_attribute.empty())  
        {            
            // This is an existing OpcTagAlias and the data needs to be loaded
            reload();
        }
        FUNCTION_EXIT;
        return m_attribute;
    }

    time_t OpcTagAliasHelper::getDateCreated()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper getDateCreated");
        // We don't want to call reload() if this is a new OpcTagAlias and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing OpcTagAlias and the data needs to be loaded
            reload();
        }
        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t OpcTagAliasHelper::getDateModified()
    {
        FUNCTION_ENTRY("OpcTagAliasHelper getDateModified");
        // We don't want to call reload() if this is a new OpcTagAlias and it has not been written to the database
        if (!m_isValidData && !m_isNew)  
        {            
            // This is an existing OpcTagAlias and the data needs to be loaded
            reload();
        }
        FUNCTION_EXIT;
        return m_dateModified;
    }

 
    void OpcTagAliasHelper::setName(const std::string& name)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper setName");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        m_name = name;
    }


    void OpcTagAliasHelper::setEntitykey(unsigned long entitykey)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper setEntitykey");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        m_entitykey = entitykey;
    }
    
    void OpcTagAliasHelper::setAttributeName(const std::string& attribute)
    {
        FUNCTION_ENTRY("OpcTagAliasHelper setAttributeName");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        m_attribute = attribute;
    }
        
    void OpcTagAliasHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new entity");

        m_isValidData = false;
        m_name = "";
        m_entitykey = INVALID_KEY;
        m_attribute = "";

        FUNCTION_EXIT;
    }


    void OpcTagAliasHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // don't need to check key - the key will never be "invalid"
        
        // Now need to determine which of the attributes are empty, and add each failure
        // to a vector of failed field names.
        std::string entityname;
        IEntityData* data = NULL;
        try
        {
            data = EntityAccessFactory::getInstance().getEntity(m_entitykey);
            entityname = data->getName();
        }
        catch (DataException) 
        {
            entityname = "";
        }
        catch (...) 
        {
            entityname = "";
        }
        delete data;

        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }
        else if (entityname.empty())
        {
            fieldNames.push_back("Entity_ID");
        }
        else if (m_attribute.empty())
        {
            fieldNames.push_back("Attribute");
        }

        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
            TA_THROW( TA_Base_Core::DataConfigurationException("OpcTagAlias data not fully specified. OpcTagAlias cannot be written to database" ,fieldNames) );
        }

        // Write the OpcTagAlias to the database

        // Need to check if this is a new OpcTagAlias or an existing OpcTagAlias - 
        // as a new OpcTagAlias will be inserted, while an existing OpcTagAlias will be updated
        if (m_isNew)
        {
             // This is a new OpcTagAlias
            addNew();
        }
        else 
        {
            // This is an existing OpcTagAlias
            modifyExisting();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void OpcTagAliasHelper::deleteThisOpcTagAlias()
    {
        FUNCTION_ENTRY("deleteThisOpcTagAlias");

        TA_ASSERT(!m_isNew, "This OpcTagAlias does not yet exist in the database, and therefore cannot be deleted");
        
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
        // We must now attempt to delete the OpcTagAlias

        /*std::ostringstream sql;
        sql << "delete " << SCOPCTAGALIAS_TBL << " where " << ENTITYIDDP_COL << " = " << m_entitykey << " and " <<  ALIAS_COL << " = '" << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_DELETE_33401, m_entitykey, databaseConnection->escapeQueryString(m_name));
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_DELETE_33401, m_entitykey, databaseConnection->escapeQueryString(m_name));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_DELETE_33401, m_entitykey, databaseConnection->escapeQueryString(m_name));

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }
     
    
    void OpcTagAliasHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

        /*std::ostringstream sql;
        sql << "select " << ENTITYIDDP_COL << ", " << ALIAS_COL  << ", " << ATTRIBUTE_COL << " , ";
        sql << "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS') as MODIFIED, ";
        sql << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM ";
        sql <<  SCOPCTAGALIAS_TBL << " WHERE " << INDEX_COL << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_Oracle_SELECT_33051, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33051, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33051, m_key);


        // Set up the columnNames vector to be passed to executeQuery()
        std::string modifiedColumn = "MODIFIED";
        std::string createdColumn = "CREATED";

        std::vector<std::string> columnNames;
        columnNames.push_back(ENTITYIDDP_COL);
        columnNames.push_back(ALIAS_COL);
        columnNames.push_back(ATTRIBUTE_COL);
        columnNames.push_back(modifiedColumn);
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( strSql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No OpcTagAlias found for entitykey = " << m_entitykey;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one OpcTagAlias found for entitykey = " << m_entitykey;
			TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_name        = data->getStringData ( 0, ALIAS_COL );
        m_entitykey =         data->getUnsignedLongData( 0, ENTITYIDDP_COL );
        m_attribute        = data->getStringData ( 0, ATTRIBUTE_COL );
        m_dateCreated   = data->getDateData   ( 0, createdColumn   );

        try
        {
            m_dateModified = data->getDateData(0,modifiedColumn);
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (DataException& ex)
        {//If we catch this exception, need to check if its a "NO_VALUE" type
            if (DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }


        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void OpcTagAliasHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "OpcTagAlias %s already exists. It's details will be updated.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

        // Create a select statement that will ensure the uniqueness of the entity name.
       /* std::ostringstream sql;
        sql << "select "  << INDEX_COL << " from " << SCOPCTAGALIAS_TBL << " where " << ENTITYIDDP_COL << " = " << m_entitykey << " and " <<  ALIAS_COL << " = '" << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_SELECT_33003, m_entitykey, databaseConnection->escapeQueryString(m_name));
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33003, m_entitykey, databaseConnection->escapeQueryString(m_name));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33003, m_entitykey, databaseConnection->escapeQueryString(m_name));

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(INDEX_COL);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        //check the returned data - we should have 1 or 0 row returned
        if (1 == data->getNumRows()) // The name was found - need to check if it was for this pkey
        {
            if (m_key != data->getUnsignedLongData(0,INDEX_COL))
            {
                // A name already exists that is the same as the currently specified name
                // clean up the pointer
                delete data;
                data = NULL;

                std::ostringstream reasonMessage;
			    reasonMessage << "An OpcTagAlias already exists with name = " << m_name.c_str();
                TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
            }
            // else - it matches, so that's okay - it means the name wasn't changed.
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one OpcTagAlias already exists with name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        delete data;
        data =  NULL;

        /*sql.str("");
        sql << "update " << SCOPCTAGALIAS_TBL  << " set " << ALIAS_COL << " = '" << (databaseConnection->escapeQueryString(m_name)).c_str();
        sql << "', " << ENTITYIDDP_COL << " = " << m_entitykey;
        sql << ", " << ATTRIBUTE_COL << " = '" << (databaseConnection->escapeQueryString(m_attribute)).c_str();
        sql << "' where "  << INDEX_COL << " = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_UPDATE_33151, databaseConnection->escapeQueryString(m_name),
//		strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_UPDATE_33151, databaseConnection->escapeQueryString(m_name),

		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_UPDATE_33151, databaseConnection->escapeQueryString(m_name),
			m_entitykey, databaseConnection->escapeQueryString(m_attribute), m_key);

        databaseConnection->executeModification(strSql);


        //now need to update the date fields
        /*sql.str("");
        sql  << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') ";
        sql << "from " << SCOPCTAGALIAS_TBL << " where " << INDEX_COL << " = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_Oracle_SELECT_33052, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33052, m_key);

		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33052, m_key);

        // Can use the same columnNames vector from above.
        columnNames.clear();
        std::string createdColumn = "DATE_CREATED";
        std::string modifiedColumn = "DATE_MODIFIED";
        columnNames.push_back(createdColumn);
        columnNames.push_back(modifiedColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "More than one entry found for name = " << m_name.c_str();			
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
            m_dateModified = data->getDateData(0,modifiedColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;
        FUNCTION_EXIT;
    }

        
    void OpcTagAliasHelper::addNew()
    {
        FUNCTION_ENTRY("addNew");
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                "OpcTagAlias %s is new. It will be added to the database.",
                m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);
        
        // First check there is not alreay an entity with this name (it is a uniqueness
        // constraint).
        /*std::ostringstream formatSQL;

        formatSQL << "select "  << INDEX_COL << " from " << SCOPCTAGALIAS_TBL << " where " << ENTITYIDDP_COL << " = " << m_entitykey << " and " <<  ALIAS_COL << " = '" << databaseConnection->escapeQueryString(m_name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_SELECT_33003, m_entitykey, databaseConnection->escapeQueryString(m_name));
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33003, m_entitykey, databaseConnection->escapeQueryString(m_name));
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33003, m_entitykey, databaseConnection->escapeQueryString(m_name));

        std::vector<std::string> columnNames;
        columnNames.push_back(INDEX_COL);

        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Check the number of rows returned (the actual data is not important) - it
        // should be 0
        if (0 != data->getNumRows()) // Name already exists
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "A OpcTagAlias already exists with name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name") );
        }

        // delte the IData pointer now that it's finished with
        delete data;
        data = NULL;

        // Now we can get on with writing the entity to the database.
        // generate the SQL string to write the entity (reuse the string)
        //firstly, need to create the index key
        unsigned long nextAvailableKey = getNextSequenceNumber();

        /*formatSQL.str("");
        formatSQL << "insert into "<< SCOPCTAGALIAS_TBL << " (" << INDEX_COL << ", " << ENTITYIDDP_COL << ", " << ALIAS_COL << ", " << ATTRIBUTE_COL << ") ";
        formatSQL << "values (" << nextAvailableKey << ", " << m_entitykey << ", '";
		formatSQL << databaseConnection->escapeInsertString(m_name) << "' , '";
		formatSQL << databaseConnection->escapeInsertString(m_attribute) << "')";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_INSERT_33301, nextAvailableKey, m_entitykey,
//		strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_INSERT_33301, nextAvailableKey, m_entitykey,

		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_INSERT_33301, nextAvailableKey, m_entitykey,
			databaseConnection->escapeInsertString(m_name), databaseConnection->escapeInsertString(m_attribute));
 
        databaseConnection->executeModification(strSql);

        //now need to get the update the date fields
       /* std::ostringstream sql;
		sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') from " << SCOPCTAGALIAS_TBL << " where " << INDEX_COL << " = ";
        sql << nextAvailableKey;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_Oracle_SELECT_33053, nextAvailableKey);
//		strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33053, nextAvailableKey);

		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33053, nextAvailableKey);

        // Can use the same columnNames vector from above. Key is already in it so we can just add
        // the new fields
        std::string createdColumn = "DATE_CREATED";
        columnNames.clear();
        columnNames.push_back(createdColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"") );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one entry found for name = " << m_name.c_str();
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,"name and description") );
        }

        // Retrieve the pkey as an unsigned long from the 
        // returned data. There should only be a single returned item (as any other 
        // circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_dateCreated = data->getDateData(0,createdColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // update key info for this new record
        m_key = nextAvailableKey;
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }

    unsigned long OpcTagAliasHelper::getNextSequenceNumber()
    {
        FUNCTION_ENTRY("getNextSequenceNumber");
        const std::string NEXTVAL("NEXTVAL");

        // Create a query to get the next number in the sequence from the database
        /*const std::string sql("select SCOTAL_SEQ.NEXTVAL from DUAL");*/

        std::vector<std::string> columnNames;
        columnNames.push_back(NEXTVAL);

        // execute the query
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Scada_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_Oracle_SELECT_33054);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33054);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33054);
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        // Should always be single row result
        TA_ASSERT(data->getNumRows() == 1, "Invalid result from next sequence number query.");

        FUNCTION_EXIT;
        // return the next auto-number 
        return data->getUnsignedLongData(0, NEXTVAL);
    }

    void OpcTagAliasHelper::getPrimarykeyAndAttributeByEntitykeyAndAlias(const unsigned long entitykey, const std::string& name)
    {
        FUNCTION_ENTRY("getNextSequenceNumber");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);

        /*std::ostringstream sql;
        sql << "select " << INDEX_COL << " , " << ATTRIBUTE_COL << " FROM ";
        sql <<  SCOPCTAGALIAS_TBL << " WHERE " << ENTITYIDDP_COL << " = " << entitykey << " and " <<  ALIAS_COL << " = '" << databaseConnection->escapeQueryString(name) << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SC_OPC_TAG_ALIAS_STD_SELECT_33004, entitykey, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(SC_OPC_TAG_ALIAS_SELECT_33004, entitykey, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SC_OPC_TAG_ALIAS_SELECT_33004, entitykey,
			databaseConnection->escapeQueryString(name));

        std::vector<std::string> columnNames;
        columnNames.push_back(INDEX_COL);
        columnNames.push_back(ATTRIBUTE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery( strSql, columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No OpcTagAlias found for alias = " << m_name;
			TA_THROW( DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one OpcTagAlias found for alias = " << m_name;
			TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_key =  data->getUnsignedLongData( 0, INDEX_COL );
        m_attribute = data->getStringData( 0, ATTRIBUTE_COL);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        FUNCTION_EXIT;
    }

} // closes TA_Base_Core


