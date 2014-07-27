#ifndef DECKSELECTOR_H
#define DECKSELECTOR_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "CardLib/CardLib.h"
}

class CardImages;

class DeckSelector
{
public:
	DeckSelector(SDL_Surface* pScreen, CardImages* pCardImages);
	~DeckSelector();

	void UpdateDisplay();

protected:
	void DrawPiece(int nX, int nY);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface *m_pPiece;
        CardImages* m_pCardImages;//Does not own
};

#endif
