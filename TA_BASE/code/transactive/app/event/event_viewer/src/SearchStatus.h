/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/SearchStatus.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * SearchStatus is a enum class which inlcudes 3 types of searching satus for a worker.
  *
  * InSearching means the worker is in loading process ( loading historical data for historical mode, 
  * or loading backlog records for real time mode), but not finished.
  *
  * FinishSearch means the worker has finished the loading.
  *
  * EndButNotFinishSearch means the worker has stopped the loading process but it hasn't been finished.
  * (e.g. the searching task has been stopped before it returns all the records)
  *
  */
#if !defined(EA_F4E08E9A_5C23_4a3d_8B62_9E0E1EF63582__INCLUDED_)
#define EA_F4E08E9A_5C23_4a3d_8B62_9E0E1EF63582__INCLUDED_

namespace TA_Base_App{

	enum SearchStatus
	{
		/*StartSearch,*/
		InSearching,
		FinishSearch,
		EndButNotFinishSearch
	};
};

#endif // !defined(EA_F4E08E9A_5C23_4a3d_8B62_9E0E1EF63582__INCLUDED_)
