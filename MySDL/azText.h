#pragma once
#include "azEngine.h"
class azTextBase:
	public azDrawer
{
protected:
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

	virtual int CalcHeight(int width);
	azTextBase(azEngine*en):e(en), color(COLOR_WHITE) {};
	~azTextBase() {};
};

class azText :
	public azTextBase
{
	std::vector<TEXCHAR>tchs;
public:

	azText(azEngine * en);
	virtual ~azText();

	// Í¨¹ý azTextBase ¼Ì³Ð
	virtual int draw(SDL_Rect * target) override;
	virtual void setText(const std::wstring&s) override;
	void setText(const std::string&s) { cstows(text, s.c_str()); updateText(); }
	void updateText();
	virtual int CalcHeight(int width) override;
	
};

