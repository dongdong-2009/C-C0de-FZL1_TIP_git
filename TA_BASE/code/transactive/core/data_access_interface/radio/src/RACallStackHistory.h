 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RACallStackHistory.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RACallStackHistory is an implementation of IRACallStackHistory. It holds the data specific to an RACallStackHistory entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(RA_CALL_STACK_HISTORY_XX_INCLUDED_)
#define RA_CALL_STACK_HISTORY_XX_INCLUDED_

#include <string>
#include <algorithm>
#include <functional>

#include "core/data_access_interface/radio/src/IRACallStackHistory.h"

namespace TA_Base_Core
{


    class RACallStackHistory : public IRACallStackHistory
    {

    public:
		static const unsigned long rowLimit;

        /**
         * RACallStackHistory (constructor)
         *
         * Construct an RACallStackHistory class based around a key.
         *
         * @param key The key of this RACallStackHistory in the database
         */
        RACallStackHistory(const unsigned long& key);

        RACallStackHistory(
            const unsigned long& key,
            const std::string& profile,
            const unsigned long& locationKey,
			const time_t& timestamp,
            OriginType originType,
            CallType callType,
            const std::string& identifier,
            const std::string& location,
            bool isNew,
			bool isEmergency // td17294
            );

        virtual ~RACallStackHistory();

    public:
		// td17294
		virtual bool isCallEmergency();
		virtual IRACallStackHistory& setEmergencyInfo(const bool isEmergency);
        
        virtual unsigned long getKey(bool reloadFlag = true); 
#if 0
        virtual IRACallStackHistory& setTSI(const std::string& TSI); 
#endif

        virtual std::string getProfileName();
        virtual IRACallStackHistory& setProfileName(const std::string& name);

        virtual unsigned long getLocationKey();
        virtual IRACallStackHistory& setLocationKey(const unsigned long& locationKey);
		
        virtual time_t getTimestamp();
        virtual IRACallStackHistory& setTimestamp(const time_t & timestamp);
		
        virtual OriginType getOrigin();
        virtual IRACallStackHistory& setOrigin(const OriginType & origin);

        virtual CallType getCallType();
        virtual IRACallStackHistory& setCallType(const CallType& callType);

        virtual std::string getIdentifier();
        virtual IRACallStackHistory& setIdentifier(const std::string& identifier);

        virtual std::string getLocation();
        virtual IRACallStackHistory& setLocation(const std::string& locationName);

        virtual void invalidate(); // IItem

#if 0 // only change through access factory
		virtual void applyChanges();
#endif

    private:

        // Assignment RACallStackHistory not used so it is made private
		RACallStackHistory& operator=(const RACallStackHistory &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RACallStackHistoryHelper
        // has only been written for ConfigLocation objects and will not copy the RACallStackHistory helper
        // for a read-only object (ie this one).
        RACallStackHistory( const RACallStackHistory& theRACallStackHistory);  

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

        unsigned long m_key;
        std::string m_profile;
        unsigned long m_locationKey;
		time_t m_timestamp;
        OriginType m_origin;
		CallType m_callType;
		std::string m_identifier;
        std::string m_location;

        bool m_isValidData;
		bool m_isWritten;
		bool m_isEmergency; // td17294
    };

    
} // closes TA_Base_Core


#endif // !defined(RA_CALL_STACK_HISTORY_XX_INCLUDED_)
