#pragma once
#include "azView.h"
#include "azText.h"
#include <functional>
class azvButtonBase :
	public azView
{
protected:
	const static int MAX_PAST_TICK = 500;
	int past_tick;
	int state,prev_state;
public:
	std::function<void(azvButtonBase&)>onClick;
	
	azvButtonBase(azView*p) :azView(p) {};
	~azvButtonBase();

	void setState(int s) { prev_state = state; state = s; past_tick = 500; }

	// ͨ�� azView �̳�
	virtual int onMoveIn(int x, int y) override;
	virtual int onMoveOut(int x, int y) override;
	virtual int onMoving(int x, int y, int dx, int dy) override;
	virtual int onPress(int x, int y, Uint32 flag) override;
	virtual int onRelease(int x, int y, Uint32 flag) override;
};

class azvButton :
	public azvButtonBase
{
public:
	int padding;
	azText text;
	SDL_Color backs[3];
	azvButton(azView*p) :azvButtonBase(p), text(e),padding(4) {
		backs[0] = COLOR(0x6f, 0x7c, 0xff, 0x63);
		backs[1] = COLOR(0x77, 0x77, 0xff, 0xbf);
		backs[2] = COLOR(0x66, 0x00, 0xff, 0xff);
	};
	virtual void draw() override;
	virtual int setSize(int w, int h) override;
	~azvButton();
};

