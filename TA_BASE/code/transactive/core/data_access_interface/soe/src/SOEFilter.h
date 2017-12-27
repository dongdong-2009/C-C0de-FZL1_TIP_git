#ifndef SOE_EVENT_FILTER_INCLUDED
#define SOE_EVENT_FILTER_INCLUDED

#include <sys/timeb.h>
#include <string>
#include <vector>

namespace TA_Base_Core
{



	struct SOEEventFilter
	{
		static const std::string FILTER_DISPLAY_VALUE;
		static const std::string FILTER_ASSET_NAME;
		static const std::string FILTER_LOCATION_KEY;
		static const std::string FILTER_DESCRIPTION;


		bool isEnabled;

		time_t tmFrom;
		time_t tmTo;

		std::string strAssetName;
		std::vector<unsigned long> locations;
		std::string strDisplayValue;

		std::string strDescription;

		std::string strCurrPageTime;

		//Li Zhongyan add 
		double currPageEventID;
		unsigned int currentPage;

		bool bSortAscend;
		std::string strSortColumn;

		SOEEventFilter::SOEEventFilter()
		{
			 strAssetName="";			 
			 strDisplayValue="";
			 strDescription="";
			 strCurrPageTime="";
             strSortColumn="";
		}
	};
}

#endif // SOE_EVENT_FILTER_INCLUDED
