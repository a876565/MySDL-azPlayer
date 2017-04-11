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
void azView::addChild(azView * v) {
	if (v->parent)
		v->parent->rmChild(v);
	v->parent = this;
	childs.push_back(v);
}
void azView::rmChild(azView * v) {
	auto it = std::find(childs.begin(), childs.end(), v);
	if (it != childs.end())
		childs.erase(it);
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
	draw_childs();
	e->SetDrawer(ori_draw, nullptr);
}

void azView::draw_childs()
{
	for (auto it = childs.begin(); it != childs.end(); it++)
	{
		(*it)->draw();

#if AZDEBUGUI
		SDL_Rect r = (*it)->area;
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
}

azView::~azView()
{
	clear();
	remove();
}
azPage::~azPage()
{
}
int azPage::onEvent(SDL_Event & event)
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

void azPage::rmChild(azView * v)
{
	if (focus_view == v)
	{
		focus_view = nullptr;
		focus_locked = false;
	}
	azView::rmChild(v);
}


void azPage::draw()
{
	back.draw(&area);
	azView::draw();
}

void azPage::LoadFromFile(const std::string & fname)
{

}

int azPage::onMoveIn(int x, int y)
{
	int ret = 0;
	for (auto p = childs.rbegin(); p != childs.rend(); p++) {
		azView*v = *p;
		if (v->isInArea(x, y))
		{
			ret = v->onMoveIn(x - v->area.x, y - v->area.y);
			if (ret)
			{
				focus_view = v;
				return ret;
			}
		}
	}
	return 0;
}

int azPage::onMoveOut(int x, int y)
{
	if (focus_view)
	{
		focus_locked = false;
		focus_view->onMoveOut(x - focus_view->area.x, y - focus_view->area.y);
		return 1;
	}
	return 0;
}

int azPage::onMoving(int x, int y, int dx, int dy)
{
	if(focus_locked&&focus_view)
		return focus_view->onMoving(x - focus_view->area.x, y - focus_view->area.y, dx, dy);

	int ret = 0;
	for (auto p = childs.rbegin(); p != childs.rend(); p++) {
		azView*v = *p;
		if (v->isInArea(x, y))
		{
			if (v == focus_view)
			{
				ret=v->onMoving(x-v->area.x, y - v->area.y, dx, dy);
			}
			else
			{
				ret = v->onMoveIn(x - v->area.x, y - v->area.y);
				if (ret)
				{
					SetFocus(v,x, y);
				}
			}
			if (ret)
				return ret;
		}
	}
	SetFocus(nullptr, x, y);
	return 0;
}

int azPage::onPress(int x, int y,Uint32 flag)
{

	int ret = 0;
	for (auto p = childs.rbegin(); p != childs.rend(); p++) {
		azView*v = *p;
		if (v->isInArea(x, y))
		{

			focus_locked = true;
			if (v == focus_view)
			{
				if (v)
				{
					ret=v->onPress(x - v->area.x, y - v->area.y, flag);
				}
			}
			else
			{
				if (v)
					ret = v->onPress(x - v->area.x, y - v->area.y, flag);
				if (ret)
				{
					if (focus_view)
						focus_view->onMoveOut(x - v->area.x, y - v->area.y);
					focus_view = v;
				}
			}
			if (ret)
				return ret;
		}
	}

	if (focus_view)
		focus_view->onMoveOut(x - focus_view->area.x, y - focus_view->area.y);
	focus_view = nullptr;
	return 0;
}

int azPage::onRelease(int x, int y,Uint32 flag)
{
	int ret = 0;
	for (auto p = childs.rbegin(); p != childs.rend(); p++) {
		azView*v = *p;
		if (v->isInArea(x, y))
		{
			focus_locked = false;
			if (v == focus_view)
			{
				if (v)
				{
					return v->onRelease(x - v->area.x, y - v->area.y, flag);
				}
			}
			else
			{
				if (v)
				{
					ret = v->onRelease(x - v->area.x, y - v->area.y, flag);
				}
				if (ret)
				{
					if (focus_view)
						focus_view->onMoveOut(x - v->area.x, y - v->area.y);
					focus_view = v;
				}
			}
			if (ret)
				return ret;
		}
	}

	if (focus_view)
		focus_view->onMoveOut(x - focus_view->area.x, y - focus_view->area.y);
	focus_view = nullptr;
	return 0;
}

int azPage::onWheel(int dx, int dy)
{
	if (focus_view)
	{
		return focus_view->onWheel(dx, dy);
	}
	return 1;
}
