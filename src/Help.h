#ifndef HELP_H
#define HELP_H

extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
}

class TornadoHelp
{
public:
	TornadoHelp(SDL_Surface* pScreen);
	~TornadoHelp();

	bool Loop();
	
protected:
	bool PollEvents();
	void UpdateDisplay();

protected:
	SDL_Surface	*m_pScreen;//Does not own
	SDL_Surface	*m_pBackground;
};

#endif
