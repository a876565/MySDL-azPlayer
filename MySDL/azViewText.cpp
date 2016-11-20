#include "azViewText.h"


int azViewText::onEvent(SDL_Event & event)
{
	return 1;
}

int azViewText::setText()
{
	texttex = e->getDefaultFont()->RenderTexText(text, color, area.w);
	if(!texttex)
		return 1;
	texttex->getSize(&drawrect.w, &drawrect.h);
	if (drawrect.w > area.w)
		drawrect.w = area.w;
	if (drawrect.h > area.h)
		drawrect.h = area.h;
	return 0;
}

int azViewText::setText(const std::string & str)
{
	text = str;
	setText();
	return 0;
}

int azViewText::setText(const std::string & str,SDL_Color col)
{	
	text = str;
	color = col;
	setText();
	return 0;
}

void azViewText::draw()
{
	shape.draw(&drawrect);
	if (texttex)
	{
		texttex->draw(&drawrect);
	}
}


azViewText::~azViewText()
{
}
