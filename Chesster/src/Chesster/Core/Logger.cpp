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

#include "pch.h"
#include "Chesster/Core/Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Chesster
{
	std::shared_ptr<spdlog::logger> Logger::s_ChessterLogger;

	void Logger::Init()
	{
		// The initialization is performed as recommended by the library's author
		try
		{
			// Create multithreaded stdout and file sinks, and insert to the std::vector
			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Chesster.log", true));

			// Set the pattern of the messages
			logSinks[0]->set_pattern("[%r][%^%l%$] %n: %v");
			logSinks[1]->set_pattern("[%b/%d/%Y][%r][%l] %n: %v");

			// Register the logger
			s_ChessterLogger = std::make_shared<spdlog::logger>("CHESSTER", begin(logSinks), end(logSinks));
			spdlog::register_logger(s_ChessterLogger);
			s_ChessterLogger->set_level(spdlog::level::trace);
			s_ChessterLogger->flush_on(spdlog::level::trace);
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
		}
	}
}
