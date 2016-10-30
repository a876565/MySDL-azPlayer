#pragma once
#include "device.h"
#include "RefObject.h"
#include "azEngine.h"
#include "azTexture.h"
#include <functional>
#include <vector>
class azGraph
	:public RefObject
{
public:
	azGraph() {};
	virtual~azGraph() {};
	virtual void draw(azEngine*e) = 0;
};

class azGraphColor
	: public azGraph
{
	Uint8 r, g, b, a;
public:
	azGraphColor(Uint8 ur, Uint8 ug, Uint8 ub, Uint8 ua):r(ur),g(ug),b(ub),a(ua) {};
	~azGraphColor() {};
	void draw(azEngine*e) {
		e->SetColor(r, g, b, a);
	}
private:

};

class azGraphTex
	:public azGraph
{
private:

	azTexture m_tex;
	SDL_Rect m_dst;
	SDL_Rect m_src;
	SDL_Rect *p_dst;
	SDL_Rect *p_src;
public:
	azGraphTex(azTexture& tex, SDL_Rect * dst, SDL_Rect * src) {
		m_tex = tex;
		if (dst)
		{
			RECT_COPY(m_dst, *dst);
			p_dst = &m_dst;
		}
		else {
			p_dst = nullptr;
		}
		if (src)
		{
			RECT_COPY(m_src, *src);
			p_src = &m_src;
		}
		else {
			p_src = nullptr;
		}
	}
	~azGraphTex() {};
	void draw(azEngine*e) {
		m_tex->draw(p_dst, p_src);
	}

};
class azGraphTex2
	:public azGraph
{
private:

	azTexture m_tex;
	SDL_Rect m_dst;
	SDL_Rect m_src;
	SDL_Rect *p_dst;
	SDL_Rect *p_src;
	double m_angle;
	SDL_Point * m_center;
	SDL_RendererFlip m_flip;
public:
	azGraphTex2(azTexture& tex, SDL_Rect * dst, SDL_Rect * src,
		double angle, SDL_Point * center, SDL_RendererFlip flip) {
		m_tex = tex;

		if (dst)
		{
			RECT_COPY(m_dst, *dst);
			p_dst = &m_dst;
		}
		else {
			p_dst = nullptr;
		}
		if (src)
		{
			RECT_COPY(m_src, *src);
			p_src = &m_src;
		}
		else {
			p_src = nullptr;
		}
	}
	~azGraphTex2() {};
	void draw(azEngine*e) {
		m_tex->draw2(p_dst, p_src, m_angle,m_center,m_flip);
	}

};
class azGraphRect
	:public azGraph
{
	int t;//1=draw 2=fill
	SDL_Rect dst;
public:
	azGraphRect(SDL_Rect*rect,int type) { t = type;
	if (rect)
	{
		RECT_COPY(dst, *rect);
	}
	else {
		RECT_INIT(dst);
	}
	};
	~azGraphRect() {};
	void draw(azEngine*e)
	{
		switch (t)
		{
		case 1:
			e->DrawRect(&dst);
			break;
		case 2:
			e->FillRect(&dst);
			break;
		default:
			break;
		}
	}
};
class azGraphLine
	:public azGraph
{
	int x, y;
public:
	azGraphLine(int px, int py) {x = px, y = py; };
	~azGraphLine() {};
	void draw(azEngine*e)
	{
		e->DrawPoint(x, y);
	}
};
class azGraphPoint
	:public azGraph
{
	SDL_Point p1, p2;
public:
	azGraphPoint(SDL_Point*a, SDL_Point*b) { p1.x = a->x, p1.y = a->y; p2.x = b->x, p2.y = b->y;};
	~azGraphPoint() {};
	void draw(azEngine*e)
	{
		e->DrawLine(&p1, &p2);
	}
};
class azGroup :
	public RefObject
{
	azEngine*engine;
public:
	azGroup();
	~azGroup();
	void draw(azEngine*e);
	void remove(int index);
	void clear();

	int draw_tex(azTexture& tex, SDL_Rect*dst = nullptr, SDL_Rect*src = nullptr);
	int draw_tex2(azTexture& tex, SDL_Rect*dst=nullptr, SDL_Rect*src = nullptr,
		double angle=0.0,SDL_Point*center=nullptr,SDL_RendererFlip flip=SDL_FLIP_NONE);
	int draw_rect(SDL_Rect*rect,int type=1);
	int draw_point(SDL_Point*point);
	int draw_points(SDL_Point*points,int num);
	int draw_line(SDL_Point*begin, SDL_Point*end);
	int draw_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	int draw_color(SDL_Color color);
private:
	std::vector<azGraph*> list;
};

azGroup::azGroup()
{
}

azGroup::~azGroup()
{
	for (auto i : list)
	{
		delete i;
	}
}

inline void azGroup::draw(azEngine*e)
{
	for (auto i : list)
	{
		i->draw(e);
	}
}

inline void azGroup::remove(int index)
{
	for (auto it = list.begin(); it != list.end(); it++,index--)
	{
		if (index == 0)
		{
			list.erase(it);
		}
	}
}

inline void azGroup::clear()
{
	for (auto i : list)
	{
		delete i;
	}
	list.clear();
}

inline int azGroup::draw_tex(azTexture& tex, SDL_Rect * dst, SDL_Rect * src)
{
	list.push_back(new azGraphTex(tex,dst,src));
	return list.capacity()-1;
}

inline int azGroup::draw_tex2(azTexture& tex, SDL_Rect * dst, SDL_Rect * src, double angle, SDL_Point * center, SDL_RendererFlip flip)
{
	list.push_back(new azGraphTex2(tex, dst, src, angle, center, flip));
	return list.capacity() - 1;
}

inline int azGroup::draw_rect(SDL_Rect * rect,int type)
{
	list.push_back(new azGraphRect(rect,type));
	return list.capacity() - 1;
}

inline int azGroup::draw_color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	list.push_back(new azGraphColor(r, g, b, a));
	return list.capacity() - 1;
}
