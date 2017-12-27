#if !defined(MediaStatus_H_)
#define MediaStatus_H_
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/MediaStatus.h $
  * @author:  Lincy
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/04/13 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * MediaStatus enumeration and BurnStatus enumeration
  */

namespace TA_Base_App
{
	enum MediaStatus
	{	
		NoValidDevice,
		DriveBusy,
		InvalidDrive,
		NoMedia,
		MediaNotEmpty,
		MediaWriteProtected,
		MediaIncompatible,
		MediaOutOfMemory,
		WriteFailed,
		WriteCancelled,
		WriteSuccessful,
		Unknown
	};

	enum BurnStatus
	{
		ValidatingMedia,
		FormattingMedia,
		InitialisingHardware,
		CalibratingPower,
		Writing,
		WriteFinalization,
		WriteCompleted,
		WriteVerifying,
		None
	};
}


#endif