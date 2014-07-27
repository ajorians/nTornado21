#ifndef HIGHSCORETABLE_H
#define HIGHSCORETABLE_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "HighScoreLib/HighScoreLib.h"
}

class HighScoreTable
{
public:
	HighScoreTable(SDL_Surface* pScreen);
	~HighScoreTable();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface	*m_pBackground;
	HighScoreLib	m_HighScore;
	nSDL_Font *m_pFontScore;
	nSDL_Font *m_pFontName;
};

#endif
