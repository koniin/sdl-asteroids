#ifndef ENGINE_H
#define ENGINE_H

#include "SDL.h"
#include <algorithm>
#include <vector>
#include <random>
#include <map>
#include <functional>
#include <iostream>
#include <sstream>

#ifdef _DEBUG
#define ASSERT_WITH_MSG(cond, msg) do \
{ if (!(cond)) { std::ostringstream str; str << msg; std::cerr << str.str(); std::abort(); } \
} while(0)
#else 
#define ASSERT_WITH_MSG(cond, msg) ;
#endif

// timer
typedef struct {
    Uint64 now;
    Uint64 last;
    double dt;
    double fixed_dt;
    double accumulator;
} gameTimer;


// Find functions here if you need them
// https://github.com/nicolausYes/easing-functions/blob/master/src/easing.cpp

inline float easing_linear(float t) {
    return t;
}

inline float easing_InSine(float t) {
	return sinf( 1.5707963f * t );
}

inline float easing_OutSine(float t) {
	return 1 + sinf( 1.5707963f * (--t) );
}

inline float easing_sine_in_out(float t) {
	return 0.5f * (1 + sinf( 3.1415926f * (t - 0.5f) ) );
}

typedef float (*easing_t)(float);

namespace Engine {
	extern int32_t current_fps;
	static bool _is_running = true;

	void init();

	inline void exit() {
		_is_running = false;
	}
	inline bool is_running() {
		return _is_running;
	}
	bool is_paused();
	void toggle_logging();
	void log(const char* fmt, ...);
	void logn(const char* fmt, ...);

	void set_base_data_folder(const std::string &name);
	std::string get_base_data_folder();

	void pause(float time);

	void update();
}

struct Vector2;
struct Point {
	int x;
	int y;
	Point() { x = 0; y = 0; };
	Point(int xPos, int yPos) {
		x = xPos;
		y = yPos;
	}
	
	template<class T>
	inline Point operator=(const T &v) const {
		return Point((int)v.x, (int)v.y);
	}
	inline Point operator-(Point const& point) const {
		return Point(x - point.x, y - point.y);
	}
	inline Point Point::operator*(int const &num) const {
		return Point(x * num, y * num);
	}
	
	Vector2 to_vector2() const;
};
inline Point operator+( Point const& lhs, Point const& rhs ) {
	return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}
inline bool operator==( Point const& lhs, Point const& rhs ) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
inline Point operator+=(Point const& lhs, int const& rhs){
	return Point(lhs.x + rhs, lhs.y + rhs);
}
inline Point operator-=(Point const& lhs, int const& rhs){
	return Point(lhs.x - rhs, lhs.y - rhs);
}
inline const Point operator*(int lhs, Point const &rhs) {
	Point result;
	result.x=rhs.x * lhs;
	result.y=rhs.y * lhs;
	return result;
}

struct Vector2 {
	float x;
	float y;
	Vector2() { x = 0; y = 0; };
	explicit Vector2(float xPos, float yPos) {
		x = xPos;
		y = yPos;
	}
	
	static const Vector2 Zero;
	static const Vector2 One;
	inline Vector2 Vector2::operator-() const {
		return Vector2(-x, -y);
   	}
	inline Vector2& Vector2::operator+=(const Vector2& vector) {
		x += vector.x;
		y += vector.y;
		return *this;
	}
	inline Vector2& Vector2::operator-=(const Vector2& vector) {
		x -= vector.x;
		y -= vector.y;
		return *this;
	}
	inline Vector2& Vector2::operator*=(const Vector2& vector) {
		x *= vector.x;
		y *= vector.y;
		return *this;
	}
	inline Vector2& Vector2::operator/=(const Vector2& vector) {
		x /= vector.x;
		y /= vector.y;
		return *this;
	}
	inline Vector2 Vector2::operator-(const Vector2& vector) const {
			return Vector2(x - vector.x, y - vector.y);
	}
	inline Vector2 Vector2::operator*(const Vector2& vector) const {
			return Vector2(x * vector.x, y * vector.y);
	}
	inline Vector2 Vector2::operator-(float const &num) const {
		return Vector2(x - num, y - num);
	}
	inline Vector2 Vector2::operator+(float const &num) const {
		return Vector2(x + num, y + num);
	}
	inline Vector2 Vector2::operator*(float const &num) const {
		return Vector2(x * num, y * num);
	}
	inline Vector2 Vector2::operator/(float const &num) const {
		return Vector2(x / num, y / num);
	}
	inline Vector2& Vector2::operator+=(const float &num) {
		x += num;
		y += num;
		return *this;
	}
	inline Vector2& Vector2::operator-=(const float &num) {
		x -= num;
		y -= num;
		return *this;
	}
	inline Vector2& Vector2::operator*=(const float &num) {
		x *= num;
		y *= num;
		return *this;
	}
	inline Vector2& Vector2::operator/=(const float &num) {
		x /= num;
		y /= num;
		return *this;
	}
	inline bool Vector2::operator==(const Vector2& vector) const {
		return x == vector.x && y == vector.y;
	}
	inline bool Vector2::operator!=(const Vector2& vector) const {
		return x != vector.x || y != vector.y;
	}

