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
	current_option(Option::test),
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
		case MainMenu::Option::test:
			current_option = Option::exit;
			break;
		case MainMenu::Option::exit:
			current_option = Option::test;
			break;
		}
	}

	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
		switch (current_option)
		{
		case MainMenu::Option::test:
			current_option = Option::exit;
			break;
		case MainMenu::Option::exit:
			current_option = Option::test;
			break;
		}
	}
	update_textures();
	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Enter)) {
		switch (current_option)
		{
		case MainMenu::Option::test:
			Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Test));
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


	test_texture->Draw(Math::TranslationMatrix(Math::ivec2{
		Engine::GetWindow().GetSize().x / 2 - test_texture->GetSize().x / 2,
		Engine::GetWindow().GetSize().y - test_texture->GetSize().y - 400 }));

	exit_texture->Draw(Math::TranslationMatrix(Math::ivec2{
		Engine::GetWindow().GetSize().x / 2 - 10 - exit_texture->GetSize().x / 2,
		Engine::GetWindow().GetSize().y - exit_texture->GetSize().y - 500 }));

}

void MainMenu::update_textures()
{
	delete test_texture;
	delete exit_texture;
	switch (current_option)
	{
	case MainMenu::Option::test:
		test_color = seleted_color;
		exit_color = non_seleted_color;
		break;
	case MainMenu::Option::exit:
		test_color = non_seleted_color;
		exit_color = seleted_color;
		break;
	}
	test_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Test", test_color);
	exit_texture = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Exit", exit_color);

}
