//Public domain :)

#include <os.h>
#include "include/TornadoLib.h"
#include "Defines.h"

struct TornadoColumn
{
   CardLib m_SetOfCards;
};

//Right now I only keep track of the most recent cards; maybe I'll adjust this in the future!
struct TornadoHistory
{
   CardLib m_Cards;
   int m_nPoints;
   struct TornadoHistory* m_pNext;
};

//The way I do this is an array of ints and I thought maybe the crash was because I needed to
//relocate data; I think it is ok though! :)
#define TORNADO_WATCH_COLUMNS

#define TORNADO_CHECK_STRAIGHTS

#ifdef TORNADO_WATCH_COLUMNS
#define TORNADO_NUMBER_COMPLETED_COLUMN_WATCHING	(5)
#endif

#define TORNADO_GOLD_CARDS_NECESSARY_FOR_JOKER_CLEAR_BONUS	(2)

struct Tornado
{
   int m_nLastError;
   CardLib m_Deck;
   struct TornadoColumn m_Column[TORNADO_NUMBER_OF_COLUMNS];
   struct TornadoHistory* m_pHistoryRoot;
   int m_nScore;
   int m_nBonus;
   int m_nLastBonus;
#ifdef TORNADO_WATCH_COLUMNS
   int m_nContinousCompletions;
   int m_nLastCompletedColumns[TORNADO_NUMBER_COMPLETED_COLUMN_WATCHING];
#endif
};

int TornadoLibCreate(TornadoLib* api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = malloc(sizeof(struct Tornado));
   if( pT == NULL ){//Out of memory
      return TORNADOLIB_OUT_OF_MEMORY;
   }

   if( CARDLIB_OK != CardLibCreate(&pT->m_Deck) ) {
      free(pT);
      return TORNADOLIB_OUT_OF_MEMORY;//Assuming
   }

   if( CARDLIB_OK != AddStandardCards(pT->m_Deck, TORNADO_NUMBER_JOKERS) ) {
      CardLibFree(&pT->m_Deck);
      free(pT);
      return TORNADOLIB_OUT_OF_MEMORY;
   }

   if( CARDLIB_OK != Shuffle(pT->m_Deck) ) {
      CardLibFree(&pT->m_Deck);
      free(pT);
      return TORNADOLIB_CARD_FAILURE;
   }

   int n;
   for(n=0; n<TORNADO_NUMBER_OF_COLUMNS; n++) {
      int nRes = CardLibCreate(&pT->m_Column[n].m_SetOfCards);

      //If problem; free memory and return
      if( CARDLIB_OK != nRes ) {
         int m;
         for( m=0; m<n; m++ ) {
            CardLibFree(&pT->m_Column[m].m_SetOfCards);
         }
         CardLibFree(&pT->m_Deck);
         free(pT);
         return TORNADOLIB_OUT_OF_MEMORY;
      }
   }

   pT->m_pHistoryRoot = NULL;

   pT->m_nScore = 0;
   pT->m_nBonus = 0;
   pT->m_nLastBonus = TORNADO_NO_BONUS;

#ifdef TORNADO_WATCH_COLUMNS
   pT->m_nContinousCompletions = 0;
   for(n=0; n<TORNADO_NUMBER_COMPLETED_COLUMN_WATCHING; n++) {
      pT->m_nLastCompletedColumns[n] = -1;
   }
#endif

   pT->m_nLastError = TORNADOLIB_OK;

   *api = pT; 

   return TORNADOLIB_OK;
}

int TornadoLibFree(TornadoLib* api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = *api;

   CardLibFree(&pT->m_Deck);
   int n;
   for(n=0; n<TORNADO_NUMBER_OF_COLUMNS; n++) {
      CardLibFree(&pT->m_Column[n].m_SetOfCards);
   }

   struct TornadoHistory* pHistory;
   while(pT->m_pHistoryRoot != NULL) {
      pHistory = pT->m_pHistoryRoot;
      pT->m_pHistoryRoot = pHistory->m_pNext;
      CardLibFree(&pHistory->m_Cards);
      free(pHistory);
   }

   free(pT);
   *api = NULL;
   return TORNADOLIB_OK;
}

