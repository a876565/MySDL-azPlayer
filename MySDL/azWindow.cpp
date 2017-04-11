#include "azWindow.h"


void azWindow::draw()
{
	SDL_Rect r, ori;
	e->MoveDrawer(area, &ori);

	r.x = r.y = 0;
	r.w = area.w, r.h = area.h;
	//绘制背景
	back.draw(&r);

	r.h = title.getHeight();
	if (r.h < FoldableSize + 4)r.h = FoldableSize + 4;
	//绘制标题
	title.draw(&r);
	title_back.draw(&r);
	//折叠和关闭按钮
	r.x = area.w - FoldableSize-4;
	r.y = (r.h - FoldableSize) / 2;
	r.w = r.h = FoldableSize;
	draw_foldable(r);
	r.h += 4;
	if ( title_size.h != r.h)
	{
		title_size.h = r.h;
		page->setPos(0, title_size.h);
		page->setSize(title_size.w, area.h - title_size.h);
	}

	e->MoveDrawer(0,r.h ,nullptr);
	if((state&WINDOW_FOLDED)==0)
		draw_childs();
	e->SetDrawer(ori);
}

void azWindow::setFlag()
{
}

void azWindow::draw_foldable(SDL_Rect&r)
{
	int y = (r.y + r.h) / 2;
	e->SetColor(FoldableColor[0]);
	e->DrawRect(&r);
	e->DrawLine(r.x, y, r.x + r.h, y);
}

void azWindow::addChild(azView * v)
{
	page->addChild(v);
}

void azWindow::rmChild(azView * v)
{
	page->rmChild(v);
}


azWindow::~azWindow()
{
}
