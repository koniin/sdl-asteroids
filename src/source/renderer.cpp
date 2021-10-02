#include "renderer.h"
#include "SDL_image.h"
#include <fstream>

unsigned gw;
unsigned gh;
static unsigned step_scale;
static int window_w;
static int window_h;
int windowPos;

Font *default_font;
gfx renderer;

namespace Resources {
	std::unordered_map<std::string, Sprite*> sprites;
	std::unordered_map<std::string, Font*> fonts;
	std::vector<SpriteSheet> sprite_sheets;
	std::unordered_map<std::string, int> sprite_sheet_map;
	
	static SDL_Texture* load_texture(const std::string &path, int &w, int &h) { 
		//The final texture 
		SDL_Texture* newTexture = NULL; 
		//Load image at specified path 
		SDL_Surface* loadedSurface = IMG_Load( path.c_str() ); 
		if( loadedSurface == NULL ) { 
			printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() ); 
		} else { 
			//Create texture from surface pixels 
			newTexture = SDL_CreateTextureFromSurface(renderer.renderer, loadedSurface ); 
			if( newTexture == NULL ) { 
				printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() ); 
			} 

			w = loadedSurface->w;
			h = loadedSurface->h;

			//Get rid of old loaded surface 
			SDL_FreeSurface( loadedSurface ); 
		} 
		return newTexture; 
	}

    Sprite *sprite_load(const std::string &name, const std::string &filename) {
		std::string path = Engine::get_base_data_folder() + filename;
		Sprite *s = new Sprite;
    	s->image = load_texture(path, s->w, s->h);
		sprites[name] = s;
		return s;
	}
	
    Sprite *sprite_get(const std::string &name) {
		return sprites.at(name);
	}
	
    Font *font_load(const std::string name, const std::string filename, int pointSize) {
		std::string path = Engine::get_base_data_folder() + filename;
		Font *f = new Font;
		TTF_Font *font = TTF_OpenFont(path.c_str(), pointSize);
		f->font = font;
		f->name = name;
		fonts[name] = f;
		return f;
	}

    Font *font_get(const std::string &name) {
		return fonts.at(name);
	}
	
	void font_set_style(const std::string &name, FontStyle style) {
		TTF_SetFontStyle(fonts.at(name)->font, style);
	}

	void font_set_outline(const std::string &name, int outline) {
		TTF_SetFontOutline(fonts.at(name)->font, outline);
	}

    void font_remove(const std::string& name) {
		auto itr = fonts.find(name);
		if (itr != fonts.end()) {
			TTF_CloseFont(itr->second->font);
    		delete itr->second;
    		fonts.erase(itr);
		}
	}

    void cleanup() {
		for(std::unordered_map<std::string, Sprite*>::iterator itr = sprites.begin(); itr != sprites.end(); itr++) {
			SDL_DestroyTexture(itr->second->image);
        	delete itr->second;
    	}
		sprites.clear();
		for(std::unordered_map<std::string, Font*>::iterator itr = fonts.begin(); itr != fonts.end(); itr++) {
			TTF_CloseFont(itr->second->font);
			delete itr->second;
    	}
		fonts.clear();

		TextCache::clear();
	}
}

namespace TextCache {
	std::unordered_map<size_t, Sprite> text_cache;
	std::hash<std::string> hasher;

	static size_t cache_key(Font *font, const SDL_Color &color, const char *text) {
		int colors = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24);
		return hasher(font->name + text + std::to_string(colors));
	}

	Sprite &load(Font *font, const SDL_Color &color, const char *text) {
		size_t key = cache_key(font, color, text);

		auto item = text_cache.find(key);
		if(item == text_cache.end()) {
			Sprite ci;
			SDL_Surface *surface = TTF_RenderText_Solid(font->font, text, color);
			ci.image = SDL_CreateTextureFromSurface(renderer.renderer, surface);
			SDL_FreeSurface(surface);
			SDL_QueryTexture(ci.image, NULL, NULL, &ci.w, &ci.h);
			text_cache[key] = ci;
		} 

		return text_cache[key];
	}

	void clear() {
		for(auto &cache_item : text_cache) {
			SDL_DestroyTexture(cache_item.second.image);
		}
		text_cache.clear();
	}
}

void window_set_position(int x, int y) {
	SDL_SetWindowPosition(renderer.sdl_window, x, y);
}

void window_center() {
	window_set_position(windowPos, windowPos);
}

void window_set_title(const char* title) {
	SDL_SetWindowTitle(renderer.sdl_window, title);
}

void window_set_scale(const unsigned s) {
	if (s == step_scale)
		return;

	step_scale = s;
	window_w = (int)(gw * s);
	window_h = (int)(gh * s);
	
	SDL_SetWindowSize(renderer.sdl_window, window_w, window_h);
}