int GetTornadoError(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;
   return pT->m_nLastError;
}

void ClearTornadoError(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;
   pT->m_nLastError = TORNADOLIB_OK;
}

//TornadoLib related functions
int GetDeckCard(TornadoLib api, Card* pCard)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   if( GetNumberOfCards(pT->m_Deck) <= 0 ) {//Should never be empty!  But it is so return error
      return TORNADOLIB_DECK_EMTPY;
   }

   int nRes = GetCard(pT->m_Deck, pCard, 0);
   if( nRes != CARDLIB_OK ) {
      pT->m_nLastError = TORNADOLIB_CARD_FAILURE;
      return TORNADOLIB_CARD_FAILURE;
   }

   return TORNADOLIB_OK;
}

//I could have added this to the .h; but I thought it woudld be confusing so I added GetColumnString instead
int GetTornadoValue(Card c, int* pValue, int* pAces, int* pJokers)
{
   DEBUG_FUNC_NAME;

   int nValue = GetValue(c);
   int nSuit = GetSuit(c);//For Jokers

   int nCardValue = 0;
   int nNumAces = 0;
   int nNumJokers = 0;
   if( nValue == JACK || nValue == QUEEN || nValue == KING ) {
      nCardValue = TORNADO_FACE_CARD_VALUE;
   }
   else if( nValue == ACE ) {
      nCardValue = 1;//Or 11; but I'll set nNumAces
      nNumAces = 1;
   }
   else if( nSuit == JOKER ) {
      nNumJokers = 1;
   }
   else {
      nCardValue = nValue;
   }
   
   if( pValue != NULL )
      *pValue = nCardValue;
   if( pAces != NULL )
      *pAces = nNumAces;
   if( pJokers != NULL )
      *pJokers = nNumJokers;

   return TORNADOLIB_OK;
}

int GetColumnCount(TornadoLib api, int nIndex, int* pnNumAces)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   //Count them up
   int nCardsOnColumn = GetNumberCardsOnColumn(api, nIndex);
   int nSum = 0;
   int n;
   int nNumAces = 0, nNumJokers = 0;
   for(n=0; n<nCardsOnColumn; n++) {
      Card cColumn;
      GetCardOnColumn(api, nIndex, n, &cColumn);
      int nValue = 0, nAnyAces = 0, nAnyJokers = 0;
      GetTornadoValue(cColumn, &nValue, &nAnyAces, &nAnyJokers);
      nSum += nValue;
      nNumAces += nAnyAces;
      nNumJokers += nAnyJokers;

      if( nAnyJokers > 0 ) {
         if( nSum < 11 ) {
            nSum = 11;//If there is a joker and sum is less than 11; sum is now 11.
         }
         else {
            nSum = TORNADO_COLUMN_TARGET_VALUE;
            break;
         }
      }
   }

   if( pnNumAces != NULL )
      *pnNumAces = nNumAces;

   return nSum;
}

int CanPlaceDeckCardOnColumn(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   if( nIndex < 0 || nIndex >= TORNADO_NUMBER_OF_COLUMNS ) {
      pT->m_nLastError = TORNADOLIB_BADARGUMENT;
      return TORNADOLIB_BADARGUMENT;
   }

   //Get deckcard
   Card c;
   int nRes = GetDeckCard(api, &c);
   if( TORNADOLIB_OK != nRes ) {
      pT->m_nLastError = nRes;
      return nRes;
   }

   int nCardValue = 0, nAces = 0, nJokers = 0;
   nRes = GetTornadoValue(c, &nCardValue, &nAces, &nJokers);
   if( TORNADOLIB_OK != nRes ) {
      pT->m_nLastError = nRes;
      return nRes;
   }

   //Jokers always work
   if( nJokers > 0 ) {
      return TORNADO_CAN_PLACE_ON_COLUMN;
   }

   //Count column
   int nSum = GetColumnCount(api, nIndex, NULL);

   if( nSum + nCardValue > TORNADO_COLUMN_TARGET_VALUE ) {
      return TORNADO_CANNOT_PLACE_ON_COLUMN;
   }

   //I shouldn't need to check for 5 cards on a column that adds up less than 21 because that should have been cleared out!

   return TORNADO_CAN_PLACE_ON_COLUMN;
}