	Point to_point() const;
	Vector2 normal() const;
	float length() const;
	float length2() const;
	float dot(const Vector2 &v) const;
};

inline Vector2 operator+( Vector2 const& lhs, Vector2 const& rhs ) {
	return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}
inline const Vector2 operator*(float lhs, Vector2 const &rhs) {
	Vector2 result;
	result.x=rhs.x * lhs;
	result.y=rhs.y * lhs;
	return result;
}

struct Rectangle {
	int x;
	int y;
	int w;
	int h;

	Rectangle() {
		x = y = w = h = 0;
	}

	Rectangle(int xx, int yy, int ww, int hh) {
		x = xx;
		y = yy;
		w = ww;
		h = hh;
	}

	bool contains(int xi, int yi) {
		return ((((x <= xi) && (xi < (x + w))) && (y <= yi)) && (yi < (y + h)));
	}

	bool contains(const Point &p) {
		return contains(p.x, p.y);
	}

	int left() {
        return x;
    }

	int right() {
        return x + w;
    }

	int top() {
        return y;
    }

	int bottom() {
        return y + h;
    }

	bool intersects(Rectangle &r2) {
        return !(r2.left() > right()
                 || r2.right() < left()
                 || r2.top() > bottom()
                 || r2.bottom() < top()
                );
    }

	Vector2 center() {
		return Vector2((float)x + w/2, (float)y + h/2);
	}
};

namespace Time {
	extern float delta_time;
	extern float delta_time_fixed;
	extern float delta_time_raw;
}

namespace Input {
    extern int mousex;
	extern int mousey;
	extern bool mouse_left_down;

    void init();
    void update_states();
    void map(const SDL_Event *event);
    bool key_down(const SDL_Scancode &scanCode);
	bool key_down_k(const SDL_Keycode &keyCode);
    bool key_released(const SDL_Keycode &keyCode);
    bool key_pressed(const SDL_Keycode &keyCode);
}

struct Scene {
	virtual void init() = 0;
  	virtual void cleanup() = 0;
	virtual void update(float dt) = 0;
	virtual void render() = 0;
};

namespace Scenes {
	void set_scene(Scene* scene);
	Scene* get_current();
	void free();
	void update(float dt);
	void render();
}

namespace Math {
	static const float Pi = 3.14159265358979323846f; // 180 degrees
	static constexpr float TwoPi = 3.14159265358979323846f * 2; // 360 degrees
	static constexpr float PiOver2 = 3.14159265358979323846f / 2.0f;
	static const float RAD_TO_DEGREE = 180.0f / (float)M_PI;
	static const float DEGREE_TO_RAD = (float)M_PI / 180.0f;
	
	inline int clamp_i(int x, int a, int b) {
    	x = std::max(x, a);
    	x = std::min(x, b);
    	return x;
	}

	inline float clamp_f(float x, float a, float b) {
    	x = std::fmax(x, a);
    	x = std::fmin(x, b);
    	return x;
	}

	inline int max_i(int a, int b) {
		return std::max(a, b);
	}

	inline float max_f(float a, float b) {
		return std::fmax(a, b);
	}

	inline int min_i(int a, int b) {
		return std::min(a, b);
	}

	inline float min_f(float a, float b) {
		return std::fmin(a, b);
	}

	inline float ceiling(float f) {
		return std::ceilf(f);
	}

	inline int abs(int i) {
		return std::abs(i);
	}
	
	inline float abs_f(float f) {
		return std::abs(f);
	}

	inline float cos_f(float f) {
		return std::cosf(f);
	}

	inline float sin_f(float f) {
		return std::sinf(f);
	}
	
