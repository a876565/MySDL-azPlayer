#pragma once
#include "azvButton.h"
#include "azvListbox.h"
class azvChoices :
	public azvButton
{
public:
	azvListbox*list;
	azvChoices(azView*p) :azvButton(p) { list = new azvListbox(list); };
	~azvChoices();
};

