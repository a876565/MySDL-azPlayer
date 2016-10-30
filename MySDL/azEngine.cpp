#include "azEngine.h"
#include "azFontMgr.h"


SDL_Color COLOR_NONE = { 0,0,0,0 };
SDL_Color COLOR_WHITE = { 255,255,255,255 };
SDL_Color COLOR_RED = { 255,0,0,255 };
SDL_Color COLOR_GREEN = { 0,255,0,255 };
SDL_Color COLOR_BLUE = { 0,0,255,255 };
SDL_Color COLOR_BLACK = { 0,0,0,255 };

#define SET_RECT(_x, _def)                                                     \
  if (rect._x == VAL_DEFAULT)                                                  \
    m_winrect._x = _def;                                                       \
  else if (rect._x > 0)                                                        \
    m_winrect._x = rect._x;


static bool libs_inited = false;
void azEngine::DeinitLibs()
{
	DBGLOG("azEngine::DeinitLibs()");
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	libs_inited = false;
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
	//��ʼ��SDL����չ��(�����Ҫ)
	if (!libs_inited)
		InitLibs();
	DBGLOG("azEngine::init()");
	//��������
	m_window = SDL_CreateWindow(m_title.c_str(), m_winrect.x, m_winrect.y,
		m_winrect.w, m_winrect.h, SDL_WINDOW_SHOWN);
	if (!m_window)
	{
		DBGLOGEX(Error, "CreateWindow:%s(%s)", m_title.c_str(), SDL_GetError());
		return;
	}
	DBGLOG("CreateWindow:%s", m_title.c_str());
	//������Ⱦ��
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if (!m_renderer)
	{
		DBGLOGEX(Error, "CreateRenderer(%s)", SDL_GetError());
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
		return;
	}
	DBGLOG("CreateRenderer", m_title.c_str());
	//���贰�ڴ�С��λ��
	//(android�±�������һ��
	//��ΪSDL���Զ�������ȫ��)
	get_winrect();
	resetArea();

	//��ʼ������
	DefaultFont = azFont(new azCFontMgr(this));
	DefaultFont->LoadFont(DEFAULT_FONT_NAME, DEFAULT_FONT_SIZE);
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
	}
}
void azEngine::test()
{
	
}