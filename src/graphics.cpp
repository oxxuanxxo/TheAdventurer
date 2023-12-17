/*!*************************************************************************
****
\file   graphics.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Feb 3 2021

\brief
This file contains the implementation of the graphics system in the
game

The functions include:
- draw
- get_relative_pos
- read_png_size
- AEVec2RotateFromPivot
- AEVec2toRad
- create_square_mesh
- to_window_coord
- text_print

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "main.h"
#include "graphics.h"
#include "camera.h"

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 */
Sprite::Sprite() : size{ 1.0f, 1.0f }, texture{ nullptr }, rot{ 0.0f }, flip_x{ false }, flip_y(false)
{
	texture = nullptr;
	mesh = nullptr;
	offset = { 0.0f, 0.0f };
	tint = { 1.0, 1.0, 1.0, 1.0 };
	transparency = 1.0f;
	scale = { 1.0f, 1.0f };
}

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 * @param tex 
 * Pointer to the texture of the sprite
 * @param s_mesh 
 * The mesh to use for this sprite
 */
Sprite::Sprite(AEGfxTexture* tex,
	AEGfxVertexList* s_mesh) : Sprite()
{
	texture = tex;
	mesh = s_mesh;
}

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 * @param tex 
 * Pointer to the texture of the sprite
 * @param s_mesh 
 * The mesh to use for this sprite
 * @param s_scale 
 * Scale of the sprite
 */
Sprite::Sprite(AEGfxTexture* tex,
	AEGfxVertexList* s_mesh,
	AEVec2 s_scale) : Sprite()
{
	texture = tex;
	mesh = s_mesh;
	scale = s_scale;
}

/**
 * @brief Construct a new Sprite:: Sprite object
 * 
 * @param rhs 
 * Reference to the sprite to copy
 */
Sprite::Sprite(Sprite const& rhs) : texture{ rhs.texture }, mesh{ rhs.mesh }, offset{ rhs.offset },
tint{ rhs.tint }, transparency{ rhs.transparency }, size{ rhs.size }, scale{ rhs.scale },
rot{ rhs.rot }, flip_x{ rhs.flip_x }, flip_y{ rhs.flip_y } {}

/**
 * @brief Destroy the Sprite:: Sprite object
 * This just sets the texture and mesh to nullptr. The programmer has to destroy the texture and
 * mesh manually themselves.
 * 
 */
Sprite::~Sprite()
{
	texture = nullptr;
	mesh = nullptr;
}

/**
 * @brief Sets the size of the sprite
 * 
 * @param new_size 
 * Size to set
 */
void Sprite::set_size(AEVec2 new_size)
{
	size = new_size;
}

/**
 * @brief Draw the sprite.
 * This draw function is non-relative to the camera's zoom factor, useful in cases like 
 * drawing UI objects.
 * 
 * @param texture 
 * Pointer to the texture to draw
 * @param mesh 
 * Mesh to draw
 * @param position 
 * Position to draw in the screen
 * @param tex_offset 
 * Offset of the texture
 * @param tint 
 * Tint of the object to draw
 * @param transparency 
 * Transparency factor
 * @param scale 
 * Scale of the 
 * @param rotation 
 * Rotation of the object to draw
 */
void draw_non_relative(AEGfxTexture* texture,
	AEGfxVertexList* mesh,
	AEVec2 position,
	AEVec2 tex_offset,
	Color tint,
	f32 transparency,
	AEVec2 scale,
	f32 rotation)
{
	AEMtx33 m_scale, rot, trans;
	AEVec2 camPos;
	AEGfxGetCamPosition(&camPos.x, &camPos.y);

	AEMtx33Scale(&m_scale, scale.x, scale.y);
	AEMtx33Rot(&rot, rotation);
	AEMtx33Trans(&trans, position.x + camPos.x, position.y + camPos.y);
	AEMtx33Concat(&rot, &rot, &m_scale);
	AEMtx33Concat(&trans, &trans, &rot);
	if (texture)
	{
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxTextureSet(texture, tex_offset.x, tex_offset.y);
	}
	else
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	}
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTintColor(tint.r, tint.g, tint.b, tint.a);
	AEGfxSetTransparency(transparency);
	AEGfxSetTransform(trans.m);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

/**
 * @brief Draw the sprite.
 * This draw function is non-relative to the camera's zoom factor, useful in cases like 
 * drawing UI objects.
 * 
 * @param sprite 
 * Pointer to the sprite
 * @param position 
 * Position
 * @param scale 
 * Scale factor
 */
