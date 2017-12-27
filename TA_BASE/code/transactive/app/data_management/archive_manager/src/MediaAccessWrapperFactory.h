/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/MediaAccessWrapperFactory.h $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Factory class to create wrapper instance for IMAPI/NERO
  */
#if !defined(MediaAccessWrapperFactory_H_)
#define MediaAccessWrapperFactory_H_

#include "app/data_management/archive_manager/src/IMediaAccess.h"
#include "app/data_management/archive_manager/src/IMAPIWrapper.h"

namespace TA_Base_App
{

	class MediaAccessWrapperFactory
	{
	public:
		static IMediaAccess* createIMAPIWrapper();
	};

}

#endif