int AddBonus(TornadoLib api, int nBonus)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   pT->m_nLastBonus = nBonus;
   switch(nBonus) {
      case TORNADO_BONUS_21LARGE:
         pT->m_nBonus += 65;
         break;
      case TORNADO_BONUS_777:
         pT->m_nBonus += 65;
         break;
      case TORNADO_BONUS_21STRAIGHT:
         pT->m_nBonus += 30;
         break;
      case TORNADO_BONUS_BIGSTRAIGHT:
         pT->m_nBonus += 30;
         break;
      case TORNADO_BONUS_LITTLESTRAIGHT:
         pT->m_nBonus += 65;
         break;
      case TORNADO_BONUS_911EMERGENCY:
         pT->m_nBonus += 65;
      break;
      case TORNADO_BONUS_DOUBLE_JOKER:
         pT->m_nBonus += 350;
         break;
      case TORNADO_BONUS_5_OF_A_KIND:
         pT->m_nBonus += 65;
         break;
      case TORNADO_BONUS_CLEARED_BOARD:
         pT->m_nBonus += 65;
         break;
      case TORNADO_BONUS_LESSER_STRAIGHT:
         pT->m_nBonus += 30;
      case TORNADO_BONUS_GREATER_STRAIGHT:
         pT->m_nBonus += 65;
      case TORNADO_BONUS_CLEARED_DECK:
         pT->m_nBonus += 30;
         break;
   }

   if( nBonus != TORNADO_BONUS_CLEARED_DECK ) {
      int nGoldCardsToMake = 1;
      if( nBonus == TORNADO_BONUS_CLEARED_BOARD || nBonus == TORNADO_BONUS_DOUBLE_JOKER ) {
         nGoldCardsToMake = 3;
      }
      if( nBonus == TORNADO_BONUS_LESSER_STRAIGHT || nBonus == TORNADO_BONUS_GREATER_STRAIGHT ) {
         //Specific bonus!

         Card cAce;
         CreateCard(&cAce, SPADES, ACE);
         SetCardExtraData(cAce, (void*)GOLD_CARD_EXTRA_DATA);
         AddCardToBeginning(pT->m_Deck, cAce);

         int n;
         for(n=2; n<=5; n++) {
            Card c;
            CreateCard(&c, SPADES, n);
            SetCardExtraData(c, (void*)GOLD_CARD_EXTRA_DATA);
            AddCardToBeginning(pT->m_Deck, c);
         }

         return TORNADOLIB_OK;
      }

      int i;
      for(i=0; i<nGoldCardsToMake; i++) {
         //Need to make a card a gold card
         int nNumberOfCards = GetNumberOfCards(pT->m_Deck);
         int nCardIndex = rand() % nNumberOfCards;
         int n;
         for(n=nCardIndex; n<(nCardIndex+nNumberOfCards); n++) {
            int nIndex = nCardIndex % nNumberOfCards;//Wrap around
            Card c;
            GetCard(pT->m_Deck, &c, nIndex);
            if( GetCardExtraData(c) == NULL ) {
               SetCardExtraData(c, (void*)GOLD_CARD_EXTRA_DATA);
               break;
            }
         }
      }
   }

   return TORNADOLIB_OK;
}

int AddJoker(TornadoLib api, int nJoker)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   Card c;
   CreateCard(&c, JOKER, 0);
   AddCardToBeginning(pT->m_Deck, c);

   return TORNADOLIB_OK;
}

int GetNumberGoldCards(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   int nCount = 0;
   int n;
   for(n=0; n<GetNumberCardsOnColumn(api, nIndex); n++) {
      Card c;
      GetCardOnColumn(api, nIndex, n, &c);

      if( GetCardExtraData(c) != NULL )
         nCount++;
   }

   return nCount;
}

