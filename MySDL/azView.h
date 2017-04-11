#pragma once
#include "azEngine.h"
#include "azFontMgr.h"
#include "azText.h"
#include <vector>
#include <functional>
#define SP_TEX_BACK 1
#define SP_COLOR_BACK 2
#define AZDEBUGUI 0

class ivec2
{
public:
	union{
		int x;
		int w;
	};
	union {
		int y;
		int h;
	};
	ivec2() :x(0), y(0) {};
	ivec2(int a) :x(a), y(a) {};
	ivec2(int a,int b) :x(a), y(b) {};

};


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

	void attach(azView*vparent) {
		vparent->addChild(this);
	}
	void remove() {
		if (parent)
			parent->rmChild(this);
	}

public:
	SDL_Rect userrect;
	azShape back;
	SDL_Rect area;
	azView(azEngine*en) :e(en), parent(nullptr),back(en) { setSize(en->get_winrect().w, en->get_winrect().h);};
	azView(azView*p) :e(p->e), back(p->e) { p->addChild(this); }
	virtual void addChild(azView*v);
	virtual void rmChild(azView*v);
	void clear(){
		while (childs.size()>0)
		{
			auto v = childs.begin();
			delete *v;
		}
	}

	inline bool isInArea(int x, int y) {
		return (x > area.x&&x<area.x + area.w&&
			y>area.y&&y < area.y + area.h);
	}

	azView*find_root() { azView*v = this; while (v->parent)v = v->parent; return v; };
	void to_abs_area(SDL_Rect*r) { azView*v = this; while (v->parent) { r->x += v->area.x; r->y += v->area.y; v = v->parent; } }
	int get_child_w() { int w = 0; for (auto &v : childs)w += v->area.w; return w; }
	int get_child_h() { int h = 0; for (auto &v : childs)h += v->area.h; return h; }
	
	virtual ivec2 getSize() { return ivec2(area.w, area.h); }
	virtual ivec2 getPos() { return ivec2(area.x, area.y); }
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
	void draw_childs();
	virtual ~azView();

};

class azPage :
	public azView
{
	azView*focus_view;
	bool focus_locked;
public :
	azPage(azEngine*en) :azView(en){}
	azPage(azView*v) :azView(v){};
	~azPage();
	int onEvent(SDL_Event&event);
	void rmChild(azView*v) override;

	azView* FindChild(int x, int y)
	{
		for (auto v = childs.rbegin(); v != childs.rend();v++) {
			if ((*v)->isInArea(x, y))
				return *v;
		}
		return nullptr;
	}
	void SetFocus(azView*v,int outx=0,int outy=0){
		if (focus_view)
			focus_view->onMoveOut(outx, outy);
		focus_view = v;
	}

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

typedef std::unique_ptr<azPage> pazvLayout;