#include "App.h"



int main(int argc, char *argv[])
{
	Elpis::Log::init();
	auto app = Elpis::createApp();
	app->run();
	delete app;
}
