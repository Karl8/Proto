#include "renderer.h"

Renderer::Renderer()
{
	
}

Renderer::~Renderer()
{
	
}

void Renderer::renderLoop()
{
	while (true)
	{
		// beginFrame();

		// render()

		// endFrame();
	}
}

void Renderer::exec()
{
	initialize();

	renderLoop();

	terminate();
}


