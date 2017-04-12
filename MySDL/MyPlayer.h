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

class MyPlayer :public azApp {


	//������ʼʱ��
	Uint32 start_tick;
	//ý����
	MediaStream media;
	//��Ļ/��Ļ������
	CDmMgr danmaku;

public:
	void init();
	void destroy();
	void onEvent(SDL_Event&event) override;
	void onDraw() override;

	void mainui();
};