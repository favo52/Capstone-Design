#include "pch.h"
#include "Time.h"

namespace Chesster
{
	Timer::Timer() :
		m_StartTicks{ 0 },
		m_PausedTicks{ 0 },
		m_Paused{ false },
		m_Started{ false }
	{
	}

	void Timer::Start()
	{
		// Start the timer
		m_Started = true;
		m_Paused = false;

		// Get current clock time
		m_StartTicks = SDL_GetTicks();
		m_PausedTicks = 0;
	}

	void Timer::Stop()
	{
		// Stop the timer
		m_Started = false;
		m_Paused = false;

		// Clear tick variables
		m_StartTicks = 0;
		m_PausedTicks = 0;
	}

	void Timer::Pause()
	{
		if (m_Started && !m_Paused)
		{
			// Pause the timer
			m_Paused = true;

			// Calculate paused ticks
			m_PausedTicks = SDL_GetTicks() - m_StartTicks;
			m_StartTicks = 0;
		}
	}

	void Timer::Unpause()
	{
		// If the timer is running and paused
		if (m_Started && m_Paused)
		{
			// Unpause timer
			m_Paused = false;

			// Reset the starting ticks
			m_StartTicks = SDL_GetTicks() - m_PausedTicks;

			// Reset the paused ticks
			m_PausedTicks = 0;
		}
	}

	Uint32 Timer::GetTicks()
	{
		Uint32 time{ 0 };

		if (m_Started)
		{
			if (m_Paused)
				time = m_PausedTicks;
			else
				time = SDL_GetTicks() - m_StartTicks;
		}

		return time;
	}

	bool Timer::IsStarted()
	{
		return m_Started;
	}

	bool Timer::IsPaused()
	{
		return m_Paused && m_Started;
	}
}
