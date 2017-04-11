#pragma once
#include "azView.h"
#include "azText.h"
class azvLabel
	:public azView
{
public:
	azText text;
	azvLabel(azView*v) 
		:azView(v),text(e){};
	azvLabel(azView*v,int x,int y,int w=0,int h=0,const std::wstring&str=L"",SDL_Color col=COLOR_WHITE) 
		:azView(v), text(e){
		area.x = x;
		area.y = y;
		area.w = w;
		area.h = h;
		text.setText(str);
		text.setColor(col);
	};
	
	~azvLabel();

	virtual void draw() override;
	// Í¨¹ý azView ¼Ì³Ð
	virtual int onMoveIn(int x, int y) override;

	virtual int onMoveOut(int x, int y) override;

	virtual int onMoving(int x, int y, int dx, int dy) override;

	virtual int onPress(int x, int y, Uint32 flag) override;

	virtual int onRelease(int x, int y, Uint32 flag) override;

	virtual int setSize(int w, int h) override;
};
