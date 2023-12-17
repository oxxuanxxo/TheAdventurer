/*!*****************************************************************************
\file   collectables.h
\author Mohammad Hanif Koh Teck Wee
\par    DP email: mohammadhanifkoh.t\@digipen.edu
\date   March 12 2021

\brief
This file outlines the implementation of all functions related Heart game
objects as outlined in collectables.h

The functions include:
- Heart::Heart (default constructor)
- heart_load
- Heart::game_object_initialize
- Heart::game_object_update
- heart_unload
- adjust_heart_movement
- collectables_collide
- spawn_heart

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*******************************************************************************/
#include "gameobject.h"
#include "audio.h"

class Heart : public gameObject
{
public:
	Heart();
	void game_object_initialize() override;
	void game_object_update() override;
	static Audio* pick_up_sound;
};

void heart_load();
void heart_unload();
void collectables_collide(gameObject& obj);
void spawn_heart(gameObject& enemy);