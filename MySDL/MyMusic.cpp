#include "MyMusic.h"
#include <fstream>
void MyMusic::run()
{
	char*s=setlocale(LC_ALL, "");
	
	engine = new azEngine();
	engine->init();
	engine->set_title("azMusic");

	engine->getDefaultFont()->LoadFont("simhei.ttf", 24);
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
	playbtn->text.setText(L"Õ£÷π");
	playbtn->onClick = [this](azvButton&) {this->stopmusic(); };

	searchbtn->setPos(96+128, page->area.h - 48);
	searchbtn->setSize(128, 36);
	searchbtn->text.setText(L"À—À˜“Ù¿÷");
	searchbtn->onClick = [this](azvButton&) {this->search_songs("."); };

	songlist->setPos(0, 32);
	songlist->setSize(page->area.w / 2, page->area.h - 128);
	for (auto&s:songs)
	{
		std::wstring ws;
		u8stows(ws, s.c_str());
		songlist->InsertText(ws);
	}
	songlist->OnItemClick = [this](azvList&, int item) {if (item >= 0)this->playmusic(item); };
	songlist->back.color = COLOR(32, 32, 188, 64);

	info->setPos(page->area.w / 2 + 32, 32);
	info->setSize(page->area.w / 2 - 64, page->area.h - 128);
	info->text.setText(L"Œ¥≤•∑≈“Ù¿÷");


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

		info->text.setText(L"’˝‘⁄≤•∑≈£∫"+songname);
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
		info->text.setText(L"Œ¥≤•∑≈“Ù¿÷");

		Mix_HaltMusic();
		Mix_FreeMusic(music);

		music = nullptr;
		nowsongid = -1;
	}
	return 0;
}

void MyMusic::init_songs()
{
	std::fstream songfile("song.txt");
	std::string s;

	if (songfile.is_open() == false)
		return;

	while (std::getline(songfile, s))
	{
		//STRTOUTF8(s);
		songs.push_back(s);
	}
}

void MyMusic::save_songs()
{
	std::fstream songfile("song.txt");
	if (songfile.is_open() == false)
		return;
	for (auto&s : songs)
	{
		songfile << s << std::endl;
	}
}

void MyMusic::search_songs(const std::string & path)
{
}


// Õ®π˝ UIPage ºÃ≥–

void MainPage::setup()
{
}

void MainPage::destroy()
{
}