static bool is_fullscreen = false;
void window_toggle_fullscreen(bool useDesktopResolution) {
	//GPU_SetFullscreen(!GPU_GetFullscreen(), useDesktopResolution);
	if(is_fullscreen) {
		SDL_SetWindowFullscreen(renderer.sdl_window, 0);
		is_fullscreen = false;
		int ww, wh;
		SDL_GetWindowSize(renderer.sdl_window, &ww, &wh);
		window_w = ww;
		window_h = wh;
	} else {
		SDL_SetWindowFullscreen(renderer.sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		is_fullscreen = true;
		int ww, wh;
		SDL_GetWindowSize(renderer.sdl_window, &ww, &wh);
		window_w = ww;
		window_h = wh;
	}
}

void set_default_font(Font *font) {
	default_font = font;
}

void draw_sprite_centered_rotated(const Sprite *sprite, const int &x, const int &y, const float &angle) {
	int w = sprite->w;
	int h = sprite->h;
	SDL_Rect destination_rect;
	destination_rect.x = x - (w / 2);
 	destination_rect.y = y - (h / 2);
  	destination_rect.w = w;
  	destination_rect.h = h;

	SDL_RenderCopyEx(renderer.renderer, sprite->image, NULL, &destination_rect, angle, NULL, SDL_FLIP_NONE);
}




void draw_sprite(const Sprite *sprite, int x, int y) {
	SDL_Rect destination_rect;
	destination_rect.x = x;
 	destination_rect.y = y;
  	destination_rect.w = sprite->w;
  	destination_rect.h = sprite->h;

	SDL_RenderCopy(renderer.renderer, sprite->image, NULL, &destination_rect);
}



void draw_sprite_centered(const Sprite *sprite, int x, int y) {
	int w = sprite->w;
	int h = sprite->h;
	SDL_Rect destination_rect;
	destination_rect.x = x - (w / 2);
 	destination_rect.y = y - (h / 2);
  	destination_rect.w = w;
  	destination_rect.h = h;

	SDL_RenderCopy(renderer.renderer, sprite->image, NULL, &destination_rect);
}

void draw_sprite_region(const Sprite *sprite, const SDL_Rect *src_rect, int x, int y) {
	SDL_Rect destination_rect;
	destination_rect.x = x;
 	destination_rect.y = y;
  	destination_rect.w = src_rect->w;
  	destination_rect.h = src_rect->h;

	SDL_RenderCopy(renderer.renderer, sprite->image, src_rect, &destination_rect);
}

void draw_sprite_region_centered(const Sprite *sprite, const SDL_Rect *src_rect, int x, int y) {
	int w = src_rect->w;
	int h = src_rect->h;
	SDL_Rect destination_rect;
	destination_rect.x = x - (w / 2);
 	destination_rect.y = y - (h / 2);
  	destination_rect.w = w;
  	destination_rect.h = h;

	SDL_RenderCopy(renderer.renderer, sprite->image, src_rect, &destination_rect);
}

void draw_sprite_region_centered_rotated(const Sprite *sprite, const SDL_Rect *src_rect, int x, int y, float angle) {
	int w = src_rect->w;
	int h = src_rect->h;
	SDL_Rect destination_rect;
	destination_rect.x = x - (w / 2);
 	destination_rect.y = y - (h / 2);
  	destination_rect.w = w;
  	destination_rect.h = h;

	SDL_RenderCopyEx(renderer.renderer, sprite->image, src_rect, &destination_rect, angle, NULL, SDL_FLIP_NONE);
}

void draw_sprite_region_centered_ex(const Sprite *sprite, const SDL_Rect *src_rect, int x, int y, int w, int h, float angle) {
	SDL_Rect destination_rect;
	destination_rect.x = x - (w / 2);
 	destination_rect.y = y - (h / 2);
  	destination_rect.w = w;
  	destination_rect.h = h;

	SDL_RenderCopyEx(renderer.renderer, sprite->image, src_rect, &destination_rect, angle, NULL, SDL_FLIP_NONE);
}

void draw_spritesheet_name(const SpriteSheet &s, const std::string &sprite, const int &x, const int &y) {
	draw_sprite_region(Resources::sprite_get(s.sprite_sheet_name), &s.sheet_sprites[s.sprites_by_name.at(sprite)].region, x, y);
}

void draw_spritesheet_name_centered(const SpriteSheet &s, const std::string &sprite, const int &x, const int &y) {
	draw_sprite_region_centered(Resources::sprite_get(s.sprite_sheet_name), &s.sheet_sprites[s.sprites_by_name.at(sprite)].region, x, y);
}

void draw_spritesheet_name_centered_rotated(const SpriteSheet &s, const std::string &sprite, const int &x, const int &y, const float &angle) {
	draw_sprite_region_centered_rotated(Resources::sprite_get(s.sprite_sheet_name), &s.sheet_sprites[s.sprites_by_name.at(sprite)].region, x, y, angle);
}

void draw_spritesheet_name_centered_ex(const SpriteSheet &s, const std::string &sprite, const int &x, const int &y, const int &w, const int &h, const float &angle) {
	draw_sprite_region_centered_ex(Resources::sprite_get(s.sprite_sheet_name), &s.sheet_sprites[s.sprites_by_name.at(sprite)].region, x, y, w, h, angle);
}

void draw_text_font(Font *font, int x, int y, const SDL_Color &color, const char *text) {
	Sprite &cacheItem = TextCache::load(font, color, text);
	draw_sprite(&cacheItem, x, y);
}

void draw_text(int x, int y, const SDL_Color &color, const std::string text) {
	draw_text_font(default_font, x, y, color, text.c_str());
}

void draw_text_font_centered(Font *font, int x, int y, const SDL_Color &color, const std::string text) {
	Sprite &cacheItem = TextCache::load(font, color, text.c_str());
	draw_sprite_centered(&cacheItem, x, y);
}

void draw_text_centered(int x, int y, const SDL_Color &color, std::string text) {
	draw_text_font_centered(default_font, x, y, color, text.c_str());
}

void draw_g_rectangle_RGBA(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_SetRenderDrawBlendMode(renderer.renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer.renderer, r, g, b, a); 
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderDrawRect(renderer.renderer, &rect);
}

void draw_g_rectangle_filled_RGBA(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	SDL_SetRenderDrawBlendMode(renderer.renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer.renderer, r, g, b, a); 
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderFillRect(renderer.renderer, &rect);
}

int renderer_init(const char *title, unsigned vw, unsigned vh, unsigned scale) {
	gw = vw;
	gh = vh;
	step_scale = scale;
	window_w = gw * step_scale;
	window_h = gh * step_scale;
	windowPos = SDL_WINDOWPOS_CENTERED;

	// SDL_INIT_AUDIO
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize: %s\n", SDL_GetError());
		return -1;
	} 
	
	renderer.sdl_window = SDL_CreateWindow(title, 
		windowPos, 
		windowPos, 
		window_w, 
		window_h, 
		SDL_WINDOW_SHOWN );

	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
	// Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE;
	renderer.renderer = SDL_CreateRenderer(renderer.sdl_window, -1, flags);

    renderer.clearColor = { 0, 0, 0, 255 };
	SDL_SetRenderDrawColor(renderer.renderer, 0x00, 0x00, 0x00, 0xFF ); 
	
	// Font init
	TTF_Init();

	//Initialize PNG loading 
	int imgFlags = IMG_INIT_PNG; 
	if( !( IMG_Init( imgFlags ) & imgFlags ) ) { 
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() ); 
		abort();
	}

    SDL_ShowCursor(SDL_DISABLE);
    
	int displays = SDL_GetNumVideoDisplays();
	printf("detected [ %d ] displays\n", displays);
	if(displays > 1 && PREFERRED_DISPLAY) {
		windowPos = SDL_WINDOWPOS_CENTERED_DISPLAY(1);
	}
	window_set_position(windowPos, windowPos);
    
	SDL_Texture *target = SDL_CreateTexture(renderer.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gw, gh);
	renderer.renderTarget = target;
	
    return 1;
}

