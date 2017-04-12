#include "DmMgr.h"

using namespace tinyxml2;
using std::string;
#define GETSUB(ch,x) \
					 q = s.find_first_of(',',p);\
					 if (q == string::npos)\
						return;\
					 x = s.substr(p, q-p);\
					 p=q+1;
SDL_Color ColorFromInt(unsigned a)
{
	SDL_Color c = { (a >> 16) & 0xff, (a >> 8) & 0xff ,(a) & 0xff,0xff };
	return c;
}
int getDmType(int a)
{
	switch (a)
	{
	case 1:
	case 2:
	case 3:
		return 1;
	case 4:
		return 2;
	case 5:
		return 3;
	default:
		return 1;
	}
}
void CDmMgr::addDm(const char * param, const char * str)
{
	if (!param || !str)
		return;

	if (*param == 0 || *str == 0)
		return;
	string s(param), sTime, sType, sSize, sColor;
	int p, q;
	DmItem dm;
	p = q = 0;
	GETSUB(',', sTime);
	GETSUB(',', sType);
	GETSUB(',', sSize);
	GETSUB(',', sColor);
	dm.s = str;
	dm.time = strtod(sTime.c_str(), nullptr);
	dm.type = getDmType(strtol(sType.c_str(), nullptr, 0));
	dm.size = strtol(sSize.c_str(), nullptr, 0);
	dm.color = ColorFromInt(strtol(sColor.c_str(), nullptr, 0));
	dm.text = nullptr;
	dms.insert(std::pair<double, DmItem>(dm.time, dm));
}

void CDmMgr::init()
{
	eng = azEngine::getSingleton();
	Area = eng->get_winrect();
	Area.x = Area.y = 0;

	float hdpi = 0.0f;
	eng->GetDisplayDPI(nullptr, &hdpi);
	speed = hdpi*2;
	if (speed <= 0)
		speed = 128;
	exist_time = 6.0;
}

void CDmMgr::Open(tinyxml2::XMLDocument * doc)
{
	time = 0.0;
	XMLElement *ele = doc->FirstChildElement();
	if (!SDL_strcmp(ele->Name(), "i"))
		ele = ele->FirstChildElement("d");
	while (ele)
	{
		if (!SDL_strcmp(ele->Name(), "d"))
		{
			addDm(ele->Attribute("p"), ele->GetText());
		}
		ele = ele->NextSiblingElement();
	}
	delete doc;
}

void CDmMgr::OpenFile(const std::string & path)
{
	XMLDocument *doc = new XMLDocument();
	if (doc->LoadFile(path.c_str()) == tinyxml2::XML_SUCCESS)
	{
		DBGLOG("Open danmu:%s", path.c_str());
		Open(doc);
	}
	else
	{
		DBGLOG("Failed open danmu:%s", path.c_str());
	}
}
#define SETAREA(list) for (auto i : list)\
{\
	i->r.x = i->r.x*r->w / Area.w;\
	i->r.y = i->r.y*r->h / Area.h;\
}
void CDmMgr::setArea(const SDL_Rect * r)
{
	for (auto i : flying)
	{
	}
	for (auto i : bottom)
	{
		i->r.x = r->x+(r->w - i->r.w) / 2;
		i->r.y += r->y+r->h -Area.y- Area.h;
	}
	for (auto i : top)
	{
		i->r.x = r->x+(r->w - i->r.w) / 2;
	}
	Area = *r;
}

