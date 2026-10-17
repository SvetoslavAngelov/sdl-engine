#include <SDL3/SDL_main.h>

#include "sdl_interface.h"

// SDL requires this signature on platforms that wrap the entry point.
int main(int, char**) {
    SDLInterface app;
    if (!app.OnInit()) return 1;
    return app.Run() ? 0 : 1;
}
