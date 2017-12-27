/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/MediaAccessWrapperFactory.cpp $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Factory class to create wrapper instance for IMAPI/NERO.
  */

#include "app/data_management/archive_manager/src/MediaAccessWrapperFactory.h"

using namespace TA_Base_App;

IMediaAccess* MediaAccessWrapperFactory::createIMAPIWrapper()
{
	IMediaAccess * _IMediaAccess = dynamic_cast<IMediaAccess*>(new IMAPIWrapper()); 
	return _IMediaAccess;

}
