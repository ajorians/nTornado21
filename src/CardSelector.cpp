#include "CardSelector.h"
#include "CardImages.h"
#include "Defines.h"

#define CARD_SELECTOR_XPOS	(256)

CardSelector::CardSelector(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_pCardImages(pCardImages)
{
	m_pPiece = SDL_CreateRGBSurface(SDL_SWSURFACE, CARD_WIDTH, CARD_HEIGHT, 16, 0, 0, 0, 0);
}	

CardSelector::~CardSelector()
{
	SDL_FreeSurface(m_pPiece);
}

void CardSelector::SetCurrentCard(Card c)
{
   m_pCardImages->GetImageForCard(m_pPiece, c);
}

void CardSelector::UpdateDisplay()
{
	DrawPiece(CARD_SELECTOR_XPOS, SCREEN_HEIGHT-CARD_HEIGHT);
}

void CardSelector::DrawPiece(int nX, int nY)
{
	SDL_Rect rectDest;
	rectDest.x = nX;
	rectDest.y = nY;
	rectDest.w = CARD_WIDTH;
	rectDest.h = CARD_HEIGHT;

    SDL_BlitSurface(m_pPiece, NULL, m_pScreen, &rectDest);
}














