 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/RAPatchMembers.h $
  * @author Glen Kidd
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RAPatchMembers is an implementation of IRAPatchMembers. It holds the data specific to an RAPatchMembers entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(RA_PATCH_MEMBERS_XX_INCLUDED_)
#define RA_PATCH_MEMBERS_XX_INCLUDED_

#include <string>

#include "core/data_access_interface/radio/src/IRAPatchMembers.h"
//#include "core/data_access_interface/radio/src/RAPatchMembersHelper.h"

#error "Not implemented"

namespace TA_Base_Core
{


    class RAPatchMembers : public IRAPatchMembers
    {

    public:

        /**
         * RAPatchMembers (constructor)
         *
         * Construct an RAPatchMembers class based around a key.
         *
         * @param key The key of this RAPatchMembers in the database
         */
        RAPatchMembers(const unsigned long key);


        virtual ~RAPatchMembers();


    public:
#if 0 // IItem not required
        /**
         * getKey
         *
         * Returns the key for this RAPatchMembers.
         *
         * @return The key for this RAPatchMembers as an unsigned long.
         */
        unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName(); // from IItem
#endif  
        
        /**
         * getSubscriberName
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
        virtual std::string getSubscriberName();


        /**
         * getLocation
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
        virtual std::string getLocation();
		
        /**
         * getSubscriberType
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
        virtual SubscriberType getSubscriberType();
		

        /**
         * getLocationKey
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
        virtual unsigned long getLocationKey();

#if 0
        /**
         * getAssociatedProfiles
         *
         * Returns the profiles that this RAPatchMembers can use
         *
         * @return The profiles accessible by this RAPatchMembers, as a vector of IProfile*'s
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<IProfile*> getAssociatedProfiles();
#endif
        /**
         * invalidate
         *
         * Make the data contained by this RAPatchMembers as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


    private:

        // Assignment RAPatchMembers not used so it is made private
		RAPatchMembers& operator=(const RAPatchMembers &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the RAPatchMembersHelper
        // has only been written for ConfigLocation objects and will not copy the RAPatchMembers helper
        // for a read-only object (ie this one).
        RAPatchMembers( const RAPatchMembers& theRAPatchMembers);  

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
        std::string m_subscriberName;
        SubscriberType m_subscriberType;
        std::string m_locationName;
        unsigned long m_locationKey;

        bool m_isValidData;
		bool m_isWritten;
//        TA_Base_Core::RAPatchMembersHelper * m_RAPatchMembersHelper;
    };

#if 0 // Turned off - VC++ doesn't like explicit method instantiation
    template<class T> void RAPatchMembers::checkValidAndReload(const T field, const T invalidValue) //, const std::string fieldName)
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
    template void RAPatchMembers::checkValidAndReload<unsigned long>(const unsigned long field, const unsigned long invalidValue); //, const std::string fieldName)
    template void RAPatchMembers::checkValidAndReload<std::string>(const std::string field, const std::string invalidValue); //, const std::string fieldName)
#endif
    
} // closes TA_Base_Core


#endif // !defined(RA_PATCH_MEMBERS_XX_INCLUDED_)
