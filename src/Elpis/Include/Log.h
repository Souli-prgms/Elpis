#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Core.h"

namespace Elpis
{

	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define EL_CORE_TRACE(...)    ::Elpis::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define EL_CORE_INFO(...)     ::Elpis::Log::GetCoreLogger()->info(__VA_ARGS__)
#define EL_CORE_WARN(...)     ::Elpis::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define EL_CORE_ERROR(...)    ::Elpis::Log::GetCoreLogger()->error(__VA_ARGS__)
#define EL_CORE_CRITICAL(...) ::Elpis::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define EL_TRACE(...)         ::Elpis::Log::GetClientLogger()->trace(__VA_ARGS__)
#define EL_INFO(...)          ::Elpis::Log::GetClientLogger()->info(__VA_ARGS__)
#define EL_WARN(...)          ::Elpis::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EL_ERROR(...)         ::Elpis::Log::GetClientLogger()->error(__VA_ARGS__)
#define EL_CRITICAL(...)      ::Elpis::Log::GetClientLogger()->critical(__VA_ARGS__)