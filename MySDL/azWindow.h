#pragma once
#include "azView.h"
enum {
	WINDOW_CLOSE_BUTTON = 1,
	WINDOW_FOLDABLE = 2
};
enum {
	WINDOW_FOLDED=1
};
class azWindow :
	public azPage
{
	azPage*page;
	ivec2 title_size;
public:
	azText title;
	azShape title_back;
	unsigned flag;
	unsigned state;
	const static int FoldableSize = 16;
	SDL_Color FoldableColor[2] = { COLOR_GREY,COLOR_BLACK };

	azWindow(azView*p) :azPage(p), title(e), title_back(e), state(0), flag(0) { 
		page = new azPage(e); 
		azPage::addChild(page); 
	};
	virtual void draw() override;
	void setFlag();
	void draw_foldable(SDL_Rect&r);
	virtual void addChild(azView*v);
	virtual void rmChild(azView*v);

	virtual int  setSize(int w, int h) { 
		title_size.w = w; 
		azPage::setSize(w, h); 
		page->setSize(w, h - title_size.h); 
		return 0;
	}
	~azWindow();
};

