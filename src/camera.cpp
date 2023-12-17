/*!*************************************************************************
****
\file   camera.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Jan 25 2021

\brief
This file contains implementation of the camera system for the game.

The functions include:
- tutorial_load
- tutorial_initialize
- tutorial_update
- tutorial_draw
- tutorial_free
- tutorial_unload

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "camera.h"
#include <iostream>

constexpr float INTERPOLATE_LERP_FACTOR = 0.04f;
constexpr float CAM_BOUND_LERP = 0.15f;
AEVec2 Camera::zoom;
AEMtx33 Camera::matrix;
AEVec2* Camera::target_pos;
AEVec2* Camera::default_target_pos;
f32 Camera::shake_intensity, Camera::zoom_factor, Camera::shake_timer, Camera::zoom_timer, Camera::look_timer;
s32 windows_x;
s32 windows_y;

/******************************************************************************
 * @brief Construct a new Camera:: Camera object
 * 
******************************************************************************/
Camera::Camera() :
	lerp_pos{ 0,0 }, curr_pos{ 0,0 }, default_cam_pos{ {0},{0} }, camera_boundary{ {},{} }, lerp_boundary{ {},{} }
{
	default_target = nullptr;
	default_cam_pos = {};
	target_pos = nullptr;
}

/******************************************************************************
 * @brief Initialize the camera object.
 * 
******************************************************************************/
void Camera::init()
{
	AEMtx33Identity(&matrix);
	windows_x = AEGetWindowWidth();
	windows_y = AEGetWindowHeight();
	zoom = {};
	shake_timer = {};
	shake_intensity = {};
}

/******************************************************************************
 * @brief Sets all variables to their default values
 * 
******************************************************************************/
void Camera::free()
{
	zoom = { 1.0f, 1.0f };
	shake_timer = 0;
	shake_intensity = 0;
	curr_pos = { 0,0 };
	AEGfxSetCamPosition(0, 0);
	lerp_pos = { 0,0 };
	shake_intensity = 0.0f;
	zoom_factor = 1.0f;
	zoom_timer = 0.0f;
	look_timer = 0.0f;
}

/******************************************************************************
 * @brief Initialize the camera with respect to the player and camera bound
 * 
 * @param plyr 
 * Reference to the player
 * @param camera_bound 
 * Initial camera bounds
******************************************************************************/
void Camera::init(Player& plyr, AABB camera_bound)
{
	AEMtx33Identity(&matrix);
	Camera::zoom = { 1,1 };
	default_target = default_target_pos = target_pos = &(plyr.curr_pos);
	curr_pos = plyr.curr_pos;
	windows_x = AEGetWindowWidth();
	windows_y = AEGetWindowHeight();
	camera_boundary = lerp_boundary = camera_bound;
	zoom = { 1.0f,1.0f };
	shake_timer = 0;
	shake_intensity = 0;
}

/******************************************************************************
 * @brief Updates the camera
 * 
******************************************************************************/
void Camera::update()
{
	bound_cam();
	interpolate_cam();
	shake();
	temp_zoom();
	look_at();
	AEVec2 rel_pos = curr_pos;
	rel_pos = get_relative_pos(curr_pos);
	AEGfxSetCamPosition(rel_pos.x, rel_pos.y);
}

/******************************************************************************
 * @brief Bounds the camera position according to the camera boundaries
 * 
******************************************************************************/
void Camera::bound_cam()
{
	AEVec2Lerp(&lerp_boundary.min, &lerp_boundary.min, &camera_boundary.min, CAM_BOUND_LERP);
	AEVec2Lerp(&lerp_boundary.max, &lerp_boundary.max, &camera_boundary.max, CAM_BOUND_LERP);
	curr_pos = {
		AEClamp(curr_pos.x, (lerp_boundary.min.x + windows_x / (2 * zoom.x)), (lerp_boundary.max.x - windows_x / (2 * zoom.x))),
		AEClamp(curr_pos.y, (lerp_boundary.min.y + windows_y / (2 * zoom.y)), (lerp_boundary.max.y - windows_y / (2 * zoom.y)))
	};
	default_cam_pos = {
		AEClamp(default_cam_pos.x, (lerp_boundary.min.x + windows_x / (2 * zoom.x)), (lerp_boundary.max.x - windows_x / (2 * zoom.x))),
		AEClamp(default_cam_pos.y, (lerp_boundary.min.y + windows_y / (2 * zoom.y)), (lerp_boundary.max.y - windows_y / (2 * zoom.y)))
	};
}

/******************************************************************************
 * @brief Interpolate the position of the camera
 * 
******************************************************************************/
void Camera::interpolate_cam()
{
	AEVec2Lerp(&lerp_pos, &curr_pos, target_pos, INTERPOLATE_LERP_FACTOR);
	curr_pos = lerp_pos;
	AEVec2Lerp(&lerp_pos, &default_cam_pos, default_target_pos, INTERPOLATE_LERP_FACTOR);
	default_cam_pos = lerp_pos;
}