	inline int sqrt(int i) {
		return (int)std::sqrt(i);
	}
	
	inline float sqrt_f(float f) {
		return std::sqrt(f);
	}

	inline float round(float f) {
		return std::round(f);
	}

	inline float round_bankers(float f) {
		if(f == 0.5f)
			return 0.0f;
		return std::round(f);
	}

	inline float lerp(float from, float to, float t) {
    	return from + (to - from) * t;
	}

	inline float length_vector_f(float x, float y) {
		return sqrt_f(x*x + y*y);
	}

	inline float interpolate(float from, float to, float amount, easing_t easing) {
    	return from + (to - from) * (easing(amount));
	}

	inline void interpolate_point(Point *target, const Point &from, const Point &to, const float amount, easing_t easing) {
		target->x = (int)(from.x + (to.x - from.x) * (easing(amount)));
		target->y = (int)(from.y + (to.y - from.y) * (easing(amount)));
	}

	inline void interpolate_vector(Vector2 *target, const Vector2 &from, const Vector2 &to, const float amount, easing_t easing) {
		target->x = from.x + (to.x - from.x) * (easing(amount));
		target->y = from.y + (to.y - from.y) * (easing(amount));
	}

	inline float distance_f(const float &x1, const float &y1, const float &x2, const float &y2) {
		auto dx = x1 - x2;
    	auto dy = y1 - y2;
		return sqrt_f(dx * dx + dy * dy);
	}

	inline float distance_v(const Vector2 &a, const Vector2 &b) {
		auto dx = a.x - b.x;
    	auto dy = a.y - b.y;
		return sqrt_f(dx * dx + dy * dy);
	}

	//Returns dot product
	inline const float dot_product(Vector2 const &lhs, Vector2 const &rhs) {
		return lhs.x*rhs.x+lhs.y*rhs.y; 
	}
	//Returns length squared (length2)
	inline const float length_squared(Vector2 const &rhs) {
		return dot_product(rhs, rhs);
	}
	//Returns magnitude (length)
	inline const float magnitude(Vector2 const &rhs) {
		//return sqrtf(dot(rhs, rhs));
		return sqrt_f(length_squared(rhs));
	}
	//Returns normalized Vector2
	inline Vector2 normalize(Vector2 const &lhs){
		return (1.f /(magnitude(lhs))) * lhs;
	}

	inline Vector2 direction(const Vector2 &first, const Vector2 &second) {
        Vector2 direction = first - second;
        if (direction == Vector2::Zero)
            return Vector2::Zero;
        else
            return normalize(direction);
	}

	inline Vector2 direction_from_angle(float angle) {
		float rotation = angle / Math::RAD_TO_DEGREE;
		Vector2 direction;
        direction.x = cos_f(rotation);
        direction.y = sin_f(rotation);
		return direction;
	}

	inline float angle_from_direction(const Vector2 &vector) {
        return (float)atan2(vector.y, vector.x) * RAD_TO_DEGREE;
	}

	inline float degrees_between_v(const Vector2 &a, const Vector2 &b) {
    	return atan2(b.y - a.y, b.x - a.x) * RAD_TO_DEGREE;
	}

	inline float rads_between_f(const float &x1, const float &y1, const float &x2, const float &y2) {
    	return atan2(y2 - y1, x2 - x1);
	}

	inline float rads_between_v(const Vector2 &a, const Vector2 &b) {
    	return atan2(b.y - a.y, b.x - a.x);
	}

	inline float rads_to_degrees(float rads) {
		return rads * RAD_TO_DEGREE;
	}

	inline Vector2 scale_to(const Vector2 &vector, const float &length) {
        return vector * (length / vector.length());
    }

	inline float IEEERemainder(float dividend, float divisor) {
		return dividend - (divisor * Math::round(dividend / divisor));
	}

	inline float wrap_angle(float angle) {
        angle = IEEERemainder(angle, 6.2831854820251465);
	    if (angle <= -3.14159274f) {
			angle += 6.28318548f;
	    }
	    else if (angle > 3.14159274f) {
			angle -= 6.28318548f;
	    }
	    return angle;
	}

	// You can define a vector as a radius and an angle - the polar coordinates.
	inline Vector2 polar_coords_to_vector(float angle, float magnitude) {
        return magnitude * Vector2(Math::cos_f(angle), Math::sin_f(angle));
    }

