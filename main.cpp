/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <filesystem>
namespace fs = std::filesystem;

#include "spng/spng.h"
#include <io.h>

namespace {

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

void loadPNG(fs::path& path)
{
    FILE* f = _wfopen(path.c_str(), L"rb");
    if (!f) return;
    const size_t len = _filelength(_fileno(f));
    std::vector<char> buff(len);
    fread(buff.data(), 1, len, f);
    fclose(f);

    spng_ctx* ctx = spng_ctx_new(0);
    spng_set_png_buffer(ctx, buff.data(), len);
    spng_ihdr ihdr;
    spng_get_ihdr(ctx, &ihdr);
    size_t out_size;
    const int fmt = SPNG_FMT_RGBA8;
    spng_decoded_image_size(ctx, fmt, &out_size);
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, ihdr.width, ihdr.height);
    if (texture) {
        void* pixels = nullptr;
        int pitch;
        if (SDL_LockTexture(texture, nullptr, &pixels, &pitch)) {
            spng_decode_image(ctx, pixels, pitch, fmt, 0);
            SDL_UnlockTexture(texture);
        }
    }
    spng_ctx_free(ctx);
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    else if (event->type == SDL_EVENT_DROP_FILE) {
        SDL_DropEvent* dropEvent = (SDL_DropEvent*)event;
        fs::path data{ dropEvent->data };
        if (data.extension() == ".png") {
            loadPNG(data);
        }
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
    const char* message = "Hello World!";
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (texture != nullptr) {
        SDL_RenderTexture(renderer, texture, NULL, NULL);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

