#pragma once
#include "device.h"
#include "azTexture.h"
#include "azFontMgr.h"
#include <exception>

class azEngine
{
	const int DEFAULT_WIN_X = 32;
	const int DEFAULT_WIN_Y = 32;
	const int DEFAULT_WIN_W = 1024;
	const int DEFAULT_WIN_H = 768;

private:
	int redraw_need;

	Uint32 last_tick, tick, past_tick, tick_per;
	double fps_limit;

	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	std::string m_title;
	SDL_Rect m_winrect;
	SDL_Rect m_area;
	azFont DefaultFont;
public:
	//初始化SDL及扩展库
	static int InitLibs();
	//关闭并退出SDL及扩展库
	static void DeinitLibs();

	static azEngine* getSingleton();

	azEngine() : m_window(nullptr), m_renderer(nullptr), m_title("SDL"), fps_limit(30.0), tick_per(1000 / fps_limit)
	{
		m_winrect.x = DEFAULT_WIN_X;
		m_winrect.y = DEFAULT_WIN_Y;
		m_winrect.w = DEFAULT_WIN_W;
		m_winrect.h = DEFAULT_WIN_H;
	};
	~azEngine();

	//初始化与退出
	void init();
	void deinit();

	//一段测试
	void test();

	//载入纹理
	azTexture CreateTexture(int width, int height,
		Uint32 format = SDL_PIXELFORMAT_RGBA8888, int access = SDL_TEXTUREACCESS_STATIC)
	{

		SDL_Texture*tex = SDL_CreateTexture(m_renderer, format, access, width, height);
		if (tex)
		{
			return azTexture(new azCTexture(tex, m_renderer));
		}
		else
		{
			DBGLOGEX(Error, "Error create texture(w=%d,h=%d,format=%u,access=%d)%s",
				width, height, format, access, SDL_GetError());
			return azTexture();
		}
	}
	azTexture LoadTexture(const std::string&name);
	azCTexture *BuildTexture0(SDL_Surface*sur)
	{
		if (sur)
		{
			SDL_Texture*tex = SDL_CreateTextureFromSurface(m_renderer, sur);
			if (tex)
				return new azCTexture(tex, m_renderer);
		}
		return nullptr;
	}
	azTexture BuildTexture(SDL_Surface*sur)
	{
		if (sur)
		{
			SDL_Texture*tex = SDL_CreateTextureFromSurface(m_renderer, sur);
			if (tex)
				return azTexture(new azCTexture(tex, m_renderer));
		}
		return azTexture();
	}
	SDL_Renderer *getRenderer() { return m_renderer; }
	float GetDisplayDPI(float * ddpi = nullptr, float * hdpi = nullptr, float * vdpi = nullptr);

