#pragma once
#include "device.h"
#include <string>
class azCSurface
{
	SDL_Surface *m_sur;
public:
	azCSurface() : m_sur(nullptr) {};
	azCSurface(SDL_Surface*sur) : m_sur(sur) {};
	int LoadBMP(const std::string &name)
	{
		clean();
		m_sur = SDL_LoadBMP(name.c_str());
		if (m_sur == nullptr)
			return -1;
		return 0;
	}
	int LoadImage(const std::string &name)
	{
		clean();
		m_sur = IMG_Load(name.c_str());
		if (m_sur == nullptr)
			return -1;
		return 0;
	}
	int height()
	{
		return m_sur->h;
	}
	int width()
	{
		return m_sur->w;
	}
	void save(const std::string &name)
	{
		SDL_SaveBMP(m_sur, name.c_str());
	}
	void*pixels() { return m_sur->pixels; }
	int pitch() {return m_sur->pitch;}
	void clean()
	{
		if (m_sur)
			SDL_FreeSurface(m_sur);
	}
	~azCSurface()
	{
		clean();
	}
};
#if 0
typedef RefPtr<azCSurface> azSurface;
#else
typedef std::shared_ptr<azCSurface> azSurface;
#endif

class azCTexture
{
	int m_w, m_h;
	/*
	int access;*/
	Uint32 m_format;
	SDL_Texture *m_tex;
	SDL_Renderer*m_ren;
public:
	azCTexture(SDL_Texture *tex, SDL_Renderer*ren) : m_tex(tex), m_ren(ren) {
		SDL_QueryTexture(m_tex, &m_format, nullptr, &m_w, &m_h);
	}
	void draw(SDL_Rect*dst = nullptr, SDL_Rect*src = nullptr)
	{
		SDL_assert((m_tex&&m_ren));
		SDL_RenderCopy(m_ren, m_tex, src, dst);
	}
	void draw2(SDL_Rect*dst = nullptr, SDL_Rect*src = nullptr, double angle = 0,
		SDL_Point*pot = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		SDL_assert((m_tex&&m_ren));
		SDL_RenderCopyEx(m_ren, m_tex, src, dst, angle, pot, flip);
	}
	void getSize(int*pw, int*ph)
	{
		SDL_QueryTexture(m_tex, &m_format, nullptr, &m_w, &m_h);
		if (pw)
			*pw = m_w;
		if (ph)
			*ph = m_h;
	}
	int update(void*pixels,SDL_Rect*pRect, int pitch)
	{
		return SDL_UpdateTexture(m_tex, pRect, pixels,pitch);
	}
	int update(azSurface sur,int x,int y)
	{
		SDL_Rect r = { x,y,sur->width(),sur->height() };
		return SDL_UpdateTexture(m_tex, &r,sur->pixels(), sur->pitch());
	}
	int setBlend(SDL_BlendMode blmod) {return SDL_SetTextureBlendMode(m_tex, blmod); }
	int setAlpha(Uint8 a) { return SDL_SetTextureAlphaMod(m_tex, a); };
	int setColor(Uint8 r, Uint8 g, Uint8 b) { return SDL_SetTextureColorMod(m_tex, r, g, b); }
	int width() { return m_w; }
	int height() { return m_h; }
	Uint32 format() { return m_format; };
	~azCTexture()
	{
		if (m_tex)
			SDL_DestroyTexture(m_tex);
	};
};

#if 0
typedef RefPtr<azCTexture> azTexture;
#else
typedef std::shared_ptr<azCTexture> azTexture;
#endif