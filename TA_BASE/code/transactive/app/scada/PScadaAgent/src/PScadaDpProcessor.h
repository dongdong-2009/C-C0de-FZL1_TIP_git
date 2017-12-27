
#ifndef PSCADA_DP_PROCESSOR_H__
#define PSCADA_DP_PROCESSOR_H__

#include <map>
#include "boost/shared_ptr.hpp"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include <core/threads/src/QueueProcessor.h>

namespace TA_Base_Bus
{
	class DataPoint;
};
namespace TA_IRS_App
{
	class PScadaDpEvent
	{
	public:
		enum PScadaDpEventType
		{
			UnknownType,
			UpdateFiledValue,
			UpdateAllQulityToBad,
			StartRecordDpUpdate,		
			StopRecordDpUpdate   
		};
	public:
		PScadaDpEvent::PScadaDpEvent(PScadaDpEventType 	type,
									 TA_Base_Bus::DataPoint*			pDataPoint,
									 boost::shared_ptr<TA_Base_Bus::DataPointState>  dataPointState);
		
		PScadaDpEvent::PScadaDpEvent(PScadaDpEventType 	type = UnknownType);

		PScadaDpEventType	getEventType();
		TA_Base_Bus::DataPoint*			getDataPoint();
		boost::shared_ptr<TA_Base_Bus::DataPointState> getDataPointState();
	private:
		PScadaDpEventType 	m_type;
		TA_Base_Bus::DataPoint*			m_pDataPoint;
		boost::shared_ptr<TA_Base_Bus::DataPointState> m_dataPointState;
	};
	class PScadaDpProcessor:public TA_Base_Core::QueueProcessor<PScadaDpEvent>
	{

	public:
		PScadaDpProcessor(std::map < unsigned long, TA_Base_Bus::DataPoint * >& dataPoints);
		~PScadaDpProcessor();
		void submitUpdateDataPoint(TA_Base_Bus::DataPoint* pDataPoint, boost::shared_ptr<TA_Base_Bus::DataPointState> state);
		void submitUpdateAllQualityToBad();
		void submitStartRecordDpUpdate();
		void submitStopRecordDpUpdate();
		virtual void processEvent( boost::shared_ptr<PScadaDpEvent> newEvent );
	private:
		bool		checkUpdatable(TA_Base_Bus::DataPoint* dp,boost::shared_ptr<TA_Base_Bus::DataPointState> newUpdate);
	private:
		bool		m_isRecording;
		std::set < TA_Base_Bus::DataPoint * >	m_recordDataPoints;
		std::map < unsigned long, TA_Base_Bus::DataPoint * >&	m_dataPoints;

		typedef std::map <TA_Base_Bus::DataPoint *, boost::shared_ptr<TA_Base_Bus::DataPointState> > DataPointSavedUpdate;

		DataPointSavedUpdate	m_lastSavedUpdate;
	};	
}

#endif

