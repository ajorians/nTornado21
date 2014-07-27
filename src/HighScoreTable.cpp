#include "HighScoreTable.h"
#include "HighScoreTableGraphic.h"
#include "Defines.h"

#define HIGHSCORE_X_POS			(0)
#define NAME_X_POS			(50)
#define HIGHSCORE_START_Y		(50)
#define HIGHSCORE_SEPARATION_AMOUNT	(20)

HighScoreTable::HighScoreTable(SDL_Surface* pScreen)
: m_pScreen(pScreen)
{
	int nRes = HighScoreCreate(&m_HighScore);
	nRes = OpenHighScoreFile(m_HighScore, TORNADO21_HIGHSCORE_FILENAME);
	m_pBackground = nSDL_LoadImage(image_HighScoreTable);
	m_pFontScore = nSDL_LoadFont(NSDL_FONT_VGA, 255, 0, 0);
	m_pFontName = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
}

HighScoreTable::~HighScoreTable()
{
	HighScoreFree(&m_HighScore);
	SDL_FreeSurface(m_pBackground);
	nSDL_FreeFont(m_pFontScore);
	nSDL_FreeFont(m_pFontName);
}

bool HighScoreTable::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool HighScoreTable::PollEvents()
{
	SDL_Event event;
	
	/* Poll for events. SDL_PollEvent() returns 0 when there are no  */
	/* more events on the event queue, our while loop will exit when */
	/* that occurs.                                                  */
	while( SDL_PollEvent( &event ) )
	{
		/* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
		switch( event.type )
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) 
				{
					case SDLK_ESCAPE:
						return false;
					break;
					
					default:
						break;
				}

				break;
			
			//Called when the mouse moves
			case SDL_MOUSEMOTION:
				break;
			
			case SDL_KEYUP:
				break;
			
			default:
				break;
		}
	}
	return true;
}

void HighScoreTable::UpdateDisplay()
{
	SDL_Rect dstRect;
        dstRect.x = 0;
        dstRect.y = 0;
        dstRect.w = SCREEN_WIDTH;
        dstRect.h = SCREEN_HEIGHT;
	SDL_BlitSurface(m_pBackground, NULL, m_pScreen, &dstRect);
        //SDL_FillRect(m_pScreen, &dstRect, SDL_MapRGB(m_pScreen->format, 255, 255, 255));
	
	for(int i=0; i<GetNumberScores(m_HighScore, ""); i++) {
		int nY = HIGHSCORE_START_Y + HIGHSCORE_SEPARATION_AMOUNT * i;
		nSDL_DrawString(m_pScreen, m_pFontScore, HIGHSCORE_X_POS, nY, "%d", GetScore(m_HighScore, "", i));
		nSDL_DrawString(m_pScreen, m_pFontName, NAME_X_POS, nY, GetName(m_HighScore, "", i));
	}
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