	inline bool intersect_circles(float c1X, float c1Y, float c1Radius, float c2X, float c2Y, float c2Radius) {
		float distanceX = c2X - c1X;
		float distanceY = c2Y - c1Y;     
		float magnitudeSquared = distanceX * distanceX + distanceY * distanceY;
		return magnitudeSquared < (c1Radius + c2Radius) * (c1Radius + c2Radius);
	}

	struct AABB {
		int left;
		int right;
		int bottom;
		int top;
	};

	struct AABB_f {
		float left;
		float right;
		float bottom;
		float top;
	};

	inline bool intersect_AABB(const Rectangle &a, const Rectangle &b) {
		AABB aa = { a.x, a.x + a.w, a.y, a.y + a.h };
		AABB bb = { b.x, b.x + b.w, b.y, b.y + b.h };
  		return (aa.left <= bb.right && aa.right >= bb.left) 
			&& (aa.bottom <= bb.top && aa.top >= bb.bottom);
	}

	inline bool intersect_circle_AABB(const float &cx, const float &cy, const float &radius, const Rectangle &rect) {
		AABB_f aa = { (float)rect.x, (float)rect.x + rect.w, (float)rect.y, rect.y + (float)rect.h };
		float delta_x = cx - Math::max_f(aa.left, Math::min_f(cx, aa.right));
		float delta_y = cy - Math::max_f(aa.bottom, Math::min_f(cy, aa.top));
		return (delta_x * delta_x + delta_y * delta_y) < (radius * radius);
	}

	inline bool intersect_lines_vector(const Vector2 &lineA1, const Vector2 &lineA2, const Vector2 &lineB1, const Vector2 &lineB2, Vector2 &collision_point) {
		float denom = ((lineB2.y - lineB1.y) * (lineA2.x - lineA1.x)) -
			((lineB2.x - lineB1.x) * (lineA2.y - lineA1.y));
		
		if (denom == 0) {
			return false;
		}
		
		float ua = (((lineB2.x - lineB1.x) * (lineA1.y - lineB1.y)) -
			((lineB2.y - lineB1.y) * (lineA1.x - lineB1.x))) / denom;
		/* The following 3 lines are only necessary if we are checking line
			segments instead of infinite-length lines */
		float ub = (((lineA2.x - lineA1.x) * (lineA1.y - lineB1.y)) -
			((lineA2.y - lineA1.y) * (lineA1.x - lineB1.x))) / denom;
		if ((ua < 0) || (ua > 1) || (ub < 0) || (ub > 1)) {
			return false;
		}

		collision_point = lineA1 + ua * (lineA2 - lineA1);
		return true;
	}

	// call repeatedly to move to target 
	// returns true when at target
	// speed_per_tick => e.g. 4 pixels per call
	inline bool move_to(float &x, float &y, float targetX, float targetY, float speed) {
		float delta_x = targetX - x;
		float delta_y = targetY - y;
		float goal_dist = Math::sqrt_f( (delta_x * delta_x) + (delta_y * delta_y) );
		if (goal_dist > speed) {
			float ratio = speed / goal_dist;
			float x_move = ratio * delta_x;  
			float y_move = ratio * delta_y;
			x = x_move + x;
			y = y_move + y;
			return false;
		}
		
		// destination reached
		return true;
	}
}

namespace RNG {
	static std::random_device RNG_seed;
	static std::mt19937 RNG_generator(RNG_seed());

    inline int next_i(int max) {
        std::uniform_int_distribution<int> range(0, max);
        return range(RNG_generator);
    }

    inline int range_i(int min, int max) {
        std::uniform_int_distribution<int> range(min, max);
        return range(RNG_generator);
    }

    inline float range_f(float min, float max) {
        std::uniform_real_distribution<float> range(min, max);
        return range(RNG_generator);
    }

	inline void random_point_i(int xMax, int yMax, int &xOut, int &yOut) {
		std::uniform_int_distribution<int> xgen(0, xMax);
		std::uniform_int_distribution<int> ygen(0, yMax);
		xOut = xgen(RNG_generator);
		yOut = ygen(RNG_generator);
	}

	inline Vector2 vector2(const float &x_min, const float &x_max, const float &y_min, const float &y_max) {
		std::uniform_real_distribution<float> xgen(x_min, x_max);
		std::uniform_real_distribution<float> ygen(y_min, y_max);
		return Vector2(xgen(RNG_generator), ygen(RNG_generator));
	}
}

#endif
