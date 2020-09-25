#include "Renderer.h"

int main(int argc, char *argv[])
{
	Elpis::Log::Init();
	Elpis::Renderer* renderer = new Elpis::Renderer;
	renderer->run();
}
