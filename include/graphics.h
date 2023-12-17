/*!*************************************************************************
****
\file   graphics.h
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Feb 3 2021

\brief
This header file outlines an implementation of the graphics system in the
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
#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include "AEEngine.h"
#include <fstream>
#include <winsock.h>
constexpr int MAX_LAYERS = 10;

struct Color {
	f32 r;
	f32 g;
	f32 b;
	f32 a;
};

class Sprite {
public:
	AEGfxTexture* texture;
	AEGfxVertexList* mesh;
	AEVec2 offset; // texture offset
	Color tint;
	f32 transparency;
	AEVec2 size; // texture size
	AEVec2 scale;
	float rot;
	bool flip_x;
	bool flip_y;
	Sprite(); // ctor
	Sprite(AEGfxTexture* tex,
		AEGfxVertexList* mesh);
	Sprite(AEGfxTexture* tex,
		AEGfxVertexList* mesh,
		AEVec2 scale); // overloaded ctor
	Sprite(Sprite const&);
	~Sprite();
	void set_size(AEVec2);
};

void draw(AEGfxTexture* texture,
	AEGfxVertexList* mesh,
	AEVec2 position,
	AEVec2 tex_offset = { 0.0f, 0.0f },
	Color tint = { 1.0f, 1.0f, 1.0f, 1.0f },
	f32 transparency = 1.0f,
	AEVec2 scale = { 1.0f, 1.0f },
	f32 rot = 0);

//TODO - Implement this
//void draw(const Sprite* sprite, AEVec2 position, AEVec2 scale = { 1,1 });

void draw(const Sprite* sprite, AEVec2 position, AEVec2 scale = { 1, 1 });

void draw_non_relative(AEGfxTexture* texture,
	AEGfxVertexList* mesh,
	AEVec2 position,
	AEVec2 tex_offset = { 0.0f, 0.0f },
	Color tint = { 1.0f, 1.0f, 1.0f, 1.0f },
	f32 transparency = 1.0f,
	AEVec2 scale = { 1.0f, 1.0f },
	f32 rot = 0);

void draw_non_relative(const Sprite* sprite, const AEVec2 position, AEVec2 scale);

AEVec2 get_relative_pos(AEVec2 position);

AEVec2 read_png_size(std::string);

void AEVec2RotateFromPivot(AEVec2* pResult, AEVec2* input, AEVec2* pivot, f32 rotation);

f32 AEVec2toRad(AEVec2* vector);

AEGfxVertexList* create_square_mesh(f32 translate_x, f32 translate_y, s32 color = 0);

void to_window_coord(f32& x, f32& y);

void text_print(s8 font, std::string text, AEVec2* pos,
	AEVec2 offset = { 0,0 }, f32 scale = 1.0f,
	Color tint = { 1.0f,1.0f,1.0f,1.0f });
