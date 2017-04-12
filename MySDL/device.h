#pragma once
#ifdef _WIN32
//#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4244)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <memory>
#include <string>

#ifdef _DEBUG
#define DBGLOG(...) SDL_Log(__VA_ARGS__)
#define DBGLOGEX(pri,...) SDL_Log##pri(SDL_LOG_CATEGORY_ERROR,__VA_ARGS__)
#else

#define DBGLOG(...) 
#define DBGLOGEX(pri,...) 
#endif

#define VAL_NOT_CARE 0
#define VAL_DEFAULT -1

#define SDL_INIT_FLAG SDL_INIT_VIDEO|SDL_INIT_AUDIO
#define SDL_INIT_WINDOW SDL_SHOWN
#define SDL_INIT_RENDERER
#define IMG_INIT_FLAG IMG_INIT_JPG|IMG_INIT_PNG
#define MIX_INIT_FLAG MIX_INIT_FLAC|MIX_INIT_MP3

#define RECT_COPY(dst,src) (dst).x=(src).x,(dst).y=(src).y,(dst).w=(src).w,(dst).h=(src).h;
#define RECT_INIT(dst) (dst).x=(dst).y=(dst).w=(dst).h=0;

#define COLOR_INIT(c) (c).r=(c).g=(c).b=(c).a=255;
#define COLOR_SET(c,r,g,b,a) (c).r=r,(c).g=g,(c).b=b,(c).a=a;


#define DEFAULT_FONT_NAME "simhei.ttf"
#define DEFAULT_FONT_SIZE 24

std::string getDataPath(const std::string&name);

template<class FileStream>
inline void  skip_utf8_bom(FileStream&f)
{
	char bom[3];
	f.read(bom, 3);
	if ((unsigned char)bom[0] != 0xef || (unsigned char)bom[1] != 0xbb || (unsigned char)bom[2] != 0xbf)
	{
		f.seekg(0, std::ios::beg);
	}
}

const std::wstring nullwstr(L"");

extern SDL_Color COLOR_NONE,COLOR_WHITE,COLOR_RED,COLOR_GREEN,COLOR_BLUE,COLOR_BLACK,COLOR_GREY,COLOR_DARK;
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


size_t cstows(std::wstring&ws, const char *pc);
size_t wstocs(std::string&cs, const wchar_t * pw);

size_t u8stows(std::wstring&ws, const char *pc);
size_t wstou8s(std::string&cs, const wchar_t * pw);

#ifdef _WIN32

#define STRTOUTF8(s) {std::wstring ws;cstows(ws,s.c_str());wstou8s(s,ws.c_str());}
#define DIR_SEP '\\'
#define DIR_SEPS "\\"

inline size_t u8stocs(std::string & cs, const char * u8s)
{
	size_t sz;
	std::wstring ws;
	sz = u8stows(ws, u8s);
	if (sz == 0)
		return 0;
	sz = wstocs(cs, ws.c_str());
	return sz;
}

inline size_t cstou8s(std::string & u8s, const char * cs)
{
	size_t sz;
	std::wstring ws;
	sz = cstows(ws, cs);
	if (sz == 0)
		return 0;
	sz = wstocs(u8s, ws.c_str());
	return sz;
}

#else

#define STRTOUTF8(s)
#define DIR_SEP '/'
#define DIR_SEPS "/"

inline size_t u8stocs(std::string & cs, const char * u8s)
{
	cs = u8s;
	return u8s.length();
}

inline size_t cstou8s(std::string & u8s, const char * cs)
{
	u8s = cs;
	return u8s.length();
}
#endif