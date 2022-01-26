#pragma once

#include "spdlog/spdlog.h"
namespace Chesster
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
	};
}

// Core log macros
#define CHESSTER_TRACE(...)	::Chesster::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define CHESSTER_INFO(...)	::Chesster::Log::GetCoreLogger()->info(__VA_ARGS__);
#define CHESSTER_WARN(...)	::Chesster::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define CHESSTER_ERROR(...)	::Chesster::Log::GetCoreLogger()->error(__VA_ARGS__);
#define CHESSTER_CRITICAL(...)	::Chesster::Log::GetCoreLogger()->critical(__VA_ARGS__);
