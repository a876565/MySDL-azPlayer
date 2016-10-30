#pragma once
#include <vector>
#include <map>

class TEXCHAR {
public:
	wchar_t ch;
	azTexture tex;
	SDL_Rect loc;
};

class TEXMAP {
public:
	azTexture tex;
	int x, y;
	int hwrap;
};

class azEngine;
class azCFontMgr
{
	const int DefTexWidth = 512;
	const int DefTexHeight = 512;
	azEngine*e;
	TTF_Font*font;
	std::vector<TEXMAP> TexMaps;
	std::map<wchar_t, TEXCHAR>TexChs;
public:
	azCFontMgr(azEngine*en) :font(nullptr),e(en) {};
	int LoadFont(const std::string str, int ftsize);
	void CloseFont();
	int NewTexMap();
	int GetTexChar(TEXCHAR&tch);
	azTexture RenderTexText(const std::string str, SDL_Color color, int maxlength = 1024);
	azSurface RenderTexSur(const std::string str, SDL_Color color, int maxlength = 1024);
	void test_drawfonttexs(int id,int x,int y)
	{
		try {
			TEXMAP& tm = TexMaps.at(id);
			SDL_Rect r = { x,y,tm.tex->width(),tm.tex->height() };
			tm.tex->draw(&r);
		}
		catch (std::out_of_range e)
		{}
	}
	~azCFontMgr();
};

typedef std::shared_ptr<azCFontMgr> azFont;