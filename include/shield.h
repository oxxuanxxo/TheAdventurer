/*!*************************************************************************
****
\file   shield.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 20 2021

\brief
This header file outlines an implementation of the shield function.

The functions include:
- shield_up
- parry_attack

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once

void shield_up(gameObject& gameObj);

void parry_attack(gameObject& gameObj);

extern constexpr f32 PARRY_TIME = 0.3f;