#pragma once
#include "azView.h"
class azvImage :
	public azView
{
	azTexture img;
public:
	SDL_Point center;
	SDL_Rect srcrect;
	double angle;
	SDL_Color colormod;
	SDL_RendererFlip flip;

	azvImage(azView*p) :azvImage(p,azTexture()) {};
	azvImage(azView*p, azTexture tex) :azView(p){
		setImage(tex);
	};

	void setImage(azTexture& tex)
	{
		img = tex;
		srcrect.x = srcrect.y = 0;
		if (tex)
		{
			srcrect.w = tex->width();
			srcrect.h = tex->height();
		}
		else
		{
			srcrect.w = srcrect.h = 0;
		}

		if (area.w == 0)
			area.w = srcrect.w;
		if (area.h == 0)
			area.h = srcrect.h;

		center.x = (srcrect.x + srcrect.w) / 2;
		center.y = (srcrect.y + srcrect.h) / 2;
		
		angle = 0.0;

		colormod = COLOR_WHITE;
	}
	void setScale(double s) {
		area.w =s* srcrect.w, area.h = s*srcrect.h;
	}
	~azvImage();
	virtual void draw() override;
	virtual int setSize(int w, int h) override;
	virtual void autosize() override;
	// Í¨¹ý azView ¼Ì³Ð
	virtual int onMoveIn(int x, int y) override;
	virtual int onMoveOut(int x, int y) override;
	virtual int onMoving(int x, int y, int dx, int dy) override;
	virtual int onPress(int x, int y, Uint32 flag) override;
	virtual int onRelease(int x, int y, Uint32 flag) override;
};

