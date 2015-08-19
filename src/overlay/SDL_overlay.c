#include "../SDL_internal.h"

#ifdef SDL_OVERLAY

#include "SDL_stdinc.h"
#include "SDL_events.h"
#include "SDL_opengl.h"
#include "SDL_render.h"

struct _Mapping
{
    const SDL_GameControllerBindType type;
    const union
    {
        SDL_GameControllerAxis axis;
        SDL_GameControllerButton button;
    } value;

    SDL_GameControllerButtonBind bind;
};
typedef struct _Mapping Mapping;

#define EMPTY_BIND {SDL_CONTROLLER_BIND_TYPE_NONE, 0};
#define NUM_MAPPINGS 21
#define NEXT_BUTTON 0
#define PREV_BUTTON 1

struct OVL_Overlay
{
	SDL_EventFilter currentEventFilter;
	void* currentEventFilterData;
	SDL_bool open;
	SDL_Renderer* overlay;
	SDL_Window* overlay_window;
	SDL_Texture* overlay_texture;

    // GameController Config
    SDL_GameController* controller;
    SDL_Joystick* joystick;
    SDL_JoystickID current_joystick_id;
    int current_mapping_index;

    SDL_bool is_mapping;

    Mapping mappings[NUM_MAPPINGS];
};

static struct OVL_Overlay *_this = NULL;

#define CHECK_INIT(retval) \
    if (!_this) { \
        SDL_SetError("Overlay not initialized"); \
        return retval; \
    }

void _ResetMappings()
{
    _this->current_mapping_index = 0;
/*    _this->mappings = {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_START, EMPTY_BIND}
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_BACK, EMPTY_BIND}
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_DPAD_UP, EMPTY_BIND}
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_DPAD_DOWN, EMPTY_BIND} 	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_DPAD_LEFT, EMPTY_BIND}	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, EMPTY_BIND} 
        {SDL_CONTROLLER_BINDTYPE_AXIS, SDL_CONTROLLER_AXIS_LEFTX, EMPTY_BIND}
        {SDL_CONTROLLER_BINDTYPE_AXIS, SDL_CONTROLLER_AXIS_LEFTY, EMPTY_BIND}
        {SDL_CONTROLLER_BINDTYPE_AXIS, SDL_CONTROLLER_AXIS_RIGHTX, EMPTY_BIND} 	
        {SDL_CONTROLLER_BINDTYPE_AXIS, SDL_CONTROLLER_AXIS_RIGHTY, EMPTY_BIND}	
        {SDL_CONTROLLER_BINDTYPE_AXIS, SDL_CONTROLLER_AXIS_TRIGGERLEFT, EMPTY_BIND} 	
        {SDL_CONTROLLER_BINDTYPE_AXIS, SDL_CONTROLLER_AXIS_TRIGGERRIGHT, EMPTY_BIND} 
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_A, EMPTY_BIND}
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_B, EMPTY_BIND}	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_X, EMPTY_BIND}	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_Y, EMPTY_BIND}	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_GUIDE, EMPTY_BIND}	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_LEFTSTICK, EMPTY_BIND} 	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_RIGHTSTICK, EMPTY_BIND} 	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_LEFTSHOULDER, EMPTY_BIND} 	
        {SDL_CONTROLLER_BINDTYPE_BUTTON, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, EMPTY_BIND}*/
}

SDL_bool
OVL_Init(const char* theme_dir, const char* language)
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

static void _StopMapping()
{
    _this->is_mapping = SDL_FALSE;
    _this->current_joystick_id = 0;
    if (_this->controller != NULL) {
        SDL_GameControllerClose(_this->controller);
        _this->controller = NULL;
    } else if (_this->joystick != NULL) {
        SDL_JoystickClose(_this->joystick);
        _this->joystick = NULL;
    }
    _ResetMappings();
}

static int _SaveMappings()
{
    char bind[2056];
    const char* name;

    if (_this->controller != NULL) {    
        name = SDL_GameControllerName(_this->controller);
    } else {
        name = SDL_JoystickName(_this->joystick);
    }

    SDL_JoystickGUID guid = SDL_JoystickGetGUID(_this->joystick);
    
    char guid_string[33];
    SDL_JoystickGetGUIDString(guid, guid_string, 33);
    guid_string[32] = 0;

    snprintf(bind, 2056, "%s,%s,platform:%s,", guid_string, name, SDL_GetPlatform());

    int index = strlen(bind);
    for (int i = 0; i < NUM_MAPPINGS; ++i) {
        const char* to = NULL;
        if (_this->mappings[i].type == SDL_CONTROLLER_BINDTYPE_AXIS) {
            to = SDL_GameControllerGetStringForAxis(_this->mappings[i].value.axis);
        } else if (_this->mappings[i].type != SDL_CONTROLLER_BINDTYPE_BUTTON){
            to = SDL_GameControllerGetStringForButton(_this->mappings[i].value.button);
        } 

        if (to == NULL) {
            continue;
        }

        char from[10];
        if (_this->mappings[i].bind.bindType == SDL_CONTROLLER_BINDTYPE_AXIS) {
            snprintf(from, 10, "a%d", _this->mappings[i].bind.value.axis);
        } else if (_this->mappings[i].bind.bindType
                   == SDL_CONTROLLER_BINDTYPE_BUTTON) {
            snprintf(from, 10, "b%d", _this->mappings[i].bind.value.button);
        } if (_this->mappings[i].bind.bindType == SDL_CONTROLLER_BINDTYPE_HAT) {
            snprintf(from, 10, "h%d.%d", _this->mappings[i].bind.value.hat.hat,
                    _this->mappings[i].bind.value.hat.hat_mask);
        } else {
            continue;
        }
        snprintf(bind, 2056 - index, "%s:%s,", to, from);
        index += strlen(to) + strlen(from);
    }
    SDL_GameControllerAddMapping(bind);
    // TODO Save to file
}

