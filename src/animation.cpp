/*!*************************************************************************
****
\file   animation.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This file contains the implementation of the animation system in the
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
#include "animation.h"
#include "level_manager.h"

/**
 * @brief Construct a new Animation:: Animation object
 *
 * @param no_frames
 * 	Number of frames the animation will have
 * @param offset
 * 	Offset position in the spritesheet
 * @param sprite
 * 	Sprite pointer representing the sprite
 * @param frame_len
 * 	Length of the frame per iteration, in pixels
 * @param interval
 * 	How long before the game will draw the next frame in the animation
 */
Animation::Animation(u8 no_frames, AEVec2 offset,
	Sprite* sprite, s32 frame_len, f32 interval) :
	curr_frame{}, frame_timer{}, starting_offset{ offset }, frame_length{ frame_len }
{
	frames = no_frames;
	this->sprite = sprite;
	this->interval = interval;
	for (u8 i = 0; i < no_frames; ++i)
	{
		AEVec2 pos = offset;
		pos.x += frame_len * i;
		offsets.push_back(pos);
	}
}

/**
 * @brief Construct a new Animation:: Animation object
 * This constructor can be used to take in an array of offsets
 *
 * @param sprite
 * Sprite pointer representing the sprite
 * @param p_offsets
 * Pointer representing an array of offsets
 * @param size
 * Size of the array pointer
 */
Animation::Animation(Sprite* sprite, AEVec2* p_offsets, s32 size, f32 interval) : Animation()
{
	frames = (u8)size;
	this->sprite = sprite;
	this->interval = interval;
	for (int i{ 0 }; i < size; ++i)
	{
		offsets.push_back(*(p_offsets + i));
	}
}

/**
 * @brief Construct a new Animation:: Animation object
 * Copy constructor
 *
 * @param rhs
 *	A read-only reference of the animation object to copy construct
 */
Animation::Animation(const Animation& rhs) : curr_frame{ rhs.curr_frame }, frames{ rhs.frames },
frame_length{ rhs.frame_length }, starting_offset{ rhs.starting_offset }, interval{ rhs.interval },
frame_timer{ rhs.frame_timer }, sprite{ rhs.sprite }, offsets{ rhs.offsets } {}

/**
 * @brief Reverse the offsets vector in the object, effectively reversing the animation
 *
*/
void Animation::reverse_anims()
{
	std::reverse(offsets.begin(), offsets.end());
}

/**
 * @brief Destroy the Animation:: Animation object
 *
 */
Animation::~Animation()
{
	offsets.clear();
	offsets.~vector();
}

/**
 * @brief
 *
 * @param new_sprite
 * New sprite to set
 */
void Animation::set_sprite(Sprite* new_sprite)
{
	sprite = new_sprite;
}

/**
 * @brief Plays the animation
 *
 * @param loop
 * 	If false, the animation will only play once (default at false)
 * @param speed
 * 	Speed multiplier of the animation (default at 1)
 */
void Animation::play_animation(bool loop, f32 speed)
{
	if (sprite)
	{
		if (!pause)
		{
			if (!frame_timer)
			{
				sprite->offset = offsets[0];
			}
			if (frame_timer >= interval) // Go back to the start
			{
				curr_frame++;
				frame_timer = 0;
				if (curr_frame > (static_cast<u32>(frames) - 1) && loop)
				{
					curr_frame = 0;
					sprite->offset = offsets[0];
				}
				else if (curr_frame > (static_cast<u32>(frames) - 1) && !loop)
					sprite->offset = offsets[frames - 1];
				else
				{
					sprite->offset = offsets[curr_frame];
				}
			}
			frame_timer += G_DELTATIME * speed;
		}
	}
}

/**
 * @brief Play the animation at a specific frame
 *
 * @param frame_no
 * Frame index
 */
void Animation::play_anim_frame(u8 frame_no)
{
	sprite->offset = offsets[frame_no];
}

/**
 * @brief Sets the sprite offset to offset
 *
 * @param offset
 * AEVec2 representing the offset
 */
void Animation::set_offset(AEVec2 offset)
{
	sprite->offset = offset;
}

/**
 * @brief Sets the interval of the animation
 *
 * @param duration
 * A floating point data type representing the duration of the interval
 */
void Animation::set_interval(f32 duration)
{
	interval = duration;
}

/**
 *	@brief Pushes an offset to the offsets vector
 *
 *	@param offset
 *	An AEVec2 value representing the offset to push back to the vector
 */
void Animation::pushback_offset(AEVec2 offset)
{
	offsets.push_back(offset);
	++frames;
}

/**
 * @brief Resets the current frame and frame_timer to 0
 *
 */
void Animation::reset()
{
	curr_frame = 0;
	frame_timer = 0;
}

/**
 * @brief Checks if the animation is at the last frame
 *
 * @return true
 * @return false
 */
bool Animation::at_last_frame()
{
	return curr_frame > (static_cast<u32>(frames) - 1);
}