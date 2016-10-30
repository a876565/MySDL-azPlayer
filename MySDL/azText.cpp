#include "azText.h"


void azTextBase::setColor(SDL_Color  col)
{
	color = col;
}

void azTextBase::setFont(azFont f)
{
	font = f;
}

int azTextBase::CalcHeight(int width)
{
	return 0;
}


azText::azText(azEngine*en) :azTextBase(en)
{
	font = e->getDefaultFont();
}


azText::~azText()
{
}

int azText::draw(SDL_Rect * target)
{
	SDL_Rect r;
	int x=0, y=0;
	int hwrap=0;
	for (TEXCHAR&tc : tchs)
	{
		hwrap = __max(hwrap, tc.loc.h);

		if (tc.ch=='\n'||x + tc.loc.w > target->w)
		{
			if (y + hwrap > target->h)
				break;
			y += hwrap;
			hwrap = 0;
			x = 0;
		}
		if (!tc.tex)
			continue;
		r.x = x + target->x;
		r.y = y + target->y;
		r.w = tc.loc.w;
		r.h = tc.loc.h;
		if (color.a != 255)
			tc.tex->setAlpha(color.a);
		tc.tex->setColor(color.r, color.g, color.b);
		tc.tex->draw(&r, &tc.loc);

		x += r.w;
	}
	return y + hwrap;
}

void azText::setText(const std::wstring& s)
{
	text = s;
	tchs.clear();
	for (wchar_t c : text)
	{
		TEXCHAR tc;
		tc.ch = c;
		if (c != '\n' && c != '\r')
			font->GetTexChar(tc);
		tchs.push_back(tc);
	}
}

int azText::CalcHeight(int width)
{
	int x = 0, y = 0;
	int hwrap = 0;
	for (TEXCHAR&tc : tchs)
	{
		hwrap = __max(hwrap, tc.loc.h);

		if (tc.ch == '\n' || x + tc.loc.w > width)
		{
			y += hwrap;
			hwrap = 0;
			x = 0;
		}
		if (!tc.tex)
			continue;

		x += tc.loc.w;
	}
	return y+hwrap;
}
