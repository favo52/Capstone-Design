#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // Output stream for logging custom types
#pragma warning(pop)

namespace Chesster
{
	/// <summary>
	/// The Logger class is an abstraction of the spdlog open-source library.
	/// spdlog is a very fast, header-only/compiled, C++ logging library.
	/// It is used to print custom messages to the console and to files.
	/// </summary>
	class Logger
	{
	public:
		/// <summary>
		/// Creates the sinks, sets the text patterns, and registers the logger.
		/// Must be called before using the logger.
		/// </summary>
		static void Init();

		/// <summary>
		/// Retrieves the logger object that is used to print all the custom messages.
		/// </summary>
		/// <returns>A shared pointer of the static member variable s_ChessterLogger.</returns>
		static std::shared_ptr<spdlog::logger>& GetChessterLogger() { return s_ChessterLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_ChessterLogger;
	};
}

// Chesster logger macro shortcuts
#define LOG_TRACE(...)		::Chesster::Logger::GetChessterLogger()->trace(__VA_ARGS__);
#define LOG_INFO(...)		::Chesster::Logger::GetChessterLogger()->info(__VA_ARGS__);
#define LOG_WARN(...)		::Chesster::Logger::GetChessterLogger()->warn(__VA_ARGS__);
#define LOG_ERROR(...)		::Chesster::Logger::GetChessterLogger()->error(__VA_ARGS__);
#define LOG_CRITICAL(...)	::Chesster::Logger::GetChessterLogger()->critical(__VA_ARGS__);
