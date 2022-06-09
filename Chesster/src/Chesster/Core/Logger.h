/*
* Copyright 2021-present, Polytechnic University of Puerto Rico
* AUTHOR: Francisco A. Villagrasa
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/** Legend
 @param A parameter of the function.
 @return What the function returns. */

#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace Chesster
{
	/* The Logger class is an abstraction of the spdlog open-source library.
	   spdlog is a very fast, header-only/compiled, C++ logging library.
	   It is used to print custom messages to the console and to files. */
	class Logger
	{
	public:
		/* Creates the sinks, sets the text patterns, and registers the logger.
		   Must be called before using the Logger. */
		static void Init();

		/* Retrieves the logger object that is used to print all the custom messages.
		 @return A shared pointer of the spdlog::logger static member variable. */
		static std::shared_ptr<spdlog::logger>& GetChessterLogger() { return s_ChessterLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_ChessterLogger;
	};
}

// Chesster logger macro shortcuts
#define LOG_TRACE(...)		::Chesster::Logger::GetChessterLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)		::Chesster::Logger::GetChessterLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)		::Chesster::Logger::GetChessterLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		::Chesster::Logger::GetChessterLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)	::Chesster::Logger::GetChessterLogger()->critical(__VA_ARGS__)
