#include "DebugConsole.h"
#include "DebugManager.h"
#include "./Engine/Engine.hpp"
#include "./Engine/Input.hpp"
#include "./Engine/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <imgui.h>
#include <cstring>

void DebugConsole::Update([[maybe_unused]]double dt)
{
}

void DebugConsole::ToggleConsole()
{
    open_ = !open_;
    
    if (open_ && commands_.empty()) {
        RegisterDefaultCommands();
        std::memset(input_buffer_, 0, sizeof(input_buffer_));
    }
    
    if (open_) {
        reclaim_focus_ = true;
    }
}

bool DebugConsole::IsOpen() const
{
    return open_;
}

void DebugConsole::RegisterCommand(const std::string& name, std::function<void(std::vector<std::string>)> handler, const std::string& helpText)
{
    CommandInfo info;
    info.handler = handler;
    info.help_text = helpText;
    commands_[name] = info;
}

void DebugConsole::ExecuteCommand(const std::string& commandLine)
{
    if (commandLine.empty()) {
        return;
    }
    
    AddToHistory(commandLine);
    output_log_.push_back("> " + commandLine);
    
    std::vector<std::string> tokens = ParseCommandLine(commandLine);
    if (tokens.empty()) {
        return;
    }
    
    std::string command = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    auto it = commands_.find(command);
    if (it != commands_.end()) {
        try {
            it->second.handler(args);
        } catch (const std::exception& e) {
            output_log_.push_back("Error: " + std::string(e.what()));
        }
    } else {
        output_log_.push_back("Unknown command: " + command);
        output_log_.push_back("Type 'help' for available commands");
    }
    
    // Maintain max output size
    while (output_log_.size() > max_output_) {
        output_log_.pop_front();
    }
}

void DebugConsole::AddToHistory(const std::string& command)
{
    command_history_.push_back(command);
    while (command_history_.size() > max_history_) {
        command_history_.pop_front();
    }
    history_index_ = -1;
}

void DebugConsole::ClearHistory()
{
    command_history_.clear();
    output_log_.clear();
    history_index_ = -1;
}

void DebugConsole::RegisterDefaultCommands()
{
    // Help command
    RegisterCommand("help", [this](std::vector<std::string> args) {
        if (args.empty()) {
            output_log_.push_back("Available commands:");
            for (const auto& [cmd_name, cmd_info] : commands_) {
                output_log_.push_back("  " + cmd_name + " - " + cmd_info.help_text);
            }
        } else {
            auto it = commands_.find(args[0]);
            if (it != commands_.end()) {
                output_log_.push_back(args[0] + ": " + it->second.help_text);
            } else {
                output_log_.push_back("Unknown command: " + args[0]);
            }
        }
    }, "Show available commands");
    
    // Clear command
    RegisterCommand("clear", [this](std::vector<std::string>) {
        output_log_.clear();
    }, "Clear console output");
    
    // Echo command - useful for testing
    RegisterCommand("echo", [this](std::vector<std::string> args) {
        std::string message;
        for (const auto& arg : args) {
            message += arg + " ";
        }
        output_log_.push_back(message);
    }, "Echo text to console");
    
    // Add more game-specific commands here as needed
    // Example: spawn, teleport, give_item, etc.
}

void DebugConsole::DrawImGui()
{
    if (!open_) {
        return;
    }
    
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(660, 520), ImGuiCond_FirstUseEver);
    
    if (!ImGui::Begin("Console", &open_, ImGuiWindowFlags_None)) {
        ImGui::End();
        return;
    }
    
    // Output log area
    const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("Output", ImVec2(0, -footer_height), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
    for (const auto& line : output_log_) {
        ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        if (line[0] == '>') {
            color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);  // Green for commands
        } else if (line.find("Error") != std::string::npos) {
            color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);  // Red for errors
        } else if (line.find("Unknown") != std::string::npos) {
            color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);  // Yellow for warnings
        }
        ImGui::TextColored(color, "%s", line.c_str());
    }
    
    if (scroll_to_bottom_) {
        ImGui::SetScrollHereY(1.0f);
        scroll_to_bottom_ = false;
    }
    
    ImGui::PopStyleVar();
    ImGui::EndChild();
    
    // Input area
    ImGui::Separator();
    
    bool reclaim = reclaim_focus_;
    if (reclaim) {
        ImGui::SetKeyboardFocusHere();
        reclaim_focus_ = false;
    }
    
    ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_EnterReturnsTrue | 
                                      ImGuiInputTextFlags_CallbackHistory;
    
    auto callback = [](ImGuiInputTextCallbackData* data) -> int {
        DebugConsole* console = static_cast<DebugConsole*>(data->UserData);
        
        if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
            const int prev_history_pos = console->history_index_;
            
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (console->history_index_ == -1) {
                    console->history_index_ = static_cast<int>(console->command_history_.size()) - 1;
                } else if (console->history_index_ > 0) {
                    console->history_index_--;
                }
            } else if (data->EventKey == ImGuiKey_DownArrow) {
                if (console->history_index_ != -1) {
                    console->history_index_++;
                    if (console->history_index_ >= static_cast<int>(console->command_history_.size())) {
                        console->history_index_ = -1;
                    }
                }
            }
            
            if (prev_history_pos != console->history_index_) {
                const char* history_str = (console->history_index_ >= 0) ? 
                    console->command_history_[console->history_index_].c_str() : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
        return 0;
    };
    
    if (ImGui::InputText("Input", input_buffer_, IM_ARRAYSIZE(input_buffer_), 
                         input_flags, callback, (void*)this)) {
        std::string command(input_buffer_);
        if (!command.empty()) {
            ExecuteCommand(command);
            std::memset(input_buffer_, 0, sizeof(input_buffer_));
            scroll_to_bottom_ = true;
        }
        reclaim_focus_ = true;
    }
    
    ImGui::SetItemDefaultFocus();
    
    ImGui::End();
}

std::vector<std::string> DebugConsole::ParseCommandLine(const std::string& commandLine)
{
    std::vector<std::string> tokens;
    std::istringstream iss(commandLine);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}
