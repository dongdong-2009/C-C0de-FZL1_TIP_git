/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ActionGroup.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * ActionGroup is an implementation of IActionGroup. It holds the data specific to a ActionGroup entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(ActionGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ActionGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>
#include <vector>

#include "core/data_access_interface/src/IActionGroup.h"

namespace TA_Base_Core
{
	// forward declarations
	class IData;
    class ActionGroupHelper;

    class ActionGroup : public IActionGroup
    {

    public:

        /**
         * Constructor
         *
         * Construct a ActionGroup class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this ActionGroup in the database
         * @param name The name of this ActionGroup in the database
         */
        ActionGroup( const unsigned long key, const std::string& name );

        /**
         * Constructor
         *
         * Construct a ActionGroup class based around action group dataset
         */
		ActionGroup(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& actions);

        /**
         * Destructor
         */
        virtual ~ActionGroup();


        /**
         * getKey
         *
         * Returns the key for this ActionGroup.
         *
         * @return The key for this ActionGroup as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this ActionGroup. If this is the first time data for this ActionGroup
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this ActionGroup as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();		


        /**
         * getDisplayName
         *
         * Returns the display name of this ActionGroup. If this is the first time data for this ActionGroup
         * has been requested, all the data is loaded from the database.
         *
         * @return The display name for this ActionGroup as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */		
		virtual std::string ActionGroup::getDisplayName();
        /**
         * isControlType
         *
         * Returns whether or not the isControlType flag is set in the database.
         *
         * @return boolean - True if the ActionGroup is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isControlType();

        /**
         * invalidate
         *
         * Make the data contained by this ActionGroup as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		ActionGroup& operator=(const ActionGroup&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the ActionGroupHelper
        // has only been written for ConfigActionGroup objects and will not copy the ActionGroup helper
        // for a read-only object (ie this one).
        ActionGroup( const ActionGroup& theActionGroup);            	
		

        ActionGroupHelper* m_actionGroupHelper;
    };
} // closes TA_Base_Core

#endif // !defined(ActionGroup_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
