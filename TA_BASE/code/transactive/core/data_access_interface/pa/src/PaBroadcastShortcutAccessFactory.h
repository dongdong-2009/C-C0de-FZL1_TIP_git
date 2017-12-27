/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaBroadcastShortcutAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by:  $Author: builder $
  * 
  * PaBroadcastShortcutAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */



#ifndef PABROADCASTSHORTCUTACCESSFACTORY__INCLUDED_
#define PABROADCASTSHORTCUTACCESSFACTORY__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/IPaBroadcastShortcut.h"
#include "core/data_access_interface/pa/src/IConfigPaBroadcastShortcut.h"



namespace TA_Base_Core
{

class PaBroadcastShortcutAccessFactory
{
private:
    // Disable the following methods
    PaBroadcastShortcutAccessFactory() {};
	PaBroadcastShortcutAccessFactory( const PaBroadcastShortcutAccessFactory& thePaBroadcastShortcutAccessFactory);
	PaBroadcastShortcutAccessFactory& operator=(const PaBroadcastShortcutAccessFactory &);


public:
    
    
    /** 
     * ~PaBroadcastShortcutAccessFactory
     *
     * Destructor for basic class cleanup.
     *
     */
    virtual ~PaBroadcastShortcutAccessFactory() {};


    /**
     * getInstance
     *
     * Creates and returns an instance of this singleton object.
     *
     * @return  a reference to an instance of an PaBroadcastShortcutAccessFactory object.
     */
	static PaBroadcastShortcutAccessFactory& getInstance();


    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleans up the members.  
     * Primarily used upon program termination (e.g. from main()) so that Purify does not 
     * consider this class and its members to be memory leaks.
     *
     */
    static void removeInstance();

    
    /**
     * getPaBroadcastShortcut
     *
     * Retrieves the specified PaDvaMessage and returns the associated data as an 
     * object conforming to the IPaDvaMessage interface.
     *
     * @param   key        the primary key/id of the PaDvaMessage to retrieve
     * @param   readWrite  (optional) set to true to return writeable
     *                     IConfigPaDvaMessage objects or set to false to return 
     *                     readonly IPaDvaMessage objects.
     *
     * @return  A pointer to an object conforming to the IPaDvaMessage interface.
     *          Note: It is the responsibility of the *client* to delete the 
     *          returned IPaDvaMessage object
     *
     * @throws  DatabaseException 
     *          - if there are any problems in communicating with the database.
     *            The possible reasons are:
     *            1) Invalid connection string (as stored in RunParams)
     *            2) Database is not/cannot be opened
     *            3) Error while attempting to execute a database query
     * @throws  DataException 
     *          - if there is no PaDvaMessage matching the supplied key
     *          - if there is more than one PaDvaMessage matching the supplied key. 
     *
     */
	IPaBroadcastShortcut* getPaBroadcastShortcut(const unsigned long key, const bool readWrite);
    
	IPaBroadcastShortcut* getPaBroadcastShortcutByLocationKeyAndShortcutNo(unsigned long locationKey, unsigned long ulShortcutNo, bool readWrite);

    /**
     * getAllPaBroadcastShortcuts
     *
     * Retrieves all PaDvaMessages and returns the associated data as a vector of  
     * objects conforming to the IPaDvaMessage interface.
     *
     * @param   readWrite  (optional) set to true to return writeable
     *                     IConfigPaDvaMessage objects or set to false to return 
     *                     readonly IPaDvaMessage objects.
     *
     * @return  a vector of pointers to objects conforming to the IPaDvaMessage interface.
     *          Note: It is the responsibility of the *client* to delete the 
     *          returned IPaDvaMessage object
     *
     * @throws  DatabaseException 
     *          - if there are any problems in communicating with the database.
     *            The possible reasons are:
     *            1) Invalid connection string (as stored in RunParams)
     *            2) Database is not/cannot be opened
     *            3) Error while attempting to execute a database query
     * @throws  DataException 
     *          - if there is no PaDvaMessage matching the supplied key
     *          - if there is more than one PaDvaMessage matching the supplied key. 
     *
     */
    IPaBroadcastShortcuts getAllPaBroadcastShortcuts( const bool readWrite = false );

    /**
     * getPaBroadcastShortcutsByLocationKey
     *
     * Retrieves all PaDvaMessages associated with a specific location as defined by the input
	 * location key parameter.  Returns the associated data as a vector of objects.		 
     *
     * @param   locationKey the key defining which location we want to fetch the PA DVA Messages from
     *                     each PA Dva Message with a locationKey matching this will be returned
     * @param   readWrite  (optional) set to true to return writeable
     *                     IConfigPaDvaMessage objects or set to false to return 
     *                     readonly IPaDvaMessage objects.
     *
     * @return  a vector of pointers to objects conforming to the IPaDvaMessage interface.
     *          Note: It is the responsibility of the *client* to delete the 
     *          returned IPaDvaMessage object
     *
     * @throws  DatabaseException 
     *          - if there are any problems in communicating with the database.
     *            The possible reasons are:
     *            1) Invalid connection string (as stored in RunParams)
     *            2) Database is not/cannot be opened
     *            3) Error while attempting to execute a database query
     *
     */
    IPaBroadcastShortcuts getPaBroadcastShortcutsByLocationKey(unsigned long locationKey, const bool readWrite = false );


    /**
     * createPaBroadcastShortcut
     * 
     * Creates an empty IConfigPaDvaMessage object, and returns a pointer to it.
     * Note: It is the responsibility of the *client* to delete the returned  
     * IConfigPaDvaMessage object.
     *
     * @return A pointer to an IConfigPaDvaMessage object
     *
     */
	IConfigPaBroadcastShortcut* createPaBroadcastShortcut();


    /**
     * copyPaBroadcastShortcut
     * 
     * Creates a new configuration PaDvaMessage object using the PaDvaMessage passed in
     *
     * @param  PaDvaMessageToCopy the ConfigPaDvaMessage to copy
     *
     * @return the new object as a pointer to an IConfigPaDvaMessage object.
     *
     * @exception DatabaseException 
     *            - if there is a database problem.
     */
   IConfigPaBroadcastShortcut* copyPaBroadcastShortcut(const IConfigPaBroadcastShortcut* paBroadcastShortcutToCopy);

private:

    static PaBroadcastShortcutAccessFactory* m_instance;

    /**
     * getPaBroadcastShortcutsReturnedFromStatement
     *
     * Retrieves all PaDvaMessages that are filtered by input clause, should be
     * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
     * is simply appended to the basic select from statement (although a space is automatically inserted)
     *
     * @param whereClause the sql statements used to define the PaDvaMessages to return, no leading space required
     *
     * @param   readWrite  (optional) set to true to return writeable
     *                     IConfigPaDvaMessage objects or set to false to return 
     *                     readonly IPaDvaMessage objects.
     *
     * @return  a vector of pointers to objects conforming to the IPaDvaMessage interface.
     *          Note: It is the responsibility of the *client* to delete the 
     *          returned IPaDvaMessage object
     *
     * @throws  DatabaseException 
     *          - if there are any problems in communicating with the database.
     *            The possible reasons are:
     *            1) Invalid connection string (as stored in RunParams)
     *            2) Database is not/cannot be opened
     *            3) Error while attempting to execute a database query
     *
     */
    IPaBroadcastShortcuts getPaBroadcastShortcutsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);

};

} // closes TA_IRS_Core

#endif // !defined(PaBroadcastShortcutAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
