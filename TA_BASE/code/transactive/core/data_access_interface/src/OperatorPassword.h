
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorPassword.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2011/10/27 16:19:08 $
  * Last modified by:  $Author: hui.wang $
  * 
  * Session is an implementation of ISession. It holds the data specific to an Session entry
  * in the database, and allows read-only access to that data.
  */

#if !defined(OperatorPassword_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define OperatorPassword_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IOperatorPassword.h"

namespace TA_Base_Core
{
    class OperatorPassword : public IOperatorPassword
    {

    public:

        /**
         * Constructor
         *
         * Construct an Session class based around a key.
         *
         * @param key The key of this Session in the database
         */
		OperatorPassword(unsigned long operatorkey,bool isNew,bool changePassword,bool rightPassword);
		
		
        /**
         * Destructor
         */
        virtual ~OperatorPassword();

        /**
         * getSessionId
         *
         * Returns the sessionId for this PMSESSION entry.
         *
         * @return The key (uuid) for this Session as a string.
         */


		 virtual std::string getPassword0();

		 virtual std::string getPassword1();

		 virtual std::string getPassword2();

		 virtual int getCurrentPasswordIndicator();

		 virtual int getLockedTimes();

		  virtual void setPassword( const std::string& password );
		 
		 virtual void changePassword(const std::string& password );

		virtual void applyChanges();

		virtual time_t getLatestLoginDate();
		virtual time_t getLatestModifiedDate();
		virtual time_t getLatestLockedTime();

    private:
        // Copy constructor and assignment operator are not used so they are made private
        OperatorPassword( const OperatorPassword& theOperatorPassword);            
		OperatorPassword& operator=(const OperatorPassword &);
		OperatorPassword();

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
		


	private:

        // The name of the local database to send updates to
        std::string m_localDatabase;
	
        unsigned long m_operatorKey;
		time_t m_latestLoginTime;
		time_t m_passwdModifiedDate;
		time_t m_latestLockedTime;
		int m_isPasswordLockout;
		int m_currentPasswdIndicator;
		std::string m_password0;
		std::string m_password1;
		std::string m_password2;
		std::string m_password;
        bool m_isValidData;
		bool m_isWritten;
		bool m_isUpdateOnly;
		bool m_rightPassword;
		bool m_changePassword;
		bool m_isNew;
    };
} // closes TA_Base_Core

#endif // !defined(Session_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
