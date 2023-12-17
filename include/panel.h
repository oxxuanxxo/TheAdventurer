/*!*************************************************************************
****
\file   panel.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 6 2021

\brief
This header file outlines an implementation of the panel UI class.

The functions include:
- update
- draw

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "button.h"

struct Panel
{
	Panel();
	Sprite* background;
	ButtonManager btns;
	AEVec2 pos;
	bool flag;
	void update();
	virtual void draw();
};

struct PromptPanel : public Panel
{
	PromptPanel();
	void draw();
	std::string title;
	std::string message;
	AEVec2 title_pos;
	AEVec2 title_offset;
	AEVec2 message_pos;
	AEVec2 message_offset;
};