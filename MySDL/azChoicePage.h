#pragma once
#include "azView.h"
#include "azvListbox.h"
#include <functional>
class azChoicePage:public azPage
{
	int shown;
	void on_list_click(azvList&, int id);
public:
	azvListbox *list;
	std::function<void(int choice)> onChoose;
	azChoicePage(azView*p);

	void show();
	void end();

	virtual void draw();
	virtual int onMoveIn(int x, int y) override;
	virtual int onMoveOut(int x, int y) override;
	virtual int onMoving(int x, int y, int dx, int dy) override;
	virtual int onPress(int x, int y, Uint32 flag) override;
	virtual int onRelease(int x, int y, Uint32 flag) override;
	virtual int onWheel(int dx, int dy) override;
	
	~azChoicePage();
};

