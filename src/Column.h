#ifndef COLUMN_H
#define COLUMN_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "CardLib/CardLib.h"
	#include "TornadoLib/TornadoLib.h"
}

#define COLUMN_WIDTH			(CARD_WIDTH + 2)
#define COLUMN_HEIGHT			(170)

class CardImages;

class Column
{
public:
	Column();
	~Column();
	void Setup(int nX, SDL_Surface* pScreen, CardImages* pCardImages);

	void SetText(char* pstr);
	void AddCard(Card c);
	void ClearCards();
	void SetEnabled(bool bEnabled);
	void UpdateDisplay();

protected:
	void DrawPiece(SDL_Surface* pSurface, int nX, int nY);
	void Gray();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface	*m_pColumn;
	SDL_Surface *m_pPiece[TORNADO_CARDS_ON_COLUMN_LIMIT];
	nSDL_Font *m_pFont;
	int m_nX;
	int m_nNumberCards;
	bool m_bEnabled;
	char m_str[16];
        CardImages* m_pCardImages;//Does not own
};

#endif
