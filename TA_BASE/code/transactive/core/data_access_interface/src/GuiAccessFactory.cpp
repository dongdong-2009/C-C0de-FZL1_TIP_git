/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GuiAccessFactory.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * GuiAccessFactory is a singleton that is used to retrieve Gui objects either from the
  * database or newly created. All Gui objects returned will adhear to the IGui interface.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/data_access_interface/src/Gui.h"
#include "core/data_access_interface/src/ConfigGui.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{
    GuiAccessFactory* GuiAccessFactory::m_instance = 0;

    GuiAccessFactory& GuiAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new GuiAccessFactory();
        }
        return *m_instance;
    }


    void GuiAccessFactory::getGuiBySql(IDatabase* pDatabase,const SQLStatement& sql, std::vector<IGui*>& guis, const bool readWrite)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("FULLNAME");
        columnNames.push_back("EXECUTABLE");
        columnNames.push_back("HELP");
        columnNames.push_back("MAXINSTANCES");
        columnNames.push_back("LAUNCHICON");
		columnNames.push_back("REPOSITION");
        columnNames.push_back("PASSPARAMS");
        columnNames.push_back("TERMINATE");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(sql, columnNames);

		time_t value = data->getNullTime();

        // Loop for each row returned
        do
        {		
			for (unsigned long i = 0; i < data->getNumRows() ; ++i )
			{
				try
				{
					if (0 == data->getUnsignedLongData(i, columnNames[0]))
					{
						// We are not interested in the zero key as this is just a place-holder so move onto the
						// next gui.
						continue;
					}

					// decide which type of entity this is, and create an object of that type.
					if (readWrite) // need a config entity
					{
						 guis.push_back( new ConfigGui(i, *data));
					}
					else // need a standard entity
					{
						 guis.push_back( new Gui(i, *data));
					}
				}
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
			}

			delete data;
			data = NULL;
		}
        while ( pDatabase->moreData(data) );
    }

    IGui* GuiAccessFactory::getGui(const unsigned long key,const bool readWrite)
    {
		FUNCTION_ENTRY("getGui");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_Oracle_SELECT_16551, key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16551, key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16551, key);
//		TA_ASSERT(strSql.size() > 0, "GuiAccessFactory::getGui(): Prepare SQL Statement error");

        // create the SQL string to retrieve the data of the gui
        // based upon the key
        /*std::ostringstream sql;
		sql << " SELECT pkey,name,fullname,executable,help,maxinstances,show_launch_icon,reposition,passregisterparams,"
			<< " terminate_on_logout, TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM guiapplication where pkey=" << key;*/
        
		std::vector<IGui*> guis;
        getGuiBySql(databaseConnection, strSql, guis, readWrite);

        if ( 0 == guis.size())
        {
            std::ostringstream message;
			message << "No data found for Gui with key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );         

			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }
		
		TA_ASSERT(1 == guis.size(), "Gui key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return guis[0];
    }
	
    std::vector<IGui*> GuiAccessFactory::getAllGuis(const bool readWrite)
    {
		FUNCTION_ENTRY("getAllGuis");

		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), guiapplication_Oracle_SELECT_16552);
//		std::string strSql  = databaseConnection->prepareSQLStatement(GUIAPPLICATION_SELECT_16552);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, GUIAPPLICATION_SELECT_16552);
//		TA_ASSERT(strSql.size() > 0, "GuiAccessFactory::getAllGuis(): Prepare SQL Statement error");

        // create the SQL string to retrieve the data of the gui
        // based upon the key
       /* std::ostringstream sql;
		sql << " SELECT pkey,name,fullname,executable,help,maxinstances,show_launch_icon,reposition,passregisterparams,"
			<< " terminate_on_logout, TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
			<< " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM guiapplication";*/
        
		std::vector<IGui*> guis;
        getGuiBySql(databaseConnection, strSql, guis, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
		return guis;
    }


    IConfigGui* GuiAccessFactory::createGui(const std::string& type)
    {            
        return new ConfigGui(type);
    }


    IConfigGui* GuiAccessFactory::copyGui(const IConfigGui* guiToCopy)
    {
        FUNCTION_ENTRY("copyGui");

        TA_ASSERT(guiToCopy !=  NULL, "The gui to copy was NULL");
        
        const ConfigGui* cast = dynamic_cast<const ConfigGui*>(guiToCopy);

        TA_ASSERT(cast != NULL, "Gui passed could not be converted into a ConfigGui");

        FUNCTION_EXIT;
        return new ConfigGui(*cast);
    }

} // closes TA_Base_Core


