#include "Column.h"
#include "CardImages.h"
#include "Defines.h"

Column::Column()
: m_pScreen(NULL), m_nX(0), m_nNumberCards(0), m_bEnabled(true), m_pCardImages(NULL)
{
        strcpy(m_str, "");
	m_pFont = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);
	for(int i=0; i<TORNADO_CARDS_ON_COLUMN_LIMIT; i++) {
		m_pPiece[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, CARD_WIDTH, CARD_HEIGHT, 16, 0, 0, 0, 0);
	}
}	

Column::~Column()
{
	nSDL_FreeFont(m_pFont);
	for(int i=0; i<TORNADO_CARDS_ON_COLUMN_LIMIT; i++) {
		SDL_FreeSurface(m_pPiece[i]);
	}
}

void Column::Setup(int nX, SDL_Surface* pScreen, CardImages* pCardImages)
{
	m_nX = nX;
	m_pScreen = pScreen;
	m_pCardImages = pCardImages;
}

void Column::SetText(char* pstr)
{
   strcpy(m_str, pstr);
}

void Column::AddCard(Card c)
{
   m_pCardImages->GetImageForCard(m_pPiece[m_nNumberCards], c);
   m_nNumberCards++;
}

void Column::ClearCards()
{
   m_nNumberCards = 0;
}

void Column::SetEnabled(bool bEnabled)
{
   m_bEnabled = bEnabled;
}

void Column::UpdateDisplay()
{
	SDL_Rect rectDest;
	rectDest.x = m_nX;
	rectDest.y = 0;
	rectDest.w = COLUMN_WIDTH;
	rectDest.h = COLUMN_HEIGHT;
	SDL_FillRect(m_pScreen, &rectDest, SDL_MapRGB(m_pScreen->format, COLUMN_BACKGROUND_R, COLUMN_BACKGROUND_G, COLUMN_BACKGROUND_B));

	for(int i=0; i<m_nNumberCards; i++) {
		int nSeparation = 2;
//This works; but I wasn't sure how user friendly it looks; so I'll leave it out.
#if 0
		if( m_nNumberCards == 4 )
			nSeparation = 3;
#endif
              
		int nY = i*CARD_HEIGHT/nSeparation;
		DrawPiece(m_pPiece[i], m_nX+1, nY);
	}

	if( m_nNumberCards > 0 ) {
		int nTextWidth = nSDL_GetStringWidth(m_pFont, m_str);
		int nX = m_nX + (CARD_WIDTH + 2 - nTextWidth)/2;
		nSDL_DrawString(m_pScreen, m_pFont, nX, COLUMN_HEIGHT-10/*Text height or so*/, m_str);
	}

	if( !m_bEnabled )
		Gray();
}

void Column::DrawPiece(SDL_Surface* pSurface, int nX, int nY)
{
	SDL_Rect rectDest;
	rectDest.x = nX;
	rectDest.y = nY;
	rectDest.w = CARD_WIDTH;
	rectDest.h = CARD_HEIGHT;

    SDL_BlitSurface(pSurface, NULL, m_pScreen, &rectDest);
}

#define DAMPEN_BLACK_AMOUNT	(50)

void Column::Gray()
{
   //If the surface must be locked 
   if( SDL_MUSTLOCK( m_pScreen ) )
   {
      //Lock the surface 
      SDL_LockSurface( m_pScreen );
   }

   //Go through columns 
   for( int x = m_nX; x < m_nX + COLUMN_WIDTH; x++ )
   {
      //Go through rows 
      for( int y = 0; y < COLUMN_HEIGHT; y++ )
      {
         Uint32 pixel = nSDL_GetPixel(m_pScreen, x, y);
         Uint8 r = 0, g = 0, b = 0;
         SDL_GetRGB(pixel, m_pScreen->format, &r, &g, &b);
	 if( r < DAMPEN_BLACK_AMOUNT && g < DAMPEN_BLACK_AMOUNT && b < DAMPEN_BLACK_AMOUNT ) {
		r += DAMPEN_BLACK_AMOUNT;
		g += DAMPEN_BLACK_AMOUNT;
		b += DAMPEN_BLACK_AMOUNT;
	 }
         r = g = b = ((r*61+g*174+b*21)/256);

         nSDL_SetPixel(m_pScreen, x, y, SDL_MapRGB(m_pScreen->format, r, g, b));
      }
   }

   //Unlock surface 
   if( SDL_MUSTLOCK( m_pScreen ) )
   {
      SDL_UnlockSurface( m_pScreen );
   }
}

