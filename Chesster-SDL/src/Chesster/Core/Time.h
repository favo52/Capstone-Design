#pragma once

#include "SDL_timer.h"

namespace Chesster
{
	struct Clock
	{
		using duration = std::chrono::milliseconds;
		using rep = duration::rep;
		using period = duration::period;
		using time_point = std::chrono::time_point<Clock>;
		static constexpr bool is_steady = true;

		static time_point now() noexcept
		{
			return time_point{ duration{SDL_GetTicks()} };
		}
	};

	class Timer
	{
	public:
		Timer();

		// Clock actions
		void Start();
		void Stop();
		void Pause();
		void Unpause();

		// Gets the timer's time;
		Uint32 GetTicks();

		// Checks status
		bool IsStarted();
		bool IsPaused();

	private:
		// Clock's time when it started
		Uint32 m_StartTicks;

		// Ticks stored when timer was paused
		Uint32 m_PausedTicks;

		// Timer status
		bool m_Paused;
		bool m_Started;
	};
}
