#ifndef GAME_H
#define GAME_H

#include "engine.h"
#include "renderer.h"
#include "asteroids.h"
#include <chrono>

inline void game_load() {
	// Allocate memory and load resources
	Engine::set_base_data_folder("data");
	Font *font = Resources::font_load("normal", "pixeltype.ttf", 15);
	set_default_font(font);
    FrameLog::enable_at(5, 5);

	asteroids_load();
}

inline void game_update() {
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	
	asteroids_update();
	
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto diff = t2 - t1;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( diff ).count();
	auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>( diff ).count();
	std::string frame_duration_mu = "update time mu: " + std::to_string(duration);
	std::string frame_duration_ms = "update time ms: " + std::to_string(duration_ms);
	FrameLog::log(frame_duration_mu);
	FrameLog::log(frame_duration_ms);
    // std::cout << "\n" << duration << " MICRO seconds";
}

inline void game_render() {
	renderer_clear();
	asteroids_render();
	renderer_draw_render_target_camera();
	renderer_flip();
}

inline void game_unload() {
}

#endif