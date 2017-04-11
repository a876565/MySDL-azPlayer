#include "azvListbox.h"




void azvListbox::draw()
{
	SDL_Rect r, ori;
	int y, i;
	e->MoveDrawer(area, &ori);

	r.x = r.y = 0;
	r.w = area.w, r.h = area.h;

	//绘制背景
	back.draw(&r);

	//绘制标题
	r.h = title.getHeight();
	title_back.draw(&r);
	title.draw(&r);

	//准备绘制items
	r.y = r.h;
	r.h = area.h - r.h;
	e->MoveDrawer(r, nullptr);
	y = 0;
	i = 0;
	for (auto& t : items)
	{
		if (y + t.h > nowpos)
		{
			y = y - nowpos;
			break;
		}
		y += t.h;
		i++;
	}
	for (; (size_t)i < items.size();)
	{
		auto&t = items[i];
		if (y >= area.h)
			break;
		r.h = t.h;
		r.y = y;

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
		if (t.icon)
		{
			SDL_Rect ico_r = { 0,y + (t.h - t.icon->height()) / 2,
				t.icon->width(),t.icon->height() };
			t.icon->draw(&ico_r);
			r.x = t.icon->width();
		}
		t.text->draw(&r);
		r.x = 0;

		y += t.h;
		i++;
	}
	e->SetDrawer(ori);
}

int azvListbox::setSize(int w, int h)
{
	if (w != area.w)
	{
		area.w = w;
		title.setSize(area.w, 0);
		for (auto&t : items)
		{
			t.text->setSize(area.w - t.iw, 0);
			chkHeight(t);
		}
	}
	area.h = h;
	if (area.h == 0)
		area.h = CalcAllHeight();
	return 0;
}

int azvListbox::SlideList(int dist)
{
	nowpos -= dist*slidespeed;
	int items_h = area.h - title.getHeight();
	if (nowpos > allheight - items_h)
		nowpos = allheight - items_h;
	if (nowpos < 0)
		nowpos = 0;
	return 0;
}



int azvListbox::FindItem(int mx, int my)
{
	SDL_Rect r = area;
	int y, i;
	r.h = title.getHeight();

	my += nowpos;
	y = r.h-r.y;
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
	ListItem it;
	azText *t = new azText(e);
	t->setText(str);
	t->setColor(color);
	t->setSize(area.w, 0);
	it.text = t;
	it.h = t->getHeight();
	it.iw = 0;

	items.push_back(it);
	CalcAllHeight();
	return items.size() - 1;
}

int azvListbox::SetItemText(size_t index, const std::wstring & str)
{
	if (index < items.size())
	{
		auto& it = items[index];
		it.text->setText(str);
		chkHeight(it);
	}
	return 0;
}

int azvListbox::SetItemColor(size_t index, SDL_Color color)
{
	if (index < items.size())
	{
		items[index].text->setColor(color);
	}
	return 0;
}

void azvListbox::chkHeight(ListItem & it)
{
	if (it.icon)
	{
		it.iw = it.icon->width();
		if (it.icon->height() > it.h)
		{
			it.h = it.icon->height();
			CalcAllHeight();
		}
		it.text->setSize(area.w - it.iw, INT_MAX);
	}
	else if (it.h != it.text->getHeight())
	{
		it.iw = 0;
		it.h = it.text->getHeight();
		it.text->setSize(area.w, INT_MAX);
		CalcAllHeight();
	}
}

int azvListbox::SetItemIcon(size_t index, azTexture & tex)
{
	if (index < items.size())
	{
		auto& it = items[index];
		it.icon = tex;
		chkHeight(it);
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

azTexture& azvListbox::GetItemIcon(size_t index)
{
	if (index < items.size())
	{
		return items[index].icon;
	}
	return items[0].icon;
}

int azvListbox::DelItem(size_t index)
{
	if (index < items.size())
	{
		delete items[index].text;
		items.erase(items.begin() + index);
		CalcAllHeight();
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

	if (OnItemClick&&select_item == FindItem(mx, my))
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
