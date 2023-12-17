/*!*************************************************************************
****
\file   audio.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 29 2021

\brief
This header file outlines an implementation of the audio system in the
game engine.

The functions include:
- audio_group_ts
- instance
- release_instance
- set_mute_group
- set_group_volume
- set_pause_group
- toggle_mute_group
- toggle_pause_group
- set_group_vol_multi
- crossfade
- play_sound
- stop_sound

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include <fmod.hpp>
#include "AEEngine.h"
#include <string>
#include <map>
using namespace std;

enum class AUDIO_GROUP : s8
{
	STAGE_BGM,
	BOSS_BGM,
	SFX
};

string audio_group_ts(AUDIO_GROUP);

struct AudioGroup
{
	AUDIO_GROUP group;
	FMOD::ChannelGroup* channelgroup;
	f32 volume;
	f32 lerp_volume;
	f32 vol_multiplier;
	AudioGroup();
	AudioGroup(AUDIO_GROUP group);
};

struct AudioSystem
{
	static AudioSystem* s_instance;
	static AudioSystem* instance()
	{
		if (!s_instance)
			s_instance = new AudioSystem;
		return s_instance;
	};
	static void release_instance()
	{
		if (s_instance)
			delete s_instance;
	};
	AudioSystem();
	~AudioSystem();
	void init();
	void update();
	void set_mute_group(AUDIO_GROUP, bool);
	void set_group_volume(AUDIO_GROUP, f32);
	void set_pause_group(AUDIO_GROUP, bool);
	void toggle_mute_group(AUDIO_GROUP);
	void toggle_pause_group(AUDIO_GROUP);
	void set_group_vol_multi(AUDIO_GROUP, f32);
	void crossfade(AUDIO_GROUP, AUDIO_GROUP);
	FMOD::System* system;
	int next_channel_id;
	FMOD_RESULT result;
	FMOD::ChannelGroup* master_group;
	map<int, FMOD::Channel*> channels;
	map<AUDIO_GROUP, AudioGroup> channel_group;
};

class Audio
{
	FMOD::Sound* sound;
	f32 volume;
	AUDIO_GROUP group;
public:
	Audio(char const* path, AUDIO_GROUP);
	~Audio();
	void play_sound(f32 volume = 1.0f, bool loop = false);
	void stop_sound();
};