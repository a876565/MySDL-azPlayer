#pragma once
#include "tinyxml2.h"
#include "azText.h"
#include <map>
#include <string>
struct DmItem
{
	double time;
	int type;
	int size;
	std::string s;
	SDL_Color color;
	SDL_Rect r;
	azText*text;
};
class CDmMgr
{
	double time;
	double exist_time;
	SDL_Rect Area;
	int speed;
	tinyxml2::XMLDocument *xml;
	std::multimap<double, DmItem> dms;
	std::list<DmItem*>flying;
	std::list<DmItem*>top;
	std::list<DmItem*>bottom;
	azEngine*eng;
	void addDm(const char*param, const char*str);
public:
	void init();
	void Open(tinyxml2::XMLDocument*doc);
	void OpenFile(const std::string&path);
	void Close();
	void  topLayout(DmItem & i, std::list<DmItem*>& list);
	void  bottomLayout(DmItem & i,std::list<DmItem*>& list);
	void flyingLayout(DmItem & i, std::list<DmItem*>& list);
	void setArea(SDL_Rect *r);
	void pass(double to_time);
	void updateDm();
	void renderDm(const std::list<DmItem*>& list);
	void jump(double t);
	CDmMgr();
	~CDmMgr();
};

