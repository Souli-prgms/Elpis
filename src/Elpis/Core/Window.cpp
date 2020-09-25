#include "Window.h"

#ifdef EL_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Elpis
{
	Scope<Window> Window::create(const WindowProperties& props)
	{
		#ifdef EL_PLATFORM_WINDOWS
			return createScope<WindowsWindow>(props);
		#else
			EL_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
}