	inline int SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { return SDL_SetRenderDrawColor(m_renderer, r, g, b, a); }
	inline int SetColor(SDL_Color color) { return SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a); }

	inline void RenderFresh() {
		SDL_RenderPresent(m_renderer);
	}
	inline int RenderClear() { SetColor(COLOR_BLACK); return SDL_RenderClear(m_renderer); }

	void resetArea() {
		m_area.x = m_area.y = 0;
		m_area.w = m_winrect.w, m_area.h = m_winrect.h;
		SDL_RenderSetViewport(m_renderer, &m_area);
	}
	inline int MoveDrawer(int dx, int dy, SDL_Rect*old) {
		if (old)
			*old = m_area;
		m_area.x += dx;
		m_area.y += dy;
		m_area.w -= dx, m_area.h -= dy;
		return SDL_RenderSetViewport(m_renderer, &m_area);
	}
	inline int MoveDrawer(const SDL_Rect&area, SDL_Rect*old) {
		if (old)
			*old = m_area;
		m_area.x += area.x;
		m_area.y += area.y;
		m_area.w = area.w, m_area.h = area.h;
		return SDL_RenderSetViewport(m_renderer, &m_area);
	}
	inline int SetDrawer(const SDL_Rect&area, SDL_Rect*old = nullptr)
	{
		if (old)
			*old = m_area;
		m_area = area;
		return SDL_RenderSetViewport(m_renderer, &m_area);
	}
	inline void getDrawer(SDL_Rect&r) {r = m_area; }
	inline int BlendMode(SDL_BlendMode blend) { return SDL_SetRenderDrawBlendMode(m_renderer, blend); }
	inline int DrawRect(SDL_Rect*r) { return SDL_RenderDrawRect(m_renderer, r); }
	inline int DrawRects(SDL_Rect*r, int n) { return SDL_RenderDrawRects(m_renderer, r, n); }
	inline int FillRect(SDL_Rect*r) { return SDL_RenderFillRect(m_renderer, r); }
	inline int FillRects(SDL_Rect*r, int n) { return SDL_RenderFillRects(m_renderer, r, n); }
	inline int DrawPoint(int x, int y) { return SDL_RenderDrawPoint(m_renderer, x, y); }
	inline int DrawPoint(SDL_Point*p) { return SDL_RenderDrawPoint(m_renderer, p->x, p->y); }
	inline int DrawPoints(SDL_Point*p, int n) { return SDL_RenderDrawPoints(m_renderer, p, n); }
	inline int DrawLine(int x1, int y1, int x2, int y2) { return SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2); }
	inline int DrawLine(SDL_Point*p1, SDL_Point*p2) { return SDL_RenderDrawLine(m_renderer, p1->x, p1->y, p2->x, p2->y); }
	inline int DrawLines(SDL_Point*p, int n) { return SDL_RenderDrawLines(m_renderer, p, n); }

	//设置&获取标题
	inline void set_title(const std::string &title)
	{
		m_title = title;
		if (m_window)
			SDL_SetWindowTitle(m_window, m_title.c_str());
	}
	inline const std::string &get_title() { return m_title; }

	//设置&获取窗口大小位置
	void set_winrect(const SDL_Rect &rect);
	void setFullScreen(int mode);
	inline void update_winrect() {
		SDL_GetWindowSize(m_window, &m_winrect.w, &m_winrect.h);
		SDL_GetWindowPosition(m_window, &m_winrect.x, &m_winrect.y);
	}
	inline void winsized() {SDL_GetWindowSize(m_window, &m_winrect.w, &m_winrect.h);resetArea();};
	inline void winmoved() { SDL_GetWindowPosition(m_window, &m_winrect.x, &m_winrect.y); };
	inline void queryWindowSize() { SDL_GetWindowSize(m_window, &m_winrect.w, &m_winrect.h); }
	inline const SDL_Rect &get_winrect() { return m_winrect; }
	inline const SDL_Rect &area() { return m_area; }


	azFont& getDefaultFont() { return DefaultFont; };
	void setDefaultFont(azFont&font) {
		DefaultFont = font;
	};
	void SetFpsLimit(double fl)
	{
		fps_limit = fl;
		tick_per = 1000 / fps_limit;
	}
	void Tick() {
		last_tick = tick;
		tick = SDL_GetTicks();
		past_tick = tick - last_tick;
		if (past_tick < (1000 / fps_limit))
		{
			SDL_Delay((1000 / fps_limit) - past_tick);
			tick = SDL_GetTicks();
			past_tick = tick - last_tick;
		}
	}
	Uint32 GetTick() { return tick; }
	Uint32 PastTick() { return past_tick; }
};
class azDrawer {
public:
	virtual int draw(SDL_Rect*target) = 0;
	azDrawer() {};
	~azDrawer() {}
};
class DrawMove {
public:
	azEngine*e;
	SDL_Rect ori;
	DrawMove(azEngine*en, const SDL_Rect&r) :
		e(en)
	{
		e->MoveDrawer(r, &ori);
	};
	DrawMove(azEngine*en, int x, int y, int w, int h) :
		e(en)
	{
		SDL_Rect r = { x,y,w,h };
		e->MoveDrawer(r, &ori);
	}
	~DrawMove()
	{
		e->SetDrawer(ori);
	}
};