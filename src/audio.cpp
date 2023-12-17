/*!*************************************************************************
****
\file   audio.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 29 2021

\brief
This file includes the implementation of the audio system in the
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
#include "main.h"
#include "audio.h"

static constexpr f32 LERP_VOLUME_FACTOR = 0.1f;
static void lerp_volume(std::pair<const AUDIO_GROUP, AudioGroup>& pair);

AudioSystem* AudioSystem::s_instance = 0;

/**
 * @brief Construct a new Audio Group:: Audio Group object
 * 
 */
AudioGroup::AudioGroup() : group{ AUDIO_GROUP::SFX }, channelgroup{ 0 }, volume{ 1.0f }, lerp_volume{ 1.0f }, vol_multiplier{ 1.0f }{ }

/**
 * @brief Construct a new Audio Group:: Audio Group object
 * 
 * @param group 
 * Audio group of the class
 */
AudioGroup::AudioGroup(AUDIO_GROUP group) : group{ group }, channelgroup{ 0 }, volume{ 1.0f }, lerp_volume{ 1.0f }, vol_multiplier{ 1.0f }{ }

/**
 * @brief Construct a new Audio System:: Audio System object
 * 
 */
AudioSystem::AudioSystem() : next_channel_id{ 0 }, system{ 0 }
{
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK)
	{
		AE_ASSERT_MESG(result, "FMOD Error!!");
	}
	result = system->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		AE_ASSERT_MESG(result, "FMOD Error!!");
	}
	channel_group[AUDIO_GROUP::BOSS_BGM] = AudioGroup(AUDIO_GROUP::BOSS_BGM);
	channel_group[AUDIO_GROUP::STAGE_BGM] = AudioGroup(AUDIO_GROUP::STAGE_BGM);
	channel_group[AUDIO_GROUP::SFX] = AudioGroup(AUDIO_GROUP::SFX);
	system->createChannelGroup(audio_group_ts(AUDIO_GROUP::BOSS_BGM).c_str(), &channel_group[AUDIO_GROUP::BOSS_BGM].channelgroup);
	system->createChannelGroup(audio_group_ts(AUDIO_GROUP::STAGE_BGM).c_str(), &channel_group[AUDIO_GROUP::STAGE_BGM].channelgroup);
	system->createChannelGroup(audio_group_ts(AUDIO_GROUP::SFX).c_str(), &channel_group[AUDIO_GROUP::SFX].channelgroup);
	system->getMasterChannelGroup(&master_group);
	result = master_group->addGroup(channel_group[AUDIO_GROUP::BOSS_BGM].channelgroup);
	result = master_group->addGroup(channel_group[AUDIO_GROUP::STAGE_BGM].channelgroup);
	result = master_group->addGroup(channel_group[AUDIO_GROUP::SFX].channelgroup);
}

/**
 * @brief Destroy the Audio System:: Audio System object
 * 
 */
AudioSystem::~AudioSystem()
{
	result = system->close();
	result = system->release();
}

/**
 * @brief Initialize the audio system. Must be called at the start of every game state in a level.
 * 
 */
void AudioSystem::init()
{
	master_group->setMute(false);
	master_group->setPaused(false);
	set_mute_group(AUDIO_GROUP::BOSS_BGM, false);
	set_pause_group(AUDIO_GROUP::BOSS_BGM, false);
	set_mute_group(AUDIO_GROUP::STAGE_BGM, false);
	set_pause_group(AUDIO_GROUP::STAGE_BGM, false);
	set_mute_group(AUDIO_GROUP::SFX, false);
	set_pause_group(AUDIO_GROUP::SFX, false);
	set_group_volume(AUDIO_GROUP::BOSS_BGM, 1.0f);
	set_group_volume(AUDIO_GROUP::STAGE_BGM, 1.0f);
	set_group_volume(AUDIO_GROUP::SFX, 1.0f);
	channel_group[AUDIO_GROUP::BOSS_BGM].lerp_volume = 1.0f;
	channel_group[AUDIO_GROUP::STAGE_BGM].lerp_volume = 1.0f;
	channel_group[AUDIO_GROUP::SFX].lerp_volume = 1.0f;
}

/**
 * @brief Update function of the audio system
 * Required to enable lerping of volume
 * 
 */
void AudioSystem::update()
{
	result = system->update();
	std::for_each(channel_group.begin(), channel_group.end(), lerp_volume);
}

/**
 * @brief Lerp the volume to the desired volume of each audio group
 * 
 * @param pair 
 * A pair of AUDIO_GROUP and an AudioGroup class
 */
