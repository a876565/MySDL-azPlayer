#pragma once
#ifdef _WIN32
extern"C"{
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4244)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
}
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <memory>
#include <string>

#ifdef _DEBUG
enum azLogCategory {
	azLogCategoryEngine = SDL_LOG_CATEGORY_CUSTOM,
	azLogCategory1
};
#define DBGLOG(...) SDL_Log(__VA_ARGS__)
#define DBGLOGEX(pri,...) SDL_Log##pri(azLogCategoryEngine,__VA_ARGS__)
#else

#define DBGLOG(...) 
#define DBGLOGEX(pri,...) 
#endif

#define VAL_NOT_CARE 0
#define VAL_DEFAULT -1

#define SDL_INIT_FLAG SDL_INIT_VIDEO
#define SDL_INIT_WINDOW SDL_SHOWN
#define SDL_INIT_RENDERER
#define IMG_INIT_FLAG IMG_INIT_JPG|IMG_INIT_PNG

#define RECT_COPY(dst,src) (dst).x=(src).x,(dst).y=(src).y,(dst).w=(src).w,(dst).h=(src).h;
#define RECT_INIT(dst) (dst).x=(dst).y=(dst).w=(dst).h=0;

#define COLOR_INIT(c) (c).r=(c).g=(c).b=(c).a=255;
#define COLOR_SET(c,r,g,b,a) (c).r=r,(c).g=g,(c).b=b,(c).a=a;

#define DEFAULT_FONT_NAME "simhei.ttf"
#define DEFAULT_FONT_SIZE 16

const std::wstring nullwstr(L"");

extern SDL_Color COLOR_NONE,COLOR_WHITE,COLOR_RED,COLOR_GREEN,COLOR_BLUE,COLOR_BLACK;
inline SDL_Color COLOR(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_Color c = { r,g,b,a };
	return c;
}
inline void CenterRect(SDL_Rect*r, const SDL_Rect*src)
{
	SDL_assert(r&&src);
	r->x = (src->w - r->w) / 2;
	r->y = (src->h - r->h) / 2;
}
inline bool isPointInRect(int x,int y, const SDL_Rect &r)
{
	return ((x > r.x) && (x < (r.x + r.w)) &&
		(y > r.y) && (y < (r.y + r.h))) ? true : false;
}
inline int is_azerr(const char *s)
{
	if(s[0]>='0'&&s[0]<='9'&&s[1] >= '0'&&s[1] <= '9')
	{ 
		return (s[0] - '0') * 10 + (s[1] - '0');
	}
	else
	{
		return -1;
	}
}

inline std::string ws2s(const std::wstring& ws)
{

	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	int e = wcstombs(_Dest, _Source, _Dsize);
	std::string result(_Dest);
	delete[]_Dest;


	return result;
}