#include "GameOver.h"
#include "Defines.h"

TornadoGameOver::TornadoGameOver(SDL_Surface* pScreen, int nScore)
: m_pScreen(pScreen), m_nScore(nScore), m_bNewHighScore(false), m_bPlayAgain(false), m_bShift(false)
{
	int nRes = HighScoreCreate(&m_HighScore);
        nRes = OpenHighScoreFile(m_HighScore, TORNADO21_HIGHSCORE_FILENAME);

	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
	m_pFontPlayAgain = nSDL_LoadFont(NSDL_FONT_VGA, 0, 255, 0);

	m_bNewHighScore = m_nScore > 0 && IsHighScore(m_HighScore, "", m_nScore);
	strcpy(m_strNameBuffer, "");
}

TornadoGameOver::~TornadoGameOver()
{
	HighScoreFree(&m_HighScore);

	nSDL_FreeFont(m_pFont);
	nSDL_FreeFont(m_pFontPlayAgain);
}

bool TornadoGameOver::PlayAgain() const
{
	return m_bPlayAgain;
}

bool TornadoGameOver::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool TornadoGameOver::PollEvents()
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
					
					case SDLK_SPACE:
						AddLetter(' ');
						break;

					case SDLK_RETURN:
					case SDLK_LCTRL:
						if( m_bNewHighScore ) {
							if( strlen(m_strNameBuffer) > 0 ) {
								int nRes = AddHighScore(m_HighScore, "", m_nScore, m_strNameBuffer, 0, NULL, NULL);
								m_bPlayAgain = false;
								return false;
							}
						}
						else {
							m_bPlayAgain = true;
							return false;
						}
						break;

					case SDLK_LSHIFT:
					case SDLK_RSHIFT:
						m_bShift = !m_bShift;
						break;
					
					case SDLK_CLEAR:
					case SDLK_DELETE:
					case SDLK_BACKSPACE:
						Backspace();
						break;
					default:
						break;
				}
				if( event.key.keysym.sym >= (Uint16)'0' && (event.key.keysym.sym <= (Uint16)'9')) {
					AddLetter((char)event.key.keysym.sym);
				}
                                if( event.key.keysym.sym >= (Uint16)'a' && (event.key.keysym.sym <= (Uint16)'z')) {
					AddLetter(m_bShift ? (char)event.key.keysym.sym - ('a' - 'A') : (char)event.key.keysym.sym);
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

void TornadoGameOver::AddLetter(char ch)
{
   int nLen = strlen(m_strNameBuffer);
   if( nLen < 16 ) {
      m_strNameBuffer[nLen] = ch;
      m_strNameBuffer[nLen+1] = '\0';
   }
}

void TornadoGameOver::Backspace()
{
   int nLen = strlen(m_strNameBuffer);
   if( nLen > 0 ) {
      m_strNameBuffer[nLen-1] = '\0';
   }
}

void TornadoGameOver::UpdateDisplay()
{
	if( m_bNewHighScore ) {
		SDL_Rect dstRect;
	        dstRect.x = 55;
	        dstRect.y = 0;
	        dstRect.w = 225;
	        dstRect.h = 115;
		SDL_FillRect(m_pScreen, &dstRect, SDL_MapRGB(m_pScreen->format, 0, 0, 255));
	}
	else {
		SDL_Rect dstRect;
	        dstRect.x = 75;
	        dstRect.y = 0;
	        dstRect.w = 200;
	        dstRect.h = 115;
	        SDL_FillRect(m_pScreen, &dstRect, SDL_MapRGB(m_pScreen->format, 0, 0, 255));
	}
	
	nSDL_DrawString(m_pScreen, m_pFont, 125, 10, "Game Over!");
	
	nSDL_DrawString(m_pScreen, m_pFont, 115, 40, "Your score was:");
	
	nSDL_DrawString(m_pScreen, m_pFont, 155, 60, "%d", m_nScore);

	if( m_bNewHighScore ) {
	        nSDL_DrawString(m_pScreen, m_pFontPlayAgain, 62, 80, "New high score!  Type your name!");

		int nNameWidth = nSDL_GetStringWidth(m_pFontPlayAgain, m_strNameBuffer);

	        nSDL_DrawString(m_pScreen, m_pFontPlayAgain, 55+(225-nNameWidth)/2, 100, m_strNameBuffer);
	}
	else {	
	nSDL_DrawString(m_pScreen, m_pFontPlayAgain, 82, 80, "Press Enter to play again");

	}
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




