#pragma once
#include "azView.h"

class azvSlideBar :
	public azView
{
protected:
	int state;
public:
	azvSlideBar(azView*p) :azView(p),
		moveable(true), state(0),
		slider_pos(0.0),line_size(15),
		 slider_height(30),slider_width(20),
		line_color(COLOR( 63,63,63,63) ), slider_color(COLOR(125, 240, 240, 128))
	{}
	bool moveable;

	SDL_Color line_color;
	int line_size;

	SDL_Color slider_color;
	int slider_width;
	int slider_height;
	double slider_pos;


	std::function<void(double)> onSliderMove;

	void setSliderPos(int x){
		slider_pos = (double)(x - area.x) / (area.w - slider_width);
		if (slider_pos < 0.0)
			slider_pos = 0.0;
		else if (slider_pos>1.0)
			slider_pos = 1.0;
		if(onSliderMove)
			onSliderMove(slider_pos);
	}
	virtual void draw() override;
	virtual int onPress(int x, int y, Uint32 flag) override;
	virtual int onRelease(int x, int y, Uint32 flag) override;
	virtual int onMoveOut(int x, int y);
	virtual int onMoving(int x, int y, int dx, int dy);

	virtual ~azvSlideBar();
};
class azvSongSlide:public azvSlideBar
{
public:
	azvSongSlide(azView*p) :azvSlideBar(p) {}
	virtual ~azvSongSlide();
	virtual void draw() override;
	virtual int onPress(int x, int y, Uint32 flag) override;

private:

};