static void lerp_volume(std::pair<const AUDIO_GROUP, AudioGroup>& pair)
{
	AudioGroup& group = pair.second;
	AEVec2 vol = { group.volume, 0 };
	AEVec2 vol_lerp = { group.lerp_volume, 0 };
	AEVec2 result;
	AEVec2Lerp(&result, &vol, &vol_lerp, LERP_VOLUME_FACTOR);
	AudioSystem::instance()->set_group_volume(pair.first, result.x * group.vol_multiplier);
}

/**
 * @brief Mutes the selected audio group
 * 
 * @param group 
 * @param mute 
 * boolean to set if the audio group should be mute
 */
void AudioSystem::set_mute_group(AUDIO_GROUP group, bool mute)
{
	channel_group[group].channelgroup->setMute(mute);
}

/**
 * @brief Pauses the selected audio group
 * 
 * @param group 
 * @param pause 
 */
void AudioSystem::set_pause_group(AUDIO_GROUP group, bool pause)
{
	channel_group[group].channelgroup->setPaused(pause);
}

/**
 * @brief Sets the volume of the selected audio group
 * 
 * @param group 
 * @param volume 
 */
void AudioSystem::set_group_volume(AUDIO_GROUP group, f32 volume)
{
	channel_group[group].channelgroup->setVolume(volume);
	channel_group[group].volume = volume;
}

/**
 * @brief Set the volume multiplier of the selected audio group
 * 
 * @param group 
 * @param volume 
 */
void AudioSystem::set_group_vol_multi(AUDIO_GROUP group, f32 volume)
{
	channel_group[group].vol_multiplier = volume;
}

/**
 * @brief Mutes group in a toggle way
 * 
 * @param group 
 */
void AudioSystem::toggle_mute_group(AUDIO_GROUP group)
{
	bool mute;
	channel_group[group].channelgroup->getMute(&mute);
	channel_group[group].channelgroup->setMute(!mute);
}

/**
 * @brief Pauses audio group in a toggle way
 * 
 * @param group 
 */
void AudioSystem::toggle_pause_group(AUDIO_GROUP group)
{
	bool pause;
	channel_group[group].channelgroup->getPaused(&pause);
	channel_group[group].channelgroup->setPaused(!pause);
}

/**
 * @brief Cross fades from one audio group to another
 * 
 * @param lhs 
 * @param rhs 
 */
void AudioSystem::crossfade(AUDIO_GROUP lhs, AUDIO_GROUP rhs)
{
	channel_group[lhs].lerp_volume = 0;
	channel_group[rhs].lerp_volume = 1.0f;
}

/**
 * @brief Construct a new Audio:: Audio object
 * 
 * @param path 
 * File path of the audio
 * @param group 
 * Audio group set to this audio
 */
Audio::Audio(char const* path, AUDIO_GROUP group) : sound{}, volume{ 1 }, group{group}
{
	AudioSystem::instance()->result = AudioSystem::instance()->system->createSound(path, FMOD_DEFAULT, 0, &sound);
}

/**
 * @brief Destroy the Audio:: Audio object
 * 
 */
Audio::~Audio()
{
	AudioSystem::instance()->result = sound->release();
}

/**
 * @brief Returns string tied to the audio group
 * 
 * @param ag 
 * Audio group
 * @return string 
 * Audio group as string
 */
string audio_group_ts(AUDIO_GROUP ag)
{
	switch (ag)
	{
	case AUDIO_GROUP::BOSS_BGM:
		return "Boss BGM";
	case AUDIO_GROUP::STAGE_BGM:
		return "Stage BGM";
	case AUDIO_GROUP::SFX:
		return "SFX";
	default:
		return "Default";
	}
}

/**
 * @brief Plays the sound
 * 
 * @param volume 
 * Volume of the audio from 0 to 1.0f
 * @param loop 
 * Bool representing if the audio is to loop or not
 */
void Audio::play_sound(f32 volume, bool loop)
{
	FMOD::Channel* pChannel = nullptr;
	if (!loop)
		sound->setMode(FMOD_LOOP_OFF);
	else
	{
		sound->setMode(FMOD_LOOP_NORMAL);
		sound->setLoopCount(-1);
	}
	AudioSystem::instance()->result = AudioSystem::instance()->system->playSound(sound, AudioSystem::instance()->channel_group[group].channelgroup, false, &pChannel);
	pChannel->setVolume(volume * AudioSystem::instance()->channel_group[group].vol_multiplier);
	AudioSystem::instance()->channels[AudioSystem::instance()->next_channel_id++] = pChannel;
}

/**
 * @brief Release the memory set to the audio
 * 
 */
void Audio::stop_sound()
{
	sound->release();
}