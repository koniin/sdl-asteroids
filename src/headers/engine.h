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
	static const float RAD_TO_DEGREE = 180.0f / (float)M_PI;
	inline float max_f(float a, float b) {
		return std::fmax(a, b);
	}
	inline float sqrt_f(float f) {
		return std::sqrt(f);
	}
	inline float length_vector_f(float x, float y) {
		return sqrt_f(x*x + y*y);
	}
	inline bool intersect_circles(float c1X, float c1Y, float c1Radius, float c2X, float c2Y, float c2Radius) {
		float distanceX = c2X - c1X;
		float distanceY = c2Y - c1Y;     
		float magnitudeSquared = distanceX * distanceX + distanceY * distanceY;
		return magnitudeSquared < (c1Radius + c2Radius) * (c1Radius + c2Radius);
	}
}

namespace RNG {
	static std::random_device RNG_seed;
	static std::mt19937 RNG_generator(RNG_seed());

    inline float range_f(float min, float max) {
        std::uniform_real_distribution<float> range(min, max);
        return range(RNG_generator);
    }
}

#endif
