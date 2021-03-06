#ifndef _RENDERER_H
#define _RENDERER_H

#include "engine.h"
#include "SDL_ttf.h"
#include <unordered_map>

// prefered display to show the window
#ifndef PREFERRED_DISPLAY
    #define PREFERRED_DISPLAY 0
    // #define PREFERRED_DISPLAY 1
#endif

extern unsigned gw;
extern unsigned gh;

typedef struct {
	SDL_Window *sdl_window;
    SDL_Renderer *renderer;
    SDL_Texture *renderTarget;
    SDL_Color default_color;
    SDL_Color clearColor;
} gfx;

extern gfx renderer;

struct Camera {
	float shake_duration = 0.0f;
	float trauma = 0.0f;
	float x = 0;
	float y = 0;
	float offset_x = 0;
	float offset_y = 0;
    float follow_x = 0;
    float follow_y = 0;
    float speed = 0.2f;

    // initialize with really large values
    // used to clamp the camera at edges of level
    float x_min = -900000.0f;
    float x_max = 900000.0f;
    float y_min = -900000.0f;
    float y_max = 900000.0f;
};

struct Sprite {
    SDL_Texture *image;
    int w;
    int h;
    bool isValid() {
        return image != NULL;
    }
};

struct SpriteFrame {
	int id;
	std::string name;
	SDL_Rect region;
};

struct SpriteSheet {
	std::string sprite_sheet_name;
	std::vector<SpriteFrame> sheet_sprites;
	std::unordered_map<int, int> sprites_by_id;
	std::unordered_map<std::string, int> sprites_by_name;
};

struct Font {
    TTF_Font *font;
    std::string name;
	inline void set_color(const SDL_Color &color) {
		//font->setDefaultColor(color);
	}
};

namespace Resources {
    Sprite *sprite_load(const std::string &name, const std::string &filename);
    Sprite *sprite_get(const std::string &name);

    Font *font_load(const std::string name, const std::string filename, int pointSize);
    Font *font_get(const std::string &name);
    void font_remove(const std::string &name);
    
	enum FontStyle {
		NORMAL = 0x00,
		BOLD = 0x01,
		ITALIC = 0x02,
		UNDERLINE = 0x04,
		STRIKETHROUGH = 0x08
	};
    // Styles must be set before drawing text with that font to cache correctly
    void font_set_style(const std::string &name, FontStyle style);
    // Outlines must be set before drawing text with that font to cache correctly
    void font_set_outline(const std::string &name, int outline);

    void cleanup();
}

namespace TextCache {
    void clear();
}

namespace Colors {
	const SDL_Color white = { 255, 255, 255, 255 };
	const SDL_Color black = { 0, 0, 0, 255 };

    inline SDL_Color make(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        return { r, g, b, a };
    }
}

void window_set_position(int x, int y);
void window_center();
void window_set_title(const char* title);
void window_set_scale(unsigned s);
void window_toggle_fullscreen(bool useDesktopResolution);
void set_default_font(Font *font);


void draw_g_rectangle_RGBA(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void draw_g_rectangle_filled_RGBA(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

void draw_text_font_centered(Font *font, int x, int y, const SDL_Color &color, std::string text);
void draw_text_centered(int x, int y, const SDL_Color &color, std::string text);
void draw_text(int x, int y, const SDL_Color &color, const std::string text);

void draw_sprite_centered_rotated(const Sprite *sprite, const int &x, const int &y, const float &angle);

// // --------

int renderer_init(const char *title, unsigned vw, unsigned vh, unsigned scale);
void renderer_clear();
void renderer_draw_render_target();
void renderer_flip();
void renderer_destroy();

#endif