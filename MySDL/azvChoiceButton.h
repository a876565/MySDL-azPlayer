#pragma once
#include "azChoicePage.h"
#include "azvButton.h"
#include "azvListbox.h"
class azvChoiceButton :
	public azvButton
{
	int select;
public:
	azChoicePage *chpage;
	SDL_Rect pop_size;
	std::function<void(int)>onSelect;
	azvChoiceButton(azView*p);
	void setSelect(int index);
	int getSelect();

	int addItem(const std::wstring &text,SDL_Color color=COLOR_WHITE, azTexture icon = azTexture());
	int delItem(int index);
	~azvChoiceButton();
};

