
#include "PScadaEventHandler.h"
#include "PScadaCommandPolicy.h"
#include "bus/scada/104_library/src/Command.h"
#include "bus/scada/104_library/src/Iec104Api.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/soe/src/SOEEvent.h"
#include "bus/soe/soe_process_library/src/SOEProcessor.h"


namespace TA_IRS_App
{
const int PScadaEventHandler::s_UpdateMonitorTimeSlice = 5000; //5s
PScadaEventHandler::PScadaEventHandler()
:m_pscadaMsgTimer(NULL),
 m_isConnectionTimerStarted(false),
 m_lastUpdateTimeSec(0),
 m_monitorConnectionTimeId(-1),
 m_monitorLastUpdateTimeId(-1),
 m_linkStatusBadToGood(false),
 m_isInControl(false)
{
	FUNCTION_ENTRY("PScadaEventHandler::PScadaEventHandler");
	m_InputDataPoints.clear();
	m_OutputDataPoints.clear();
	m_pscadaDpProcessor = new PScadaDpProcessor(m_InputDataPoints);
	FUNCTION_EXIT("PScadaEventHandler::PScadaEventHandler");
}
PScadaEventHandler::~PScadaEventHandler()
{
	FUNCTION_ENTRY("PScadaEventHandler::~PScadaEventHandler");

	delete m_pscadaDpProcessor;
	m_pscadaDpProcessor=NULL;
	TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock(this);
	FUNCTION_EXIT("PScadaEventHandler::~PScadaEventHandler");
}
void PScadaEventHandler::setMsgTimer(TA_IRS_App::PScadaMsgTimer* pscadaMsgTimer)
{
	m_pscadaMsgTimer = pscadaMsgTimer;
}

void PScadaEventHandler::handleSinglePoint(int rtuAddr,b_ui8 bCot, vector<SSinglePoint> &vSinglePoint)
{
	FUNCTION_ENTRY("handleSinglePoint");
	int num = vSinglePoint.empty() ? -1 : static_cast<int>(vSinglePoint.size());
	SSinglePoint sp;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 1 Message: CommonAddress=%02x, cot_%03d, Num=%d",rtuAddr,bCot,num);
	b_ui8 SIQ;
	timeb currentSystemTime;   //TD16182
	ftime ( &currentSystemTime );
	m_lastUpdateTimeSec = currentSystemTime.time;

	TA_Base_Bus::DataPoint * dp;

	for(int i=0;i < num;i++)
	{
		sp = vSinglePoint[i];
		memcpy(&SIQ,&sp.SIQ,1);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point: %03d IOA=0X%06x (%6d), { SIQ:%02x, SPI:%02x}",i,sp.iOffset,sp.iOffset,SIQ,sp.SIQ.SPI);

		PScadaDataPointMap::iterator itr = m_InputDataPoints.find( sp.iOffset );

		if ( itr != m_InputDataPoints.end() )
		{
			dp = itr->second;

			// set up DataPointState with the specified timestamp, quality and value
			TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			bool bResult = false;

			if (sp.SIQ.SPI == 1)
				bResult = true;
			else
				bResult = false;

			TA_Base_Bus::EQualityStatus quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

			if (sp.SIQ.BL == 1)
				quality = TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;

			if (sp.SIQ.SB == 1)
				quality = TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE;

			if (sp.SIQ.NT == 1)
				quality = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

			if (sp.SIQ.IV == 1)
				quality = TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE;

			dpValue.setBoolean( bResult);
			boost::shared_ptr<TA_Base_Bus::DataPointState> newState(new TA_Base_Bus::DataPointState( dpValue, currentSystemTime, quality));
			m_pscadaDpProcessor->submitUpdateDataPoint(dp,  newState);

		}
	}
	FUNCTION_EXIT("handleSinglePoint");
}
void PScadaEventHandler::handleSinglePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SSinglePointTimeTag> &vSinglePointTm)
{
	FUNCTION_ENTRY("handleSinglePointWithTimeTag");

	int num = vSinglePointTm.empty() ? -1 : static_cast<int>(vSinglePointTm.size());
	SSinglePointTimeTag sp;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 30 Message: CommonAddress=%02x, cot_%03d, Num=%d",rtuAddr,bCot,num);
	b_ui8 SIQ;
	SFepTime sysTime;
	struct tm dateTm;
	timeb currentSystemTime;   //TD16182
	ftime ( &currentSystemTime );
	m_lastUpdateTimeSec = currentSystemTime.time;

	timeb sourceSystemTime;  
	ftime ( &sourceSystemTime );

	TA_Base_Bus::DataPoint * dp;

	for(int i=0;i < num;i++)
	{
		sp = vSinglePointTm[i];
		memcpy(&SIQ,&sp.SIQ,1);
		memcpy(&sysTime,&(sp.cp56TimeTag),sizeof(SFepTime));

		dateTm.tm_mday = sysTime.day;
		dateTm.tm_mon = sysTime.month - 1; 
		dateTm.tm_year = sysTime.year - 1900;
		dateTm.tm_hour = sysTime.hour;
		dateTm.tm_isdst = -1;
		dateTm.tm_min = sysTime.minute;
		dateTm.tm_sec = sysTime.second;

		time_t time = mktime( &dateTm );
		if ( time == static_cast< time_t >( -1 ) )
		{
			//TA_THROW( TA_Base_Core::ArchiveException( "Unable to convert date/time." ) );
		}

		
		sourceSystemTime.millitm = sysTime.millisecond;
		sourceSystemTime.time = time;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point: %03d IOA=0X%06x (%6d), { SIQ:%02x, SPI:%02x}",i,sp.iOffset,sp.iOffset,SIQ,sp.SIQ.SPI);

		PScadaDataPointMap::iterator itr = m_InputDataPoints.find( sp.iOffset );

		if ( itr != m_InputDataPoints.end() )
		{
			dp = itr->second;

			// set up DataPointState with the specified timestamp, quality and value
			TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			bool bResult = false;

			if (sp.SIQ.SPI == 1)
				bResult = true;
			else
				bResult = false;

			std::string strValue = bResult?"True":"False";
			dpValue.setBoolean( bResult);

			TA_Base_Core::SOEEvent soeEvent;
			soeEvent.setEntityKey(dp->getEntityKey());
			soeEvent.setLocationKey(dp->getLocationKey());
			soeEvent.setLocationName(dp->getLocationName());
			soeEvent.setAssetName(dp->getAssetName());
			soeEvent.setDescription(dp->getDescription());
			soeEvent.setSourceTime(sourceSystemTime);
			soeEvent.setTransactiveTime(currentSystemTime);
			soeEvent.setValueData(strValue);
			soeEvent.setDisplayValue(dpValue.getValueAsString());

			TA_Base_Bus::SOEProcessor::getInstance().submitSOE(soeEvent);

		}
	}
	FUNCTION_EXIT("handleSinglePointWithTimeTag");

}

