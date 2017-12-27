 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialButtonAccessFactory.cpp $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RASpeedDialButtonAccessFactory is a singleton that is used to retrieve operator objects either from the
  * database or newly created. All operator objects returned will adhear to the IRASpeedDialButton interface.
  */
#if 0
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif
#endif

#include <sstream>

#include "core/data_access_interface/radio/src/RASpeedDialButtonAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialButton.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"


using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    TA_Base_Core::NonReEntrantThreadLockable RASpeedDialButtonAccessFactory::m_singletonLock;
	RASpeedDialButtonAccessFactory* RASpeedDialButtonAccessFactory::s_instance = NULL;


    RASpeedDialButtonAccessFactory& RASpeedDialButtonAccessFactory::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if (s_instance == NULL)
        {
            s_instance = new RASpeedDialButtonAccessFactory;
        }
        return *s_instance;
    }


    RASpeedDialButtonAccessFactory::~RASpeedDialButtonAccessFactory()
    {

    }


    IRASpeedDialButton* RASpeedDialButtonAccessFactory::getRASpeedDialButtonDetails(unsigned long key)
    {
        // Construct a new RASpeedDialButton type with the key we have
        IRASpeedDialButton* myRASpeedDialButton = new RASpeedDialButton(key);

        // Force it to load the details so we know if this key is valid or not. If this key is not valid
        // this call will throw an exception
        try
        {
            myRASpeedDialButton->getKey(); 
        }
        catch ( ... )
        {
            delete myRASpeedDialButton;
            throw;
        }

        return myRASpeedDialButton;
    }


    std::vector<IRASpeedDialButton*> RASpeedDialButtonAccessFactory::getSpeedDialSetByName(const std::string& setName)
    {
		// Perform SQL here
		std::vector< IRASpeedDialButton* > buttons;

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Read);

		// create the SQL string 
        /*std::stringstream s;
        s   << "select  "
            << "	   RASDBU_ID, "
            << "	   nvl(RASDSE_ID,0) as RASDSE_ID,  "
            << "	   BUTTON_POSITION,  "
            << "	   BUTTON_LABEL,  "
            << "	   BUTTON_TSI "
            << " from  "
            << "     RA_SPEED_DIAL_BUTTON NATURAL INNER JOIN RA_SPEED_DIAL_SET "
            << " where  "
            << "     SET_NAME = '" << databaseConnection->escapeQueryString(setName) << "' "
            << std::ends;
        std::string sql(s.str()); */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_BUTTON_Oracle_SELECT_36052,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_BUTTON_SELECT_36052,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_BUTTON_SELECT_36052,
			databaseConnection->escapeQueryString(setName));
        
        // Set up the columnNames vector to be passed to executeQuery()
        std::string colButtonKey("RASDBU_ID");
		std::string colSetKey("RASDSE_ID");
        std::string colButtonPosition("BUTTON_POSITION");
		std::string colButtonLabel("BUTTON_LABEL");
		std::string colButtonTSI("BUTTON_TSI");
        std::vector<std::string> columnNames;
        columnNames.push_back(colButtonKey);
        columnNames.push_back(colSetKey);
        columnNames.push_back(colButtonPosition);
        columnNames.push_back(colButtonLabel);
        columnNames.push_back(colButtonTSI);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
        // Loop for each row returned
        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					// Create the subsystem
                    IRASpeedDialButton* aButton = new RASpeedDialButton( 
                        data->getUnsignedLongData(i, colButtonKey ),
                        data->getUnsignedLongData(i, colSetKey ),
                        data->getUnsignedLongData(i, colButtonPosition ),
                        data->getStringData(i, colButtonLabel ),
                        data->getStringData(i, colButtonTSI ),
                        false); // false indicates not a new session
                    
                    buttons.push_back( aButton );
                }
                catch (const TA_Base_Core::DataException&)
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
        while ( databaseConnection->moreData(data) );

		return buttons;

        
    }


    IRASpeedDialButton* RASpeedDialButtonAccessFactory::createSpeedDialButton(IRASpeedDialButton * x)
    {
        // dummy method to conform to AF usage, RASpeedDialButton(...) constructor more 
        // than effective at creating new object, no need to wrap here. 
        // usage: new_subscriber = createSubscriber(new RASubscriber(...))
        return x;
    }


    void RASpeedDialButtonAccessFactory::deleteSpeedDialButton(IRASpeedDialButton* x)
    {
        if (x == NULL)
        {
            return;
        }

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Radio_Ad, Write);

        /*std::stringstream s;
        s   << "delete from RA_SPEED_DIAL_BUTTON where RASDBU_ID = "
            << x->getKey(false) << " " << std::ends;
        std::string sql(s.str());*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), RA_SPEED_DIAL_BUTTON_STD_DELETE_36401,
//		std::string strSql  = databaseConnection->prepareSQLStatement(RA_SPEED_DIAL_BUTTON_DELETE_36401,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_SPEED_DIAL_BUTTON_DELETE_36401,
			x->getKey(false));
        databaseConnection->executeModification(strSql);
    }



} //end namespace TA_Base_Core


