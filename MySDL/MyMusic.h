#pragma once
#include "azEngine.h"
#include "azView.h"
#include "azText.h"
#include <string>
#include <stdio.h>
#include <sstream>

class azApp {
public:
	virtual void run() = 0;
	void eventloop() {

	}
};
class UIPage
{
public:
	virtual void setup() = 0;
	virtual void destroy() = 0;
};
class MainPage :public UIPage
{
	azvLayout* root;
	azvListbox*songs;
	// Í¨¹ý UIPage ¼Ì³Ð
	virtual void setup() override;
	virtual void destroy() override;
};
struct MusicInfo {
	std::string name;
	std::string path;
};
class MyMusic :public azApp {
	azvLayout* page;
	azvTextButton*playbtn ;
	azvTextButton*searchbtn;
	azvLabel*info;
	azvListbox*songlist;

	azEngine*engine;
	int nowsongid;
	Mix_Music*music;
	std::vector<std::string>songs;
public:
	void run();
	void mainui();
	int playmusic(int i);
	int stopmusic();

	void load_config();
	
	void init_songs();
	void save_songs();
	void search_songs(const std::string&path);
	void show_dir_choose(std::string&path);
	void add_song(const std::string & path, const std::wstring & name);


	void setuplist();
};