int FindBonuses(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   int nClearBonus = 0;

#ifdef TORNADO_WATCH_COLUMNS
   int n;
   for(n=(TORNADO_NUMBER_COMPLETED_COLUMN_WATCHING-1-1/*0-based index*/); n>=0; n--) {
      pT->m_nLastCompletedColumns[n+1] = pT->m_nLastCompletedColumns[n];
   }
   pT->m_nLastCompletedColumns[0] = nIndex;
#endif

   int nCardsOnColumn = GetNumberCardsOnColumn(api, nIndex);
   int nSum = GetColumnCount(api, nIndex, NULL);

   if( nSum == TORNADO_COLUMN_TARGET_VALUE && nCardsOnColumn == 5 ) {
      //Known as 21 Large
      AddBonus(api, TORNADO_BONUS_21LARGE);
      nClearBonus = 1;
   }
   //Test for 7-7-7 (65)
   if( nCardsOnColumn == 3 && nSum == TORNADO_COLUMN_TARGET_VALUE ) {
      Card c1, c2, c3;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      GetCardOnColumn(api, nIndex, 2, &c3);
      if( GetValue(c1) == GetValue(c2) && GetValue(c2) == GetValue(c3) && GetValue(c3) == 7 ) {
         AddBonus(api, TORNADO_BONUS_777);
         nClearBonus = 1;
      }
   }

   //Test for 21 Straight: 6-7-8 or 8-7-6 (30)
   if( nCardsOnColumn == 3 && nSum == TORNADO_COLUMN_TARGET_VALUE ) {
      Card c1, c2, c3;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      GetCardOnColumn(api, nIndex, 2, &c3);
      if( (GetValue(c1) == 6 && GetValue(c2) == 7 && GetValue(c3) == 8) ||
          (GetValue(c1) == 8 && GetValue(c2) == 7 && GetValue(c3) == 6) ) {
         AddBonus(api, TORNADO_BONUS_21STRAIGHT);
         nClearBonus = 1;
      }
   }

   //Test for Big Straight: Q-K-A (30)
   if( nCardsOnColumn == 3 && nSum == TORNADO_COLUMN_TARGET_VALUE ) {
      Card c1, c2, c3;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      GetCardOnColumn(api, nIndex, 2, &c3);
      if( GetValue(c1) == QUEEN && GetValue(c2) == KING && GetValue(c3) == ACE ) {
         AddBonus(api, TORNADO_BONUS_BIGSTRAIGHT);
         nClearBonus = 1;
      }
   }

   //Test for Little Straight A-2-3-4-5 or 2-3-4-5-6 in any order (65)
   if( nCardsOnColumn == 5 ) {
      Card c1, c2, c3, c4, c5;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      GetCardOnColumn(api, nIndex, 2, &c3);
      GetCardOnColumn(api, nIndex, 3, &c4);
      GetCardOnColumn(api, nIndex, 4, &c5);
      int nHas2 = (GetValue(c1) == 2 || GetValue(c2) == 2 || GetValue(c3) == 2 || GetValue(c4) == 2 || GetValue(c5) == 2) ? 1 : 0;
      int nHas3 = (GetValue(c1) == 3 || GetValue(c2) == 3 || GetValue(c3) == 3 || GetValue(c4) == 3 || GetValue(c5) == 3) ? 1 : 0;
      int nHas4 = (GetValue(c1) == 4 || GetValue(c2) == 4 || GetValue(c3) == 4 || GetValue(c4) == 4 || GetValue(c5) == 4) ? 1 : 0;
      int nHas5 = (GetValue(c1) == 5 || GetValue(c2) == 5 || GetValue(c3) == 5 || GetValue(c4) == 5 || GetValue(c5) == 5) ? 1 : 0;
      if( nHas2 && nHas3 && nHas4 && nHas5 ) {
         int nHasAce = (GetValue(c1) == ACE || GetValue(c2) == ACE || GetValue(c3) == ACE || GetValue(c4) == ACE || GetValue(c5) == ACE) ? 1 : 0;
         int nHas6 = (GetValue(c1) == 6 || GetValue(c2) == 6 || GetValue(c3) == 6 || GetValue(c4) == 6 || GetValue(c5) == 6) ? 1 : 0;
         if( nHasAce || nHas6 ) {
            AddBonus(api, TORNADO_BONUS_LITTLESTRAIGHT);
            nClearBonus = 1;
         }
      }
   }

   //Test for 911 Emergency (65)
   if( nCardsOnColumn == 3 ) {
      Card c1, c2, c3;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      GetCardOnColumn(api, nIndex, 2, &c3);
      if( GetValue(c1) == 9 && GetValue(c2) == ACE && GetValue(c3) == ACE ) {
         AddBonus(api, TORNADO_BONUS_911EMERGENCY);
         nClearBonus = 1;
      }
   }

   //Test for Double Jokers 2 Jokers (only) (350)
   if( nCardsOnColumn == 2 ) {
      Card c1, c2;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      if( GetSuit(c1) == JOKER && GetSuit(c2) == JOKER ) {
         AddBonus(api, TORNADO_BONUS_DOUBLE_JOKER);
         nClearBonus = 1;
      }
   }

   //Test for 5 of a Kind Five A's, 2's, 3's, 4's or 5's (65)
   if( nCardsOnColumn == 5 ) {
      Card c1, c2, c3, c4, c5;
      GetCardOnColumn(api, nIndex, 0, &c1);
      GetCardOnColumn(api, nIndex, 1, &c2);
      GetCardOnColumn(api, nIndex, 2, &c3);
      GetCardOnColumn(api, nIndex, 3, &c4);
      GetCardOnColumn(api, nIndex, 4, &c5);
      if( GetValue(c1) == GetValue(c2) && GetValue(c2) == GetValue(c3) && GetValue(c3) == GetValue(c4) && GetValue(c4) == GetValue(c5) ) {
         AddBonus(api, TORNADO_BONUS_5_OF_A_KIND);
         nClearBonus = 1;
      }
   }

   if( nClearBonus ) {
      int nNumGoldCards = GetNumberGoldCards(api, nIndex);
      if( nNumGoldCards >= TORNADO_GOLD_CARDS_NECESSARY_FOR_JOKER_CLEAR_BONUS ) {
         AddJoker(api, TORNADO_ADD_JOKER_CLEAR_GOLD);
      }
   }

#ifdef TORNADO_WATCH_COLUMNS
   //Test for Cleared Board; clear all 5 slots in only 5 turns (65)
   if( pT->m_nContinousCompletions >= 5 && pT->m_nLastCompletedColumns[0] >= 0 && pT->m_nLastCompletedColumns[1] >= 0 && pT->m_nLastCompletedColumns[2] >= 0 && pT->m_nLastCompletedColumns[3] >= 0 && pT->m_nLastCompletedColumns[4] >= 0 && ( pT->m_nLastCompletedColumns[0] +  pT->m_nLastCompletedColumns[1] +  pT->m_nLastCompletedColumns[2] +  pT->m_nLastCompletedColumns[3] +  pT->m_nLastCompletedColumns[4] ) == 10 ) {
      AddBonus(api, TORNADO_BONUS_CLEARED_BOARD);
   }
#endif

   return TORNADOLIB_OK;
}