static int _EventFilter(void* userdata, SDL_Event* event)
{
    CHECK_INIT(0)
    if (_this->open) {
        return 0;
    }

    int joystick_id = -1;
    SDL_GameControllerButtonBind bind;
    const int axis_deadzone = 32767/2;
    // Not thread safe!

    switch(event->type) {
//  Ignored Events
    case SDL_JOYBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
    case SDL_JOYBALLMOTION:
        return 1;
//  
//  Device changed
    case SDL_JOYDEVICEADDED:
    case SDL_CONTROLLERDEVICEADDED:
        break;
    case SDL_JOYDEVICEREMOVED:
        if (event->jdevice.which == _this->current_joystick_id) {
            _StopMapping();
        }
        break;
    case SDL_CONTROLLERDEVICEREMAPPED:
        if (event->cdevice.which == _this->current_joystick_id) {
            _StopMapping();
        }
        break;
//
    case SDL_CONTROLLERAXISMOTION:
        if (abs(event->caxis.value) >= axis_deadzone) {
            joystick_id = event->caxis.which;
            bind = SDL_GameControllerGetBindForAxis(_this->controller,
                                                    event->caxis.axis);
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        joystick_id = event->cbutton.which;
        bind = SDL_GameControllerGetBindForButton(_this->controller,
                                                  event->cbutton.button);
        break;
    case SDL_JOYAXISMOTION:
        if (event->caxis.value >= axis_deadzone) {
            joystick_id = event->jaxis.which;
            bind.value.axis = event->jaxis.axis;
            bind.bindType = SDL_CONTROLLER_BINDTYPE_AXIS;
        }
        break;
    case SDL_JOYBUTTONUP:
        joystick_id = event->jbutton.which;
        bind.value.button = event->jbutton.button;
        bind.bindType = SDL_CONTROLLER_BINDTYPE_BUTTON;
        break;
    case SDL_JOYHATMOTION:
        joystick_id = event->jhat.which;
        bind.value.hat.hat = event->jhat.hat;
        bind.value.hat.hat_mask = event->jhat.value;
        bind.bindType = SDL_CONTROLLER_BINDTYPE_HAT;
        default: break;
    }
    
    if (_this->is_mapping && joystick_id
        == _this->current_joystick_id) {
            // TODO Prevent duplicate mappings
            if (_this->current_mapping_index > NEXT_BUTTON 
                && memcmp(&bind, &_this->mappings[NEXT_BUTTON].bind,
                          sizeof(SDL_GameControllerButtonBind)) == 0)
            {
                ++_this->current_mapping_index;
            } else if (_this->current_mapping_index > PREV_BUTTON 
                && memcmp(&bind, &_this->mappings[PREV_BUTTON].bind,
                          sizeof(SDL_GameControllerButtonBind)) == 0)
            {
                --_this->current_mapping_index;
            } else {
                _this->mappings[_this->current_mapping_index].bind = bind;
                ++_this->current_mapping_index;
            }
            if (_this->current_mapping_index >= NUM_MAPPINGS) {
                _SaveMappings();
                _StopMapping();
            }
        return 1;
    }

    return 0;
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

    int num_joysticks = SDL_NumJoysticks();
    for (int i = 0; i < num_joysticks; ++i)
    {
        if (SDL_IsGameController(i)) {
            return SDL_FALSE;
        } else {
            SDL_Joystick * tmp_joystick = SDL_JoystickOpen(i);
            if (id == SDL_JoystickInstanceID(tmp_joystick)) {
                _this->joystick = tmp_joystick;
                _this->current_joystick_id = id;
                break;
            } else {
                SDL_JoystickClose(tmp_joystick);
            }
        }
    } 

    if (_this->joystick == NULL) {
        return SDL_FALSE;
    }

    if (!SDL_GetEventFilter(&_this->currentEventFilter,
                            &_this->currentEventFilterData)) {
	    _this->currentEventFilter = NULL; 
	    _this->currentEventFilterData = NULL;   
    }

	SDL_SetEventFilter(_EventFilter, NULL);
    _ResetMappings();
	_this->open = SDL_TRUE;
    _this->is_mapping = SDL_TRUE;
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
