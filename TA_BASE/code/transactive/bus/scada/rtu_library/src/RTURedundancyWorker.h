/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source: $File: //depot/3002_TIP/3002/transactive/bus/scada/rtu_library/src/RTURedundancyWorker.h $
  * @author  Liu Yong
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  * The RTURedundancyWorker is responsible for check and handle the RTU redundancy mechanism.
  *
  */

#ifndef RTUREDENDANCYWORKER_H
#define RTUREDENDANCYWORKER_H

#include <vector>

namespace TA_Base_Bus
{
  class RTU;
  class BaseRTUManager;

  enum RTU_REDENDENCY_MODE
  {
      //Single Primary represent hot-standby mechanism.
      //At any time, only one primary rtu is allowed, Mulitple primary
      //RTU or no primary RTU should be processed as abnormal circumstance.

      SINGLE_PRIMARY_RTU_PATTERN, 	

      //Multiple Primary means hot-hot mechansim.
      //will choose one primary to process. If the choosed RTU change to 
      //other mode, will choose the other primary rtu as main RTU to process.
      MULTIPLE_PRIMARY_RTU_PATTERN	
  };

  class RTURedundancyWorker
  {
  public:
      RTURedundancyWorker(BaseRTUManager& rtuManager);
      virtual ~RTURedundancyWorker();
      void addRTU(RTU* rtu);
      RTU* getPrimaryRTU();
      virtual void processRTUData(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock ) = 0;

  protected:
      void checkAllRTUStatus();
      bool checkSwitchingRTUs();
      RTU*                m_currentPrimaryRTU;
      BaseRTUManager&     m_rtuManager;
      std::vector< RTU* > m_rtus;
      // std::vector< RTU* > m_commsDownRTUs;
      std::vector< RTU* > m_swithchingRTUs;
      std::vector< RTU* > m_primaryRTUs;
      // std::vector< RTU* > m_standbyRTUs;

      TA_Base_Core::ReEntrantThreadLockable   m_primaryRTULock;      
  };

  //SinglePrimaryRTURedundancyWorker process only one primary RTU policy.
  class SinglePrimaryRTURedundancyWorker : public RTURedundancyWorker
  {
  public:
      SinglePrimaryRTURedundancyWorker(BaseRTUManager& rtuManager);
      virtual ~SinglePrimaryRTURedundancyWorker(){};    
      virtual void processRTUData(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock );
  };

  //MultiplePrimaryRTURedundancyWorker process multiple primary RTU policy.
  class MultiplePrimaryRTURedundancyWorker : public RTURedundancyWorker
  {
  public:
      MultiplePrimaryRTURedundancyWorker(BaseRTUManager& rtuManager);
      virtual ~MultiplePrimaryRTURedundancyWorker(){};    
      virtual void processRTUData(RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock );
  };
}

#endif  // RTUREDENDANCYWORKER_H
