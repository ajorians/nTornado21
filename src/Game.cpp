#include "Game.h"
#include "GameOver.h"

#define HIGHLIGHT_EXTEND_AMOUNT			(3)

Game::Game(SDL_Surface* pScreen, CardImages* pCardImages)
: m_pScreen(pScreen), m_Selector(pScreen, pCardImages), m_Deck(pScreen, pCardImages), m_History(pScreen, pCardImages), m_BonusMessage(pScreen), m_pCardImages(pCardImages), m_nCurrentColumn(0), m_nCurrentHistoryItem(0), m_bGameOver(false)
{
	TornadoLibCreate(&m_Tornado);

	for(int i=0; i<TORNADO_NUMBER_OF_COLUMNS; i++) {
		m_Columns[i].Setup(GetColumnXPos(i), pScreen, pCardImages); 
	}

	m_pFont = nSDL_LoadFont(NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/);

	UpdateParts();
}

Game::~Game()
{
	nSDL_FreeFont(m_pFont);
}

int Game::GetColumnWidth() const
{
   return CARD_WIDTH + 2;
}

int Game::GetColumnSpacing() const
{
   int nSpacing = (SCREEN_WIDTH-(TORNADO_NUMBER_OF_COLUMNS*GetColumnWidth()))/(TORNADO_NUMBER_OF_COLUMNS+1);
   return nSpacing;
}

int Game::GetColumnXPos(int nColumnIndex) const
{
   int nX = (nColumnIndex+1)*GetColumnSpacing() + (nColumnIndex*GetColumnWidth());
   return nX; 
}

bool Game::Loop()
{
	//Handle keypresses
	if( PollEvents() == false )
		return false;
	
	//Update screen
	UpdateDisplay();
	
	//SDL_Delay(5);
	
	return !IsGameOver();
}

bool Game::IsGameOver() const
{
	return m_bGameOver;
}

int Game::GetBonus() const
{
	return GetTornadoBonusScore(m_Tornado);
}

int Game::GetScore() const
{
	return GetTornadoScore(m_Tornado) + GetBonus();
}

bool Game::PollEvents()
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
						fprintf(stderr, "Hit Escape!n");
					return false;
					break;

					case SDLK_RETURN:
					case SDLK_LCTRL:
					case SDLK_RCTRL:
						PlaceDeckCard();
					break;
					
					case SDLK_RIGHT:
					case SDLK_6:
						if( m_nCurrentColumn < (TORNADO_NUMBER_OF_COLUMNS-1) )
							m_nCurrentColumn++;
					break;
					
					case SDLK_LEFT:
					case SDLK_4:
						if( m_nCurrentColumn > 0 )
							m_nCurrentColumn--;
						break;
					case SDLK_PLUS:
						if( (m_nCurrentHistoryItem+1)<GetNumberOfHistoryItems(m_Tornado) ) {
							m_nCurrentHistoryItem++;
							UpdateParts();
						}
					break;
					case SDLK_MINUS:
						m_nCurrentHistoryItem--;
						if( m_nCurrentHistoryItem < 0 ) {
							m_nCurrentHistoryItem = 0;
						}
						UpdateParts();
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

void Game::PlaceDeckCard()
{
   if( TORNADOLIB_COMPLETED_COLUMN == PlaceDeckCardOnColumn(m_Tornado, m_nCurrentColumn) ) {
      if( m_nCurrentHistoryItem > 0 )//I hope this isn't weird; I completed a column; so keep history item
         m_nCurrentHistoryItem++;
   }
   UpdateParts();
}