int AddJokersAsNecessary(TornadoLib api, int nIndex, Card card)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   //Add in Jokers
#ifdef TORNADO_WATCH_COLUMNS
   //Clearing same column 4 times in a row
   if( pT->m_nLastCompletedColumns[0] ==  pT->m_nLastCompletedColumns[1] &&  pT->m_nLastCompletedColumns[1] ==  pT->m_nLastCompletedColumns[2] &&  pT->m_nLastCompletedColumns[2] ==  pT->m_nLastCompletedColumns[3] &&  pT->m_nLastCompletedColumns[0] >= 0 ) {
      AddJoker(api, TORNADO_ADD_JOKER_COLUMN4TIMES);
    }
#endif

   //Getting 3 gold cards in same column or playing a card on a column that already has 3 or more gold cards
   int nNumGoldCards = GetNumberGoldCards(api, nIndex);
   if( nNumGoldCards >= 3 ) {
      AddJoker(api, TORNADO_ADD_JOKER_3MORE_GOLDCARDS);
   }

   //Getting a clear bonus with 2 or more gold cards; I do this in the FindBonuses

   //Playing an Ace, 2, 3, or 4 of Spade when number of cards in deck % 13 == 1
   if( GetSuit(card) == SPADES && (GetValue(card) == ACE || GetValue(card) == 2 || GetValue(card) == 3 || GetValue(card) == 4) && GetNumberOfCards(pT->m_Deck)%13 == 1 ) {
      AddJoker(api, TORNADO_ADD_JOKER_DIVISIBLE);
   }

   return TORNADOLIB_OK;
}

