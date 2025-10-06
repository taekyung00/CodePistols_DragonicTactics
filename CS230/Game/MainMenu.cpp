/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    May 6, 2025
*/

#include "MainMenu.h"
#include "States.h"

MainMenu::MainMenu() : 
	current_option(Option::side_scroller),
	title_texture(nullptr){}

void MainMenu::Load()
{
	title_texture = (Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("CS230 Engine Test", title_color));
	update_textures();
}

void MainMenu::Update([[maybe_unused]] double dt)
{
	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Down)) {
		switch (current_option)
		{
		case MainMenu::Option::side_scroller:
			current_option = Option::space_shooter;			
			break;
		case MainMenu::Option::space_shooter:
			current_option = Option::exit;
			break;
		case MainMenu::Option::exit:
			current_option = Option::side_scroller;
			break;
		}
	}

	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
		switch (current_option)
		{
		case MainMenu::Option::side_scroller:
			current_option = Option::exit;
			break;
		case MainMenu::Option::space_shooter:
			current_option = Option::side_scroller;
			break;
		case MainMenu::Option::exit:
			current_option = Option::space_shooter;
			break;
		}
	}
	update_textures();
	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Enter)) {
		switch (current_option)
		{
		case MainMenu::Option::side_scroller:
			Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode1));
			break;
		case MainMenu::Option::space_shooter:
			Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode2));
			break;
		case MainMenu::Option::exit:
			Engine::GetGameStateManager().ClearNextGameState();
			break;
		}
	}
}

void MainMenu::Unload()
{
}

void MainMenu::Draw()
{
	Engine::GetWindow().Clear(0x000000FF);
	title_texture->Draw(Math::TranslationMatrix(Math::ivec2{ 
		Engine::GetWindow().GetSize().x/2  - title_texture->GetSize().x/2 - 100, 
		Engine::GetWindow().GetSize().y - title_texture->GetSize().y - 100 }) * Math::ScaleMatrix(1.5));

	side_scroller_texture->Draw(Math::TranslationMatrix(Math::ivec2{
		Engine::GetWindow().GetSize().x / 2 - side_scroller_texture->GetSize().x / 2,
		Engine::GetWindow().GetSize().y - side_scroller_texture->GetSize().y - 200 }));

	space_shooter_texture->Draw(Math::TranslationMatrix(Math::ivec2{ 
		Engine::GetWindow().GetSize().x / 2  - space_shooter_texture->GetSize().x / 2,
		Engine::GetWindow().GetSize().y - space_shooter_texture->GetSize().y - 300 }));

	exit_texture->Draw(Math::TranslationMatrix(Math::ivec2{
		Engine::GetWindow().GetSize().x / 2 - 10 - exit_texture->GetSize().x / 2,
		Engine::GetWindow().GetSize().y - exit_texture->GetSize().y - 400 }));

}

void MainMenu::update_textures()
{
	delete side_scroller_texture;
	delete space_shooter_texture;
	delete exit_texture;
	switch (current_option)
	{
	case MainMenu::Option::side_scroller:
		side_scroller_color = seleted_color;
		space_shooter_color = non_seleted_color;
		exit_color = non_seleted_color;
		break;
	case MainMenu::Option::space_shooter:
		side_scroller_color = non_seleted_color;
		space_shooter_color = seleted_color;
		exit_color = non_seleted_color;
		break;
	case MainMenu::Option::exit:
		side_scroller_color = non_seleted_color;
		space_shooter_color = non_seleted_color;
		exit_color = seleted_color;
		break;
	}
	side_scroller_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Side Scroller", side_scroller_color);
	space_shooter_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Space Shooter", space_shooter_color);
	exit_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Exit", exit_color);

}
