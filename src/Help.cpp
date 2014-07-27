#include "Help.h"
#include "HelpGraphic.h"

TornadoHelp::TornadoHelp(SDL_Surface* pScreen)
: m_pScreen(pScreen)
{
	m_pBackground = nSDL_LoadImage(image_data);
}

TornadoHelp::~TornadoHelp()
{
	SDL_FreeSurface(m_pBackground);
}

bool TornadoHelp::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	UpdateDisplay();
	
	return true;
}

bool TornadoHelp::PollEvents()
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
					case SDLK_RETURN:
					case SDLK_SPACE:
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

void TornadoHelp::UpdateDisplay()
{
	SDL_BlitSurface(m_pBackground, NULL, m_pScreen, NULL);
	
	SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}




