 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASpeedDialSet.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RASpeedDialSet is an implementation of IRASpeedDialSet. It holds the data specific to an RASpeedDialSet entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(RA_SPEED_DIAL_SET_XX_INCLUDED_)
#define RA_SPEED_DIAL_SET_XX_INCLUDED_

#include <string>

#include "core/data_access_interface/radio/src/IRASpeedDialSet.h"

namespace TA_Base_Core
{
    class IDatabase;

    class RASpeedDialSet : public IRASpeedDialSet
    {

    public:

        /**
         * RASpeedDialSet (constructor)
         *
         * Construct an RASpeedDialSet class based around a key.
         *
         * @param key The key of this RASpeedDialSet in the database
         */
        RASpeedDialSet(const unsigned long key);

        RASpeedDialSet(const std::string& name);

        RASpeedDialSet(
            const unsigned long key,
            const bool isSystem,
            const unsigned long operatorKey,
            const unsigned long profileId,
            const std::string& setName,
            const bool isNew
            );

        virtual ~RASpeedDialSet();

        static IRASpeedDialSet* createNewSetByName(const std::string& name,const unsigned long sessionId = 0);


    public:
        
        virtual unsigned long getKey(bool reloadFlag = true);

        virtual bool isSystem();
        virtual IRASpeedDialSet& setSystem(bool isSystem);

        virtual unsigned long getOperatorKey();
        virtual IRASpeedDialSet& setOperatorKey(unsigned long operatorKey);
		
        virtual unsigned long getProfileId();
        virtual IRASpeedDialSet& setProfileId(unsigned long profileId);		

        virtual std::string getSetName();
        virtual IRASpeedDialSet& setSetName(const std::string& setName);


        virtual void invalidate(); // IItem

		virtual void applyChanges();

    private:

        // Assignment RASpeedDialSet not used so it is made private
		RASpeedDialSet& operator=(const RASpeedDialSet &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RASpeedDialSetHelper
        // has only been written for ConfigLocation objects and will not copy the RASpeedDialSet helper
        // for a read-only object (ie this one).
        RASpeedDialSet( const RASpeedDialSet& theRASpeedDialSet);  


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
         * getNextKey()
         *
         * Get the next primary key value from the table's associated sequence. 
         */
        unsigned long getNextKey( IDatabase* databaseConnection );

        unsigned long m_key;
        bool m_isSystem;
        unsigned long m_operatorKey;
        unsigned long m_profileId;
        std::string m_setName;

        bool m_isValidData;
		bool m_isWritten;
    };

    
} // closes TA_Base_Core


#endif // !defined(RA_SPEED_DIAL_SET_XX_INCLUDED_)