void CDmMgr::Close()
{
	for (auto i : dms)
	{
		if (i.second.text)
			delete(i.second.text);
	}
	dms.clear();
	flying.clear();
	top.clear();
	bottom.clear();

	time = 0.0;
}
void CDmMgr::topLayout(DmItem&i, std::list<DmItem*>&list)
{
	auto insertItem = list.end();

	for (auto j = list.begin(); j != list.end(); j++)
	{
		auto v = *j;
		//ÖØµþ¼ì²â
		if (i.r.y >= v->r.y&&i.r.y < v->r.y + i.r.h)
		{
			insertItem = j;
			i.r.y = v->r.y + v->r.h;
			if (i.r.y + i.r.h > Area.y + Area.h)
			{
				i.r.y = Area.y;
			}
		}
		else if (i.r.y + i.r.h <= v->r.y)
		{
			break;
		}
	}
	if (insertItem == list.end())
		list.push_front(&i);
	else
		list.insert(++insertItem, &i);
}
void CDmMgr::bottomLayout(DmItem&i, std::list<DmItem*>&list)
{
	auto insertItem = list.end();

	for (auto j = list.begin(); j != list.end(); j++)
	{
		auto v = *j;
		//ÖØµþ¼ì²â
		if (i.r.y >= v->r.y&&i.r.y < v->r.y + i.r.h)
		{
			insertItem = j;
			i.r.y = v->r.y - i.r.h;
			if (i.r.y < Area.y)
			{
				i.r.y = Area.y + Area.h - i.r.h;
			}
		}
		else if (v->r.y+v->r.h>=i.r.y)
		{
			break;
		}
	}
	if (insertItem == list.end())
		list.push_front(&i);
	else
		list.insert(++insertItem, &i);
}
void CDmMgr::flyingLayout(DmItem & i, std::list<DmItem*>& list)
{
	auto insertItem = list.end();

	for (auto j = list.begin(); j != list.end(); j++)
	{
		auto v = *j;
		//ÖØµþ¼ì²â
		if (v->r.x + v->r.w > i.r.x)
		{
			if (i.r.y >= v->r.y&&i.r.y < v->r.y + i.r.h)
			{
				insertItem = j;
				i.r.y = v->r.y + v->r.h;
				if (i.r.y + i.r.h > Area.y + Area.h)
				{
					i.r.y = Area.y;
				}
			}
			else if (i.r.y + i.r.h <= v->r.y)
			{
				break;
			}
		}
	}
	if (insertItem == list.end())
		list.push_front(&i);
	else
		list.insert(++insertItem, &i);
}
#define REMOVEBYTIME(list) for (auto i = list.begin(); i != list.end(); ){\
	if ((*i)->time+exist_time < to_time){\
		delete (*i)->text;\
		(*i)->text = nullptr;\
		i = list.erase(i); \
	}\
	else\
		i++;\
}
void CDmMgr::pass(double to_time)
{
	if (to_time < time)
	{
		clearDm();
		if(to_time>=0.0)
			time = to_time;
		else 
			time = 0.0;
		return;
	}
	int past = speed*(to_time - time);
	for (auto i = flying.begin(); i != flying.end();)
	{
		(*i)->r.x -= past;

		if ((*i)->r.x + (*i)->r.w < Area.x)
		{
			delete (*i)->text;
			(*i)->text = nullptr;
			i = flying.erase(i);
		}
		else
		{
			i++;
		}
	}
	REMOVEBYTIME(top);
	REMOVEBYTIME(bottom);


	auto beg = dms.upper_bound(time);
	auto end = dms.upper_bound(to_time);
	for (auto i = beg; i != end; i++)
	{
		azText*t = new azText(eng);
		t->setColor(i->second.color);
		t->setText(i->second.s);
		t->setSize(65536, 65536);
		i->second.text = t;
		i->second.r.w = t->getWidth();
		i->second.r.h = t->getHeight();
		switch (i->second.type)
		{
		case 1:
			i->second.r.x = Area.x + Area.w;
			i->second.r.y = Area.y;
			flyingLayout(i->second, flying);
			break;
		case 2:
			i->second.r.x = Area.x + (Area.w - i->second.r.w) / 2;
			i->second.r.y = Area.y +Area.h- i->second.r.h;
			bottomLayout(i->second, bottom);
			break;
		case 3:
			i->second.r.x = Area.x + (Area.w - i->second.r.w) / 2;
			i->second.r.y = Area.y;
			topLayout(i->second, top);
			break;
		default:
			break;
		}
	}
	time = to_time;

	renderDm(flying);
	renderDm(top);
	renderDm(bottom);
}

void CDmMgr::updateDm()
{
}

void CDmMgr::renderDm(const std::list<DmItem*>& list)
{
	for (auto i : list)
	{
		i->text->draw(i->r.x, i->r.y);
	}
}

#define CLEARDM(list) for(auto i:list){if(i->text)delete i->text;i->text=nullptr;}list.clear();
void CDmMgr::clearDm()
{
	CLEARDM(flying);
	CLEARDM(top);
	CLEARDM(bottom);
}

void CDmMgr::jump(double t)
{
}

CDmMgr::CDmMgr()
{
}


CDmMgr::~CDmMgr()
{
}
