/*!*************************************************************************
****
\file   animation.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This header file outlines an implementation of the animation system in the
game engine.

The functions include:
- set_sprite
- play_animation
- play_anim_frame
- set_offset
- reset
- at_last_frame

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include <vector>
#include "graphics.h"
#include "globals.h"

class Animation
{
private:
	u32 curr_frame; // current frame
	u8 frames; // How many "pics" there are in the row
	s32 frame_length; // length of each frame
	AEVec2 starting_offset; 
	f32 interval; // How long between each frame
	f32 frame_timer;
	Sprite* sprite;
	std::vector<AEVec2> offsets;
public:
	Animation() : curr_frame{ 0 }, frames{ 1 }, frame_length{ 0 }, starting_offset{ 0,0 }, interval{ 0 }, frame_timer{ 0 }, sprite{ nullptr }{}
	Animation(u8 no_frames, AEVec2 offset, 
		Sprite* sprite, s32 frame_len, f32 interval = G_DELTATIME);
	Animation(Sprite* sprite, AEVec2* offsets, s32 offset_arr_size, f32 interval = G_DELTATIME);
	Animation(const Animation&);
	~Animation();
	void reverse_anims();
	void set_sprite(Sprite*);
	void play_animation(bool loop = true, f32 speed = 1.0f);
	void play_anim_frame(u8 frame_no);
	void set_offset(AEVec2);
	void set_interval(f32);
	void pushback_offset(AEVec2);
	void reset();
	bool at_last_frame();
};