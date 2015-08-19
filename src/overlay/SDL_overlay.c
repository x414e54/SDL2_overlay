#include "../SDL_internal.h"

#ifdef SDL_OVERLAY

#include "SDL_stdinc.h"
#include "SDL_events.h"
#include "SDL_opengl.h"
#include "SDL_render.h"

struct OVL_Overlay
{
	SDL_EventFilter currentEventFilter;
	void* currentEventFilterData;
	SDL_bool open;
	SDL_Renderer* overlay;
	SDL_Window* overlay_window;
	SDL_Texture* overlay_texture;

    // GameController Config
    int current_joystick_id;
};

static struct OVL_Overlay *_this = NULL;

#define CHECK_INIT(retval) \
    if (!_this) { \
        SDL_SetError("Overlay not initialized"); \
        return retval; \
    }

SDL_bool
OVL_Init(const char* theme_dir)
{
    _this = SDL_calloc(sizeof(*_this), 0);

    SDL_CreateWindowAndRenderer(1,
                                1,
                                SDL_WINDOW_HIDDEN,
                                &_this->overlay_window,
                                &_this->overlay);

    _this->overlay_texture = SDL_CreateTexture(_this->overlay,
                        SDL_PIXELFORMAT_BGRA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        800, 600);

    SDL_SetRenderTarget(_this->overlay,
                        _this->overlay_texture);

    return SDL_TRUE;
}

static int _EventFilter(void* userdata, SDL_Event* event)
{
	
}

static void _CloseOverlay()
{
    if (_this->currentEventFilter != NULL) {
	    SDL_SetEventFilter(_this->currentEventFilter,
                           _this->currentEventFilterData);
    }    
	_this->open = SDL_FALSE;
}

void
OVL_Quit()
{
    CHECK_INIT()

    if (_this->open) {
        _CloseOverlay();
    }

    SDL_DestroyTexture(_this->overlay_texture);
    SDL_DestroyRenderer(_this->overlay);
    SDL_DestroyWindow(_this->overlay_window);
    SDL_free(_this);
}

SDL_bool
OVL_IsOverlayOpen()
{
    CHECK_INIT(SDL_FALSE)
	return _this->open;
}

SDL_bool
OVL_OpenControllerOverlay(int id)
{
    CHECK_INIT(SDL_FALSE)

    if (_this->open) {
        return SDL_FALSE;
    }

    if (!SDL_GetEventFilter(&_this->currentEventFilter,
                            &_this->currentEventFilterData)) {
	    _this->currentEventFilter = NULL; 
	    _this->currentEventFilterData = NULL;   
    }

	SDL_SetEventFilter(_EventFilter, NULL);
	_this->open = SDL_TRUE;
}

void
OVL_UpdateFrame()
{
    CHECK_INIT()

    // Test frame update
    SDL_SetRenderDrawColor(_this->overlay,
                           255,
                           0,
                           0,
                           255);
    SDL_Rect rect;
    SDL_zero(rect);
    rect.w = 100;
    rect.h = 100;
    SDL_RenderFillRect(_this->overlay, &rect);
}

SDL_bool
OVL_GL_SwapWindow(SDL_Window* window)
{
    CHECK_INIT(SDL_FALSE)

	if (_this->open) {
        SDL_Renderer* renderer = SDL_CreateRenderer(window, 
                                    -1, SDL_RENDERER_ACCELERATED);
        SDL_RenderCopy(renderer, _this->overlay_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyRenderer(renderer);
	} else {
		SDL_GL_SwapWindow(window);
        return SDL_TRUE;
	}
}

SDL_bool
OVL_RendererPresent(SDL_Renderer* renderer)
{
    CHECK_INIT(SDL_FALSE)

	if (_this->open) {
        SDL_RenderCopy(renderer, _this->overlay_texture, NULL, NULL);
	}

	SDL_RenderPresent(renderer);
    return SDL_TRUE;
}
#endif /* SDL_OVERLAY */

/* vi: set ts=4 sw=4 expandtab: */
