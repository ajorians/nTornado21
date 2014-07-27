#ifndef GAME_H
#define GAME_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "TornadoLib/TornadoLib.h"
}

#include "Defines.h"
#include "CardImages.h"
#include "CardSelector.h"
#include "DeckSelector.h"
#include "Column.h"
#include "History.h"
#include "BonusMessage.h"

class Game
{
public:
	Game(SDL_Surface* pScreen, CardImages* pCardImages);
	~Game();

	bool Loop();
	bool IsGameOver() const;
	int GetBonus() const;
	int GetScore() const;
	
protected:
	bool PollEvents();
	void PlaceDeckCard();
	void UpdateParts();
	void UpdateDisplay();

	int GetColumnWidth() const;
	int GetColumnSpacing() const;
	int GetColumnXPos(int nColumnIndex) const;

protected:
	SDL_Surface	*m_pScreen;//Does not own
	nSDL_Font *m_pFont;
	CardSelector m_Selector;
        DeckSelector m_Deck;
	Column m_Columns[TORNADO_NUMBER_OF_COLUMNS];
	History m_History;
        BonusMessage m_BonusMessage;
	CardImages* m_pCardImages;
	TornadoLib m_Tornado;
	int m_nCurrentColumn;
	int m_nCurrentHistoryItem;
	bool m_bGameOver;
};

#endif
