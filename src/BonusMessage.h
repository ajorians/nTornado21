#ifndef BONUS_MESSAGE_H
#define BONUS_MESSAGE_H

extern "C"
{
        #include <os.h>
        #include "SDL/SDL.h"
}

struct LetterSprite
{
   SDL_Surface* img;
   SDL_Surface* replace;
   char ch;
   int x,y;
   int toX, toY;
   struct LetterSprite* next;
};

class BonusMessage
{
public:
   BonusMessage(SDL_Surface* pScreen);
   ~BonusMessage();

   void CreateMessage(const char* pstrMessage);
   bool IsAnimating() const;
   bool CheckStayDuration();
   bool Animate();

protected:
   void ClearMessage();

protected:
   SDL_Surface* m_pScreen;//Does NOT own
   LetterSprite* m_pRoot;
   nSDL_Font *m_pFont;
   int m_nBonusStayDuration;
};

#endif


