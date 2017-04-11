#include "azLayout.h"


/*
x为要对齐的值(会被修改)
align对齐方式:1=left 2=right other=middle
width总宽度
w控件宽度
*/
#define SetAlign(x,align,width,w) if (align==1) x=padding; \
									else if(align==2) x=width-w-padding;\
									else x=(width-w)/2;
#define SetLayout(x,y,varea,total)

#define GetAlignType(align,type) (align>>type)&0x3
#define Align_V ((Align>>2)&0x3)
#define Align_H (Align&0x3)
azLayout::~azLayout()
{
}


void azBoxLayout::updateLayout()
{
	int x, y;
	SDL_Rect total;
	SDL_Rect area = this->area;
	getTotalRect(total);
	if (Dirction == LAYOUT_HORIZONTAL)
	{
		total.w += spacing*(childs.size() - 1);
		SetAlign(x, Align_H, area.w, total.w);
		for (auto&v : childs)
		{
			SetAlign(y, Align_V, area.h, v->area.h);
			v->setPos(x, y);
			x += v->area.w+spacing;
		}
	}
	else if (Dirction == LAYOUT_VERTICAL)
	{
		total.h += spacing*(childs.size() - 1);

		SetAlign(y, Align_V, area.h, total.h);
		for (auto&v : childs)
		{
			SetAlign(x, Align_H, area.w, v->area.w);
			v->setPos(x, y);
			y += v->area.h + spacing;
		}
	}
}

void azBoxLayout::getTotalRect(SDL_Rect & r)
{
	r.w = r.h = 0;
	if (Dirction == LAYOUT_HORIZONTAL)
	{
		for (auto&v : childs)
		{
			r.w += v->area.w;
			r.h = __max(v->area.h, r.h);
		}
	}
	else
	{
		for (auto&v : childs)
		{
			r.h += v->area.h;
			r.w = __max(v->area.w, r.w);
		}

	}
}

azBoxLayout::~azBoxLayout()
{
}
