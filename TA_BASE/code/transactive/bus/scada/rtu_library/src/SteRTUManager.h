/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_Review/transactive/bus/scada/rtu_library/src/SteRTUManager.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The RTUManager object is responsible for managing RTUs' running mode 
  * And processing RTUs' poll/write data
  *
  */

#if !defined(AFX_STERTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
#define AFX_STERTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/scada/rtu_library/src/BaseRTUManager.h"

// GF don't use StatusWordNumber in Station System. but the other project use it.
#define USE_STATUS_WORD_NUMBER

namespace TA_Base_Core
{
    class IEntityData;
};

namespace TA_Base_Bus
{
    //struct RTUDataBlock;
    class IEntity;
    class DataPoint;
    class RTU;
    class SteRTUManager : public TA_Base_Bus::BaseRTUManager
    {
    public:
        explicit SteRTUManager( int groupId, 
                                bool supportOutput, 
                                bool supportHeartBeat,
                                RTU_REDENDENCY_MODE rtuRedundancyMode);
        virtual  ~SteRTUManager();
        
        // for Agent to use.
        virtual TA_Base_Bus::IEntity* createRTUEntity(TA_Base_Core::IEntityDataPtr EntityData);
        bool addStationSystem(TA_Base_Core::IEntityData& EntityData);

        // set running parameter
    		virtual int  sendDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock) {return 0;};

        //for redundancymanager callback to process 
        virtual void processNoPrimaryRTU();
        virtual void processMultiplePrimaryRTU();
        virtual void processOnePrimaryRTU();
        virtual void dispatchDataBlock(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock);

    private:
        void    switchPrimaryRTUstoStandby();
        void    switchStandbyRTUstoPrimary();
        // global parameters
        bool    m_isRTUSwitchingDisabled;

        bool    m_foundMultiplePrimaryRTU;
        bool    m_foundNoPrimaryRTU;
        time_t  m_foundWrongStatusTime;
        bool    m_wrongStatus;

        TA_Base_Core::ReEntrantThreadLockable   m_primaryRTULock;

    };
}

#endif // !defined(AFX_STERTUMANAGER_H__0B1D7617_0C90_4D43_BE53_C02F342EB891__INCLUDED_)
