/*!*************************************************************************
****
\file   status_effect.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 5 2021

\brief
This header file outlines an implementation of the status effect class.

The functions include:
- update
- activate
- bleed

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "main.h"
#include "game_enums.h"
#include "gameobject.h"

struct Effect
{
	STATUS_TYPE type;
	f32 timer;
	f32 expiry;
	f32 tick;
	f32 tick_timer;
	gameObject* owner;
	bool activated;
	Effect();
	void (*effect)(gameObject&);
	void update();
	void activate(f32 duration);
};

namespace Status_Effect
{
	void bleed(gameObject&);
}