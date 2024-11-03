#include <SDL3/SDL.h>
#include <bgfx/platform.h>
#include "debug.hpp"

static constexpr char NAME[] = "app";
static constexpr int INIT_WINDOW_WIDTH = 800;
static constexpr int INIT_WINDOW_HEIGHT = 600;

bgfx::PlatformData pd;
bgfx::Init init;

SDL_Window* window = nullptr;

void render()
{
    bgfx::touch(0);
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    bgfx::reset(width, height, BGFX_RESET_VSYNC);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x64512DFF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, 100, 100);

    bgfx::frame();
}

bool SDLCALL eventcallback(void *userdata, SDL_Event *event)
{            
    switch (event->type)
    {
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            render();
            break;
    }
    return true;
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    // Create an application window with the following settings:
    constexpr SDL_WindowFlags WFLAGS = SDL_WINDOW_METAL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    window = SDL_CreateWindow("main", // window title
        640,                          // width, in pixels
        480,                          // height, in pixels
        WFLAGS                        // flags - see below
    );

    // Check that the window was successfully created
    if (window == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // This call should be only used on platforms that don't
    // allow creating separate rendering thread. If it is called before
    // bgfx::init, render thread won't be created by bgfx::init call.
    bgfx::renderFrame();

    // initialize bgfx
    pd.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window),
        SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
    if (pd.nwh == nullptr)
    {
        DEBUG_PRINT("Failed to get valid window handle\n");
        return 1;
    }
    init.type = bgfx::RendererType::Count; // auto choose renderer
    init.resolution.width = INIT_WINDOW_WIDTH;
    init.resolution.height = INIT_WINDOW_HEIGHT;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.platformData = pd;

    if (!bgfx::init(init))
    {
        DEBUG_PRINT("bgfx initialization failed\n");
        return 1;
    }

    if (!SDL_AddEventWatch(eventcallback, nullptr))
    {
        DEBUG_PRINT("Failed to create callback%s\n", SDL_GetError());
    }
    
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x6495EDFF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, 10, 10);

    // The window is open: could enter program loop here (see SDL_PollEvent()
    bool running = true;
    int i = 0;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {  // poll until all events are handled!
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }

        render();
    }
    
    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
