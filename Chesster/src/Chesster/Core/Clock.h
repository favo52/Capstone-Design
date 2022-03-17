#pragma once

#include <chrono>

#include <SDL_timer.h>

namespace Chesster
{
	/// <summary>
	/// A clock useful to calculate the passage of time.
	/// It uses the chrono library from the C++ standard library
	/// and the SDL_GetTicks64() function from the SDL library.
	/// </summary>
	struct Clock
	{
		using duration = std::chrono::milliseconds;
		using rep = duration::rep;
		using period = duration::period;
		using time_point = std::chrono::time_point<Clock>;
		static constexpr bool IsSteady{ true };

		/// <summary>
		/// Used for the variable timestep.
		/// </summary>
		/// <returns>The number of milliseconds since the application started.</returns>
		static time_point Now() noexcept
		{
			return time_point{ duration{SDL_GetTicks64()} };
		}
	};
}
