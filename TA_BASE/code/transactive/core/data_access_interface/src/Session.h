/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Session.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Session is an implementation of ISession. It holds the data specific to an Session entry
  * in the database, and allows read-only access to that data.
  */

#if !defined(Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/ISession.h"

namespace TA_Base_Core
{
    class Session : public ISession
    {

    public:

        /**
         * Constructor
         *
         * Construct an Session class based around a key.
         *
         * @param key The key of this Session in the database
         */
        Session(const std::string& sessionId);

        /** 
          * Session
          *
          * Create a new session that needs to be written to the database (using applyChanges), 
		  * the sessionID will be created automagically
          *
          * @param  unsigned long operatorKey
          * @param unsigned long consoleKey
          * @param unsigned long profileKey
          */
		Session( unsigned long operatorKey, unsigned long consoleKey, unsigned long profileKey,
			unsigned long locationKey, unsigned long entityKey, bool displayOnly );
		
		
        /** 
          * Session
          *
          * Create a new session that needs to be written to the database (using applyChanges)
          *
		  * @param std::string sessionID	
          * @param unsigned long operatorKey
          * @param unsigned long consoleKey
          * @param unsigned long profileKey
          * @param bool isNew - indicates if this is a new session yet to be written to the database
          */
		Session( const std::string& sessionID, 
			unsigned long operatorKey,
			unsigned long consoleKey, 
			unsigned long profileKey,
			unsigned long locationKey,
			unsigned long entityKey,
			bool		  displayOnly,
			bool          hasEnded,
            bool          isNew );

        /**
         * Destructor
         */
        virtual ~Session();

        /**
         * getSessionId
         *
         * Returns the sessionId for this PMSESSION entry.
         *
         * @return The key (uuid) for this Session as a string.
         */
        virtual std::string getSessionId();

        /**
         * getOperatorKey
         *
         * Returns the key for the operator of this session
         *
         * @return The key for the operator as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getOperatorKey();

        /**
         * getOperatorName
         *
         * Returns the name for the operator of this session
         *
         * @return The name for the operator as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getOperatorName();

        /**
         * getConsoleKey
         *
         * Returns the key for the console of this session
         *
         * @return The key for the console as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getConsoleKey();


        /**
         * getProfileKey
         *
         * Returns the key for the profile of this session
         *
         * @return The key for the profile as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getProfileKey();

		/**
         * getLocationKey
         *
         * Returns the key for the location of this session
         *
         * @return The key for the location as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getLocationKey();

		/**
		* getDisplayMode
		*
		* Returns the status of display mode 
		*
		* @return A boolean indicating whether the session is display only 
		*
		* @exception DatabaseException A DatabaseException will be thrown if there is a
		*            problem establishing a connection with the database, or if an
		*            error is encountered while retrieving data.
		* @exception DataException A DataException will be thrown if the data cannot be 
		*            converted to the required format (e.g. OPERATORKEY should be an 
		*            unsigned long), or if the wrong ammount of data is retrieved.
		*/
		virtual bool getDisplayMode();

		/**
         * getHasEnded
         *
         * Returns the ended status of this session
         *
         * @return A boolean indicating whether the session has ended.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual bool getHasEnded();

		/**
         * setHasEnded
         *
         * Marks the session as ended
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual void setHasEnded();

        /**
         * invalidate
         *
         * Make the data contained by this Session as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        virtual void applyChanges();
		virtual unsigned long getOverrideOperatorKey();
		virtual unsigned long getOverrideProfileKey();
		virtual void setDisplayOnly(bool displayOnly);
		void updateOverrideInfoToDb(unsigned long userOverrideId, unsigned long profileOverrideId ) ;
		void updateOverrideInfoToDb();

    private:
        // Copy constructor and assignment operator are not used so they are made private
        Session( const Session& theSession);            
		Session& operator=(const Session &);
		Session();

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. OPERATORKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        void reload();

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();
		
		virtual unsigned long getEntityKey();
	private:

        // The name of the local database to send updates to
        std::string m_localDatabase;

		std::string m_sessionId;
        unsigned long m_operatorKey;
		unsigned long m_consoleKey;
		unsigned long m_profileKey;
		unsigned long m_locationKey;
		unsigned long m_entityKey;
		unsigned long m_overrideOperatorKey;
		unsigned long m_overrideProfileKey;
		bool          m_hasEnded;
		bool		  m_displayOnly;
        std::string m_operatorName;

        bool m_isValidData;
		bool m_isWritten;
		bool m_isUpdateOnly;
    };
} // closes TA_Base_Core

#endif // !defined(Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
