#ifndef MENU_H
#define MENU_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

class MainMenu
{
public:
	MainMenu(SDL_Surface* pScreen);
	~MainMenu();

	bool Loop();

	bool ShouldQuit() const;
	bool ShowShowHighScores() const;
	bool ShouldShowHelp() const;
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface *m_pBackground;
	nSDL_Font *m_pFont;
	enum MenuChoice{Play, HighScore, Help, Quit};
	MenuChoice m_eChoice;
};

#endif