void PScadaEventHandler::handleMeasureValueSVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueSVA> &vMeasuredValueSVA)
{
	FUNCTION_ENTRY("handleMeasureValueSVA");
	int num = vMeasuredValueSVA.empty() ? -1 : static_cast<int>(vMeasuredValueSVA.size());
	SMeasuredValueSVA sva;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 11 Message: CommonAddress=%02x, cot=%03d, num=%d",rtuAddr,bCot,num);
	b_ui16 SVA;b_ui8 QDS;
	timeb currentSystemTime;   //TD16182
	ftime ( &currentSystemTime );
	m_lastUpdateTimeSec = currentSystemTime.time;

	TA_Base_Bus::DataPoint * dp;
	for(int i=0;i< num;i++)
	{
		sva =vMeasuredValueSVA[i];
		memcpy(&SVA, &sva.SVA,2);
		memcpy(&QDS,&sva.QDS,1);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point: %03d IOA=%06d, { SVA:%02d, QDS:%02x}",i,sva.iOffset,SVA,QDS);

		PScadaDataPointMap::iterator itr = m_InputDataPoints.find( sva.iOffset );

		if (itr != m_InputDataPoints.end() )
		{
			dp = itr->second;

			TA_Base_Bus::EQualityStatus quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

			if (sva.QDS.OV == 1)
				quality = TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE;

			if (sva.QDS.BL == 1)
				quality = TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;

			if (sva.QDS.SB == 1)
				quality = TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE;

			if (sva.QDS.NT == 1)
				quality = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

			if (sva.QDS.IV == 1)
				quality = TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE;


			// set up DataPointState with the specified timestamp, quality and value
			TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( sva.SVA, dp->getPrecision() );
			boost::shared_ptr<TA_Base_Bus::DataPointState> newState(new TA_Base_Bus::DataPointState( dpValue, currentSystemTime, quality));
			m_pscadaDpProcessor->submitUpdateDataPoint(dp,  newState);
		}

	}
	FUNCTION_EXIT("handleMeasureValueSVA");
}

