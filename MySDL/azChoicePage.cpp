#include "azChoicePage.h"




void azChoicePage::on_list_click(azvList &list, int id)
{
	if (onChoose)
		onChoose(id);
	shown = 0;
}

azChoicePage::azChoicePage(azView * p):azPage(p)
{
	shown = 0;
	list = new azvListbox(this);
}

void azChoicePage::show()
{
	shown = 1;
	area = parent->area;
	area.x = area.y = 0;

	list->OnItemClick=[this](azvList&,int id)
	{
		if (this->onChoose)
			this->onChoose(id);
	};
}

void azChoicePage::end()
{
	shown = 0;
	list->OnItemClick = nullptr;
	area.w = area.h = 0;
}

void azChoicePage::draw()
{
	if (shown)
		azPage::draw();
}

int azChoicePage::onMoveIn(int x, int y)
{
	azPage::onMoveIn(x, y);
	return 1;
}

int azChoicePage::onMoveOut(int x, int y)
{
	azPage::onMoveOut(x, y);
	return 1;
}

int azChoicePage::onMoving(int x, int y, int dx, int dy)
{
	azPage::onMoving(x, y, dx, dy);
	return 1;
}

int azChoicePage::onPress(int x, int y, Uint32 flag)
{
	azPage::onPress(x, y, flag);
	return 1;
}

int azChoicePage::onRelease(int x, int y, Uint32 flag)
{
	azPage::onRelease(x, y, flag);
	end();
	return 1;
}

int azChoicePage::onWheel(int dx, int dy)
{
	azPage::onWheel(dx, dy);
	return 1;
}

azChoicePage::~azChoicePage()
{
	if (shown&&onChoose)
	{
		shown = 0;
		list->OnItemClick = nullptr;
	}
}
