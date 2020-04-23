#include "Syinx/include/SyInc.h"
#include "Syinx/include/Syinx.h"

int main(int argc, char* argv[])
{
	if (!g_pSyinx.Config(argc, argv))
		return -1;
	if (!g_pSyinx.Initialize())
		return -1;

	g_pSyinx.Run();

	g_pSyinx.Close();
}