void PScadaEventHandler::handleMeasureValueSVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSVATimeTag> &vMeasuredValueSVATm)
{
	FUNCTION_ENTRY("handleMeasureValueSVAWithTimeTag");
	int num = vMeasuredValueSVATm.empty() ? -1 : static_cast<int>(vMeasuredValueSVATm.size());
	SMeasuredValueSVATimeTag sva;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 11 Message: CommonAddress=%02x, cot=%03d, num=%d",rtuAddr,bCot,num);
	b_ui16 SVA;b_ui8 QDS;
	SFepTime sysTime;
	struct tm dateTm;
	timeb currentSystemTime;   //TD16182
	ftime ( &currentSystemTime );
	m_lastUpdateTimeSec = currentSystemTime.time;

	timeb sourceSystemTime;  
	ftime ( &sourceSystemTime );

	TA_Base_Bus::DataPoint * dp;
	for(int i=0;i< num;i++)
	{
		sva =vMeasuredValueSVATm[i];
		memcpy(&SVA, &sva.SVA,2);
		memcpy(&QDS,&sva.QDS,1);
		memcpy(&sysTime,&(sva.cp56TimeTag),sizeof(SFepTime));

		dateTm.tm_mday = sysTime.day;
		dateTm.tm_mon = sysTime.month - 1; 
		dateTm.tm_year = sysTime.year - 1900;
		dateTm.tm_hour = sysTime.hour;
		dateTm.tm_isdst = -1;
		dateTm.tm_min = sysTime.minute;
		dateTm.tm_sec = sysTime.second;

		time_t time = mktime( &dateTm );
		if ( time == static_cast< time_t >( -1 ) )
		{
			//TA_THROW( TA_Base_Core::ArchiveException( "Unable to convert date/time." ) );
		}


		sourceSystemTime.millitm = sysTime.millisecond;
		sourceSystemTime.time = time;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point: %03d IOA=%06d, { SVA:%02d, QDS:%02x}",i,sva.iOffset,SVA,QDS);

		PScadaDataPointMap::iterator itr = m_InputDataPoints.find( sva.iOffset );

		if (itr != m_InputDataPoints.end() )
		{
			dp = itr->second;

			// set up DataPointState with the specified timestamp, quality and value
			TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( sva.SVA, dp->getPrecision() );
			
			char szTemp[64];
			sprintf(szTemp,"%d",sva.SVA);
			std::string strValue = szTemp;
			
			TA_Base_Core::SOEEvent soeEvent;
			soeEvent.setEntityKey(dp->getEntityKey());
			soeEvent.setLocationKey(dp->getLocationKey());
			soeEvent.setLocationName(dp->getLocationName());
			soeEvent.setAssetName(dp->getAssetName());
			soeEvent.setDescription(dp->getDescription());
			soeEvent.setSourceTime(sourceSystemTime);
			soeEvent.setTransactiveTime(currentSystemTime);
			soeEvent.setValueData(strValue);
			soeEvent.setDisplayValue(dpValue.getValueAsString());

			TA_Base_Bus::SOEProcessor::getInstance().submitSOE(soeEvent);

		}

	}
	FUNCTION_EXIT("handleMeasureValueSVAWithTimeTag");

}