int FindStraights(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

#ifdef TORNADO_CHECK_STRAIGHTS
   int nColumnCount[TORNADO_NUMBER_OF_COLUMNS];
   int n;
   for(n=0; n<TORNADO_NUMBER_OF_COLUMNS; n++) {
      nColumnCount[n] = GetColumnCount(api, n, NULL);
   }

   int nLesserStraight = 0;
   int nGreaterStraight = 0;

   //Test for Lesser Straight: slot totals: 16, 17, 18, 19, 20 in any order
   int n16 = 0, n17 = 0, n18 = 0, n19 = 0, n20 = 0;
   for(n=0; n<5; n++) {
      if( nColumnCount[n] == 16 )
         n16 = 1;
      if( nColumnCount[n] == 17 )
         n17 = 1;
      if( nColumnCount[n] == 18 )
         n18 = 1;
      if( nColumnCount[n] == 19 )
         n19 = 1;
      if( nColumnCount[n] == 20 )
         n20 = 1;
   }
   if( n16 && n17 && n18 && n19 && n20 ) {
      nLesserStraight = 1;
   }

   //Test for Greater Straight: slot totals in order 16, 17, 18, 19, 20
   if( nColumnCount[0] == 16 && nColumnCount[1] == 17 && nColumnCount[2] == 18 && nColumnCount[3] == 19 && nColumnCount[4] == 20 ) {
      nGreaterStraight = 1;
   }

   if( nGreaterStraight ) {
      AddBonus(api, TORNADO_BONUS_GREATER_STRAIGHT);
   }
   else if( nLesserStraight ) {
      AddBonus(api, TORNADO_BONUS_LESSER_STRAIGHT);
   }

#endif

   return TORNADOLIB_OK;
}

