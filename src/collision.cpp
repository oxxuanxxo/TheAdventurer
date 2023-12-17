/*!*************************************************************************
****
\file   collision.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Feb 7 2021

\brief
This file contains the implementation of the collision system in the game
 engine.

The functions include:
- rtr_collide
- collision_update

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "main.h"
#include "gameobject.h"

/**
 * @brief AABB collision checking between 2 rectangles
 *
 * @param aabb1
 * LHS AABB
 * @param vel1
 * LHS velocity
 * @param aabb2
 * RHS AABB
 * @param vel2
 * RHS velocity
 * @return true
 * There is collision
 * @return false
 * There is no collision
 */
bool rtr_collide(const AABB& aabb1, const AEVec2& vel1,
	const AABB& aabb2, const AEVec2& vel2)
{
	if ((aabb1.max.x < aabb2.min.x || aabb1.max.y < aabb2.min.y) ||
		(aabb1.min.x > aabb2.max.x || aabb1.min.y > aabb2.max.y))
		return 0;
	float tFirst = 0, tLast = G_DELTATIME;
	AEVec2 velRel, velA = vel1, velB = vel2;
	AEVec2Sub(&velRel, &velB, &velA);
	if (velRel.x < 0)
	{
		if (aabb1.min.x > aabb2.max.x)
			return 0;
		if (aabb1.max.x < aabb2.min.x)
		{
			tFirst = AEMax((aabb1.max.x - aabb2.min.x) / velRel.x, tFirst);
		}
		if (aabb1.min.x < aabb2.max.x)
		{
			tLast = AEMax((aabb1.min.x - aabb2.max.x) / velRel.x, tLast);
		}
	}
	if (velRel.x > 0)
	{
		if (aabb1.min.x > aabb2.max.x)
		{
			tFirst = AEMax((aabb1.min.x - aabb2.max.x) / velRel.x, tFirst);
		}
		if (aabb1.max.x > aabb2.min.x)
		{
			tLast = AEMax((aabb1.max.x - aabb2.min.x) / velRel.x, tLast);
		}
		if (aabb1.max.x < aabb2.min.x)
			return 0;
	}

	if (velRel.y < 0)
	{
		if (aabb1.min.y > aabb2.max.y)
			return 0;
		if (aabb1.max.y < aabb2.min.y)
		{
			tFirst = AEMax((aabb1.max.y - aabb2.min.y) / velRel.y, tFirst);
		}
		if (aabb1.min.y < aabb2.max.y)
		{
			tLast = AEMax((aabb1.min.y - aabb2.max.y) / velRel.y, tLast);
		}
	}
	if (velRel.y > 0)
	{
		if (aabb1.min.y > aabb2.max.y)
		{
			tFirst = AEMax((aabb1.min.y - aabb2.max.y) / velRel.y, tFirst);
		}
		if (aabb1.max.y > aabb2.min.y)
		{
			tLast = AEMax((aabb1.max.y - aabb2.min.y) / velRel.y, tLast);
		}
		if (aabb1.max.y < aabb2.min.y)
			return 0;
	}

	if (tFirst > tLast)
		return 0;

	return 1;
}

/**
 * @brief Performs collision checking in the vector of game object
 * No collision check is done if both game objects are static and if one of their flag is false
 * Checks for collision between all game objects. If they are colliding, a reference will
 * be added to the vector of colliders for each game object.
 *
 * Iterate once again to call the on_collide functions of the game object if there is collision.
 * Clear the colliders afterwards.
 *
 * @param v_obj
 */
void collision_update(vector<gameObject*>& v_obj)
{
	for (std::vector<gameObject*>::iterator i = v_obj.begin(); i != v_obj.end(); ++i)
	{
		for (std::vector<gameObject*>::iterator j = i + 1; j != v_obj.end(); j++)
		{
			gameObject* a = *i;
			gameObject* b = *j;
			if ((!a->is_static || !b->is_static) && (a->flag && b->flag) &&
				rtr_collide(a->bounding_box, a->curr_vel, b->bounding_box, b->curr_vel))
			{
				a->is_colliding = true;
				b->is_colliding = true;
				a->colliders.push_back(b);
				b->colliders.push_back(a);
			}
		}
	}

	for (size_t i{ 0 }; i < v_obj.size(); i++)
	{
		gameObject*& obj = v_obj.at(i);
		bool can_move = false;
		if (!obj->is_colliding)
		{
			can_move = true;
		}
		else
		{
			if (!obj->colliders.empty() && obj->on_collide != nullptr)
				obj->on_collide(*obj);
		}
		obj->colliders.clear();
		obj->is_colliding = false;
	}
}