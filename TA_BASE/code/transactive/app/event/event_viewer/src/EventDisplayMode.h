/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/EventDisplayMode.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * EventDisplayMode is a enum structure which represent 2 modes of event displaying:
  * historical mode and real time mode. In real time mode, the main GUI will refresh 
  * for adding the latest new event. In the historical mode, it won't refresh for new events.
  *
  */

#if !defined(EA_194A702F_C7A1_420e_903E_61A1F3E0FFFB__INCLUDED_)
#define EA_194A702F_C7A1_420e_903E_61A1F3E0FFFB__INCLUDED_

namespace TA_Base_App{

	enum EventDisplayMode
	{
		HistoricalMode,
		RealTimeMode
	};
};

#endif // !defined(EA_194A702F_C7A1_420e_903E_61A1F3E0FFFB__INCLUDED_)
