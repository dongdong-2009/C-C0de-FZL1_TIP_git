/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/IRACallStackHistory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IRACallStackHistory is an interface to an operator object. It allows the operator object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRACallStackHistory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRACallStackHistory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>


namespace TA_Base_Core
{
	class IProfile;

    class IRACallStackHistory 
    {
    public:

        enum OriginType    
        {
            ORIGIN_INVALID     = 0,

            INBOUND  = 'I',
            OUTBOUND = 'O'
        };

		enum CallType    
        {
            CALL_INVALID     = 0,

			VOICE		= 'V',
			CONFERENCE	= 'C',
			TEXT		= 'T',
			GROUP_ACK = 'A',
			GROUP_UNACK = 'U',
			GROUP_BROADCAST = 'B'
		};


    public:
        virtual ~IRACallStackHistory() {};


        /**
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		// td17294 
		virtual bool isCallEmergency() = 0;
        virtual unsigned long getKey(bool reloadFlag = true) = 0; 

#if 0
        /**
         * Sets the XXX.
         * Makes use of Named Parameter Idiom to allow object.setA().setB().setC() style.
         */        
        virtual IRACallStackHistory& setTSI(const unsigned long& key) = 0; 
#endif

        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getProfileName() = 0;
        virtual IRACallStackHistory& setProfileName(const std::string& name) = 0;


        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
        virtual unsigned long getLocationKey() = 0;
        virtual IRACallStackHistory& setLocationKey(const unsigned long& locationKey) = 0;
		
        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual time_t getTimestamp() = 0;
        virtual IRACallStackHistory& setTimestamp(const time_t & timestamp) = 0;
		

        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual OriginType getOrigin() = 0;
        virtual IRACallStackHistory& setOrigin(const OriginType & origin) = 0;

        
		/**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual CallType getCallType() = 0;
        virtual IRACallStackHistory& setCallType(const CallType& callType) = 0;

		
		
        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getIdentifier() = 0;
        virtual IRACallStackHistory& setIdentifier(const std::string& identifier) = 0;

		
		
        /**
         *
         * Returns the XXX.
         *
         * @return The as XXX a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getLocation() = 0;
        virtual IRACallStackHistory& setLocation(const std::string& locationName) = 0;

        /**
         * invalidate
         *
         * Make the data contained by this RACallStackHistory as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0; // IItem

#if 0
        virtual void applyChanges() = 0;
#endif
    };

} //close namespace TA_Base_Core

#endif // !defined(IRACallStackHistory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