int PlaceDeckCardOnColumn(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   if( nIndex < 0 || nIndex >= TORNADO_NUMBER_OF_COLUMNS ) {
      pT->m_nLastError = TORNADOLIB_BADARGUMENT;
      return TORNADOLIB_BADARGUMENT;
   }

   //Check if can work!
   if( TORNADO_CANNOT_PLACE_ON_COLUMN == CanPlaceDeckCardOnColumn(api, nIndex) ) {
      pT->m_nLastError = TORNADOLIB_CANNOT_ADD_TO_COLUMN;
      return TORNADOLIB_CANNOT_ADD_TO_COLUMN;
   }

   //Add card
   Card card;
   int nRes = TakeNextCard(pT->m_Deck, &card);
   if( CARDLIB_OK != nRes ) {
      pT->m_nLastError = TORNADOLIB_CARD_FAILURE;
      return TORNADOLIB_CARD_FAILURE;
   }

   AddCard(pT->m_Column[nIndex].m_SetOfCards, card);
   AddJokersAsNecessary(api, nIndex, card);//Should be fine to send card; because it wasn't destroyed

   //Check if completes column
   int nCompleted = 0;
   int nCardsOnColumn = GetNumberCardsOnColumn(api, nIndex);
   if( nCardsOnColumn >= TORNADO_CARDS_ON_COLUMN_LIMIT ) {
      printf("COMPLETED: There are 5 or more cards\n");
      nCompleted = 1;
   }

   int nSum = 0;
   if( !nCompleted ) {
      //Count them up
      int nNumAces = 0;
      nSum = GetColumnCount(api, nIndex, &nNumAces);

      if( nSum == TORNADO_COLUMN_TARGET_VALUE ) {
         printf("COMPLETED: Adds up to exactly 21\n");
         nCompleted = 1;
      }
      else if( nSum == (TORNADO_COLUMN_TARGET_VALUE - 10) && nNumAces > 0 ) {
         printf("COMPLETED: Using an Ace as 11 and so sum is 21\n");
         nCompleted = 1;
      }
   }

   //Get new deck card; I do this before bonus; because bonus makes cards gold cards.
   if( IsCardsEmpty(pT->m_Deck) == CARDLIB_DECK_EMPTY ) {
      AddBonus(api, TORNADO_BONUS_CLEARED_DECK);
      //Deck cleared bonus
      AddStandardCards(pT->m_Deck, TORNADO_NUMBER_JOKERS);
      Shuffle(pT->m_Deck);
   }

   FindStraights(api);

   if( nCompleted ) {
#ifdef TORNADO_WATCH_COLUMNS
      pT->m_nContinousCompletions++;
#endif

      //Update score
      int nPoints = 21;

      if( nCardsOnColumn == 2 ) {
         nPoints += 10;//Extra 10 points for completing in 2 cards
      }

      int nNumGoldCards = GetNumberGoldCards(api, nIndex);
      int n;
      for(n=0; n<nNumGoldCards; n++) {
         nPoints *= 2;//Doubles as I understand for each gold card; so 2 gold cards means 4x
      }

      pT->m_nScore += nPoints;

      FindBonuses(api, nIndex);

      struct TornadoHistory* pHistory = malloc(sizeof(struct TornadoHistory));

      pHistory->m_pNext = pT->m_pHistoryRoot;
      pT->m_pHistoryRoot = pHistory;
      int nRes = CardLibCreate(&pT->m_pHistoryRoot->m_Cards);

      for(n=0; n<nCardsOnColumn; n++) {
         Card c;
         TakeNextCard(pT->m_Column[nIndex].m_SetOfCards, &c);//ownership of c
         AddCard(pHistory->m_Cards, c);//m_MostRecentCards has ownership of c
      }
      pHistory->m_nPoints = nPoints;

      RemoveAllCards(pT->m_Column[nIndex].m_SetOfCards, CARDLIB_REMOVE_CARD_ONLY);//Though should be empty as I moved them to history
   }
   else {
#ifdef TORNADO_WATCH_COLUMNS
      pT->m_nContinousCompletions = 0;
#endif
   }

   return nCompleted == 1 ? TORNADOLIB_COMPLETED_COLUMN : TORNADOLIB_OK;
}

int IsTornadoGameOver(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   int n;
   for(n=0; n<TORNADO_NUMBER_OF_COLUMNS; n++) {
      if( TORNADO_CAN_PLACE_ON_COLUMN == CanPlaceDeckCardOnColumn(api, n) )
          return TORNADO_NOT_GAME_OVER;
   }
   
   return TORNADO_GAME_OVER;
}

int GetNumberCardsOnColumn(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   if( nIndex < 0 || nIndex >= TORNADO_NUMBER_OF_COLUMNS )
      return 0;

   return GetNumberOfCards(pT->m_Column[nIndex].m_SetOfCards);
}

int GetCardOnColumn(TornadoLib api, int nColumnIndex, int nCardIndex, Card* pCard)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   if( nColumnIndex < 0 || nColumnIndex >= TORNADO_NUMBER_OF_COLUMNS )
      return TORNADOLIB_BADARGUMENT;

   int nRes = GetCard(pT->m_Column[nColumnIndex].m_SetOfCards, pCard, nCardIndex);
   if( CARDLIB_OK != nRes ) {
      pT->m_nLastError = TORNADOLIB_CARD_FAILURE;
      return TORNADOLIB_CARD_FAILURE;
   }

   return TORNADOLIB_OK;
}