void PScadaEventHandler::handleMeasureValueSFA(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFA> &vMeasuredValueSFA)
{
	FUNCTION_ENTRY("handleMeasureValueSFA");
	int num = vMeasuredValueSFA.empty() ? -1 : static_cast<int>(vMeasuredValueSFA.size());
	SMeasuredValueSFA sfa;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 13 Message: CommonAddress=%02x, cot=%03d, num=%d",rtuAddr,bCot,num);
	float SFA;b_ui8 QDS;
	timeb currentSystemTime;   //TD16182
	ftime ( &currentSystemTime );
	m_lastUpdateTimeSec = currentSystemTime.time;

	TA_Base_Bus::DataPoint * dp;
	for(int i=0;i< num;i++)
	{
		sfa =vMeasuredValueSFA[i];
		memcpy(&SFA, &sfa.SFA,4);
		memcpy(&QDS,&sfa.QDS,1);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point: %03d IOA=%06d, { SFA:%f, QDS:%02x}",i,sfa.iOffset,SFA,QDS);

		PScadaDataPointMap::iterator itr = m_InputDataPoints.find( sfa.iOffset );

		if (itr != m_InputDataPoints.end() )
		{
			dp = itr->second;

			TA_Base_Bus::EQualityStatus quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

			if (sfa.QDS.OV == 1)
				quality = TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE;

			if (sfa.QDS.BL == 1)
				quality = TA_Base_Bus::QUALITY_UNCERTAINTY_NO_SPECIFIC_REASON;

			if (sfa.QDS.SB == 1)
				quality = TA_Base_Bus::QUALITY_UNCERTAINTY_SENSOR_NOT_ACCURATE;

			if (sfa.QDS.NT == 1)
				quality = TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE;

			if (sfa.QDS.IV == 1)
				quality = TA_Base_Bus::QUALITY_BAD_OUT_OF_SERVICE;


			// set up DataPointState with the specified timestamp, quality and value
			TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( sfa.SFA );
			boost::shared_ptr<TA_Base_Bus::DataPointState> newState(new TA_Base_Bus::DataPointState( dpValue, currentSystemTime, quality));
			m_pscadaDpProcessor->submitUpdateDataPoint(dp,  newState);
		}

	}
	FUNCTION_EXIT("handleMeasureValueSFA");
}


void PScadaEventHandler::handleMeasureValueSFAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFATimeTag> &vMeasuredValueSFATm)
{
		FUNCTION_ENTRY("handleMeasureValueSFAWithTimeTag");
		int num = vMeasuredValueSFATm.empty() ? -1 : static_cast<int>(vMeasuredValueSFATm.size());
		SMeasuredValueSFATimeTag sfa;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 36 Message: CommonAddress=%02x, cot=%03d, num=%d",rtuAddr,bCot,num);
		float SFA;b_ui8 QDS;
		SFepTime sysTime;
		struct tm dateTm;
		timeb currentSystemTime;   //TD16182
		ftime ( &currentSystemTime );
		m_lastUpdateTimeSec = currentSystemTime.time;

		timeb sourceSystemTime;  
		ftime ( &sourceSystemTime );
	
		TA_Base_Bus::DataPoint * dp;
		for(int i=0;i< num;i++)
		{
			sfa =vMeasuredValueSFATm[i];
			memcpy(&SFA, &sfa.SFA,4);
			memcpy(&QDS,&sfa.QDS,1);
			memcpy(&sysTime,&(sfa.cp56TimeTag),sizeof(SFepTime));
	
			dateTm.tm_mday = sysTime.day;
			dateTm.tm_mon = sysTime.month - 1; 
			dateTm.tm_year = sysTime.year - 1900;
			dateTm.tm_hour = sysTime.hour;
			dateTm.tm_isdst = -1;
			dateTm.tm_min = sysTime.minute;
			dateTm.tm_sec = sysTime.second;
	
			time_t time = mktime( &dateTm );
			if ( time == static_cast< time_t >( -1 ) )
			{
				//TA_THROW( TA_Base_Core::ArchiveException( "Unable to convert date/time." ) );
			}
	
	
			sourceSystemTime.millitm = sysTime.millisecond;
			sourceSystemTime.time = time;
	
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point: %03d IOA=%06d, { SFA:%f, QDS:%02x}",i,sfa.iOffset,SFA,QDS);
	
			PScadaDataPointMap::iterator itr = m_InputDataPoints.find( sfa.iOffset );
	
			if (itr != m_InputDataPoints.end() )
			{
				dp = itr->second;
	
				// set up DataPointState with the specified timestamp, quality and value
				TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
				dpValue.setFloat( sfa.SFA );
				
				char szTemp[64];
				sprintf(szTemp,"%f",sfa.SFA);
				std::string strValue = szTemp;
				
				TA_Base_Core::SOEEvent soeEvent;
				soeEvent.setEntityKey(dp->getEntityKey());
				soeEvent.setLocationKey(dp->getLocationKey());
				soeEvent.setLocationName(dp->getLocationName());
				soeEvent.setAssetName(dp->getAssetName());
				soeEvent.setDescription(dp->getDescription());
				soeEvent.setSourceTime(sourceSystemTime);
				soeEvent.setTransactiveTime(currentSystemTime);
				soeEvent.setValueData(strValue);
				soeEvent.setDisplayValue(dpValue.getValueAsString());
	
				TA_Base_Bus::SOEProcessor::getInstance().submitSOE(soeEvent);
	
			}
	
		}
		FUNCTION_EXIT("handleMeasureValueSVAWithTimeTag");

}


