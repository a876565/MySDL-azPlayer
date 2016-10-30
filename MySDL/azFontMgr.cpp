#include "azEngine.h"
#include "azFontMgr.h"


int azCFontMgr::LoadFont(const std::string str,int ftsize)
{
	if (font)
		CloseFont();
	font = TTF_OpenFont(str.c_str(), ftsize);
	if (!font)
	{
		DBGLOGEX(Error, "TTF_OpenFont:%s(%s)", str.c_str(), SDL_GetError());
		return 1;
	}
	return 0;
}

void azCFontMgr::CloseFont()
{
	if (font)
	{
		TexChs.clear();
		TexMaps.clear();
		TTF_CloseFont(font);
		font = nullptr;
	}
}
int azCFontMgr::NewTexMap()
{
	TEXMAP tm;
	azTexture tex;
	tex= e->CreateTexture(DefTexWidth, DefTexHeight, SDL_PIXELFORMAT_ARGB8888);
	if(!tex)
		tex = e->CreateTexture(DefTexWidth, DefTexHeight, SDL_PIXELFORMAT_RGBA8888);
	if (!tex)
		return 1;
	tex->setBlend(SDL_BLENDMODE_BLEND);
	tm.tex = tex;
	tm.x = tm.y = 0;
	TexMaps.push_back(tm);
	return 0;
}
int azCFontMgr::GetTexChar(TEXCHAR & tch)
{
	static int num = 1;
	auto it = TexChs.find(tch.ch);
	if (it != TexChs.end())
	{
		tch = (*it).second;
		return 0;
	}
	if (TexMaps.empty())
	{
		if (NewTexMap()<0)
			return -1;
	}
	TEXMAP &tm = *(TexMaps.rbegin());
	azSurface sur=azSurface(new azCSurface(TTF_RenderGlyph_Blended(font, tch.ch, COLOR_WHITE)));
	union PIXEL {
		unsigned char p[4];
		struct {
			unsigned char r, g, b, a;
		};
	};

	if (tm.tex->format() == SDL_PIXELFORMAT_RGBA8888)
	{
		PIXEL *p = (PIXEL*)sur->pixels();

		for (int i = 0; i < sur->width()*sur->height(); i++)
		{

			auto t = p[i].p[0];
			p[i].p[0] = p[i].p[3];
			p[i].p[3] = t;
		}
	}
	if (sur->height() > tm.hwrap)
	{
		tm.hwrap = sur->height();
	}
	if (!sur)
	{
		DBGLOGEX(Error, "Error TTF_RenderGlyph_Blended (ch=%04x) %s", tch.ch, SDL_GetError());
		return -1;
	}
	if (sur->width() + tm.x > tm.tex->width())
	{
		if (tm.hwrap + tm.y > tm.tex->height())
		{
			if (NewTexMap()<0)
				return -1;
			tm = *(TexMaps.end() - 1);
			tm.hwrap = sur->height();
		}
		else
		{
			tm.x = 0;
			tm.y += tm.hwrap;
			tm.hwrap = sur->height();
		}
	}

	tm.tex->update(sur, tm.x, tm.y);


	TEXCHAR tc;
	tc.loc.x = tm.x;
	tc.loc.y = tm.y;
	tc.loc.w = sur->width();
	tc.loc.h = sur->height();
	tc.tex = tm.tex;

	TexChs.insert(std::pair<wchar_t, TEXCHAR>(tch.ch, tc));
	tch = tc;
	tm.x += sur->width();
	return 0;
}


azTexture azCFontMgr::RenderTexText(const std::string str,SDL_Color color,int maxlength)
{
	if(!font)
		return azTexture();
	SDL_Surface*sur=TTF_RenderText_Blended_Wrapped(font, str.c_str(), color, maxlength);
	if (!sur)
	{
		DBGLOGEX(Error, "TTF_RenderText:%s(%s)", str.c_str(), SDL_GetError());
		return azTexture();
	}
	azCTexture* tex = e->BuildTexture0(sur);
	SDL_FreeSurface(sur);
	if(!tex)
	{
		DBGLOGEX(Error, "BuildTexture:%s(%s)", str.c_str(), SDL_GetError());
	}
	return azTexture(tex);
}

azSurface azCFontMgr::RenderTexSur(const std::string str, SDL_Color color, int maxlength)
{
	if (!font)
		return azSurface();
	SDL_Surface*sur = TTF_RenderText_Blended_Wrapped(font, str.c_str(), color, maxlength);
	if (!sur)
		return azSurface();
	return azSurface(new azCSurface(sur));
}

azCFontMgr::~azCFontMgr()
{
	CloseFont();
}