/******************************************************************************
 * @brief Sets the camera boundary
 * 
 * @param min 
 * Minimum position
 * @param max 
 * Maximum position
******************************************************************************/
void Camera::set_camera_boundary(AEVec2 min, AEVec2 max)
{
	camera_boundary.min = min;
	camera_boundary.max = max;
}

/******************************************************************************
 * @brief Sets the minimum boundary of the camera
 * 
 * @param min 
 * Minimum position
******************************************************************************/
void Camera::set_min_boundary(AEVec2 min)
{
	camera_boundary.min = min;
}

/******************************************************************************
 * @brief Sets the maximum boundary of the camera
 * 
 * @param max 
 * Maximum position
******************************************************************************/
void Camera::set_max_boundary(AEVec2 max)
{
	camera_boundary.max = max;
}

/******************************************************************************
 * @brief Sets the default target of the camera
 * 
 * @param target 
 * Pointer representing the position of the camera target
******************************************************************************/
void Camera::set_default_target(AEVec2* target)
{
	default_target = target;
}

/******************************************************************************
 * @brief Sets the camera target
 * 
 * @param target 
 * Pointer representing the position of the camera target
******************************************************************************/
void Camera::set_camera_target(AEVec2* target)
{
	target_pos = target;
}

/******************************************************************************
 * @brief Returns the camera target
 * 
 * @return AEVec2 
 * Position of the camera target
******************************************************************************/
AEVec2 Camera::get_camera_target()
{
	return *target_pos;
}

/******************************************************************************
 * @brief Returns the matrix applied on the camera
 * 
 * @return AEMtx33 
 * Camera matrix
******************************************************************************/
AEMtx33 Camera::get_camera_matrix(void)
{
	return Camera::matrix;
}

/******************************************************************************
 * @brief Returns the current position of the camera
 *
******************************************************************************/
AEVec2 Camera::get_curr_pos(void)
{
	return curr_pos;
}

/******************************************************************************
 * @brief Returns the camera scale
 * 
******************************************************************************/
AEVec2 Camera::get_camera_scale(void)
{
	return Camera::zoom;
}

/******************************************************************************
 * @brief Shakes the camera by shake_intensity if the shake_timer
 * is more than 0.
 * 
******************************************************************************/
void Camera::shake()
{
	if (shake_timer > 0)
	{
		curr_pos.x += shake_intensity * AERandFloat();
		curr_pos.x -= shake_intensity * AERandFloat();
		curr_pos.y += shake_intensity * AERandFloat();
		curr_pos.y -= shake_intensity * AERandFloat();
		shake_timer -= G_DT;
	}
}

/******************************************************************************
 * @brief Lerp the zoom factor of the camera if the zoom_timer is
 * more than 0.
 * 
******************************************************************************/
void Camera::temp_zoom()
{
	if (zoom_timer > 0)
	{
		AEVec2 target_zoom{ zoom_factor, zoom_factor };
		AEVec2Lerp(&Camera::zoom, &Camera::zoom, &target_zoom, 0.14f);
		zoom_timer -= G_DT;
	}
	else
	{
		AEVec2 normal{ 1.0f, 1.0f };
		AEVec2Lerp(&Camera::zoom, &Camera::zoom, &normal, 0.14f);
	}
	AEMtx33Scale(&matrix, Camera::zoom.x, Camera::zoom.y);
}

/******************************************************************************
 * @brief Sets the target_position of the camera to the default_target
 *  when the look_timer is <= 0.
 * 
******************************************************************************/
void Camera::look_at()
{
	if (look_timer > 0)
	{
		look_timer -= G_DT;
	}
	else
		target_pos = default_target;
}

/******************************************************************************
 * @brief Shake the camera temporarily
 * 
 * @param intensity 
 * Intensity of the shake
 * @param duration 
 * How long the camera will shake
******************************************************************************/
void Camera::shake(f32 const intensity, f32 const duration)
{
	shake_intensity = intensity;
	shake_timer = duration;
}

/******************************************************************************
 * @brief Camera will zoom temporarily
 * 
 * @param zoom 
 * Zoom factor of the camera
 * @param duration 
 * How long the camera will zoom
******************************************************************************/
void Camera::temp_zoom(f32 const zoom, f32 const duration)
{
	zoom_factor = zoom;
	zoom_timer = duration;
}

/******************************************************************************
 * @brief Temporarily sets the target of the camera
 * 
 * @param target 
 * Position pointer representing the temporary target
 * @param duration 
 * How long the camera will look at the target
******************************************************************************/
void Camera::look_at(AEVec2* target, f32 const duration)
{
	target_pos = target;
	look_timer = duration;
}