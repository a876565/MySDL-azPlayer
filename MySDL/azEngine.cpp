#include "azEngine.h"
#include "azFontMgr.h"


SDL_Color COLOR_NONE = { 0,0,0,0 };
SDL_Color COLOR_WHITE = { 255,255,255,255 };
SDL_Color COLOR_RED = { 255,0,0,255 };
SDL_Color COLOR_GREEN = { 0,255,0,255 };
SDL_Color COLOR_BLUE = { 0,0,255,255 };
SDL_Color COLOR_BLACK = { 0,0,0,255 };
SDL_Color COLOR_GREY = { 0x66,0x66,0x66,255 };
SDL_Color COLOR_DARK = { 0x43,0x43,0x43,255 };
#define SET_RECT(_x, _def)                                                     \
  if (rect._x == VAL_DEFAULT)                                                  \
    m_winrect._x = _def;                                                       \
  else if (rect._x > 0)                                                        \
    m_winrect._x = rect._x;


static bool libs_inited = false;
static azEngine*singleton=nullptr;

void azEngine::DeinitLibs()
{
	DBGLOG("azEngine::DeinitLibs()");
	IMG_Quit();
	TTF_Quit();
	//Mix_Quit();
	SDL_Quit();
	libs_inited = false;
}
azEngine * azEngine::getSingleton()
{
	return singleton;
}
int azEngine::InitLibs()
{
	DBGLOG("azEngine::InitLibs()");
	int ret = 0;
	if (SDL_Init(SDL_INIT_FLAG) < 0)
	{
		DBGLOG("Error Init SDL %s\n", SDL_GetError());
		return -1;
	}
	if (TTF_Init()< 0)
	{
		DBGLOG("Error Init TTF %s\n", SDL_GetError());
	}
	if (IMG_Init(IMG_INIT_FLAG) < 0)
	{
		ret++;
		DBGLOG("Error Init IMG %s\n", SDL_GetError());
	}
	/*if (Mix_Init(MIX_INIT_FLAG) < 0)
	{
		ret++;
	}*/
	libs_inited = true;
	return ret;
}
azEngine::~azEngine() 
{ 
	deinit(); 
}
void azEngine::deinit()
{
	DBGLOG("azEngine::deinit()");
	DefaultFont->CloseFont();
	if (m_renderer)
		SDL_DestroyRenderer(m_renderer);
	if (m_window)
		SDL_DestroyWindow(m_window);
	m_window = nullptr;
	m_renderer = nullptr;
}
void azEngine::init()
{
	//初始化SDL及扩展库(如果需要)
	if (!libs_inited)
		InitLibs();
	DBGLOG("azEngine::init()");
	//创建窗口
	m_window = SDL_CreateWindow(m_title.c_str(), m_winrect.x, m_winrect.y,
		m_winrect.w, m_winrect.h, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if (!m_window)
	{
		DBGLOGEX(Error, "CreateWindow:%s(%s)", m_title.c_str(), SDL_GetError());
		return;
	}
	DBGLOG("CreateWindow:%s", m_title.c_str());
	//创建渲染器
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_TARGETTEXTURE);
	if (!m_renderer)
	{
		DBGLOGEX(Error, "CreateRenderer(%s)", SDL_GetError());
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
		return;
	}
	DBGLOG("CreateRenderer", m_title.c_str());
	//重设窗口大小和位置
	//(android下必须有这一步
	//因为SDL会自动调整到全屏)
	queryWindowSize();
	resetArea();

	//初始化字体
	DefaultFont = azFont(new azCFontMgr(this));
	DefaultFont->LoadFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE);

	singleton = this;
}
azTexture azEngine::LoadTexture(const std::string&name)
{
	SDL_Texture *pTex = IMG_LoadTexture(m_renderer, name.c_str());
	if (!pTex)
	{
		DBGLOGEX(Error, "CreateTexture(%s)",  SDL_GetError());
		return azTexture();
	}
	else{
		DBGLOG("CreateTexture:[%#p] %s",pTex,name.c_str());
		return azTexture(new azCTexture(pTex, m_renderer));
	}
}
float azEngine::GetDisplayDPI(float * ddpi, float * hdpi, float * vdpi)
{
	float d = 0.0f;
	SDL_GetDisplayDPI(SDL_GetWindowDisplayIndex(m_window), &d, hdpi, vdpi);
	if (ddpi)
		*ddpi = d;
	return d;
}
void azEngine::set_winrect(const SDL_Rect & rect)
{
	SET_RECT(x, DEFAULT_WIN_X);
	SET_RECT(y, DEFAULT_WIN_Y);
	SET_RECT(w, DEFAULT_WIN_W);
	SET_RECT(h, DEFAULT_WIN_H);
	if (m_window)
	{
		SDL_SetWindowPosition(m_window, m_winrect.x, m_winrect.y);
		SDL_SetWindowSize(m_window, m_winrect.w, m_winrect.h);
		resetArea();
	}
}
void azEngine::setFullScreen(int mode)
{
	switch (mode)
	{
	case -1:
		if (SDL_GetWindowFlags(m_window)&SDL_WINDOW_FULLSCREEN)
			SDL_SetWindowFullscreen(m_window, 0);
		else
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	case 0:
		SDL_SetWindowFullscreen(m_window,0);
	case 1:
		SDL_SetWindowFullscreen(m_window,SDL_WINDOW_FULLSCREEN_DESKTOP);
	default:
		break;
	}
	queryWindowSize();
	resetArea();
}
void azEngine::test()
{
	
}