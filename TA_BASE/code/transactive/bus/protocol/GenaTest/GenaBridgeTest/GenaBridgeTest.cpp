// GenaBridgeTest.cpp : Defines the entry point for the console application.
//

#include <Poco/Util/ServerApplication.h>
#include "bus/protocol/Gena/src/GENALibrary.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Bus;
using namespace TA_Base_Core;

int main(int argc, char* argv[])
{
	struct App : Poco::Util::ServerApplication {
		~App() {
			waitForTerminationRequest();
		}
	} app;

	char LogPath[256];
	sprintf(LogPath, "GenalBridgeTesting.log");
	TA_Base_Core::DebugUtil::getInstance().setFile(LogPath);
	TA_Base_Core::DebugUtil::getInstance().setMaxSize(2000000);
	TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugDebug);

	TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv);

	TA_Base_Core::CorbaUtil::getInstance().initialise(7777);
	TA_Base_Core::CorbaUtil::getInstance().activate();

	//ISCSGenaHelper::getInstance().initialize(10015063);
	GENALibrary::getInstance().initialize();
	GENALibrary::getInstance().startService();

	return 0;
}

