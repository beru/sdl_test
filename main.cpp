#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_iostream.h>

#include <string_view>
#include "spng/spng.h"

#include "matrix4.h"
using Mat4f = Matrix4<float>;
using Vec4f = Vector4<float>;

namespace {

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
Mat4f tmat = Mat4f::Identity();
Mat4f mousedown_tmat;
SDL_FPoint mousedown_pos;

}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    /* Create the window */
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (!SDL_CreateWindowAndRenderer("Hello World", 1280, 960, window_flags, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void loadPNG(const char* path)
{
    size_t datasize;
    void* buff = SDL_LoadFile(path, &datasize);
    spng_ctx* ctx = spng_ctx_new(0);
    spng_set_png_buffer(ctx, buff, datasize);
    spng_ihdr ihdr;
    spng_get_ihdr(ctx, &ihdr);
    size_t out_size;
    const int fmt = SPNG_FMT_RGBA8;
    spng_decoded_image_size(ctx, fmt, &out_size);
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, ihdr.width, ihdr.height);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    if (texture) {
        void* pixels = nullptr;
        int pitch;
        if (SDL_LockTexture(texture, nullptr, &pixels, &pitch)) {
            spng_decode_image(ctx, pixels, pitch, fmt, 0);
            SDL_UnlockTexture(texture);
        }
    }
    spng_ctx_free(ctx);
    SDL_free(buff);
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    else if (event->type == SDL_EVENT_KEY_DOWN) {
        SDL_KeyboardEvent* key_event = (SDL_KeyboardEvent*)event;
        if (key_event->key == SDLK_F11) {
            static bool fulscreen = false;
            fulscreen = !fulscreen;
            SDL_SetWindowFullscreen(window, fulscreen);
        }
        else if (key_event->key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }
    else if (event->type == SDL_EVENT_DROP_FILE) {
        SDL_DropEvent* drop_event = (SDL_DropEvent*)event;
         if (std::string_view{ drop_event->data }.ends_with(".png")) {
            loadPNG(drop_event->data);
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        if (SDL_ConvertEventToRenderCoordinates(renderer, event)) {
            SDL_MouseWheelEvent* wheel_event = (SDL_MouseWheelEvent*)event;
            float scale = 1.0f + 0.1f * wheel_event->y;
            tmat = Mat4f::Scaling(scale, scale, 1.0f, wheel_event->mouse_x, wheel_event->mouse_y, 0.0f) * tmat;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (SDL_ConvertEventToRenderCoordinates(renderer, event)) {
            SDL_MouseButtonEvent* mouse_event = (SDL_MouseButtonEvent*)event;
            mousedown_pos.x = mouse_event->x;
            mousedown_pos.y = mouse_event->y;
            mousedown_tmat = tmat;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (SDL_ConvertEventToRenderCoordinates(renderer, event)) {
            SDL_MouseButtonEvent* mouse_event = (SDL_MouseButtonEvent*)event;
            mousedown_pos.x = mouse_event->x;
            mousedown_pos.y = mouse_event->y;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_MOTION) {
        if (SDL_ConvertEventToRenderCoordinates(renderer, event)) {
            SDL_MouseMotionEvent* mouse_event = (SDL_MouseMotionEvent*)event;
            if (mouse_event->state & SDL_BUTTON_LEFT) {
                auto tx = mouse_event->x - mousedown_pos.x;
                auto ty = mouse_event->y - mousedown_pos.y;
                tmat = Mat4f::Translation(tx, ty, 0.0f) * mousedown_tmat;
            }
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

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);

    const float scale = 1.0f;
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;

    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, x, y, message);
    if (texture != nullptr) {
        Vec4f vtl, vtr(texture->w, 0), vbl(0, texture->h);
        vtl = tmat * vtl;
        vtr = tmat * vtr;
        vbl = tmat * vbl;

        SDL_FPoint top_left, top_right, bottom_left;
        top_left.x = vtl.x;
        top_left.y = vtl.y;
        top_right.x = vtr.x;
        top_right.y = vtr.y;
        bottom_left.x = vbl.x;
        bottom_left.y = vbl.y;
        SDL_RenderTextureAffine(renderer, texture, NULL,
            &top_left, &top_right, &bottom_left);
    }
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

int SDL_main(int argc, char* argv[])
{
    if (SDL_AppInit(nullptr, argc, argv) != SDL_APP_CONTINUE) {
        return 0;
    }
    if (!SDL_SetRenderVSync(renderer, 1)) {
        ;
    }
    while (true) {
        SDL_Event event;
        if (SDL_WaitEvent(&event)) {
            auto res = SDL_AppEvent(nullptr, &event);
            if (res == SDL_APP_SUCCESS) {
                SDL_AppQuit(nullptr, res);
                return 0;
            }
            else if (res == SDL_APP_CONTINUE) {
                SDL_AppIterate(nullptr);
            }
        }
        else {
            SDL_GetError();
        }
    }
    return 0;
}