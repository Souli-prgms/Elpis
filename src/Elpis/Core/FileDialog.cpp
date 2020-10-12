#include "FileDialog.h"

#ifdef EL_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsFileDialog.h"
#endif

namespace Elpis
{
	namespace FileDialog
	{
		std::string getFile()
		{
			#ifdef EL_PLATFORM_WINDOWS
				return WindowsFileDialog::getFile();
			#else
				EL_CORE_ASSERT(false, "Unknown platform!");
				return nullptr;
			#endif
		}
	}
}