int GetColumnString(TornadoLib api, int nIndex, char* pstr)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   if( nIndex < 0 || nIndex >= TORNADO_NUMBER_OF_COLUMNS )
      return TORNADOLIB_BADARGUMENT;

   int nCardsOnColumn = GetNumberCardsOnColumn(api, nIndex);
   int n;
   int nSum = 0, nNumAces = 0, nNumJokers;
   for(n=0; n<nCardsOnColumn; n++) {
      Card cColumn;
      int nRes = GetCardOnColumn(api, nIndex, n, &cColumn);
      if( TORNADOLIB_OK != nRes ) {
         pT->m_nLastError = nRes;
         return nRes;
      }

      int nValue = 0, nAnyAces = 0, nAnyJokers = 0;
      nRes = GetTornadoValue(cColumn, &nValue, &nAnyAces, &nAnyJokers);
      if( TORNADOLIB_OK != nRes ) {
         pT->m_nLastError = nRes;
         return nRes;
      }

      nSum += nValue;
      nNumAces += nAnyAces;
      nNumJokers += nAnyJokers;
      if( nSum < 11 && nAnyJokers > 0 ) {
         nSum = 11;
      }
   }

   char buffer[16];
   T_itoa(nSum, buffer, 16);
   int nLen = strlen(buffer);

   if( nNumAces >= 1 && nSum < (TORNADO_COLUMN_TARGET_VALUE - 10) ) {
      char buf[8];
      T_itoa(nSum+10, buf, 8);
      strcpy(buffer+nLen, "/ ");
      nLen += 2;
      strcpy(buffer+nLen, buf);
   }

   strcpy(pstr, buffer);

   return TORNADOLIB_OK;
}

int GetTornadoScore(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   return pT->m_nScore;
}

int GetTornadoBonusScore(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   return pT->m_nBonus;
}

int GetLastTornadoBonus(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   return pT->m_nLastBonus;
}

int ClearLastTornadoBonus(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   pT->m_nLastBonus = TORNADO_NO_BONUS;

   return TORNADOLIB_OK;
}

//History related
int GetNumberOfHistoryItems(TornadoLib api)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   int nCount = 0;
   struct TornadoHistory* pCurrent = pT->m_pHistoryRoot;
   while(pCurrent != NULL) {
      pCurrent = pCurrent->m_pNext;
      nCount++;
   }
   return nCount;
}

struct TornadoHistory* GetHistory(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   int nCount = 0;
   struct TornadoHistory* pCurrent = pT->m_pHistoryRoot;
   while(pCurrent != NULL) {
      if( nCount == nIndex )
         return pCurrent;

      pCurrent = pCurrent->m_pNext;
      nCount++;
   }

   return NULL;
}

int GetNumberCardsOnHistory(TornadoLib api, int nIndex)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   struct TornadoHistory* pHistory = GetHistory(api, nIndex);
   if( pHistory == NULL )
      return 0;

   return GetNumberOfCards(pHistory->m_Cards);
}

int GetCardOnHistory(TornadoLib api, int nIndex, int nCardIndex, Card* pCard)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   struct TornadoHistory* pHistory = GetHistory(api, nIndex);

   int nRes = GetCard(pHistory->m_Cards, pCard, nCardIndex);
   return TORNADOLIB_OK;
}

int GetHistoryString(TornadoLib api, int nIndex, char* pstr)
{
   DEBUG_FUNC_NAME;

   struct Tornado* pT = (struct Tornado*)api;

   struct TornadoHistory* pHistory = GetHistory(api, nIndex);

   char buffer[16];
   T_itoa(pHistory->m_nPoints, buffer, 16);
   int nLen = strlen(buffer);

   strcpy(pstr, buffer);

   return TORNADOLIB_OK;
}

