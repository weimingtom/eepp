/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL_config.h"

#ifndef _SDL_waylandvideo_h
#define _SDL_waylandvideo_h

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <wayland-egl.h>

#include <EGL/egl.h>

struct xkb_context;
struct SDL_WaylandInput;

typedef struct {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_output *output;
    struct wl_shm *shm;
    struct wl_cursor_theme *cursor_theme;
    struct wl_cursor *default_cursor;
    struct wl_pointer *pointer;
    struct wl_shell *shell;

    struct {
        int32_t x, y, width, height;
    } screen_allocation;

    struct wl_list modes_list;

    EGLDisplay edpy;
    EGLContext context;
    EGLConfig econf;

    struct xkb_context *xkb_context;
    struct SDL_WaylandInput *input;

    uint32_t shm_formats;
} SDL_VideoData;

static inline void
wayland_schedule_write(SDL_VideoData *data)
{
    wl_display_flush(data->display);
}

#endif /* _SDL_nullvideo_h */

/* vi: set ts=4 sw=4 expandtab: */
