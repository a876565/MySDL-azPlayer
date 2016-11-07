#include "azvListbox.h"




void azvListbox::draw()
{
	SDL_Rect r,ori;
	int y,i;
	e->MoveDrawer(area, &ori);

	r.x = r.y = 0;
	r.w = area.w, r.h = area.h;

	back.draw(&r);
	r.h = title.CalcHeight(r.w);
	title_back.draw(&r);
	title.draw(&r);

	y = 0;
	i = 0;
	for (auto& t : items)
	{
		if (y + t.h > nowpos)
		{
			y =r.h+y-nowpos ;
			break;
		}
		y += t.h;
		i++;
	}
	for (; (size_t)i<items.size();)
	{
		auto&t = items[i];
		r.h = t.h;
		if (y + r.h > area.h)
			break;
		r.y =y;

		if (i == select_item)
		{
			e->SetColor(select_color);
			e->FillRect(&r);
		}
		else if (i == hover_item)
		{
			e->SetColor(hover_color);
			e->FillRect(&r);
		}
		t.text->draw(&r);

		y += r.h;
		i++;
	}

}

int azvListbox::SlideList(int dist)
{
	nowpos -= dist*slidespeed;
	if (nowpos > allheight-area.h)
		nowpos = allheight - area.h;
	if (nowpos < 0)
		nowpos = 0;
	return 0;
}



int azvListbox::FindItem(int mx, int my)
{
	SDL_Rect r = area;
	int y,i;
	r.h = title.CalcHeight(area.w);

	my += nowpos;
	y = r.h;
	i = 0;
	for (auto t : items)
	{
		r.h = t.h;
		r.y = area.y + y;

		if (isPointInRect(mx, my, r))
		{
			return i;
		}

		y += r.h;
		i++;
	}
	return -1;
}

int azvListbox::InsertText(const std::wstring & str, SDL_Color color)
{
	ListItem lt;
	azText *t = new azText(e);
	t->setText(str);
	t->setColor(color);
	lt.h = t->CalcHeight(area.w);
	lt.text = t;
	items.push_back(lt);
	CalcAllHeight();
	return items.size()-1;
}

int azvListbox::SetItemText(size_t index, const std::wstring & str)
{
	if (index < items.size())
	{
		auto& it = items[index];
		it.text->setText(str);
		int h = it.text->CalcHeight(area.w);
		if (it.h != h)
		{
			it.h = h;
			CalcAllHeight();
		}
	}
	return 0;
}

int azvListbox::SetItemColor(size_t index,SDL_Color color)
{
	if (index < items.size())
	{
		items[index].text->setColor(color);
	}
	return 0;
}

const std::wstring &  azvListbox::GetItemText(size_t index)
{
	if (index < items.size())
	{
		return items[index].text->getText();
	}
	return nullwstr;
}

SDL_Color azvListbox::GetItemColor(size_t index)
{
	if (index < items.size())
	{
		return items[index].text->getColor();
	}
	return COLOR_WHITE;
}

int azvListbox::DelItem(size_t index)
{
	if (index < items.size())
	{
		delete items[index].text;
		items.erase(items.begin()+index);
	}
	return 0;
}

int azvListbox::Clear()
{
	for (auto& t : items)
	{
		delete t.text;
	}
	items.clear();
	return 0;
}


azvListbox::~azvListbox()
{
	Clear();
}


int azvList::onMoving(int mx, int my, int dx, int dy)
{
	hover_item = FindItem(mx, my);
	return 1;
}
int azvList::onPress(int mx, int my, Uint32 flag)
{
	select_item = FindItem(mx, my);
	return 1;
}
int azvList::onRelease(int mx, int my, Uint32 flag)
{

	if (OnItemClick&&select_item ==FindItem(mx,my))
		OnItemClick(*this, select_item);
	return 1;
}

int azvList::onMoveOut(int x, int y)
{
	hover_item = -1;
	return 1;
}

int azvList::onWheel(int dx, int dy)
{
	SlideList(dy);
	return 1;
}



azvList::~azvList()
{
}
