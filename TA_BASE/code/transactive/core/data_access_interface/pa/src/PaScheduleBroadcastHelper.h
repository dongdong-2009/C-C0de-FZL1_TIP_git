/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_T00010200/3002/transactive/core/data_access_interface/pa/src/PaScheduleBroadcastHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/01/11 18:41:48 $
  * Last modified by:  $Author: builder $
  *
  * PaScheduleBroadcastHelper is an object that is held by PaScheduleBroadcast and ConfigPaScheduleBroadcast objects. 
  * It contains all data used by PaScheduleBroadcast, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaScheduleBroadcast.
  */

#ifndef PASCHEDULEBROADCASTHELPER__INCLUDED_
#define PASCHEDULEBROADCASTHELPER__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class IData;

    class PaScheduleBroadcastHelper : public GenericDaiHelper<PaScheduleBroadcastHelper>
    {

    public:
        PaScheduleBroadcastHelper(const unsigned long key);
        PaScheduleBroadcastHelper( const PaScheduleBroadcastHelper& thePaScheduleBroadcastHelper);
        PaScheduleBroadcastHelper(unsigned long row, TA_Base_Core::IData& data);
        PaScheduleBroadcastHelper();
        virtual ~PaScheduleBroadcastHelper();

        virtual bool isNew() const
        {
            return m_isNew;
        };

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getDvaMsgKey();
        virtual unsigned long getCyclicTimes();
        virtual unsigned long getPriorityGroup();
        virtual time_t getStartTime();
        virtual time_t getEndTime();
        virtual std::string getScheduleName();
        virtual std::string getIntervalType();
        virtual std::string getSourceType();
        virtual std::string getStatus();
        virtual bool getIsEnable();
        virtual bool getAutoDel();
        virtual std::vector<unsigned long> getZoneKeys( );

        virtual void setLocationKey( unsigned long ulLocationKey );
        virtual void setDvaMsgKey( unsigned long ulDvaMsgKey );
        virtual void setCyclicTimes( unsigned long ulCyclicTimes );
        virtual void setPriorityGroup( unsigned long ulPriorityGroup );
        virtual void setStartTime( time_t tmStartTime );
        virtual void setEndTime( time_t tmEndTime );
        virtual void setScheduleName( const std::string& strScheduleName );
        virtual void setIntervalType( const std::string& strIntervalType );
        virtual void setSourceType( const std::string& strSourceType );
        virtual void setStatus( const std::string& strStatus );
        virtual void setIsEnable( bool bEnable );
        virtual void setAutoDel( bool bAutoDel );
        virtual void setZoneKeys( const std::vector<unsigned long>& lstZoneKeys );

        void invalidate();
        void writePaScheduleBroadcast();
        void deletePaScheduleBroadcast();
	
        static const DatabaseQueryHelper::QueryData getBasicQueryData();

    protected:

        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
        void reloadAll();
        void reloadZones(); 

    private:

        // Made private as it is not used
		PaScheduleBroadcastHelper& operator=(const PaScheduleBroadcastHelper &);

        void modifyExistingPaScheduleBroadcast();
        void addNewPaScheduleBroadcast();
        void validateLocationKeyExistence();

        virtual unsigned long getKeyDirect() const { return m_key; }

        unsigned long m_key;

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulLocationKey;        // Mustn't be 'unset', all table columns non null
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulDvaMsgKey;
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulCyclicTimes;
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_ulPriorityGroup;
        time_t m_tmStartTime;
        time_t m_tmEndTime;
        std::string m_strScheduleName;
        std::string m_strIntervalType;
        std::string m_strSourceType;
        std::string m_strStatus;
        std::vector<unsigned long> m_lstZoneKeys;

        bool m_bEnable;
        bool m_bAutoDel;

        bool m_isValidData;
        bool m_isNew;
        bool m_bZonesLoaded;
    };
} // closes TA_IRS_Core

#endif // !defined(PASCHEDULEBROADCASTHELPER__INCLUDED_)
