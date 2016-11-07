#include "azEngine.h"
#include "azView.h"
#include "azText.h"
#include <string>
#include <stdio.h>
#include <sstream>
#include "MyMusic.h"
void printrenflag(Uint32 flag)
{
	printf("RendererFlags:");
	if (flag & SDL_RENDERER_ACCELERATED)
		printf("ACCELERATED ");
	if (flag & SDL_RENDERER_SOFTWARE)
		printf("SOFTWARE ");
	if (flag & SDL_RENDERER_PRESENTVSYNC)
		printf("PRESENTVSYNC ");
	if (flag & SDL_RENDERER_TARGETTEXTURE)
		printf("TARGETTEXTURE ");
	printf("\n");
}



int test_sdl()
{
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_RendererInfo info;
	win = SDL_CreateWindow("MySDL", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	if (!win)
	{
		printf("Error create SDL Window %s\n", SDL_GetError());
		return -1;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!ren)
	{
		printf("Error create SDL Renderer %s\n", SDL_GetError());
		return -1;
	}
	SDL_GetRendererInfo(ren, &info);
	printf("RendererName:%s\n", info.name);
	printrenflag(info.flags);
	printf("TextureMax:Height:%d,Width:%d\n", info.max_texture_height,
		info.max_texture_width);
	SDL_Surface *sur = SDL_LoadBMP("test.bmp");
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, sur);
	SDL_Event event;
	Uint32 tick;
	if (!tex)
	{
		printf("Error create Texture %s\n", SDL_GetError());
		return -1;
	}
	tick = SDL_GetTicks();
	while (1)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				return 0;
			}
			if (event.type == SDL_WINDOWEVENT_CLOSE)
			{
				return 0;
			}
			if (event.type == SDL_FINGERUP)
			{
				return 0;
			}
		}
		double angle = (SDL_GetTicks() % 60000) * 360.0 / 30000;
		SDL_Rect dstRect;
		dstRect.x = 64, dstRect.y = 48, dstRect.w = 512, dstRect.h = 384;
		SDL_RenderClear(ren);
		SDL_RenderCopyEx(ren, tex, NULL, &dstRect, angle, NULL, SDL_FLIP_NONE);
		SDL_RenderPresent(ren);
		tick = SDL_GetTicks() - tick;
		if (tick < (1000 / 60))
			SDL_Delay((1000 / 60) - tick);
		tick = SDL_GetTicks();
	}
}
void test_az(azEngine*e)
{

	const wchar_t* hello_strs[] = { L"Hello World",L"Hello azEngine",L"How are you?" ,L"你好，世界" };
	int hello_num = 0;
	//一段测试代码
	SDL_Event event;

	e->getDefaultFont()->LoadFont("simhei.ttf", 24);
	//
	azvLayout *ui = new azvLayout(e);

	ui->setSize(640, 480);
	ui->move(48, 32);
	ui->back.tex = e->LoadTexture("bg1.jpg");

	azvImage *img1 = new azvImage(ui);
	azvLabel *label1 = new azvLabel(ui);
	azvTextButton *btn1 = new azvTextButton(ui);
	azvTextButton *btn2 = new azvTextButton(ui);
	azvListbox *listbox1 = new azvListbox(ui);


	label1->text.setText(L"hello \n azEngine");
	label1->back.color = COLOR(0, 255, 0, 127);
	label1->setPos(40, 40);
	label1->setSize(180, 80);

	img1->setImage(e->LoadTexture("test.bmp"));
	img1->setSize(384, 256);
	img1->angle = 0.0;

	btn1->text.setText(L"ClickMe");
	btn1->setSize(128, 60);
	btn1->setPos(0, 120);
	btn1->onClick = [label1, &hello_num, &hello_strs](azvButton&btn) {label1->text.setText(hello_strs[hello_num]); if (++hello_num > 3)hello_num = 0; };

	btn2->setSize(128, 60);
	btn2->setPos(0, 220);
	btn2->text.setText(L"LONG LABEL Test");
	btn2->onClick = [label1](azvButton&) {
		label1->setSize(320, 480);
		label1->text.setText(L"《一切》\n"
			"北岛\n"
			"一切都是命运\n"
			"一切都是烟云\n"
			"一切都是没有结局的开始\n"
			"一切都是稍纵即逝的追寻\n"
			"一切欢乐都没有微笑\n"
			"一切苦难都没有泪痕\n"
			"一切语言都是重复\n"
			"一切交往都是初逢\n"
			"一切爱情都在心里\n"
			"一切往事都在梦中\n"
			"一切希望都带着注释\n"
			"一切信仰都带着呻吟\n"
			"一切爆发都有片刻的宁静\n"
			"一切死亡都有冗长的回声");
	};

	listbox1->setPos(320, 0);
	listbox1->setSize(320, 480);
	listbox1->title.setText(L"LISTBOX1");
	listbox1->title.setColor(COLOR_BLUE);
	listbox1->InsertText(L"Hello World");
	listbox1->InsertText(L"Hello azEngine");
	listbox1->InsertText(L"How are you?");
	listbox1->InsertText(L"你好，世界");


	e->BlendMode(SDL_BLENDMODE_BLEND);

	while (1)
	{

		//清屏
		e->SetColor(0, 0, 0, 0);
		e->RenderClear();

		//绘制ui
		ui->draw();
		//刷新以显示绘制的内容
		e->RenderFresh();

		e->Tick();
		while (SDL_PollEvent(&event))
		{
			ui->onEvent(event);
			if (event.type == SDL_QUIT)
				goto end;
		}
	}
end:
	delete ui;
}
#ifdef _DEBUG
#undef main
#endif
int main(int argc, char **argv)
{
	// test_sdl();
	azEngine::InitLibs();

	MyMusic mApp;
	mApp.run();
	azEngine::DeinitLibs();
	return 0;
}
