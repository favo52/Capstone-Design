#pragma once

#include "SDL_rect.h"

namespace Chesster
{
	class Vector2f
	{
	public:
		Vector2f(float a = 0.0f, float b = 0.0f)
		{
			x = a;
			y = b;
		}

		Vector2f operator-() { return Vector2f(x * -1, y * -1); }
		Vector2f operator-(const Vector2f& vec2f) { return Vector2f(x + (vec2f.x * -1), y + (vec2f.y * -1)); }

		Vector2f operator+(const Vector2f& vec2f) { return Vector2f(x + vec2f.x, y + vec2f.y); }
		Vector2f operator+(const SDL_Point& sdl_point) { return Vector2f(x + sdl_point.x, y + sdl_point.y); }

		Vector2f operator*(const float& f) { return Vector2f(x * f, y * f); }
		Vector2f operator/(const float& f) { return Vector2f(x / f, y / f); }
		Vector2f operator/(const Vector2f& f) { return Vector2f(x / f.x, y / f.y); }

		bool operator==(const Vector2f& vec2f) { return x == vec2f.x && y == vec2f.y; }
		bool operator!=(const Vector2f& vec2f) { return !(*this == vec2f); }

		float x = 0.0f, y = 0.0f;
	};

	class Vector2i
	{
	public:
		Vector2i(int a = 0, int b = 0)
		{
			x = a;
			y = b;
		}

		Vector2i operator-() { return Vector2i(x * -1, y * -1); }
		Vector2i operator-(const Vector2i& vec2i) { return Vector2i(x + (vec2i.x * -1), y + (vec2i.y * -1)); }

		Vector2i operator+(const Vector2i& vec2i) { return Vector2i(x + vec2i.x, y + vec2i.y); }
		Vector2i operator+(const SDL_Point& sdl_point) { return Vector2i(x + sdl_point.x, y + sdl_point.y); }

		Vector2i operator*(const int& i) { return Vector2i(x * i, y * i); }
		Vector2i operator/(const int& i) { return Vector2i(x / i, y / i); }
		Vector2i operator/(const Vector2i& i) { return Vector2i(x / i.x, y / i.y); }
		Vector2i operator/(const Vector2f& i) { return Vector2i(x / i.x, y / i.y); }
		
		bool operator==(const Vector2i& vec2i) { return x == vec2i.x && y == vec2i.y; }
		bool operator!=(const Vector2i& vec2i) { return !(*this == vec2i); }

		Vector2i operator-=(const Vector2i& vec2i)
		{
			x = x - vec2i.x;
			y = y - vec2i.y;
			return Vector2i(x, y);
		}

		int x = 0, y = 0;
	};
	
	class Vector2d
	{
	public:
		Vector2d(double a = 0.0, double b = 0.0)
		{
			x = a;
			y = b;
		}

		Vector2d operator-() { return Vector2d(x * -1, y * -1); }
		Vector2d operator-(const Vector2d& vec2f) { return Vector2d(x + (vec2f.x * -1), y + (vec2f.y * -1)); }

		Vector2d operator+(const Vector2d& vec2f) { return Vector2d(x + vec2f.x, y + vec2f.y); }
		Vector2d operator+(const SDL_Point& sdl_point) { return Vector2d(x + sdl_point.x, y + sdl_point.y); }

		Vector2d operator*(const double& d) { return Vector2d(x * d, y * d); }
		Vector2d operator/(const double& d) { return Vector2d(x / d, y / d); }

		bool operator==(const Vector2d& vec2f) { return x == vec2f.x && y == vec2f.y; }
		bool operator!=(const Vector2d& vec2f) { return !(*this == vec2f); }

		double x = 0.0, y = 0.0;
	};
}
