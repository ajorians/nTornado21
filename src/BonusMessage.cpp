#include "BonusMessage.h"
#include "Defines.h"

#define BONUS_SPRITE_WIDTH	(20)
#define BONUS_SPRITE_HEIGHT	(20)
#define BONUS_LINE_SPACING	(10)
#define BONUS_STAY_DURATION	(40)
#define BONUS_SPRITE_SPACING	(10)
#define BONUS_STEP_AMOUNT	(7)

BonusMessage::BonusMessage(SDL_Surface* pScreen)
: m_pScreen(pScreen), m_pRoot(NULL), m_nBonusStayDuration(BONUS_STAY_DURATION)
{
   m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, BONUS_TEXT_R, BONUS_TEXT_G, BONUS_TEXT_B);
}

BonusMessage::~BonusMessage()
{
   ClearMessage();
   nSDL_FreeFont(m_pFont);
}

void BonusMessage::ClearMessage()
{
   LetterSprite* pLetter = m_pRoot;
   while(pLetter != NULL) {
      LetterSprite* pTemp = pLetter;
      SDL_FreeSurface(pLetter->img);
      SDL_FreeSurface(pLetter->replace);
      pLetter = pLetter->next;
      free(pTemp);
   }
   m_pRoot = NULL;
}

int GetLettersInLine(const char* pstrMessage)
{
   int nCount = 0;
   const char* pstr = pstrMessage;
   while(*pstr != '\0') {
      if( *pstr == '\n' )
         return nCount;
      pstr = pstr+1;
      nCount++;
   }
   return nCount;
}

int GetStartXForBonusMessage(int nLettersInLine)
{
   int nLengthForLine = (BONUS_SPRITE_WIDTH + BONUS_SPRITE_SPACING)*nLettersInLine;   

   nLengthForLine -= BONUS_SPRITE_SPACING;//So flush with last letter!

   int nX = (SCREEN_WIDTH - nLengthForLine)/2;
   return nX;
}

void BonusMessage::CreateMessage(const char* pstrMessage)
{
   ClearMessage();

   int nNumLines = 1;
   for(int n=0; n<strlen(pstrMessage); n++){
      if( *(pstrMessage+n) == '\n' )
         nNumLines++;
   }

   int nLettersInLine = GetLettersInLine(pstrMessage);
   int nX = GetStartXForBonusMessage(nLettersInLine);
   int nY = (SCREEN_HEIGHT - (nNumLines*(BONUS_SPRITE_HEIGHT+BONUS_LINE_SPACING)))/2;
   int nLettersIntoLine = 0;

   LetterSprite* pPrevious = NULL;
   for(int n=0; n<strlen(pstrMessage); n++){
      LetterSprite* pSprite = (LetterSprite*)malloc(sizeof(LetterSprite));

      pSprite->img = SDL_CreateRGBSurface(SDL_SWSURFACE, BONUS_SPRITE_WIDTH, BONUS_SPRITE_HEIGHT, 16, 0, 0, 0, 0);

      SDL_FillRect(pSprite->img, NULL, SDL_MapRGB(m_pScreen->format, BONUS_BACKGROUND_R, BONUS_BACKGROUND_G, BONUS_BACKGROUND_B));

      char ch = *(pstrMessage+n);
      if( ch == '\n' ) {
         nLettersInLine = GetLettersInLine(pstrMessage+n+1/*Get past newline*/);
         nX = GetStartXForBonusMessage(nLettersInLine);
         nLettersIntoLine = 0;
         nY += (BONUS_SPRITE_HEIGHT+BONUS_LINE_SPACING);
         continue;
      }
      if( ch == ' ' ) {
         nLettersIntoLine++;
         continue;
      }

      nSDL_DrawString(pSprite->img, m_pFont, BONUS_SPRITE_WIDTH/2-5, BONUS_SPRITE_HEIGHT/2-5, "%c", ch);
      pSprite->replace = SDL_CreateRGBSurface(SDL_SWSURFACE, BONUS_SPRITE_WIDTH, BONUS_SPRITE_HEIGHT, 16, 0, 0, 0, 0);

      pSprite->ch = ch;

      pSprite->x = rand() % 320;//I do random start position; I suppose it is alright :)
      pSprite->y = rand() % 240;

      int nDesiredX = nX + nLettersIntoLine*(BONUS_SPRITE_WIDTH + BONUS_SPRITE_SPACING);
      pSprite->toX = nDesiredX;
      pSprite->toY = nY;

      nLettersIntoLine++;

      pSprite->next = pPrevious;
      pPrevious = pSprite;

      m_pRoot = pSprite;
   }

   m_nBonusStayDuration = BONUS_STAY_DURATION;
}

