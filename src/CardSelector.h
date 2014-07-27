#ifndef CARDSELECTOR_H
#define CARDSELECTOR_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "CardLib/CardLib.h"
}

class CardImages;

class CardSelector
{
public:
	CardSelector(SDL_Surface* pScreen, CardImages* pCardImages);
	~CardSelector();

	void SetCurrentCard(Card c);
	void UpdateDisplay();

protected:
	void DrawPiece(int nX, int nY);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface *m_pPiece;
        CardImages* m_pCardImages;//Does not own
};

#endif
