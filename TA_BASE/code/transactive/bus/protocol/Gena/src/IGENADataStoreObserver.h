/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENATopicData.h  $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for IGENADataStoreObserver
  * Usage: class is used in receiving and updating data changed event
  */

#ifndef __IGENA_DATA_STORE_OBSERVER_H_INCLUDED__
#define __IGENA_DATA_STORE_OBSERVER_H_INCLUDED__

namespace TA_Base_Bus
{

	class IGENADataStoreObserver
	{
	public:
		/*
		* Virtual Destructor
		*/
		virtual ~IGENADataStoreObserver() {};

		/* 
		 * OnDataChange
		 *
		 * @purpose: function called to notify observers that a data change has occurred.
		 * @Param: topicName - topic name of the data
				   topicData - data containing the update information
		 */
		virtual void onDataChange(const std::string& topicName,  GENATopicDataPtr topicData) = 0;
	
	private:

	};

}

#endif //__IGENA_DATA_STORE_OBSERVER_H_INCLUDED__