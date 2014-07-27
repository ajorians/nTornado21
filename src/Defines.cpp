#include "Defines.h"

Uint16 get_pixel16( SDL_Surface *surface, int x, int y )
{
   int bpp = surface->format->BytesPerPixel;
   Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp; //Get the requested pixel 
   return *(Uint16 *)p;
}

void put_pixel16( SDL_Surface *surface, int x, int y, Uint16 pixel )
{
   int bpp = surface->format->BytesPerPixel;
   Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
   *(Uint16 *)p = pixel;
}

