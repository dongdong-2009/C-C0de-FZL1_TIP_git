/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/GuiAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * GuiAccessFactory is a singleton that is used to retrieve Gui objects either from the
  * database or newly created. All Gui objects returned will adhear to the IGui interface.
  */



#if !defined(GuiAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define GuiAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IGui;
    class IConfigGui;


	class GuiAccessFactory
	{

	public:
        virtual ~GuiAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an GuiAccessFactory object.
         */
		static GuiAccessFactory& getInstance();


        /**
         * getGui
         *
         * Retrieves the specified gui and returns the associated data as an 
         * object conforming to the IGui interface.
         *
         * @param key The database key to the gui to retrieve
         *
         * @return A pointer to an object conforming to the IGui interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IGui 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no gui
         *            matching the supplied PKEY, or if there is more than one gui
         *            matching the supplied PKEY. 
         */
		IGui* getGui(const unsigned long key, const bool readWrite = false);

		
        /**
         * getAllGuis
         *
         * Retrieves all specified guis and returns the associated data as a vector of  
         * objects conforming to the IGui interface.
         *
         * @return A vector of pointers to objects conforming to the IGui interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IGui 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IGui*> getAllGuis( const bool readWrite = false );


        /**
         * createGui
         * 
         * Creates an empty IConfigGui object, and returns a pointer to it.
         * N.B.: It is the responsibility of the *client* to delete the returned  
         * IConfigGui object
         *
         * @param string - An entity type. This will be used to create a related entity
         *                 for the Gui.
         *
         * @return A pointer to an IConfigGui object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        IConfigGui* createGui(const std::string& type);


        /**
         * copyGui
         * 
         * Creates a new configuration GUI object using the gui passed in
         *
         * @param IConfigGui* The location to copy
         *
         * @return The configuration location object as a pointer to an IConfigGui object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigGui* copyGui(const IConfigGui* guiToCopy);


    private:
        //
        // These are private as this method is a singleton
        //
		GuiAccessFactory() { };
		GuiAccessFactory( const GuiAccessFactory& theGuiAccessFactory);
		GuiAccessFactory& operator=(const GuiAccessFactory &);
		
		void getGuiBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IGui*>& guis, const bool readWrite);


        static GuiAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(GuiAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
