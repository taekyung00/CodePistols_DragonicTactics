/**
 * \file
 * \author Junyoung Ki
 * \date 2025 Fall
 * \copyright DigiPen Institute of Technology
 */
#pragma once
#include "./Engine/Component.h"
#include "./Engine/Vec2.h"
#include "./Game/DragonicTactics/Objects/Character.h"
// #include "./Game/DragonicTactics/States/Test.h"
#include "./Game/DragonicTactics/Test/Week1TestMocks.h"
#include "./Game/DragonicTactics/StateComponents/MapDataRegistry.h"
#include <map>

struct MapData;

class GridSystem : public CS230::Component
{
  public:
  enum class TileType
  {
	Empty,
	Wall,
	Lava,
	Difficult,
	Exit,
	Invalid
  };

  //출구 위치 관리
  void SetExitPosition(Math::ivec2 pos);

  Math::ivec2 GetExitPosition() const;

  bool HasExit() const;

  // ========================================
  // 신규 추가: 이동 범위 시각화
  // ========================================

  /// @brief 이동 가능한 타일들을 계산 (BFS 기반)
  /// @param start 시작 위치
  /// @param max_distance 최대 이동 거리 (Speed)
  /// @return 이동 가능한 타일 목록
  std::vector<Math::ivec2> GetReachableTiles(Math::ivec2 start, int max_distance);

  /// @brief 이동 모드 활성화 (이동 가능 타일 계산 및 저장)
  /// @param character_pos 캐릭터 현재 위치
  /// @param movement_range 캐릭터 이동 범위
  void EnableMovementMode(Math::ivec2 character_pos, int movement_range);

  /// @brief 이동 모드 비활성화 (시각화 데이터 초기화)
  void DisableMovementMode();

  /// @brief 마우스 호버 위치 설정 (경로 계산)
  /// @param hovered_tile 마우스가 위치한 타일
  void SetHoveredTile(Math::ivec2 hovered_tile);

  /// @brief 마우스 호버 해제
  void ClearHoveredTile();

  /// @brief 이동 모드 활성화 여부
  bool IsMovementModeActive() const
  {
	return movement_mode_active_;
  }

  /// @brief 특정 타일이 이동 가능한지 확인
  bool IsReachable(Math::ivec2 tile) const;

  private:
  static const int MAP_WIDTH  = 8;
  static const int MAP_HEIGHT = 8;
  TileType		   tile_grid[MAP_HEIGHT][MAP_WIDTH];
  Character*	   character_grid[MAP_HEIGHT][MAP_WIDTH]; // instead of std::map<Math::vec2, Character*> occupiedTiles;

  // A* pathfinding node
  struct Node
  {
	Math::ivec2 position;
	int			gCost;
	int			hCost;

	int fCost() const
	{
	  return gCost + hCost;
	}

	Node* parent;

	Node(Math::ivec2 pos, int g, int h, Node* p = nullptr) : position(pos), gCost(g), hCost(h), parent(p)
	{
	}
  };

  Math::ivec2 exit_position_ = {-1, -1};  // 출구 위치 (-1, -1은 없음)

  // ========================================
  // 신규 추가: 시각화 데이터
  // ========================================
  bool					   movement_mode_active_ = false;	   // 이동 모드 활성화 여부
  Math::ivec2			   movement_source_pos_	 = { -1, -1 }; // 이동 시작 위치
  std::set<Math::ivec2>	   reachable_tiles_;				   // 이동 가능한 타일 집합
  std::vector<Math::ivec2> hovered_path_;					   // 마우스 호버 시 경로
  Math::ivec2			   hovered_tile_ = { -1, -1 };		   // 현재 마우스 호버 타일
  double				   pulse_timer_	 = 0.0;

  public:
  static const int TILE_SIZE = MAP_WIDTH * MAP_HEIGHT;

  GridSystem();

  void Reset();

  // validation methods
  bool	   IsValidTile(Math::ivec2 tile) const;
  bool	   IsWalkable(Math::ivec2 tile) const;
  bool	   IsOccupied(Math::ivec2 tile) const;
  void	   SetTileType(Math::ivec2 tile, TileType type);
  TileType GetTileType(Math::ivec2 tile) const;

  // Character placement
  void		 AddCharacter(Character* character, Math::ivec2 pos); // instead of void PlaceCharacter(Character* character, Math::vec2 pos);
  void		 RemoveCharacter(Math::ivec2 pos);
  void		 MoveCharacter(Math::ivec2 old_pos, Math::ivec2 new_pos);
  Character* GetCharacterAt(Math::ivec2 pos) const;

  // week2 : pathfinding methods
  std::vector<Math::ivec2> FindPath(Math::ivec2 start, Math::ivec2 goal);
  // int						GetPathLength(Math::ivec2 start, Math::ivec2 goal);
  // std::vector<Math::ivec2> GetReachableTiles(Math::ivec2 start, int maxDistance);

  // week2 : helper methods
  int					   ManhattanDistance(Math::ivec2 a, Math::ivec2 b) const;
  std::vector<Math::ivec2> GetNeighbors(Math::ivec2 position) const;

  std::vector<Character*> GetAllCharacters();

  void Draw() const;

  void Update(double dt) override;

  void LoadMap(const MapData& map_data);
};

// ========================================
// Math::ivec2 비교 연산자 (std::set 사용을 위해 필요)
// ========================================
inline bool operator<(const Math::ivec2& a, const Math::ivec2& b)
{
  if (a.x != b.x)
	return a.x < b.x;
  return a.y < b.y;
}

//inline bool operator==(const Math::ivec2& a, const Math::ivec2& b)
//{
//  return a.x == b.x && a.y == b.y;
//}

//inline bool operator!=(const Math::ivec2& a, const Math::ivec2& b)
//{
//  return !(a == b);
//}