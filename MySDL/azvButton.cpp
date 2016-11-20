#include "azvButton.h"



azvButtonBase::~azvButtonBase()
{
}

int azvButtonBase::onMoveIn(int x, int y)
{
	setState(1);
	return 1;
}

int azvButtonBase::onMoveOut(int x, int y)
{
	setState(0);
	return 1;
}

int azvButtonBase::onMoving(int x, int y, int dx, int dy)
{
	return 1;
}

int azvButtonBase::onPress(int x, int y, Uint32 flag)
{
	setState(2);
	return 1;
}

int azvButtonBase::onRelease(int x, int y, Uint32 flag)
{
	setState(1);
	if (onClick)
		onClick(*this);
	return 1;
}

void azvButton::draw()
{
	if (past_tick > 0)
	{
		SDL_Color c;
		c = backs[prev_state];
		c.a = c.a*past_tick / MAX_PAST_TICK;
		e->SetColor(c);
		e->FillRect(&area);

		c = backs[state];
		c.a = c.a*(MAX_PAST_TICK - past_tick) / MAX_PAST_TICK;
		e->SetColor(c);
		e->FillRect(&area);

		past_tick -= e->PastTick();
	}
	else
	{
		e->SetColor(backs[state]);
		e->FillRect(&area);
	}
	SDL_Rect r = area;
	r.x += padding;
	r.y += padding;
	r.w -= padding + padding;
	r.h -= padding + padding;
	text.draw(&r);
}

azvButton::~azvButton()
{
}
