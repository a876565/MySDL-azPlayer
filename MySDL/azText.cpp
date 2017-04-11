#include "azText.h"


void azTextBase::setColor(SDL_Color  col)
{
	color = col;
}

void azTextBase::setFont(azFont f)
{
	font = f;
}

void azTextBase::move(int dx, int dy)
{
}


azText::azText(azEngine*en) :azTextBase(en),now_x(0),now_y(0),
width(0),height(0),real_w(0),real_h(0)
{
	font = e->getDefaultFont();

}


azText::~azText()
{
}

int azText::draw(int x,int y)
{
	if (changed)
	{
		now_x = x, now_y = y;
		updateLayout();
	}

	font->SetColor(color.r, color.g, color.b);
	if (color.a != 255)
		font->SetAlpha(color.a);
	if (now_x == x&&now_y == y)
	{
		for (TEXCHAR&tc : tchs)
		{
			if (tc.flag == TEXCHAR_TEXT_END)
				break;
			if (tc.tex)
			{
				tc.tex->draw(&tc.dst, &tc.loc);
			}
		}
	}
	else
	{
		int dx = x - now_x, dy = y-now_y;
		now_x = x, now_y = y;
		for (TEXCHAR&tc : tchs)
		{
			if (tc.flag == TEXCHAR_TEXT_END)
				break;
			if (tc.tex)
			{
				tc.dst.x += dx;
				tc.dst.y += dy;
				tc.tex->draw(&tc.dst, &tc.loc);
			}
		}

	}
	return real_h;
}

int azText::draw(SDL_Rect * target)
{
	setSize(target->w, target->h);
	return draw(target->x, target->y);
}

void azText::setText(const std::wstring& s)
{
	text = s;
	updateText();
}

void azText::setSize(int w, int h)
{
	if (width != w || height != h)
		changed = true;
	else
		return;
	width = w; 
	height = h;
}


void azText::updateText()
{
	changed=true;
	tchs.clear();
	for (wchar_t c : text)
	{
		TEXCHAR tc;
		tc.ch = c;
		if (c != '\n' && c != '\r')
		{
			font->GetTexChar(tc);
			tc.dst.w = tc.loc.w;
			tc.dst.h = tc.loc.h;
		}
		tchs.push_back(tc);
	}
}

void azText::updateLayout()
{
	int x = 0, y = 0;
	int hwrap = 0;

	changed = false;
	real_w = real_h = 0;
	for (TEXCHAR&tc : tchs)
	{
		hwrap = __max(hwrap, tc.loc.h);

		if (tc.ch == '\n' || x + tc.loc.w > width)
		{
			if (x + tc.dst.w> real_w)
				real_w = x + tc.dst.w;
			if (height&&y + hwrap > height)
			{
				tc.flag = TEXCHAR_TEXT_END;
				break;
			}
			tc.flag = TEXCHAR_LINE_END;
			y += hwrap;
			hwrap = tc.dst.h;
			x = 0;
		}
		else
		{
			tc.flag = TEXCHAR_SHOW;
		}
		if (!tc.tex)
			continue;
		
		tc.dst.x = x+now_x;
		tc.dst.y = y+now_y;

		x += tc.dst.w;
	}
	if (real_w == 0)
		real_w = x + tchs.back().dst.w;
	real_h=y + hwrap;
}
