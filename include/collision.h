/*!*************************************************************************
****
\file   collision.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Feb 7 2021

\brief
This header file outlines an implementation of the collision system in a
game engine.

The functions include:
- rtr_collide

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "AEEngine.h"

struct AABB
{
	AEVec2	min;
	AEVec2	max;
};

bool rtr_collide(const AABB& aabb1, const AEVec2& vel1,
	const AABB& aabb2, const AEVec2& vel2);