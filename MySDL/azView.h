#pragma once
#include "azEngine.h"
#include "azFontMgr.h"
#include <vector>

#define SP_TEX_BACK 1
#define SP_COLOR_BACK 2
#define AZDEBUGUI 0

#define SET_DRAW_AREA SDL_Rect ori_draw_area;e->SetDrawer(area,&ori_draw_area);
#define BACK_DRAW_AREA e->SetDrawer(ori_draw_area,nullptr);

class azShape
{
private:
	azEngine *e;
public:
	SDL_Color border;
	SDL_Color color;
	azTexture tex;
	azShape(azEngine *eng) :e(eng),color(COLOR_NONE) {};
	~azShape() {};
	void draw(SDL_Rect*dst)
	{
		if (tex)
		{
			tex->draw(dst);
		}
		else if(color.a>0)
		{
			e->SetColor(color);
			e->FillRect(dst);
		}
		if (border.a > 0)
		{
			e->SetColor(border);
			e->DrawRect(dst);
		}
	}
	int draw(SDL_Rect*dst,Uint8 alpha)
	{
		if (tex)
		{
			tex->setAlpha(alpha);
			tex->draw(dst);
			tex->setAlpha(255);
		}
		else if (color.a>0)
		{
			SDL_Color c = color;
			c.a = c.a*alpha / 255;
			e->SetColor(c);
			e->FillRect(dst);
		}
		if (border.a > 0)
		{
			e->SetColor(border);
			e->DrawRect(dst);
		}
		return 0;
	}
};
class azView
{
protected:
	azView*parent;
	std::vector<azView*>childs;
	azEngine*e;

public:

	azShape back;
	SDL_Rect area;
	azView(azEngine*en) :e(en), parent(nullptr),back(en) { setSize(en->get_winrect().w, en->get_winrect().h);};
	azView(azView*p) :e(p->e), back(p->e) { attach(p); }
	void attach(azView*vparent){
		if (parent) {
			parent->rmChild(this);
		}
		parent = vparent;
		parent->childs.push_back(this);
		setSize(parent->area.w, parent->area.h);
	}
	void remove() {
		if (parent)
			parent->rmChild(this);
	}
	void addChild(azView*v) {
		v->attach(this);
	}
	void rmChild(azView*v) { 
		auto it = std::find(childs.begin(), childs.end(), v); 
		if (it != childs.end())
			childs.erase(it); 
	}

	bool isInArea(int x, int y) {
		return (x > area.x&&x<area.x + area.w&&
			y>area.y&&y < area.y + area.h);
	}
	virtual int setSize(int w, int h) ;
	virtual int setPos(int x, int y);
	virtual int move(int dx, int dy);
	virtual void autosize();

	virtual int onMoveIn(int x, int y) ;
	virtual int onMoveOut(int x, int y) ;
	virtual int onMoving(int x, int y, int dx, int dy) ;
	virtual int onPress(int x, int y, Uint32 flag) ;
	virtual int onRelease(int x, int y, Uint32 flag);
	virtual int onWheel(int dx,int dy);

	virtual void draw();
	virtual ~azView();

};

class azvLayout :
	public azView
{
	azView*focus_view;
public :
	azvLayout(azEngine*en) :azView(en){}
	azvLayout(azView*v) :azView(v){};
	~azvLayout();
	int onEvent(SDL_Event&event);

	azView* FindChild(int x, int y)
	{
		for (auto v = childs.rbegin(); v != childs.rend();v++) {
			if ((*v)->isInArea(x, y))
				return *v;
		}
		return nullptr;
	}

	void Clear();

	virtual void draw();

	void LoadFromFile(const std::string&fname);
	// Í¨¹ý azView ¼Ì³Ð
	virtual int onMoveIn(int x, int y) override;
	virtual int onMoveOut(int x, int y) override;
	virtual int onMoving(int x, int y, int dx, int dy) override;
	virtual int onPress(int x, int y, Uint32 flag) override;
	virtual int onRelease(int x, int y, Uint32 flag) override;
	virtual int onWheel(int dx, int dy) override;
};

typedef std::unique_ptr<azvLayout> pazvLayout;
#include "azvLabel.h"
#include "azvImage.h"
#include "azvButton.h"
#include "azvListbox.h"