void Game::UpdateParts()
{
   Card c;
   GetDeckCard(m_Tornado, &c);
   m_Selector.SetCurrentCard(c);

   //TODO: Could be faster if needed; because I won't need to go through all columns!
   for(int i=0; i<TORNADO_NUMBER_OF_COLUMNS; i++) {
      int nCardsOnColumn = GetNumberCardsOnColumn(m_Tornado, i);

      m_Columns[i].ClearCards();
      for(int j=0; j<nCardsOnColumn; j++) {
         Card card;
         GetCardOnColumn(m_Tornado, i, j, &card);
         m_Columns[i].AddCard(card);
      }

      m_Columns[i].SetEnabled(CanPlaceDeckCardOnColumn(m_Tornado, i) == TORNADO_CAN_PLACE_ON_COLUMN );

      char buffer[16];
      GetColumnString(m_Tornado, i, buffer);
      m_Columns[i].SetText(buffer);
   }

   m_History.ClearCards();
   if( GetNumberOfHistoryItems(m_Tornado) > 0 ) {
      int nCardsOnHistory = GetNumberCardsOnHistory(m_Tornado, m_nCurrentHistoryItem);
      for(int i=0; i<nCardsOnHistory; i++) {
         Card card;
         GetCardOnHistory(m_Tornado, m_nCurrentHistoryItem, i, &card);
         m_History.AddCard(card);
      }
      char buffer[16];
      GetHistoryString(m_Tornado, m_nCurrentHistoryItem, buffer);
      m_History.SetText(buffer);
   }

   m_bGameOver = IsTornadoGameOver(m_Tornado);

   int nBonus = GetLastTornadoBonus(m_Tornado);
   if( nBonus != TORNADO_NO_BONUS ) {
      switch(nBonus) {
         case TORNADO_BONUS_21LARGE:
            m_BonusMessage.CreateMessage("21 Large\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_777:
            m_BonusMessage.CreateMessage("Triple 7's\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_21STRAIGHT:
            m_BonusMessage.CreateMessage("21\nStraight\nJackpot!\n30 Points!");
         break;
         case TORNADO_BONUS_BIGSTRAIGHT:
            m_BonusMessage.CreateMessage("Big\nStraight\nJackpot!\n30 Points!");
         break;
         case TORNADO_BONUS_LITTLESTRAIGHT:
            m_BonusMessage.CreateMessage("Little\nStraight\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_911EMERGENCY:
            m_BonusMessage.CreateMessage("911\nEmergency\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_DOUBLE_JOKER:
            m_BonusMessage.CreateMessage("Double Joker\nJackpot!\n350 Points!");
         break;
         case TORNADO_BONUS_5_OF_A_KIND:
            m_BonusMessage.CreateMessage("5 of a Kind\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_CLEARED_BOARD:
            m_BonusMessage.CreateMessage("Cleared\nBoard\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_LESSER_STRAIGHT:
            m_BonusMessage.CreateMessage("Lesser\nStraight\nJackpot!\n30 Points!");
         break;
         case TORNADO_BONUS_GREATER_STRAIGHT:
            m_BonusMessage.CreateMessage("Greater\nStraight\nJackpot!\n65 Points!");
         break;
         case TORNADO_BONUS_CLEARED_DECK:
            m_BonusMessage.CreateMessage("Cleared\nDeck\nBonus!\n30 Points!");
         break;
      }
      ClearLastTornadoBonus(m_Tornado);
   }
}

void Game::UpdateDisplay()
{
	//Draw background
	SDL_FillRect(m_pScreen, NULL, SDL_MapRGB(m_pScreen->format, GAME_BACKGROUND_R, GAME_BACKGROUND_G, GAME_BACKGROUND_B));
	
	//Draw score stuff
	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT-20, "Bonus:         %d", GetBonus() );
	nSDL_DrawString(m_pScreen, m_pFont, 0, SCREEN_HEIGHT-10, "Current score: %d", GetScore() );
	m_Selector.UpdateDisplay();
	m_Deck.UpdateDisplay();

	SDL_Rect dstRect;
	dstRect.x = GetColumnXPos(m_nCurrentColumn) - HIGHLIGHT_EXTEND_AMOUNT;
	dstRect.y = 0;
	dstRect.w = GetColumnWidth() + 2 * HIGHLIGHT_EXTEND_AMOUNT;
	dstRect.h = COLUMN_HEIGHT + HIGHLIGHT_EXTEND_AMOUNT;

	bool bCanPlaceOnCurrentColumn = CanPlaceDeckCardOnColumn(m_Tornado, m_nCurrentColumn) == TORNADO_CAN_PLACE_ON_COLUMN;

	Uint32 uColor = bCanPlaceOnCurrentColumn ?
			SDL_MapRGB(m_pScreen->format, 0, 0, 255) 
			: SDL_MapRGB(m_pScreen->format, 255, 0, 0);

	SDL_FillRect(m_pScreen, &dstRect, uColor); 

	for(int i=0; i<TORNADO_NUMBER_OF_COLUMNS; i++) {
		m_Columns[i].UpdateDisplay();
	}

	m_History.UpdateDisplay();

	if( !m_BonusMessage.Animate() ){
		SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
}


