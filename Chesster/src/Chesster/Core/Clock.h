#pragma once

#include <chrono>

#include <SDL_timer.h>

namespace Chesster
{
	/*	A clock useful to calculate the passage of time.
		It uses the chrono library from the C++ standard library
		and the SDL_GetTicks64() function from the SDL library. */
	struct Clock
	{
		using duration = std::chrono::milliseconds;
		using rep = duration::rep;
		using period = duration::period;
		using time_point = std::chrono::time_point<Clock>;
		static constexpr bool IsSteady{ true };

		/** Used for the variable timestep.
		 @return The number of milliseconds since the application started. */
		static time_point Now() noexcept
		{
			return time_point{ duration{ SDL_GetTicks64() } };
		}
	};
}