bool IsLetterMoving(LetterSprite* pSprite)
{
   return (pSprite->y != pSprite->toY) || (pSprite->x != pSprite->toX);
}

bool BonusMessage::IsAnimating() const
{
   LetterSprite* pCurrent = NULL;
   for(pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
      if( IsLetterMoving(pCurrent) )
         return true;
   }

   return false;
}

bool BonusMessage::CheckStayDuration()
{
   //This is how I keep the letters still to be readable
   if( m_nBonusStayDuration > 0 ) {
      m_nBonusStayDuration--;
      //If been there long enough have them drop
      if( m_nBonusStayDuration <= 0 ) {
         for(LetterSprite* pCurrent = m_pRoot; pCurrent != NULL; pCurrent = pCurrent->next) {
            pCurrent->toY = SCREEN_HEIGHT;
         }
      }
      return true;
   }

   return false;
}

void SetBackground(SDL_Surface* pScreen, LetterSprite* pSprite)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = BONUS_SPRITE_WIDTH;
   rect.h = BONUS_SPRITE_HEIGHT;

   SDL_BlitSurface(pScreen, &rect, pSprite->replace, NULL);
}

void MoveLetter(SDL_Surface* pScreen, LetterSprite* pSprite)
{
   if( (pSprite->x != pSprite->toX) || (pSprite->y != pSprite->toY) ) {
      int nDistX = pSprite->toX - pSprite->x;
      int nDistY = pSprite->toY - pSprite->y;

      for(int n=0; n<BONUS_STEP_AMOUNT; n++) {
         if( pSprite->x < pSprite->toX ) {
            pSprite->x++;
         }
         if( pSprite->x > pSprite->toX ) {
            pSprite->x--;
         }
      }

      for(int n=0; n<BONUS_STEP_AMOUNT; n++) {
         if( pSprite->y < pSprite->toY ) {
            pSprite->y++;
         }
         if( pSprite->y > pSprite->toY ) {
            pSprite->y--;
         }
      }
   }

   SetBackground(pScreen, pSprite);
}

void ShowLetter(SDL_Surface* pScreen, LetterSprite* pSprite)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = BONUS_SPRITE_WIDTH;
   rect.h = BONUS_SPRITE_HEIGHT;

   //SDL_SetColorKey(pSprite->img, SDL_SRCCOLORKEY, SDL_MapRGB(pSprite->img->format, 255, 255, 255);
   SDL_BlitSurface(pSprite->img, NULL, pScreen, &rect);
}

void ResetBackground(SDL_Surface* pScreen, LetterSprite* pSprite)
{
   SDL_Rect rect;
   rect.x = pSprite->x;
   rect.y = pSprite->y;
   rect.w = BONUS_SPRITE_WIDTH;
   rect.h = BONUS_SPRITE_HEIGHT;

   //SDL_SetColorKey(pSprite->img, SDL_SRCCOLORKEY, SDL_MapRGB(pSprite->img->format, 255, 255, 255);
   SDL_BlitSurface(pSprite->replace, NULL, pScreen, &rect);
}

bool BonusMessage::Animate()
{
   if( !IsAnimating() ) {
      if( !CheckStayDuration() )
         return false;
   }

   LetterSprite* pCurrent = m_pRoot;

   while(pCurrent != NULL) {
      MoveLetter(m_pScreen, pCurrent);
      pCurrent = pCurrent->next;
   }

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ShowLetter(m_pScreen, pCurrent);
      pCurrent = pCurrent->next;
   }
   SDL_UpdateRect(m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   pCurrent = m_pRoot;
   while(pCurrent != NULL) {
      ResetBackground(m_pScreen, pCurrent);
      pCurrent = pCurrent->next;
   }

   return true;
}



