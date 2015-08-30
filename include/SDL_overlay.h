#ifndef _SDL_overlay_h_
#define _SDL_overlay_h_

#include "SDL_stdinc.h"
#include "SDL_platform.h"

#include "begin_code.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * 
 * This function initialized the Overlay subsystem
 *
 * \return True if subsystem was init successfully
 */
extern DECLSPEC SDL_bool SDLCALL OVL_Init(const char* theme_dir,
                                          const char* language);

/**
 * This function cleans up the Overlay subsystem
 */
extern DECLSPEC void SDLCALL OVL_Quit();

/**
 * Check to see if the Overlay is open.
 * 
 * \return True if the overlay is currently open.
 */
extern DECLSPEC SDL_bool SDLCALL OVL_IsOverlayOpen();

/**
 * Sets the callback to use if the user opens the overlay.
 */
extern DECLSPEC void SDLCALL OVL_SetOverlayCallback();

/**
 * Open the overlay on the controller config screen.
 *
 * \return False if the overlay was already open or could not be open.
 */
extern DECLSPEC SDL_bool SDLCALL OVL_OpenControllerOverlay(int id);

/**
 * Call this to force presentation of the overlay on
 *
 * \return False on error.
 */
extern DECLSPEC SDL_bool SDLCALL OVL_PresentOverlay();

/**
 * Replace calls to SDL_GL_SwapWindow with this.
 *
 * \return False on error.
 */
extern DECLSPEC SDL_bool SDLCALL OVL_GL_SwapWindow(SDL_Window* window);

/**
 * Replace calls to SDL_RenderPresent with this.
 *
 * \return False on error.
 */
extern DECLSPEC SDL_bool SDLCALL OVL_RenderPresent(SDL_Renderer* window);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#include "close_code.h"

#endif /* _SDL_overlay_h_ */

/* vi: set ts=4 sw=4 expandtab: */
