#include "DeckSelector.h"
#include "CardImages.h"
#include "Defines.h"

#define DECK_SELECTOR_XPOS	(196)

DeckSelector::DeckSelector(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_pCardImages(pCardImages)
{
	m_pPiece = SDL_CreateRGBSurface(SDL_SWSURFACE, CARD_WIDTH, CARD_HEIGHT, 16, 0, 0, 0, 0);
	m_pCardImages->GetImageForDeckStyle(m_pPiece);
}	

DeckSelector::~DeckSelector()
{
	SDL_FreeSurface(m_pPiece);
}

void DeckSelector::UpdateDisplay()
{
	DrawPiece(DECK_SELECTOR_XPOS, SCREEN_HEIGHT-CARD_HEIGHT);
}

void DeckSelector::DrawPiece(int nX, int nY)
{
	SDL_Rect rectDest;
	rectDest.x = nX;
	rectDest.y = nY;
	rectDest.w = CARD_WIDTH;
	rectDest.h = CARD_HEIGHT;

    SDL_BlitSurface(m_pPiece, NULL, m_pScreen, &rectDest);
}














