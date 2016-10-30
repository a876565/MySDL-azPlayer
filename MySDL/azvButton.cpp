#include "azvButton.h"



azvButton::~azvButton()
{
}

int azvButton::onMoveIn(int x, int y)
{
	setState(1);
	return 1;
}

int azvButton::onMoveOut(int x, int y)
{
	setState(0);
	return 1;
}

int azvButton::onMoving(int x, int y, int dx, int dy)
{
	return 1;
}

int azvButton::onPress(int x, int y, Uint32 flag)
{
	setState(2);
	return 1;
}

int azvButton::onRelease(int x, int y, Uint32 flag)
{
	setState(1);
	if (onClick)
		onClick(*this);
	return 1;
}

void azvTextButton::draw()
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

azvTextButton::~azvTextButton()
{
}
