#pragma once

#include <chrono>

#include "Core.h"
#include "SDL_timer.h"

namespace Chesster
{
	struct Clock
	{
		using duration = std::chrono::milliseconds;
		using rep = duration::rep;
		using period = duration::period;
		using time_point = std::chrono::time_point<Clock>;
		static constexpr bool IsSteady{ true };

		static time_point Now() noexcept
		{
			return time_point{ duration{SDL_GetTicks64()} };
		}
	};
}
