#pragma once
#include "azView.h"
class azViewButton :
	public azView
{
public:
	azViewButton(azView*p) :azView(p) {};
	virtual int setText(std::string&str);
	virtual int setBackground(SDL_Color color, int flag);
	virtual int setForeground(SDL_Color color, int flag);
	virtual int setBackground(azTexture&tex, int flag);
	virtual int onEvent(SDL_Event&event) = 0;
	virtual ~azViewButton();
};

