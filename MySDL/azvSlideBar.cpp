#include "azvSlideBar.h"



void azvSlideBar::draw()
{
	SDL_Rect r;

	r.x = area.x;
	r.y = area.y+(area.h-line_size)/2;
	r.w = area.w;
	r.h = line_size;
	e->SetColor(line_color);
	e->FillRect(&r);

	r.x = area.x + (area.w - slider_width)*slider_pos;
	r.y = area.y + (area.h - slider_height) / 2;
	r.w = slider_width;
	r.h = slider_height;
	e->SetColor(slider_color);
	e->FillRect(&r);

}

int azvSlideBar::onPress(int x, int y, Uint32 flag)
{
	state = 1;
	setSliderPos(x);
	return 0;
}

int azvSlideBar::onRelease(int x, int y, Uint32 flag)
{
	state = 0;
	return 0;
}

int azvSlideBar::onMoveOut(int x, int y)
{
	state = 0;
	/*
	if (x < area.x)
		slider_pos = 0.0;
	if (x > area.x + area.w)
		slider_pos = 1.0;
	*/
	return 0;
}

int azvSlideBar::onMoving(int x, int y, int dx, int dy)
{
	if (state)
	{
		setSliderPos(x);
	}
	return 0;
}

azvSlideBar::~azvSlideBar()
{
}


azvSongSlide::~azvSongSlide()
{
}

void azvSongSlide::draw()
{
	SDL_Rect r;

	r.x = area.x;
	r.y = area.y + (area.h - line_size) / 2;
	r.w = area.w;
	r.h = line_size;
	e->SetColor(line_color);
	e->FillRect(&r);

	r.x = area.x;
	r.y = area.y + (area.h - slider_height) / 2;
	r.w = area.w*slider_pos;
	r.h = slider_height;
	e->SetColor(slider_color);
	e->FillRect(&r);

}

int azvSongSlide::onPress(int x, int y, Uint32 flag)
{
	return 0;
}
