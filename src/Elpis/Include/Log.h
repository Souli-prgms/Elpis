#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Core.h"

namespace Elpis
{
	class Log
	{
	public:
		static void init();

		static Ref<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
		static Ref<spdlog::logger>& getClientLogger() { return s_clientLogger; }
	private:
		static Ref<spdlog::logger> s_coreLogger;
		static Ref<spdlog::logger> s_clientLogger;
	};
}

// Core log macros
#define EL_CORE_TRACE(...)		::Elpis::Log::getCoreLogger()->trace(__VA_ARGS__)
#define EL_CORE_INFO(...)		::Elpis::Log::getCoreLogger()->info(__VA_ARGS__)
#define EL_CORE_WARN(...)		::Elpis::Log::getCoreLogger()->warn(__VA_ARGS__)
#define EL_CORE_ERROR(...)		::Elpis::Log::getCoreLogger()->error(__VA_ARGS__)
#define EL_CORE_CRITICAL(...)	::Elpis::Log::getCoreLogger()->critical(__VA_ARGS__)
#define EL_CORE_ASSERT(x, msg)	assert(x && msg)

// Client log macros
#define EL_TRACE(...)			::Elpis::Log::GetClientLogger()->trace(__VA_ARGS__)
#define EL_INFO(...)			::Elpis::Log::GetClientLogger()->info(__VA_ARGS__)
#define EL_WARN(...)			::Elpis::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EL_ERROR(...)			::Elpis::Log::GetClientLogger()->error(__VA_ARGS__)
#define EL_CRITICAL(...)		::Elpis::Log::GetClientLogger()->critical(__VA_ARGS__)