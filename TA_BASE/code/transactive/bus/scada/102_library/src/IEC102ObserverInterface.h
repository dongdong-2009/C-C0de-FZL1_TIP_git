/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/102_library/src/IEC102ObserverInterface.h $
  * @author:	HoaVu
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2016/02/16 09:11:29 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description:
  *
  *	The IEC102Interface is responsible for providing interface to IEC 102 library users.
  *
  */


#ifndef IEC102INTERFACE_H__
#define IEC102INTERFACE_H__


namespace TA_Base_Bus
{
	//Application/Agent  will derive from this interface, in order to get the channel status update from 102 library.
	class IEC102Observer
	{
	public:

		virtual void updateChannelStatus(const bool& status)=0;
	};

	//102 library will implement these to provide the interface
	class IIEC102Channel
	{
	public:

		virtual void registerIEC102Observer(IEC102Observer * observer)=0;
		virtual void removeIEC102Observer(const IEC102Observer *observer)=0;

	};
};


#endif

