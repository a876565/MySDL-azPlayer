#include "azView.h"
#include <iostream>
inline int isUIEvent(Uint32 type)
{
	switch (type)
	{
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:

	//case SDL_MOUSEWHEEL:

	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
	case SDL_FINGERMOTION:

		return 1;

	default:
		return 0;
	}
}
int azView::setSize(int w, int h)
{
	area.w = w;
	area.h = h;
	return 0;
}

int azView::setPos(int x, int y)
{
	area.x = x;
	area.y = y;
	return 0;
}

int azView::move(int dx, int dy)
{
	area.x += dx;
	area.y += dy;
	return 0;
}

void azView::autosize()
{
}

int azView::onMoveIn(int x, int y)
{
	return 1;
}

int azView::onMoveOut(int x, int y)
{
	return 1;
}



int azView::onMoving(int x, int y, int dx, int dy)
{
	return 1;
}

int azView::onPress(int x, int y, Uint32 flag)
{
	return 1;
}

int azView::onRelease(int x, int y, Uint32 flag)
{
	return 1;
}

int azView::onWheel(int dx, int dy)
{
	return 1;
}

void azView::draw()
{
	SDL_Rect ori_draw;
	e->MoveDrawer(area, &ori_draw);
	for (auto it=childs.begin(); it!=childs.end();it++)
	{
		(*it)->draw();
#if AZDEBUGUI
		SDL_Rect r=(*it)->area;
		e->SetColor(COLOR_RED);
		e->DrawRect(&r);
#endif // AZDEBUG
	}
#if AZDEBUGUI
	SDL_Rect r = area;
	r.x = r.y = 0;
	e->SetColor(COLOR_RED);
	e->DrawRect(&r);
#endif // AZDEBUG
	e->SetDrawer(ori_draw, nullptr);
}

azView::~azView()
{
}
azvLayout::~azvLayout()
{
	Clear();
}
int azvLayout::onEvent(SDL_Event & event)
{
	switch (event.type)
	{
	case SDL_MOUSEMOTION:
		onMoving(event.motion.x-area.x,event.motion.y - area.y, event.motion.xrel, event.motion.yrel);
		break;
	case SDL_MOUSEBUTTONDOWN:
		onPress(event.button.x - area.x, event.button.y - area.y,event.button.button);
		break;
	case SDL_MOUSEBUTTONUP:
		onRelease(event.button.x - area.x, event.button.y - area.y, event.button.button);
		break;

	case SDL_MOUSEWHEEL:
		onWheel(event.wheel.x, event.wheel.y);
	/*
	case SDL_FINGERDOWN:
	case SDL_FINGERUP:
	case SDL_FINGERMOTION:
		onMoving(event.tfinger.x, event.tfinger.y, event.tfinger.dx, event.tfinger.dy);
		break;
	*/
	default:
		break;
	}
	return	0;
}

void azvLayout::Clear()
{
	for (auto v : childs)
	{
		delete v;
	}
}

void azvLayout::draw()
{
	back.draw(&area);
	azView::draw();
}

void azvLayout::LoadFromFile(const std::string & fname)
{

}

int azvLayout::onMoveIn(int x, int y)
{
	azView* v = FindChild(x, y);
	if (v)
	{
		focus_view = v;
		v->onMoveIn(x, y);
		return 1;
	}
	return 0;
}

int azvLayout::onMoveOut(int x, int y)
{
	if (focus_view)
	{
		focus_locked = false;
		focus_view->onMoveOut(x, y);
		return 1;
	}
	return 0;
}

int azvLayout::onMoving(int x, int y, int dx, int dy)
{
	if(focus_locked&&focus_view)
		return focus_view->onMoving(x, y, dx, dy);

	azView*v = FindChild(x, y);
	if (v == focus_view)
	{
		if (v)
		{
			return v->onMoving(x, y, dx, dy);
		}
	}
	else
	{
		if (focus_view)
			focus_view->onMoveOut(x, y);
		focus_view = v;
		if (v)
		{
			return v->onMoveIn(x , y );
		}
	}
	return 0;
}

int azvLayout::onPress(int x, int y,Uint32 flag)
{
	azView*v = FindChild(x, y);
	focus_locked = true;
	if (v == focus_view)
	{
		if (v)
		{
			return v->onPress(x , y,flag);
		}
	}
	else
	{
		if (focus_view)
			focus_view->onMoveOut(x , y);
		focus_view = v;
		if (v)
		{
			return v->onPress(x , y,flag);
		}
	}
	return 0;
}

int azvLayout::onRelease(int x, int y,Uint32 flag)
{
	azView*v = FindChild(x, y);
	focus_locked = false;
	if (v == focus_view)
	{
		if (v)
		{
			return v->onRelease(x , y , flag);
		}
	}
	else
	{
		if (focus_view)
			focus_view->onMoveOut(x , y );
		focus_view = v;
		if (v)
		{
			return v->onRelease(x , y , flag);
		}
	}
	return 0;
}

int azvLayout::onWheel(int dx, int dy)
{
	if (focus_view)
	{
		return focus_view->onWheel(dx, dy);
	}
	return 1;
}
