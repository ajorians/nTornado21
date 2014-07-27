#ifndef HISTORY_H
#define HISTORY_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "CardLib/CardLib.h"
	#include "TornadoLib/TornadoLib.h"
}

class CardImages;

class History
{
public:
	History(SDL_Surface* pScreen, CardImages* pCardImages);
	~History();

	void SetText(char* pstr);
	void AddCard(Card c);
	void ClearCards();
	void UpdateDisplay();

protected:
	void DrawPiece(SDL_Surface* pSurface, int nX, int nY);

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface *m_pPiece[TORNADO_CARDS_ON_COLUMN_LIMIT];
	nSDL_Font *m_pFont;
	int m_nNumberCards;
	char m_str[16];
        CardImages* m_pCardImages;//Does not own
};

#endif