void PScadaEventHandler::handleIntegratedTotal(int rtuAddr,b_ui8 bCot,vector<SCounterValue>&vCounterValue)
{
	FUNCTION_ENTRY("handleIntegratedTotal");
	int num = vCounterValue.empty() ? -1 : static_cast<int>(vCounterValue.size());
	SCounterValue cv;
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Type 15 Message: Common Address=%02x, cot_%02x, num_ %d",rtuAddr,bCot,num);
	b_ui8 sequence;
	timeb currentSystemTime;   //TD16182
	ftime ( &currentSystemTime );
	m_lastUpdateTimeSec = currentSystemTime.time;

	TA_Base_Bus::DataPoint * dp;

	for(int i=0;i< num;i++)
	{
		cv =vCounterValue[i];
		memcpy(&sequence, &cv.BCR.SequenceNotation,1);
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive Point:%02d IOA=%06d,{Counter:%04x,Sequence:%02d,IV:%d,CY:%d,CA:%d}",
			i,cv.iOffset,cv.BCR.Counter,sequence,
			cv.BCR.SequenceNotation.IV,
			cv.BCR.SequenceNotation.CY,
			cv.BCR.SequenceNotation.CA);
		
		PScadaDataPointMap::iterator itr = m_InputDataPoints.find( cv.iOffset );

		if (itr != m_InputDataPoints.end() )
		{
			dp = itr->second;

			TA_Base_Bus::EQualityStatus quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

			if (cv.BCR.SequenceNotation.CY == 1)
				quality = TA_Base_Bus::QUALITY_BAD_DEVICE_FAILURE;

			if (cv.BCR.SequenceNotation.IV == 1)
				quality = TA_Base_Bus::QUALITY_BAD_SENSOR_FAILURE;

			//if (cv.BCR.SequenceNotation.CA == 1)
			//	quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;

			// set up DataPointState with the specified timestamp, quality and value
			TA_Base_Bus::DpValue dpValue( dp->getDataPointDataType(), dp->getEnumLabelsMap(), dp->getBooleanLabels() );
			dpValue.setFloat( cv.BCR.Counter, dp->getPrecision() );
			boost::shared_ptr<TA_Base_Bus::DataPointState> newState(new TA_Base_Bus::DataPointState( dpValue, currentSystemTime, quality));
			m_pscadaDpProcessor->submitUpdateDataPoint(dp,  newState);

		}	
	}
	FUNCTION_EXIT("handleIntegratedTotal");
}

void PScadaEventHandler::handleIntegratedTotalWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCounterValueTimeTag>&vCounterValueTm)
{
	FUNCTION_ENTRY("handleIntegratedTotalWithTimeTag");
	FUNCTION_EXIT("handleIntegratedTotalWithTimeTag");
}

void PScadaEventHandler::handleInterrogateGeneral(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bQoi)
{
	//all data transfereded
	if(10 == bCot)
	{
		m_pscadaUnit->setAllDataTransfered(true);
	}
}

