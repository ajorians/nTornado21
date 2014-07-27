#ifndef TORNADOLIB_H_INCLUDED
#define TORNADOLIB_H_INCLUDED

#include "CardLib/CardLib.h"

typedef void* TornadoLib;

#define TORNADOLIB_OK			(0)
#define TORNADOLIB_BADARGUMENT		(-1)
#define TORNADOLIB_OUT_OF_MEMORY	(-2)
#define TORNADOLIB_DECK_EMTPY		(-3)
#define TORNADOLIB_CARD_FAILURE		(-4)
#define TORNADOLIB_CANNOT_ADD_TO_COLUMN	(-5)

#define TORNADOLIB_COMPLETED_COLUMN	(1)

#define TORNADO_GAME_OVER		(1)
#define TORNADO_NOT_GAME_OVER		(0)

#define TORNADO_CAN_PLACE_ON_COLUMN	(1)
#define TORNADO_CANNOT_PLACE_ON_COLUMN	(0)

#define TORNADO_NUMBER_OF_COLUMNS	(5)
#define TORNADO_NUMBER_JOKERS		(0)

#define TORNADO_FACE_CARD_VALUE		(10)

#define TORNADO_CARDS_ON_COLUMN_LIMIT	(5)
#define TORNADO_COLUMN_TARGET_VALUE	(21)

#define GOLD_CARD_EXTRA_DATA		(1)

#define TORNADO_NO_BONUS		(0)
#define TORNADO_BONUS_21LARGE		(1)
#define TORNADO_BONUS_777		(2)
#define TORNADO_BONUS_21STRAIGHT	(3)
#define TORNADO_BONUS_BIGSTRAIGHT	(4)
#define TORNADO_BONUS_LITTLESTRAIGHT	(5)
#define TORNADO_BONUS_911EMERGENCY	(6)
#define TORNADO_BONUS_DOUBLE_JOKER	(7)
#define TORNADO_BONUS_5_OF_A_KIND	(8)
#define TORNADO_BONUS_CLEARED_BOARD	(9)
#define TORNADO_BONUS_LESSER_STRAIGHT	(10)
#define TORNADO_BONUS_GREATER_STRAIGHT	(11)
#define TORNADO_BONUS_CLEARED_DECK	(12)//No gold card for this one

#define TORNADO_ADD_JOKER_COLUMN4TIMES	(1)
#define TORNADO_ADD_JOKER_3MORE_GOLDCARDS (2)
#define TORNADO_ADD_JOKER_CLEAR_GOLD	(3)
#define TORNADO_ADD_JOKER_DIVISIBLE	(4)

//////////////////////////////////////////////
//Initalization/Error checking/Mode functions
//////////////////////////////////////////////
int TornadoLibCreate(TornadoLib* api);
int TornadoLibFree(TornadoLib* api);

int GetTornadoLibError(TornadoLib api);
void ClearTornadoLibError(TornadoLib api);

//////////////////////////////////////////////
//TornadoLib related functions
//////////////////////////////////////////////
int GetDeckCard(TornadoLib api, Card* pCard);
int CanPlaceDeckCardOnColumn(TornadoLib api, int nIndex);
int PlaceDeckCardOnColumn(TornadoLib api, int nIndex);
int IsTornadoGameOver(TornadoLib api);
int GetNumberCardsOnColumn(TornadoLib api, int nIndex);
int GetCardOnColumn(TornadoLib api, int nColumnIndex, int nCardIndex, Card* pCard);
int GetColumnString(TornadoLib api, int nIndex, char* pstr);
int GetTornadoScore(TornadoLib api);
int GetTornadoBonusScore(TornadoLib api);
int GetLastTornadoBonus(TornadoLib api);
int ClearLastTornadoBonus(TornadoLib api);
//History related
int GetNumberOfHistoryItems(TornadoLib api);
int GetNumberCardsOnHistory(TornadoLib api, int nIndex);
int GetCardOnHistory(TornadoLib api, int nIndex, int nCardIndex, Card* pCard);
int GetHistoryString(TornadoLib api, int nIndex, char* pstr);

#endif //TORNADOLIB_H_INCLUDED