void draw_non_relative(const Sprite* sprite, const AEVec2 position, AEVec2 scale)
{
	if (sprite)
	{
		AEVec2 temp_scale = scale;
		if (sprite->flip_x && sprite->flip_y)
		{
			temp_scale = { -scale.x, -scale.y };
		}
		else if (sprite->flip_x)
		{
			temp_scale = { -scale.x, scale.y };
		}
		else if (sprite->flip_y)
		{
			temp_scale = { scale.x, -scale.y };
		}
		AEVec2 tex_offset = sprite->offset;
		tex_offset.x = tex_offset.x / sprite->size.x;
		tex_offset.y = tex_offset.y / sprite->size.y;
		draw_non_relative(sprite->texture, sprite->mesh, position,
			tex_offset, sprite->tint, sprite->transparency, temp_scale, sprite->rot);
	}
}

/**
 * @brief Draw the sprite with zoom factor from the camera
 * 
 * @param texture 
 * Pointer to the texture to draw
 * @param mesh 
 * Mesh to draw
 * @param position 
 * Position to draw in the screen
 * @param tex_offset 
 * Offset of the texture
 * @param tint 
 * Tint of the object to draw
 * @param transparency 
 * Transparency factor
 * @param scale 
 * Scale of the 
 * @param rotation 
 * Rotation of the object to draw
 */
