#ifndef DATA_BLOCK_INTERFACE_H__
#define DATA_BLOCK_INTERFACE_H__

#include"bus/modbus_comms/src/DataBlock.h"
#include"core/types/public_types/Constants.h"

namespace TA_Base_Bus
{
	class DataBlockObserver
	{
	public:

		virtual TA_Base_Bus::DataBlock<WORD>& getDataBlock() = 0;
		virtual void updataDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock) = 0;
		virtual void updateQualityStatus(const TA_Base_Core::EDataPointQuality& quality) = 0;
	
	};


	class IProcessDataBlock
	{
	public:
		/**
		*	send data block use modbus function code 0x10
		*	@param dataBlock	the data block to be sent
		*	@return				if return 0,success;if return <0,fail;
		**/
		virtual int sendDataBlock(const TA_Base_Bus::DataBlock<WORD> &dataBlock) = 0;

		virtual void registerDataBlockObserver(DataBlockObserver * observer) = 0;

		virtual void removeDataBlockObserver( const DataBlockObserver *observer) = 0;

	};
};

#endif
