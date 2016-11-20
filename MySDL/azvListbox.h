#pragma once
#include "azView.h"

#include <functional>

static SDL_Color DefHoverColor = COLOR(0x77, 0x77, 0xff, 0xb0);
static SDL_Color DefSelectColor = COLOR(0x77, 0x77, 0xff, 0xb0);
class azvList :
	public azView
{
protected:
	int press_item, select_item, hover_item;
public:
	int slidespeed;
	azText title;
	azShape title_back;
	SDL_Color hover_color, select_color;
	std::function<void(azvList&list, int item)>OnItemClick;

	virtual int FindItem(int mx, int my)=0;
	virtual int SlideList(int dist) = 0;

	virtual int onMoving(int x, int y, int dx, int dy) override;
	virtual int onPress(int x, int y, Uint32 flag) override;
	virtual int onRelease(int x, int y, Uint32 flag) override;
	virtual int onMoveOut(int x, int y) override;
	virtual int onWheel(int dx, int dy) override;

	azvList(azView*p) :azView(p), title(e), title_back(e),
		slidespeed(20),
		press_item(-1),select_item(-1), hover_item(-1),
		hover_color(DefHoverColor),
		select_color(DefSelectColor)
	{
		back.color = COLOR(0x55, 0xdd, 0xff, 0x63);
		back.border = COLOR_NONE;
		title_back.color = COLOR(0x55, 0xdd, 0xff, 0xc0);
	};
	virtual ~azvList();
};

struct ListItem
{
	int h;
	azText*text;
	azTexture icon;
};
class azvListbox :
	public azvList
{
	int nowpos;
	int allheight;
	std::vector<ListItem>items;
public:
	azvListbox(azView*p) :azvList(p){};

	virtual void draw() override;
	virtual int SlideList(int dist) override;

	virtual int FindItem(int mx, int my) override;

	int InsertText(const std::wstring&str,SDL_Color color=COLOR_WHITE);
	int SetItemText(size_t index,const std::wstring&str);
	int SetItemColor(size_t index, SDL_Color color);
	int SetItemIcon(size_t index,azTexture&tex);

	const std::wstring& GetItemText(size_t index);
	SDL_Color GetItemColor(size_t index);
	azTexture& GetItemIcon(size_t index);

	int CalcAllHeight(){
		allheight = 0;
		for (auto&t : items)
			allheight += t.h;
		return allheight;
	}

	int DelItem(size_t index);
	int Clear();
	~azvListbox();
};


