#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/PidStationIdAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "StationZonesProcess.h"

const std::string zonetypes[] =
{
	"UT01",
	"DT01",
	"CONC01",
	"GT01"
};
std::vector<std::string> makezones()
{
	std::vector<std::string> tmpvector;
    std::vector<TA_Base_Core::ILocation*> locations 
        = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
    unsigned long j = 0;
    for (std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin();
        it != locations.end();
        it++)
    {
        std::string stationname = (*it)->getName().c_str();
        if (stationname.compare("All Locations")!=0 && stationname.compare("OCC")!=0 && stationname.compare("TDS")!=0)
        {
		    for(int j = 0; j < sizeof(zonetypes)/sizeof(zonetypes[0]); j++)
		    {
			    tmpvector.push_back(stationname+".PIDS."+zonetypes[j]);
		    }
        }
        delete (*it);
        (*it) = NULL;
	}
	return tmpvector;
}

std::map<std::string, unsigned long> makestaionMap()
{
    static bool init = false;
    static std::map<std::string, unsigned long> tmpMap;
    if ( !init )
    {
        // [ToDo] Do not use sql in application layer, use the ones in core/DAI
        //TA_Base_Core::IDatabase* databaseConnection =
        //    TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Tis_Cd, TA_Base_Core::Read );
        //std::string sql = "select l.name as station_name, t.station_id as station_id from location l, ti_station_id t where l.pkey = t.ta_location";
        //std::vector<std::string> columns;
        //std::string stationName;
        //unsigned long stationId;
        //columns.push_back( "station_name" );
        //columns.push_back( "station_id" );
        //try
        //{
        //    TA_Base_Core::IData* data = databaseConnection->executeQuery( sql, columns );
        //    do
        //    {
        //        std::auto_ptr<TA_Base_Core::IData> dataWrap( data );
        //        for ( unsigned long i = 0, size = data->getNumRows(); i < size; ++i )
        //        {
        //            stationName = dataWrap->getStringData( i, "station_name" );
        //            stationId = dataWrap->getUnsignedLongData( i, "station_id" );
        //            tmpMap.insert( std::make_pair( stationName, stationId ) );
        //        }
        //    } while ( databaseConnection->moreData( data ) );
        //    init = true;
        //}
        //catch ( const TA_Base_Core::TransactiveException& e )
        //{
        //    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        //}
		try
		{
			tmpMap = TA_Base_Core::PidStationIdAccessFactory::getInstance().getLocationNameToStationIdMap();
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Failed init station id location key map");
		}
    }
    return tmpMap;
}
