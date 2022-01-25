#pragma once

#include "Chesster_Unleashed/Core/Core.h"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // Output stream for logging custom types
#pragma warning(pop)

namespace Chesster
{
	class Logger
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetChessterLogger() { return s_ChessterLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_ChessterLogger;
	};
}

// Chesster log macros
#define CHESSTER_TRACE(...)		::Chesster::Logger::GetChessterLogger()->trace(__VA_ARGS__);
#define CHESSTER_INFO(...)		::Chesster::Logger::GetChessterLogger()->info(__VA_ARGS__);
#define CHESSTER_WARN(...)		::Chesster::Logger::GetChessterLogger()->warn(__VA_ARGS__);
#define CHESSTER_ERROR(...)		::Chesster::Logger::GetChessterLogger()->error(__VA_ARGS__);
#define CHESSTER_CRITICAL(...)	::Chesster::Logger::GetChessterLogger()->critical(__VA_ARGS__);