void PScadaEventHandler::handleInterrogateCounter(b_ui16 iRtuAddr,b_ui8 bCot , b_ui8 bRqt,b_ui8 bFrz)
{
	if(10 == bCot)
	{
		m_pscadaUnit->setKwhDataTransfered(true);
		/*if(	true == m_isConnectionTimerStarted &&
			true == m_pscadaUnit->isAllDataTransfered() && 
	   		true == m_pscadaUnit->isKwhDataTransfered() )
		{
			TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock(this, m_monitorConnectionTimeId);
			m_isConnectionTimerStarted = false;
			m_monitorConnectionTimeId = -1;
		}*/
	}
}
void PScadaEventHandler::handleSingleCommand(int rtuAddr,int iInfoAddr,b_ui8 btCot,b_ui8 bSco)
{
	FUNCTION_ENTRY("handleSingleCommand");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Receive type 45 message: rtu_%02x,info_%06x cot_%02x Sco_%02x",rtuAddr,iInfoAddr,btCot,bSco);

	PScadaDataPointMap::iterator itr = m_OutputDataPoints.find( iInfoAddr );

	if (m_OutputDataPoints.end() == itr)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "no datapoint find for address:%d", iInfoAddr);
		return;
	}

	int test = btCot & 0x80;
	int pn = btCot & 0x40;
	int cot = btCot & 0x3f;
	TA_Base_Bus::DataPoint * dp = itr->second;
	ECommandPolicyResult policyResult;

	switch (cot)
	{
	case 7:
		if (bSco & 0x80)
		{
			if (m_pscadaMsgTimer)
			{
				m_pscadaMsgTimer->removeMessage(dp,TA_Base_Bus::selectCmdConfirmTimeout);
			}
			if (pn > 0)
			{
				PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::selectCmdCfmNegative, true );
			}
			else
			{
				policyResult = PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::selectCmdConfirm, true );
				if (m_pscadaMsgTimer && PR_ACCEPTED == policyResult)
				{
					m_pscadaMsgTimer->addMessage(dp,TA_Base_Bus::operateCmdTimeout);
				}
			}
		}
		else
		{
			if (m_pscadaMsgTimer)
			{
				m_pscadaMsgTimer->removeMessage(dp,TA_Base_Bus::execCmdConfirmTimeout);
			}
			if (pn > 0)
			{
				PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::execCmdCfmNegative, true );
			}
			else
			{
				policyResult = PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::execCmdConfirm, true );
				if (m_pscadaMsgTimer && PR_ACCEPTED == policyResult)
				{
					m_pscadaMsgTimer->addMessage(dp,TA_Base_Bus::execCmdSuccessTimeout);
				}
			}
		}
		break;

	case 10:
		if (m_pscadaMsgTimer)
		{
			m_pscadaMsgTimer->removeMessage(dp,TA_Base_Bus::execCmdSuccessTimeout);
		}
		policyResult = PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::execCmdSuccess, true );
		if (PR_ACCEPTED == policyResult)
		{
			dp->startABETimeout(dp->getSession());
		}
		break;

	case 9:
		if (m_pscadaMsgTimer)
		{
			m_pscadaMsgTimer->removeMessage(dp,TA_Base_Bus::cancelCmdConfirmTimeout);
		}
		if (pn > 0)
		{
			PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::cancelCmdCfmNegative, true );
		}
		else
		{
			PScadaCommandPolicy::updateControlState (dp, TA_Base_Bus::cancelCmdConfirm, true );
		}	
		break;

	default:
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "the cot:%d is not been handled.", cot);
	}

	FUNCTION_EXIT("handleSingleCommand");
}	


bool PScadaEventHandler::addDataPoint(TA_Base_Bus::DataPoint * dp)
{
	FUNCTION_ENTRY("addDataPoint");
	TA_ASSERT(NULL != dp, "dp is NULL in PScadaEventHandler::addDataPoint");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add DataPoint:0x%p Address:%04d Name(%s) ",dp,dp->getDataPointRegisterAddress(),dp->getDataPointName().c_str());

	// only hold output datapoints
	bool isAdded = false;
	if (dp->isWriteable())
	{
		m_OutputDataPoints[ dp->getDataPointRegisterAddress() ] = dp;

		isAdded = true;
	}
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin Add DataPoint to PScadaEventHandler");
		m_InputDataPoints[ dp->getDataPointRegisterAddress() ] = dp;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End Add DataPoint to PScadaEventHandler");

		isAdded = true;
	}
	FUNCTION_EXIT("addDataPoint");
	return isAdded;
}


