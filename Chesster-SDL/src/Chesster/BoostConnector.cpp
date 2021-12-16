#include "pch.h"
#include "BoostConnector.h"

#include "Chesster/States/GameState.h"

#define PY_SSIZE_T_CLEAN
#include "Python.h"

namespace Chesster
{
	BoostConnector::BoostConnector() :
		//EnginePath{ "C:\\Dev\\PUPR-Projects\\Capstone-Design\\Chesster-SDL\\resources\\engines\\stockfish\\stockfish_14.1_win_x64_avx2\\stockfish_14.1_win_x64_avx2.exe" },
		EnginePath{ "resources/engines/stockfish/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe" },
		m_IO{ 1 },
		m_Sink{ m_IO },
		m_Source{ m_IO },
		m_Engine_Child{ EnginePath, boost::process::std_in < m_Sink, boost::process::std_out > m_Source, m_IO },
		m_Input{ "" }
	{
		// Initialize stockfish
		boost::asio::spawn([this](yield_context yield)
		{
			SendCommand("uci\nucinewgame\nisready\n", yield);
			for(;;)
			{
				std::string msg{ GetEngineReply(yield) };
				GameState::ImGuiMainWindow.AddLog(msg.c_str());

				if (msg.find("readyok") != std::string::npos)
					m_IO.stop();
			}
		});

		m_IO.run();
		m_IO.reset();

		Py_Initialize();
	}

	BoostConnector::~BoostConnector()
	{
		Py_Finalize();

		boost::asio::spawn([this](yield_context yield)
		{ SendCommand("quit\n", yield); });

		m_Engine_Child.terminate();
	}

	bool BoostConnector::SendCommand(const std::string& command, yield_context yield)
	{
		using boost::asio::buffer;
		boost::asio::async_write(m_Sink, buffer(command), yield);
		return true;
	}

	const std::string& BoostConnector::GetEngineReply(yield_context yield)
	{
		m_Input.clear();
		auto buf = boost::asio::dynamic_buffer(m_Input);

		boost::asio::async_read_until(m_Source, buf, "\n", yield);

		return m_Input;
	}
}
