#pragma once
#include "azView.h"
class azViewText :
	public azView
{
	Uint32 style;
	std::string text;
	azTexture texttex;
	SDL_Rect drawrect;
public:
	SDL_Color color;

	azViewText(azView*p) :azView(p), style(0) { RECT_INIT(drawrect); }

	int onEvent(SDL_Event&event) override;
	int setText();
	int setText(const std::string&str);
	int setText(const std::string&str,SDL_Color color);
	void draw() override;
	virtual ~azViewText();
};

