#pragma once
#include "./Engine/Vec2.h"

#include <deque>
#include <functional>
#include <map>
#include <string>
#include <vector>

// Debug console for runtime commands using ImGui
// Owned and managed by DebugManager
class DebugConsole
{
  public:
  DebugConsole()							   = default;
  ~DebugConsole()							   = default;
  DebugConsole(const DebugConsole&)			   = delete;
  DebugConsole& operator=(const DebugConsole&) = delete;

  void Update(double dt);
  void DrawImGui(); // Draw ImGui console window

  void ToggleConsole();
  bool IsOpen() const;

  void RegisterCommand(const std::string& name, std::function<void(std::vector<std::string>)> handler, const std::string& helpText);
  void ExecuteCommand(const std::string& commandLine);

  // Command history
  void AddToHistory(const std::string& command);
  void ClearHistory();

  private:
  struct CommandInfo
  {
	std::function<void(std::vector<std::string>)> handler;
	std::string									  help_text;
  };

  void					   RegisterDefaultCommands();
  std::vector<std::string> ParseCommandLine(const std::string& commandLine);

  bool								 open_{ false };
  char								 input_buffer_[256];
  std::deque<std::string>			 command_history_;
  std::deque<std::string>			 output_log_;
  std::map<std::string, CommandInfo> commands_;

  size_t max_history_{ 50 };
  size_t max_output_{ 100 };
  int	 history_index_{ -1 };
  bool	 scroll_to_bottom_{ false };
  bool	 reclaim_focus_{ false };
};