#include "azvChoiceButton.h"


azvChoiceButton::azvChoiceButton(azView * p) :azvButton(p), select(0), pop_size({ 0,0,0,0 })
{
	chpage = new azChoicePage(find_root());
	chpage->list->back.color = COLOR(0x77, 0x77, 0x77, 0xdd);
	onClick = [this](azvButtonBase&) {
		SDL_Rect r = { 0,0,0,0 };
		int scr_height = find_root()->area.h;
//		int h;
		to_abs_area(&r);
		r.w = pop_size.w ? pop_size.w : area.w;
		chpage->list->setSize(r.w, pop_size.h);
		r.h = chpage->list->CalcAllHeight();
		if (r.h > scr_height)
		{
			r.h = scr_height;
			r.y = 0;
		}
		else
		{
			if (area.y + r.h > scr_height)
				r.y = scr_height - r.h;
		}
		chpage->list->setPos(r.x, r.y);

		chpage->show();

	};
	chpage->onChoose = [this](int id) {
		chpage->end();
		if (onSelect)
			onSelect(id); 
		setSelect(id);
	};
}

void azvChoiceButton::setSelect(int index)
{
	if (index >= 0 && index < chpage->list->getItemNum())
	{
		select = index;
		text.setText(chpage->list->GetItemText(select));
		chpage->list->setSelItem(select);
	}
}

int azvChoiceButton::getSelect()
{
	return select;
}

int azvChoiceButton::addItem(const std::wstring &text, SDL_Color color, azTexture icon)
{
	auto item=chpage->list->InsertText(text, color);
	if (icon)
		chpage->list->SetItemIcon(item, icon);
	return item;
}

int azvChoiceButton::delItem(int index)
{
	return chpage->list->DelItem(index);
}

azvChoiceButton::~azvChoiceButton()
{
}
