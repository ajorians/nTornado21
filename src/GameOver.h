#ifndef GAMEOVER_H
#define GAMEOVER_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "HighScoreLib/HighScoreLib.h"
}

class TornadoGameOver
{
public:
	TornadoGameOver(SDL_Surface* pScreen, int nScore);
	~TornadoGameOver();

	bool PlayAgain() const;

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();

	void AddLetter(char ch);
	void Backspace();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	HighScoreLib	m_HighScore;
	nSDL_Font *m_pFont;
	nSDL_Font *m_pFontPlayAgain;
	int m_nScore;
	bool m_bNewHighScore;
	bool m_bPlayAgain;
	bool m_bShift;
	char m_strNameBuffer[16];
};

#endif