void PScadaEventHandler::removeDataPoint (TA_Base_Bus::DataPoint * dp)
{
	FUNCTION_ENTRY("removeDataPoint");
	TA_ASSERT(NULL != dp, "dp is NULL in PScadaEventHandler::removeDataPoint");


	PScadaDataPointMap::iterator itr = m_InputDataPoints.find( dp->getDataPointRegisterAddress() );

	if ( itr != m_InputDataPoints.end() )
	{
		m_InputDataPoints.erase( itr );
	}

	itr = m_OutputDataPoints.find( dp->getDataPointRegisterAddress() );

	if ( itr != m_OutputDataPoints.end() )
	{
		m_OutputDataPoints.erase( itr );
	}
	FUNCTION_EXIT("removeDataPoint");

}


///This method will be called when one link is close by connectThread actively
//connectStatus=0,disconnect;
//connectStatus=1,connect;
void PScadaEventHandler::handleLinkConnectStatusChanged(b_ui8 groupID,string &linkIP,int connectStatus)
{
	FUNCTION_ENTRY("handleLinkConnectStatusChanged");

	TA_Base_Core::ThreadGuard guard(m_linkStatusLock);

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "HB--Link Status Changed: Group %d - IPAddress:%s Connect Status: %d",groupID,linkIP.c_str(),connectStatus);
	
	m_pscadaUnit->setPScadaLinkConnectStatus(groupID,linkIP.c_str(),connectStatus==0?false:true);

	if (!m_linkStatusBadToGood && m_pscadaUnit->isLinkConnected())
	{
		m_linkStatusBadToGood = true;
		// update m_lastUpdateTimeSec when link status from bad to good 
		timeb currentSystemTime;
		ftime ( &currentSystemTime );
		m_lastUpdateTimeSec = currentSystemTime.time;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--Link Status bad to good update time:%u", m_lastUpdateTimeSec);
	}
	
	if (m_linkStatusBadToGood && !m_pscadaUnit->isLinkConnected())
	{
		m_linkStatusBadToGood = false;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--Link Status good to bad");
	}

	//master link disconnect, other link is ok, if data not tranfered in time set quality to bad; 
	if(0 == connectStatus && m_pscadaUnit->isMasterPScadaLink(groupID, linkIP))
	{

		int dataLostDurableTime = m_pscadaUnit->getDataLostDurableTimeInsec();
		if(false == m_pscadaUnit->isLinkConnected() || 0 >= dataLostDurableTime)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "submit request set all datapoint's quality to bad,due to all link disconnected");
			m_pscadaDpProcessor->submitUpdateAllQualityToBad();
			return;
		}
		else if(false == m_isConnectionTimerStarted )
		{
			m_monitorConnectionTimeId = TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock(this,dataLostDurableTime*1000,true);
			m_isConnectionTimerStarted = true;
			m_pscadaDpProcessor->submitStartRecordDpUpdate();
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "start timer due to pscada master link disconnected,wait data");
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Timer has started");
		}
	}

	FUNCTION_EXIT("handleLinkConnectStatusChanged");
}

//mode=1 master
//mode=0 stanby
void PScadaEventHandler::handleLinkModeStatus(b_ui8 groupID,string &linkIP,int mode)
{
	m_pscadaUnit->setPScadaLinkModeStatus(groupID,linkIP.c_str(),mode==0?false:true);
	// change master link
	if (mode != 0)
	{
		timeb currentSystemTime;
		ftime ( &currentSystemTime );
		m_lastUpdateTimeSec = currentSystemTime.time;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"HB--Link Mode Changed: Group:%d,IPAddress:%s,mode:%d,update time:%u",
			groupID,linkIP.c_str(),mode, m_lastUpdateTimeSec);
	}
	else
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--Link Mode Changed: Group:%d,IPAddress:%s,mode:%d",groupID,linkIP.c_str(),mode);
	}
}

