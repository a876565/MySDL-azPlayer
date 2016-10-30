#include "azvImage.h"


azvImage::~azvImage()
{
}

void azvImage::draw()
{
	if (img)
	{
		img->draw2(&area, &srcrect, angle, &center, flip);
	}
}

int azvImage::setSize(int w, int h)
{
	if (img)
	{
		area.w = w;
		area.h = h;
	}
	else
	{
		area.w = area.h = 0;
	}
	return 0;
}

void azvImage::autosize()
{
	area.w = srcrect.w - srcrect.x;
	area.h = srcrect.h - srcrect.y;
}

int azvImage::onMoveIn(int x, int y)
{
	return 1;
}

int azvImage::onMoveOut(int x, int y)
{
	return 1;
}

int azvImage::onMoving(int x, int y, int dx, int dy)
{
	return 1;
}

int azvImage::onPress(int x, int y, Uint32 flag)
{
	return 1;
}

int azvImage::onRelease(int x, int y, Uint32 flag)
{
	return 1;
}
