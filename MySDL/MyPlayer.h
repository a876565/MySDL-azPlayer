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
	//开始运行
	//初始化
	virtual void init();
	virtual void initui();
	//销毁并释放资源
	virtual void destroy();
	//绘制图形
	virtual void onDraw();
	//处理事件
	virtual void onEvent(SDL_Event&event);
	//事件之后的时间回调
	virtual void onTick();


	void eventloop();
};

class MyPlayer :public azApp {


	//播放起始时间
	Uint32 start_tick;
	//媒体流
	MediaStream media;
	//弹幕/字幕管理器
	CDmMgr danmaku;

public:
	void init();
	void destroy();
	void onEvent(SDL_Event&event) override;
	void onDraw() override;

	void mainui();
};