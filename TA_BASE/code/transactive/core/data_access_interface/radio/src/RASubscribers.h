 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RASubscribers.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RASubscribers is an implementation of IRASubscribers. It holds the data specific to an RASubscribers entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(RA_SUBSCRIBERS_XX_INCLUDED_)
#define RA_SUBSCRIBERS_XX_INCLUDED_

#include <string>
#include <algorithm>
#include <functional>

#include "core/data_access_interface/radio/src/IRASubscribers.h"

namespace TA_Base_Core
{


    class RASubscribers : public IRASubscribers
    {

    public:

        /**
         * RASubscribers (constructor)
         *
         * Construct an RASubscribers class based around a key.
         *
         * @param key The key of this RASubscribers in the database
         */
        RASubscribers(const std::string& TSI);

        RASubscribers(
            const std::string& groupTSI,
            const std::string& subsName,
            SubscriberType subsType,
            const std::string& location,
            unsigned long locationKey,
            bool isNew
            );

        virtual ~RASubscribers();

        /** 
         * Function object adapter conforming to a stl Predicate
         */
        class compareByTSI : public std::unary_function<IRASubscribers*, bool > 
        {
        public:
            compareByTSI(const std::string& TSI) : m_TSI(TSI) {};
            result_type operator() (argument_type x) { return x->getTSI() == m_TSI; };
        private:
            const std::string m_TSI;
        };

    public:
        
        virtual std::string getTSI(bool reloadFlag = true); 
        virtual IRASubscribers& setTSI(const std::string& TSI); 
        
        virtual std::string getSubscriberName();
        virtual IRASubscribers& setSubscriberName(const std::string& name);

        virtual std::string getLocation();
        virtual IRASubscribers& setLocation(const std::string& location);
		
        virtual SubscriberType getSubscriberType();
        virtual IRASubscribers& setSubscriberType(const SubscriberType& type);
		
        virtual unsigned long getLocationKey();
        virtual IRASubscribers& setLocationKey(const unsigned long& locationKey);

        virtual void invalidate(); // IItem

		virtual void applyChanges();

    private:

        // Assignment RASubscribers not used so it is made private
		RASubscribers& operator=(const RASubscribers &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RASubscribersHelper
        // has only been written for ConfigLocation objects and will not copy the RASubscribers helper
        // for a read-only object (ie this one).
        RASubscribers( const RASubscribers& theRASubscribers);  

#if 0 // Turned off - VC++ doesn't like explicit method instantiation
        /** helper method */
        template<class T> void checkValidAndReload(const T& field, const T& invalidValue);//, const std::string fieldName)
#endif

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
        std::string m_rasubsId;
        std::string m_subscriberName;
        SubscriberType m_subscriberType;
        std::string m_locationName;
        unsigned long m_locationKey;

        bool m_isValidData;
		bool m_isWritten;
		
		//TD18317, zhanghongzhi
		std::string getLocalDatabaseName();

    };

#if 0 // Turned off - VC++ doesn't like explicit method instantiation
    template<class T> void RASubscribers::checkValidAndReload(const T field, const T invalidValue) //, const std::string fieldName)
    {
		// If the data isn't written & the [field] isn't set then we must reload
		if ( ! m_isWritten && field == invalidValue )
		{
			TA_ASSERT( true, "The [field] must be set before it can be retrieved" );
		}
        
		if ( m_isWritten && ! m_isValidData )
        {
            reload();
        }
    }

    // explicit instantiation
    template void RASubscribers::checkValidAndReload<unsigned long>(const unsigned long field, const unsigned long invalidValue); //, const std::string fieldName)
    template void RASubscribers::checkValidAndReload<std::string>(const std::string field, const std::string invalidValue); //, const std::string fieldName)
#endif
    

} // closes TA_Base_Core


#endif // !defined(RA_SUBSCRIBERS_XX_INCLUDED_)
