#pragma once

#include <boost/asio.hpp>
#include "boost/asio/spawn.hpp"
#include <boost/process.hpp>
#include <boost/process/async.hpp>

namespace Chesster
{
	using boost::asio::yield_context;

	class BoostConnector
	{
	public:
		BoostConnector();
		~BoostConnector();

	private:
		bool SendCommand(const std::string& command, yield_context yield);
		const std::string& GetEngineReply(yield_context yield);

	private:
		boost::filesystem::path EnginePath;

		boost::asio::io_context m_IO;
		boost::process::async_pipe m_Sink, m_Source;

		boost::process::child m_Engine_Child;
		std::string m_Input;
	};
}
