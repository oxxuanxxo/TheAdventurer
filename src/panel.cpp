/*!*************************************************************************
****
\file   panel.cpp
\author Cruz Rolly Matthew Capiral
\par    DP email: cruzrolly.m\@digipen.edu
\date   Apr 6 2021

\brief
This file contains the implementation of the panel UI class.

The functions include:
- update
- draw

Copyright (C) 2021 DigiPen Institure of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
****************************************************************************
***/
#include "panel.h"

/******************************************************************************
 * @brief Construct a new Panel:: Panel object
 * 
******************************************************************************/
Panel::Panel() : background{ nullptr }, pos{}, flag{ 0 } {}

/******************************************************************************
 * @brief Construct a new Prompt Panel:: Prompt Panel object
 * 
 */
PromptPanel::PromptPanel() : Panel() , title{ "" }, message{ "" }, title_pos{}, title_offset{}, message_pos{}, message_offset{} {}

/******************************************************************************
 * @brief Update the buttons in the panel
 * 
******************************************************************************/
void Panel::update()
{
	btns.update();
}

/******************************************************************************
 * @brief Draw the panel and its buttons
 * 
******************************************************************************/
void Panel::draw()
{
	draw_non_relative(background, pos, background->scale);
	btns.draw();
}

/******************************************************************************
 * @brief Draw the prompt panel
 * 
******************************************************************************/
void PromptPanel::draw()
{
	Panel::draw();
	text_print(bold_font, title, &title_pos, title_offset);
	text_print(desc_font, message, &message_pos, message_offset);
}