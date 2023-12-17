/*!*************************************************************************
****
\file   camera.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Mar 11 2021

\brief
This header file outlines an implementation of the camera system in a
game engine.

The functions include:
- load
- init
- update
- draw
- init
- interpolate_cam
- bound_cam
- set_camera_boundary
- set_min_boundary
- set_max_boundary
- set_default_target
- set_camera_target
- get_camera_matrix
- get_camera_target
- get_camera_scale
- shake
- temp_zoom
- look_at

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#pragma once
#include "AEEngine.h"
#include "player.h"

class Camera
{
private:
	AEVec2* default_target;
	AEVec2 curr_pos;
	static AEVec2* target_pos;
	static AEVec2* default_target_pos;
	AEVec2 lerp_pos;
	AEVec2 default_cam_pos;
	AABB lerp_boundary;
	AABB camera_boundary;
	static f32 shake_intensity;
	static f32 zoom_factor;
	static f32 shake_timer;
	static f32 zoom_timer;
	static f32 look_timer;
	void interpolate_cam();
	void bound_cam();
public:
	static AEVec2 zoom;
	static AEMtx33 matrix;
	Camera();
	void init();
	void init(Player& plyr, AABB camera_bound);
	void update();
	void free();
	void set_camera_boundary(AEVec2 min, AEVec2 max);
	void set_min_boundary(AEVec2 min);
	void set_max_boundary(AEVec2 max);
	void set_default_target(AEVec2*);
	void set_camera_target(AEVec2*);
	static AEMtx33 get_camera_matrix(void);
	AEVec2 get_curr_pos();
	AEVec2 get_camera_target();
	AEVec2 get_camera_scale(void);
	void shake();
	void temp_zoom();
	void look_at();
	static void shake(f32 const intensity, f32 const duration); // shake the camera for x duration
	static void temp_zoom(f32 const zoom_factor, f32 const duration); // zoom for x duration
	static void look_at(AEVec2 * target, f32 const duration); // temp target for x duration
};