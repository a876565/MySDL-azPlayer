#pragma once
#include "azEngine.h"
#include <list>
class azTextBase:
	public azDrawer
{
protected:
	SDL_Rect area;
	azEngine*e;
	azFont font;
	std::wstring text;
	SDL_Color color;
public:
	virtual void setText(const std::wstring&s)=0;
	const std::wstring&getText() {return text;}

	virtual void setColor(SDL_Color  col);
	SDL_Color getColor() { return color; }

	virtual void setFont(azFont font);
	const azFont&getFont() { return font; }

	virtual void setSize(int w, int h)=0;
	virtual void move(int dx, int dy);
	azTextBase(azEngine*en):e(en), color(COLOR_WHITE) {};
	~azTextBase() {};
};
enum {
	MARGIN_LEFT=1,
	MARGIN_MEDIUM = 2,
	MARGIN_RIGHT = 4,
};
enum {
	TEXCHAR_SHOW,
	TEXCHAR_LINE_END,
	TEXCHAR_TEXT_END
};
class TextLine {
	std::vector<TEXCHAR>tchs;
	int w, h, margin;
};
class azText :
	public azTextBase
{
	bool changed;
	int width, height;
	int now_x, now_y;
	int real_w, real_h;
	std::vector<TEXCHAR>tchs;
	std::list<TextLine>lines;
public:

	azText(azEngine * en);
	virtual ~azText();

	// Í¨¹ý azTextBase ¼Ì³Ð
	virtual int draw(int x,int y);
	virtual int draw(SDL_Rect * target) override;
	virtual void setText(const std::wstring&s) override;
	virtual void setSize(int w, int h);

	void setText(const std::string&s) { u8stows(text, s.c_str()); updateText(); }
	void updateText();
	void updateLayout();
	int getHeight() {
		if (changed)
			updateLayout();
		return real_h;
	};
	int getWidth(){
		if (changed)
			updateLayout();
		return real_w;
	}
	
};

