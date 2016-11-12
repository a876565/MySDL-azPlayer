#include "MyMusic.h"
#include <fstream>
#include <io.h>
#include "tinyxml2.h"
static const char*music_suffix[] = { "mp3","wav","fla",nullptr };
int is_suffix_file(const char*name,const char*suffix[])
{
	const char*p = name;
	const char*dot = nullptr;
	while (*p)
	{
		if (*p == '.')
			dot = p+1;
		p++;
	}
	if (dot == nullptr)
		return 0;

	for (int i=0;suffix[i];i++)
	{
		if (!_stricmp(dot, suffix[i]))
			return i+1;
	}
	return 0;
}


void MyMusic::run()
{
	char*s=setlocale(LC_ALL, "");
	
	engine = new azEngine();
	engine->init();
	engine->set_title("azMusic");

	engine->BlendMode(SDL_BLENDMODE_BLEND);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	init_songs();

	mainui();

	Mix_CloseAudio();
	delete engine;
}

void MyMusic::mainui()
{
	page = new azvLayout(engine);
	playbtn = new azvTextButton(page);
	searchbtn = new azvTextButton(page);
	info = new azvLabel(page);
	songlist = new azvListbox(page);
	page->setSize(engine->get_winrect().w, engine->get_winrect().h);
	page->back.tex = engine->LoadTexture("test.bmp");

	playbtn->setPos(96, page->area.h - 48);
	playbtn->setSize(96, 36);
	playbtn->text.setText(L"Í£Ö¹");
	playbtn->onClick = [this](azvButton&) {this->stopmusic(); };

	searchbtn->setPos(96+128, page->area.h - 48);
	searchbtn->setSize(128, 36);
	searchbtn->text.setText(L"ËÑË÷ÒôÀÖ");
	searchbtn->onClick = [this](azvButton&) {
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLElement *eles;
		tinyxml2::XMLElement *p;
		;
		doc.LoadFile(SONG_FILE_NAME);
		eles=doc.FirstChildElement("SearchSong");
		if (eles)
		{
			p = eles->FirstChildElement("Path");
			while (p)
			{
				const char*s = p->Attribute("path");
				if(s)
					this->search_songs(s);
				p = p->NextSiblingElement("Path");
			}
		}
	
	};

	songlist->setPos(0, 32);
	songlist->setSize(page->area.w / 2, page->area.h - 128);
	songlist->OnItemClick = [this](azvList&, int item) {if (item >= 0)this->playmusic(item); };
	songlist->back.color = COLOR(32, 32, 188, 64);
	setuplist();

	info->setPos(page->area.w / 2 + 32, 32);
	info->setSize(page->area.w / 2 - 64, page->area.h - 128);
	info->back.color = COLOR(63, 63, 0, 32);
	info->text.setText(L"Î´²¥·ÅÒôÀÖ");


	while (1)
	{
		engine->RenderClear();
		page->draw();
		engine->RenderFresh();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			page->onEvent(event);
			if (event.type == SDL_QUIT)
				goto end;
		}
		engine->Tick();
	}
end:
	delete page;
}

int MyMusic::playmusic(int i)
{
	if (i < 0 || (size_t)i >= songs.size())
	{
		stopmusic();
		return -1;
	}
	if (music)
		stopmusic();

	nowsongid = i;
	music = Mix_LoadMUS(songs[nowsongid].c_str());
	if (music)
	{
		std::wstring songname;
		Mix_PlayMusic(music, -1);
		songlist->SetItemColor(nowsongid, COLOR_BLUE);
		DBGLOG("Mix_LoadMUS %s", songs[nowsongid].c_str());
		u8stows(songname, songs[nowsongid].c_str());

		info->text.setText(L"ÕýÔÚ²¥·Å£º"+songname);
	}
	else
	{
		info->text.setText(SDL_GetError());
		DBGLOG("Mix_LoadMUS Error %s", SDL_GetError());
	}
	return 0;
}


int MyMusic::stopmusic()
{
	if (music)
	{
		songlist->SetItemColor(nowsongid, COLOR_WHITE);
		info->text.setText(L"Î´²¥·ÅÒôÀÖ");

		Mix_HaltMusic();
		Mix_FreeMusic(music);

		music = nullptr;
		nowsongid = -1;
	}
	return 0;
}

void MyMusic::load_config()
{
}

void MyMusic::init_songs()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement*song_ele;
	tinyxml2::XMLElement*song_path;
	doc.LoadFile(SONG_FILE_NAME);
	song_ele = doc.FirstChildElement("Songs");
	if (!song_ele)
		return;
	song_path = song_ele->FirstChildElement("Path");
	while (song_path)
	{
		const char*s = song_path->Attribute("path");
		if (s)
			songs.push_back(s);
		song_path = song_path->NextSiblingElement("Path");
	}

}

void MyMusic::save_songs()
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement*song_ele;
	doc.LoadFile(SONG_FILE_NAME);

	song_ele=doc.FirstChildElement("Songs");
	if (song_ele)
		song_ele->DeleteChildren();
	else
	{
		song_ele = doc.NewElement("Songs");
		doc.LinkEndChild(song_ele);
	}

	for (auto&s : songs)
	{
		tinyxml2::XMLElement*song_name = doc.NewElement("Path");
		song_name->SetAttribute("path", s.c_str());
		song_ele->LinkEndChild(song_name);
	}
	doc.SaveFile(SONG_FILE_NAME);
}

void MyMusic::search_songs(const std::string & path)
{
	std::string serpath;
	bool songadded = false;

	u8stocs(serpath, path.c_str());
	serpath+=DIR_SEPS"*.*";
	_finddata_t FileInfo;
	long handle = _findfirst(serpath.c_str(), &FileInfo);
	if (handle == -1)
		return;
	do {
			std::string s(path);
			s += DIR_SEP;
			s += FileInfo.name;
			if (is_suffix_file(s.c_str(), music_suffix))
			{
				std::wstring ws;
				size_t beg;
				cstows(ws, s.c_str());
				wstou8s(s, ws.c_str());
				
				beg = ws.find_last_of(DIR_SEP);
				if (beg == std::string::npos)
					add_song(s,ws);
				else
					add_song(s, ws.substr(beg+1));

				songadded = true;
			}
	} while (_findnext(handle, &FileInfo) == 0);
	if (songadded)
		save_songs();
	_findclose(handle);
}

void MyMusic::show_dir_choose(std::string & path)
{

}

void MyMusic::add_song(const std::string & path,const std::wstring&name)
{
	auto it = std::find(songs.begin(), songs.end(), path);
	if (it == songs.end())
	{
		songs.push_back(path);
		songlist->InsertText(name);
	}
}


// Í¨¹ý UIPage ¼Ì³Ð

void MainPage::setup()
{
}

void MainPage::destroy()
{
}


void MyMusic::setuplist()
{
	songlist->Clear();
	for (auto&s : songs)
	{
		size_t beg;
		beg= s.find_last_of(DIR_SEP);

		std::wstring ws;
		if (beg ==std::string::npos)beg = 0;else beg++;
		u8stows(ws, s.substr(beg).c_str());

		songlist->InsertText(ws);
	}
}
