#include "History.h"
#include "CardImages.h"
#include "Defines.h"

#define HISTORY_CARD_WIDTH	(CARD_WIDTH/2)
#define HISTORY_CARD_HEIGHT	(CARD_HEIGHT/2)
#define HISTORY_WIDTH		(HISTORY_CARD_WIDTH*TORNADO_CARDS_ON_COLUMN_LIMIT+30/*For text area*/)
#define HISTORY_HEIGHT		(HISTORY_CARD_HEIGHT+2)

History::History(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_nNumberCards(0), m_pCardImages(pCardImages)
{
        strcpy(m_str, "");
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
	for(int i=0; i<TORNADO_CARDS_ON_COLUMN_LIMIT; i++) {
		m_pPiece[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, HISTORY_CARD_WIDTH, HISTORY_CARD_HEIGHT, 16, 0, 0, 0, 0);
	}
}	

History::~History()
{
	nSDL_FreeFont(m_pFont);
	for(int i=0; i<TORNADO_CARDS_ON_COLUMN_LIMIT; i++) {
		SDL_FreeSurface(m_pPiece[i]);
	}
}

void History::SetText(char* pstr)
{
   strcpy(m_str, pstr);
}

void History::AddCard(Card c)
{
   m_pCardImages->GetImageForCard(m_pPiece[m_nNumberCards], c, HISTORY_CARD_WIDTH, HISTORY_CARD_HEIGHT);
   m_nNumberCards++;
}

void History::ClearCards()
{
   m_nNumberCards = 0;
}

void History::UpdateDisplay()
{
	int nY = SCREEN_HEIGHT-25/*above score & bonus*/-HISTORY_CARD_HEIGHT;
	SDL_Rect rectDest;
	rectDest.x = 0;
	rectDest.y = nY;
	rectDest.w = HISTORY_WIDTH;
	rectDest.h = HISTORY_HEIGHT;
	SDL_FillRect(m_pScreen, &rectDest, SDL_MapRGB(m_pScreen->format, HISTORY_BACKGROUND_R, HISTORY_BACKGROUND_G, HISTORY_BACKGROUND_B));

	for(int i=0; i<m_nNumberCards; i++) {
		int nX = i*HISTORY_CARD_WIDTH;
		DrawPiece(m_pPiece[i], nX, nY);
	}

	if( m_nNumberCards > 0 ) {
		int nTextWidth = nSDL_GetStringWidth(m_pFont, m_str);
		int nX = TORNADO_CARDS_ON_COLUMN_LIMIT*HISTORY_CARD_WIDTH;
		nSDL_DrawString(m_pScreen, m_pFont, nX+2/*padding*/, nY+HISTORY_CARD_HEIGHT/2, m_str);
	}
}

void History::DrawPiece(SDL_Surface* pSurface, int nX, int nY)
{
	SDL_Rect rectDest;
	rectDest.x = nX;
	rectDest.y = nY;
	rectDest.w = CARD_WIDTH;
	rectDest.h = CARD_HEIGHT;

    SDL_BlitSurface(pSurface, NULL, m_pScreen, &rectDest);
}


