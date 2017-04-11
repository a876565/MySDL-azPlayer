#include "azvLabel.h"




azvLabel::~azvLabel()
{
}


void azvLabel::draw()
{
	back.draw(&area);
	text.draw(&area);
}

int azvLabel::onMoveIn(int x, int y)
{
	return 1;
}

int azvLabel::onMoveOut(int x, int y)
{
	return 1;
}

int azvLabel::onMoving(int x, int y, int dx, int dy)
{
	return 1;
}

int azvLabel::onPress(int x, int y, Uint32 flag)
{
	return 1;
}

int azvLabel::onRelease(int x, int y, Uint32 flag)
{
	return 1;
}

int azvLabel::setSize(int w, int h)
{
	area.w = w;
	area.h = h;
	text.setSize(w, h);
	return 0;
}