void draw(AEGfxTexture* texture,
	AEGfxVertexList* mesh,
	AEVec2 position,
	AEVec2 tex_offset,
	Color tint,
	f32 transparency,
	AEVec2 scale,
	f32 rotation)
{
	AEMtx33 camera_matrix = Camera::get_camera_matrix();
	AEMtx33 rot;
	AEMtx33Rot(&rot, rotation);
	AEVec2 rel_pos = get_relative_pos(position);
	if (texture)
	{
		AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
		AEGfxTextureSet(texture, tex_offset.x, tex_offset.y); 
	}

	else
	{
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	}

	AEMtx33ScaleApply(&camera_matrix, &camera_matrix, scale.x, scale.y);
	AEMtx33Concat(&camera_matrix, &rot, &camera_matrix);
	AEMtx33TransApply(&camera_matrix, &camera_matrix, rel_pos.x, rel_pos.y);
	AEGfxSetTransform(camera_matrix.m);
	AEGfxSetTintColor(tint.r, tint.g, tint.b, tint.a);

	AEGfxSetTransparency(transparency);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

/**
 * @brief Draw the sprite with zoom factor from the camera
 * 
 * @param sprite 
 * Pointer to the sprite
 * @param position 
 * Position
 * @param scale 
 * Scale factor
 */
void draw(const Sprite* sprite, const AEVec2 position, AEVec2 scale)
{
	if (sprite)
	{
		AEVec2 temp_scale = scale;
		if (sprite->flip_x && sprite->flip_y)
		{
			temp_scale = { -scale.x, -scale.y };
		}
		else if (sprite->flip_x)
		{
			temp_scale = { -scale.x, scale.y };
		}
		else if (sprite->flip_y)
		{
			temp_scale = { scale.x, -scale.y };
		}
		AEVec2 tex_offset = sprite->offset;
		tex_offset.x = tex_offset.x / sprite->size.x;
		tex_offset.y = tex_offset.y / sprite->size.y;
		draw(sprite->texture, sprite->mesh, position,
			tex_offset, sprite->tint, sprite->transparency, temp_scale, sprite->rot);
	}
}

/**
 * @brief Draw the sprite with zoom factor from the camera
 * 
 * @param sprite 
 * Pointer to the sprite
 * @param position 
 * Position
 */
void draw(const Sprite* sprite, const AEVec2 position)
{
	if (sprite)
	{
		AEVec2 temp_scale = sprite->scale;
		if (sprite->flip_x && sprite->flip_y)
		{
			temp_scale = { -sprite->scale.x, -sprite->scale.y };
		}
		else if (sprite->flip_x)
		{
			temp_scale = { -sprite->scale.x, sprite->scale.y };
		}
		else if (sprite->flip_y)
		{
			temp_scale = { sprite->scale.x, -sprite->scale.y };
		}
		AEVec2 tex_offset = sprite->offset;
		tex_offset.x = tex_offset.x / sprite->size.x;
		tex_offset.y = tex_offset.y / sprite->size.y;
		draw(sprite->texture, sprite->mesh, position,
			tex_offset, sprite->tint, sprite->transparency, temp_scale, sprite->rot);
	}
}

/**
 * @brief Get position relative to camera position and matrix
 * 
 * @param position 
 * @return AEVec2 
 */
AEVec2 get_relative_pos(AEVec2 position)
{
	AEMtx33 camera_matrix = Camera::get_camera_matrix();
	AEMtx33MultVec(&position, &camera_matrix, &position);
	return position;
}

/**
 * @brief Read png file and returns the dimensions (width x height) in pixels
 * 
 * @param s 
 * File path to the png file.
 * @return AEVec2 
 * Dimensions of the png file (x is the width, y is the height)
 */
AEVec2 read_png_size(std::string s)
{
	int width, height;
	std::ifstream in(s);
	in.seekg(16);
	in.read((char*)&width, 4);
	in.read((char*)&height, 4);
	in.clear();
	in.close();
	width = ntohl(width);
	height = ntohl(height);
	return { static_cast<f32>(width), static_cast<f32>(height) };
}

/**
 * @brief Rotate the AEVec2 from a pivot point
 * 
 * @param pResult 
 * Resulting vector
 * @param input 
 * Input vector
 * @param pivot
 * Pivot point 
 * @param rotation 
 * Angle in radians
 */
void AEVec2RotateFromPivot(AEVec2* pResult, AEVec2* input, AEVec2* pivot, f32 rotation)
{
	AEMtx33 rot;
	AEMtx33Rot(&rot, rotation);
	AEVec2Sub(input, input, pivot);
	AEMtx33MultVec(input, &rot, input);
	AEVec2Add(pResult, input, pivot);
}

/**
 * @brief Returns the angle in radians from a vector
 * 
 * @param vector 
 * Input vector
 * @return f32 
 * Angle in radians
 */
f32 AEVec2toRad(AEVec2* vector)
{
	AEVec2 result, e1{ 1,0 };
	AEVec2Normalize(&result, vector);
	f32 angle = acosf(AEVec2DotProduct(&result, &e1));
	return result.y < 0 ? -angle : angle;
}

/**
 * @brief Abstracts the mesh creation
 * 
 * @param translate_x 
 * Texture translation value in x
 * @param translate_y 
 * Texture translation value in y
 * @param color 
 * Color value, (0xAARRGGBB) where A = Alpha, R = Red, G = Green, B = Blue
 * @return AEGfxVertexList* 
 * Pointer to the created mesh
 */
AEGfxVertexList* create_square_mesh(f32 translate_x, f32 translate_y, s32 color)
{
	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, color, 0.0f, translate_y,
		0.5f, -0.5f, color, translate_x, translate_y,
		-0.5f, 0.5f, color, 0.0f, 0.0f); 

	AEGfxTriAdd(
		0.5f, -0.5f, color, translate_x, translate_y,
		0.5f, 0.5f, color, translate_x, 0.0f,
		-0.5f, 0.5f, color, 0.0f, 0.0f);
	return AEGfxMeshEnd();
}

/**
 * @brief Normalize the input x and y by the window's size
 * 
 * @param x 
 * @param y 
 */
void to_window_coord(f32& x, f32& y)
{
	x += AEGetWindowWidth() / 2;
	y += AEGetWindowHeight() / 2;
	x = AEWrap(x / AEGetWindowWidth(), 0, 1);
	y = AEWrap(y / AEGetWindowHeight(), 0, 1);
}

/**
 * @brief Prints a text to the screen
 * 
 * @param font 
 * Font index
 * @param text
 * Text to print 
 * @param pos 
 * Pointer to the position which will be normalized. Center is {0,0}
 * @param offset 
 * Offset of the text
 * @param scale 
 * Scale factor of the text
 * @param tint 
 * Tint of the text. Alpha value will not be used here.
 */
void text_print(s8 font, std::string text, AEVec2* pos, AEVec2 offset, f32 scale, Color tint)
{
	AEVec2 coord = { pos->x, pos->y };
	coord.x += offset.x;
	coord.y -= offset.y;
	coord.x /= AEGetWindowWidth() / 2;
	coord.y /= AEGetWindowHeight() / 2;
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxPrint(font, const_cast<s8*>(text.c_str()), coord.x, coord.y, scale, tint.r, tint.g, tint.b);
}