void renderer_clear() {
	SDL_SetRenderDrawColor(renderer.renderer, renderer.clearColor.r, renderer.clearColor.g, renderer.clearColor.b, renderer.clearColor.a);	
	SDL_SetRenderTarget(renderer.renderer, renderer.renderTarget);
	SDL_RenderClear(renderer.renderer);
}

void renderer_draw_render_target() {
	SDL_SetRenderTarget(renderer.renderer, NULL);
	
	// USE TO CREATE BLACK BARS (That can be filled with other things if we want)
	SDL_Rect destination_rect;
	destination_rect.x = (window_w / 2) - (gw * step_scale / 2);
 	destination_rect.y = (window_h / 2) - (gh * step_scale / 2);
  	destination_rect.w = gw * step_scale;
  	destination_rect.h = gh * step_scale;
	SDL_RenderCopy(renderer.renderer, renderer.renderTarget, NULL, &destination_rect);
	
	// USE TO STRETCH TO FILL SCREEN
	// SDL_RenderCopy(renderer.renderer, renderer.renderTarget, NULL, NULL);
}

void renderer_flip() {
	SDL_RenderPresent(renderer.renderer);
}

void renderer_destroy() {
	Resources::cleanup();
	IMG_Quit();
	TTF_Quit();
	SDL_DestroyRenderer(renderer.renderer);
	SDL_DestroyWindow(renderer.sdl_window);
}