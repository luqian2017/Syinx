#include <stdio.h>
#include <iostream>
using namespace std;
#include "./Syinx/Syinx.h"
#include "./Sylog/easylogging++.h"

INITIALIZE_EASYLOGGINGPP;

int main()
{
	// Load configuration from file
	el::Configurations conf("./Config/Syinx-Server.logger.conf");
#ifdef WIN32
	conf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
#endif
	// Actually reconfigure all loggers instead
	el::Loggers::reconfigureAllLoggers(conf);

	if (!g_pSyinx.Initialize())
		return -1;

	g_pSyinx.Run();

	g_pSyinx.Close();
}