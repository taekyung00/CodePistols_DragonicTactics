/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Animation.cpp
Project:    CS230 Engine
Author:     Taekyung Ho
Created:    April 16, 2025
*/
#include "Animation.h"
#include "Logger.hpp"
#include "Path.hpp"

CS230::Animation::Animation(const std::filesystem::path& animation_file) : current_command(0)
{
    const std::filesystem::path anm_path = assets::locate_asset(animation_file);

    if (anm_path.extension() != ".anm")
    {
        throw std::runtime_error(animation_file.generic_string() + " is not a .anm file");
    }


    std::ifstream in_file(anm_path);
    if (in_file.is_open() == false)
    {
        throw std::runtime_error("Failed to load " + animation_file.generic_string());
    }

    std::string command;
    while (in_file.eof() == false)
    {
        in_file >> command;
        if (command == "PlayFrame")
        {
            size_t frame;
            float  target_time;
            in_file >> frame;
            in_file >> target_time;

            commands.push_back(new PlayFrame(frame, static_cast<double>(target_time)));
        }
        else if (command == "Loop")
        {
            size_t loop_to_frame;
            in_file >> loop_to_frame;
            commands.push_back(new Loop(loop_to_frame));
        }
        else if (command == "End")
        {
            commands.push_back(new End());
        }
        else
        {
            Engine::GetLogger().LogError(command + " in " + animation_file.generic_string());
        }
    }
    Reset();
}

CS230::Animation::Animation() : Animation("./Assets/animations/None.anm")
{
}

CS230::Animation::~Animation()
{
    for (Command* command : commands)
    {
        delete command;
    }
    commands.clear();
}

void CS230::Animation::Update(double dt)
{
    current_frame->Update(dt);
    if (current_frame->Ended() == true)
    {
        current_frame->ResetTime();
        current_command++;
        if (commands[current_command]->Type() == CommandType::PlayFrame)
        {
            current_frame = static_cast<PlayFrame*>(commands[current_command]);
        }
        else if (commands[current_command]->Type() == CommandType::Loop)
        {
            Loop* loop_data = static_cast<Loop*>(commands[current_command]);
            current_command = loop_data->LoopIndex();
            if (commands[current_command]->Type() == CommandType::PlayFrame)
            {
                current_frame = static_cast<PlayFrame*>(commands[current_command]);
            }
            else
            {
                Engine::GetLogger().LogError("Loop does not go to PlayFrame");
                Reset();
            }
        }
        else if (commands[current_command]->Type() == CommandType::End)
        {
            ended = true;
        }
    }
}

size_t CS230::Animation::CurrentFrame()
{
    return current_frame->Frame();
}

void CS230::Animation::Reset()
{
    current_command = 0;
    ended           = false;
    current_frame   = static_cast<PlayFrame*>(commands[current_command]);
    current_frame->ResetTime();
}

bool CS230::Animation::Ended()
{
    return ended;
}

CS230::Animation::PlayFrame::PlayFrame(size_t _frame, double duration) : frame(_frame), target_time(duration), timer(0.0)
{
}

void CS230::Animation::PlayFrame::Update(double dt)
{
    timer += dt;
}

bool CS230::Animation::PlayFrame::Ended()
{
    if (timer >= target_time)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CS230::Animation::PlayFrame::ResetTime()
{
    timer = 0.0;
}

size_t CS230::Animation::PlayFrame::Frame()
{
    return frame;
}

CS230::Animation::Loop::Loop(size_t _loop_index) : loop_index(_loop_index)
{
}

size_t CS230::Animation::Loop::LoopIndex()
{
    return loop_index;
}
