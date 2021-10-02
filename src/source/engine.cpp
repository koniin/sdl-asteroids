#include "engine.h"
#include <unordered_set>
#include <fstream>

namespace Engine {
	int32_t current_fps = 0;

	void init() {
		Input::init();
	}

	static bool logging_enabled = true;
	void toggle_logging() {
		logging_enabled = !logging_enabled;
	}
	
	void logn(const char* fmt, ...) {
		if(!logging_enabled) {
			return;
		}
		
		va_list args;
		va_start(args, fmt);
		printf("\n");
		vprintf(fmt, args);
		va_end(args);
	}

	void log(const char* fmt, ...) {
		if(!logging_enabled) {
			return;
		}

		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
	
    static std::string base_data_folder = "";
	void set_base_data_folder(const std::string &name) {
		base_data_folder = name + "/";
	}

	std::string get_base_data_folder() {
		return base_data_folder;
	}

	static float pause_timer = 0.0f;
	bool is_paused() {
		return pause_timer > 0.0f;
	}

	void pause(float time) {
		pause_timer = time;
	}

	void update() {
		if(pause_timer > 0.0f) {
			pause_timer -= Time::delta_time_fixed;
		}
	}
}

namespace Time {
	float delta_time = 0.0f;
	float delta_time_fixed = 0.0f;
	float delta_time_raw = 0.0f;
}

namespace Input {
	int mousex = 0;
	int mousey = 0;
	bool mouse_left_down = false;
    SDL_GameController *controller;
    const Uint8* current_keyboard_state;

    std::unordered_set<int> keysDown;
	std::unordered_set<int> keysDownNow;
	std::unordered_set<int> keysUpNow;

    void init() {
        controller = NULL;
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                controller = SDL_GameControllerOpen(i);
                if (controller) break;
            }
        }
    }
 
    void update_states() {
        current_keyboard_state = SDL_GetKeyboardState(NULL);
        keysDownNow.clear();
		keysUpNow.clear();
		SDL_GetMouseState(&mousex, &mousey);
		mouse_left_down = false;
    }

    void map(const SDL_Event *event) {
        if (event->type == SDL_KEYDOWN) {
		    keysDown.insert(event->key.keysym.sym);
			keysDownNow.insert(event->key.keysym.sym);
		}
        if (event->type == SDL_KEYUP) {
			keysDown.erase(event->key.keysym.sym);
		    keysUpNow.insert(event->key.keysym.sym);
		}
		if(event->type == SDL_MOUSEBUTTONDOWN) {
			if(event->button.button == SDL_BUTTON_LEFT ) {
				mouse_left_down = true;
			}
		}
    }

    bool key_down(const SDL_Scancode &scanCode) {
        return current_keyboard_state[scanCode];
    }

	bool key_down_k(const SDL_Keycode &keyCode) {
        return keysDown.count(keyCode) > 0;
    }

    bool key_released(const SDL_Keycode &keyCode) {
        return keysUpNow.count(keyCode) > 0;
    }

    bool key_pressed(const SDL_Keycode &keyCode) {
        return keysDownNow.count(keyCode) > 0;
    }
}

namespace Scenes {
	static Scene* currentScene;

	void set_scene(Scene* scene) {
		if(currentScene != nullptr)
			currentScene->cleanup();
		delete currentScene;
		
		scene->init();
		currentScene = scene;
	}

	Scene* get_current() {
		ASSERT_WITH_MSG(currentScene != nullptr, "current scene is null");
		return currentScene;
	}

	void free() {
		if(currentScene != nullptr) {
			currentScene->cleanup();
			delete currentScene;
		}
	}

	void update(float dt) {
		ASSERT_WITH_MSG(currentScene != nullptr, "current scene is null");
		currentScene->update(dt);
	}

	void render() {
		ASSERT_WITH_MSG(currentScene != nullptr, "current scene is null");
		currentScene->render();
	}
}