///This method will be called when link which is used to transfer data changes
//changeReason=0, link connect,                
//changeReason=1, link disconnect,		
//changeReason=2, pscada status SLAVE2MASTER	
//changeReason=3, pscada status MASTER2SLAVE	
void PScadaEventHandler::handleDataTransferLinkChanged(b_ui8 newGroupID,string &newLinkIP,b_ui8 oldGroupID,string&oldLinkIP,int changeReason)
{
	FUNCTION_ENTRY("handleDataTransferLinkChanged");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "HB--Link Status Changed: old (Group %d - IPAddress:%s) to new(Group %d - IPAddress:%s) Reason: %d",oldGroupID,oldLinkIP.c_str(),newGroupID,newLinkIP.c_str(),changeReason);
	//m_pscadaUnit->setPScadaMasterLink(oldGroupID,oldLinkIP.c_str(),false);
	//m_pscadaUnit->setPScadaMasterLink(newGroupID,newLinkIP.c_str(),true);
	/*switch (changeReason)
	{
	case 0:
		m_pscadaUnit->setPScadaMasterLink(newGroupID,newLinkIP.c_str(),true);
		break;
	case 1:
		m_pscadaUnit->setPScadaLink(newGroupID,newLinkIP.c_str(),false);
		m_pscadaUnit->setAllDataTransfered(false);
		TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock(this,5*1000,true);
		
		break;
	case 2:
		m_pscadaUnit->setPScadaLink(newGroupID,newLinkIP.c_str(),true,true);
		break;
	case 3:
		m_pscadaUnit->setPScadaLink(newGroupID,newLinkIP.c_str(),true,true);
		break;
	default:
		break;
	}*/
	
	FUNCTION_EXIT("handleDataTransferLinkChanged");
}



void PScadaEventHandler::setPScadaUnit(TA_IRS_App::PScadaUnit* pscadaUnit)
{
	m_pscadaUnit = pscadaUnit;
	int updateLostTime = pscadaUnit->getUpdateLostPeriodInSec();
	if (updateLostTime <= 0)
	{
		m_updateLostPeriodTime = 0;
	}
	else if (updateLostTime < 60)
	{
		m_updateLostPeriodTime = 60;
	}
	else
	{
		m_updateLostPeriodTime = updateLostTime;
	}
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--set update lost period time:%us", m_updateLostPeriodTime);
}

void PScadaEventHandler::timerExpired(long timerId, void* userData)
{
	if (timerId == m_monitorLastUpdateTimeId)
	{
		timeb currentSystemTime;
		ftime ( &currentSystemTime );
		unsigned long currentTimeSec = currentSystemTime.time;
		bool updateExpired = (currentTimeSec - m_lastUpdateTimeSec >= m_updateLostPeriodTime );
		if(m_isInControl && updateExpired && m_pscadaUnit->isLinkConnected())
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--update expired handle time out");
			handleUpdateTimeOut();
		}
	}
	else if (timerId == m_monitorConnectionTimeId)
	{
		m_isConnectionTimerStarted = false;
		m_monitorConnectionTimeId = -1;
		m_pscadaDpProcessor->submitStopRecordDpUpdate();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "timerExpired:submit submitStopRecordDpUpdate");
	}
}

void PScadaEventHandler::handleUpdateTimeOut()
{
	m_pscadaUnit->resetLink();
}

void PScadaEventHandler::setOperationMode(bool bControl)
{
	m_isInControl = bControl;
	if (bControl && m_updateLostPeriodTime > 0)
	{
		// update m_lastUpdateTimeSec when control mode 
		timeb currentSystemTime;
		ftime ( &currentSystemTime );
		m_lastUpdateTimeSec = currentSystemTime.time;

		// start timer if not start
		if (m_monitorLastUpdateTimeId == -1)
		{
			m_monitorLastUpdateTimeId = TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock(this, s_UpdateMonitorTimeSlice, false);
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--Set to control, timer:%d, update time:%u",
			m_monitorLastUpdateTimeId, m_lastUpdateTimeSec);
	}
	else if (m_monitorLastUpdateTimeId != -1)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "HB--stop update monitor timer:%u for monitor mode", m_monitorLastUpdateTimeId);
		TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock(this, m_monitorLastUpdateTimeId);
		m_monitorLastUpdateTimeId = -1;
	}
}

void PScadaEventHandler::stop()
{
	TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock(this);

	if (NULL != m_pscadaDpProcessor)
	{
		m_pscadaDpProcessor->terminateAndWait();
	}
}
}
