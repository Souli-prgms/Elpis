#include "Renderer.h"

int main(int argc, char *argv[])
{
	Elpis::Log::Init();
	Renderer* renderer = new Renderer;
	renderer->run();
}
