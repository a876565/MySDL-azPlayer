#pragma once
#include "azEngine.h"
#include "MediaStream.h"
#include "azView.h"
#include "azText.h"

#include "azvLabel.h"
#include "azvImage.h"
#include "azvButton.h"
#include "azvListbox.h"
#include "azvSlideBar.h"
#include "azvUIMgr.h"
#include "azChoicePage.h"
#include "azvChoiceButton.h"
#include "azLayout.h"
#include "azWindow.h"

#include <string>
#include <stdio.h>
#include <sstream>

class azApp {
	int running;
protected:
	azPage * ui;
	azEngine * engine;
public:

	virtual void run();
	void endApp();
	//���/�Ƴ�page
	void addPage(azPage*p);
	void delPage(azPage*);
	//��ʼ����
	//��ʼ��
	virtual void init();
	virtual void initui();
	//���ٲ��ͷ���Դ
	virtual void destroy();
	//����ͼ��
	virtual void onDraw();
	//�����¼�
	virtual void onEvent(SDL_Event&event);
	//�¼�֮���ʱ��ص�
	virtual void onTick();


	void eventloop();
};
class UIPage
{
public:
	virtual void setup() = 0;
	virtual void destroy() = 0;
};
class MainPage :public UIPage
{
	azPage* root;
	azvListbox*songs;
	// ͨ�� UIPage �̳�
	virtual void setup() override;
	virtual void destroy() override;
};
struct MusicInfo {
	std::string name;
	std::string path;
};
class MyMusic :public azApp {
	MediaStream media;
public:
	void init();
	void destroy();
	void onEvent(SDL_Event&event) override;
	void onDraw() override;

	void mainui();
};