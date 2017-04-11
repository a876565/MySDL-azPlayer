#pragma once
#include "azView.h"
class azLayout :
	public azPage
{
public:
	azLayout(azView*p) :azPage(p) {};

	~azLayout();
};
enum {
	LAYOUT_NONE,
	LAYOUT_VERTICAL,
	LAYOUT_HORIZONTAL,
};
enum {
	ALIGN_LEFT=1,
	ALIGN_RIGHT=2,
	ALIGN_TOP=4,
	ALIGN_BOTTOM=8
};
class azBoxLayout :
	public azLayout
{
public:
	//NONE VERTICAL HORIZONTAL
	int Dirction;

	//bit (bottom)(top)(right)(left)
	int Align;
	int padding, spacing;
	azBoxLayout(azView*p) :azLayout(p),Dirction(LAYOUT_VERTICAL),Align(0),
	padding(0),spacing(0){};
	void updateLayout();
	void getTotalRect(SDL_Rect&r);
	~azBoxLayout();
};

