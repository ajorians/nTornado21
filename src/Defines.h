#ifndef DEFINES_H
#define DEFINES_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
}

#ifndef DEBUG_MSG
#define DEBUG_MSG//     printf
#endif

#define CARD_WIDTH	(42)
#define CARD_HEIGHT	(62)

#define TORNADO21_HIGHSCORE_FILENAME    "Tornado21HighScore.tns"

#define GAME_BACKGROUND_R	(153)
#define GAME_BACKGROUND_G	(153)
#define GAME_BACKGROUND_B	(102)

#define COLUMN_BACKGROUND_R	(218)
#define COLUMN_BACKGROUND_G	(218)
#define COLUMN_BACKGROUND_B	(145)

#define HISTORY_BACKGROUND_R	(COLUMN_BACKGROUND_R)
#define HISTORY_BACKGROUND_G	(COLUMN_BACKGROUND_G)
#define HISTORY_BACKGROUND_B	(COLUMN_BACKGROUND_B)

#define BONUS_TEXT_R		(107)
#define BONUS_TEXT_G		(107)
#define BONUS_TEXT_B		(71)

#define BONUS_BACKGROUND_R	(255)
#define BONUS_BACKGROUND_G	(255)
#define BONUS_BACKGROUND_B	(207)

Uint16 get_pixel16( SDL_Surface *surface, int x, int y );
void put_pixel16( SDL_Surface *surface, int x, int y, Uint16 pixel );

#endif
