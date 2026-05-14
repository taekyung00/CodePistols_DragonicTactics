#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm>
#include <string>

constexpr int x_ratio = 16;
constexpr int y_ratio = 9;
constexpr int tile_size = 64;
constexpr int grid_size = 12;

// 64픽셀을 1 Unit으로 계산하여 비례를 보여주는 디버그 함수
// text_outside 매개변수를 추가하여 텍스트를 상자 밖에 그릴지 결정합니다.
void DrawDebugUnits(float position_x, float position_y, float width, float height, float base_unit, const char* element_name, bool text_outside = false) {
    // 테두리는 마젠타 유지
    DrawRectangleLines(position_x, position_y, width, height, MAGENTA);

    // 위치와 크기를 기본 단위(64)로 나누어 배수 계산
    float unit_x = position_x / base_unit;
    float unit_y = position_y / base_unit;
    float unit_width = width / base_unit;
    float unit_height = height / base_unit;

    // 텍스트 포맷팅 (U = Unit = 64px)
    const char* debug_text = TextFormat("%s\nPos: %.2f U, %.2f U\nSize: %.2f U x %.2f U",
        element_name, unit_x, unit_y, unit_width, unit_height);

    // text_outside가 true이면 상자 위쪽으로 40픽셀 띄워서 출력
    float text_pos_x = position_x + 5;
    float text_pos_y = text_outside ? (position_y - 40) : (position_y + 5);

    // 글자 색상을 BLACK으로 변경
    DrawText(debug_text, text_pos_x, text_pos_y, 10, BLACK);
}

int main() {
    int virtual_screen_width = x_ratio * 100;  // 1600
    int virtual_screen_height = y_ratio * 100; // 900

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtual_screen_width, virtual_screen_height, "Tactical Game UI & Camera with Unit Debug");
    SetTargetFPS(60);

    RenderTexture2D target_texture = LoadRenderTexture(virtual_screen_width, virtual_screen_height);

    Camera2D camera = { 0 };
    camera.target = { (grid_size * tile_size) / 2.0f, (grid_size * tile_size) / 2.0f };
    camera.offset = { virtual_screen_width / 2.0f, virtual_screen_height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    int ui_box_position_x = tile_size;
    int ui_box_position_y = virtual_screen_height - 2 * tile_size;
    int ui_box_width = virtual_screen_width - 2 * tile_size;
    int ui_box_height = tile_size * 1.5;
    int slot_size = tile_size;
    int off_set = tile_size / 4;

    bool is_debug_mode = false;

    while (!WindowShouldClose()) {
        int current_window_width = GetScreenWidth();
        int current_window_height = GetScreenHeight();

        float scale = std::min((float)current_window_width / virtual_screen_width,
            (float)current_window_height / virtual_screen_height);
        float offset_x = (current_window_width - (virtual_screen_width * scale)) * 0.5f;
        float offset_y = (current_window_height - (virtual_screen_height * scale)) * 0.5f;

        Vector2 mouse_position = GetMousePosition();
        Vector2 virtual_mouse_position = {
            (mouse_position.x - offset_x) / scale,
            (mouse_position.y - offset_y) / scale
        };

        if (IsKeyPressed(KEY_TAB)) {
            is_debug_mode = !is_debug_mode;
        }

        // --- [카메라 제어 및 Clamping 로직] ---
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse_delta = GetMouseDelta();
            camera.target.x -= (mouse_delta.x / scale) / camera.zoom;
            camera.target.y -= (mouse_delta.y / scale) / camera.zoom;
        }

        float wheel_move = GetMouseWheelMove();
        if (wheel_move != 0.0f) {
            Vector2 mouse_world_position = GetScreenToWorld2D(virtual_mouse_position, camera);
            camera.zoom += (wheel_move * 0.125f);
            if (camera.zoom < 0.25f) camera.zoom = 0.25f;
            if (camera.zoom > 3.0f) camera.zoom = 3.0f;

            camera.offset = virtual_mouse_position;
            camera.target = mouse_world_position;
        }

        float map_limit = grid_size * tile_size;
        camera.target.x = Clamp(camera.target.x, 0.0f, map_limit);
        camera.target.y = Clamp(camera.target.y, 0.0f, map_limit);

        // --- [렌더링 파이프라인] ---
        BeginTextureMode(target_texture);
        ClearBackground(DARKGRAY);

        BeginMode2D(camera);

        // 월드 렌더링 (Row = Y축, Column = X축)
        for (int row = 0; row < grid_size; ++row) {
            for (int col = 0; col < grid_size; ++col) {
                Color tile_color = ((row + col) % 2 == 0) ? LIGHTGRAY : GRAY;
                DrawRectangle(col * tile_size, row * tile_size, tile_size, tile_size, tile_color);
            }
        }
        DrawRectangleLines(0, 0, grid_size * tile_size, grid_size * tile_size, BLACK);

        EndMode2D();

        // --- [UI 렌더링] ---
        DrawRectangle(ui_box_position_x, ui_box_position_y, ui_box_width, ui_box_height, RAYWHITE);

        int max_slots = 10;

        // 남는 빈 공간 계산 = 전체 박스 너비 - (슬롯 10개 너비 + 버튼 1개 너비)
        float remaining_space = ui_box_width - (max_slots * slot_size) - (2 * slot_size);

        // 간격의 크기 = 슬롯 10개 + 버튼 1개 + 양끝 여백 = 총 12개의 간격으로 나눔
        float dynamic_offset = remaining_space / 12.0f;

        // 첫 번째 요소가 그려질 시작 위치 (시작 여백 적용)
        float current_element_x = ui_box_position_x + dynamic_offset;

        // 슬롯 10개 그리기
        for (int i = 0; i < max_slots; ++i) {
            DrawRectangle(current_element_x, ui_box_position_y + (ui_box_height - slot_size) / 2, slot_size, slot_size, BLUE);

            if (is_debug_mode) {
                std::string slot_name = "Slot " + std::to_string(i);
                DrawDebugUnits(current_element_x, ui_box_position_y + (ui_box_height - slot_size) / 2, slot_size, slot_size, tile_size, slot_name.c_str());
            }

            // 다음 위치로 이동 (현재 슬롯 너비 + 자동 계산된 간격)
            current_element_x += slot_size + dynamic_offset;
        }

        // 턴 종료 버튼 (슬롯 반복문이 끝난 후의 current_element_x 위치에 그대로 그림)
        float turn_button_position_x = current_element_x;
        float turn_button_position_y = ui_box_position_y + (ui_box_height - slot_size) / 2;
        DrawRectangle(turn_button_position_x, turn_button_position_y, 2 * slot_size, slot_size, RED);

        // --- [디버그 모드 오버레이] ---
        if (is_debug_mode) {
            DrawDebugUnits(ui_box_position_x, ui_box_position_y, ui_box_width, ui_box_height, tile_size, "Main UI Box", true);
            DrawDebugUnits(turn_button_position_x, turn_button_position_y, 2 * slot_size, slot_size, tile_size, "Turn End Btn");

            Vector2 tile_screen_position = GetWorldToScreen2D({ 0.0f, 0.0f }, camera);
            float tile_screen_size = tile_size * camera.zoom;
            DrawDebugUnits(tile_screen_position.x, tile_screen_position.y, tile_screen_size, tile_screen_size, tile_size, "Tile [0][0]");
        }


        // --- [디버그 모드 오버레이] ---
        if (is_debug_mode) {
            // UI 영역 디버그 (tile_size 기준) - 마지막 인자를 true로 주어 텍스트를 상자 밖으로 빼냄
            DrawDebugUnits(ui_box_position_x, ui_box_position_y, ui_box_width, ui_box_height, tile_size, "Main UI Box", true);

            DrawDebugUnits(turn_button_position_x, turn_button_position_y, 2 * slot_size, slot_size, tile_size, "Turn End Btn");

            // 그리드 1칸의 디버그 (카메라 줌에 따른 크기 변화 확인용)
            Vector2 tile_screen_position = GetWorldToScreen2D({ 0.0f, 0.0f }, camera);
            float tile_screen_size = tile_size * camera.zoom;
            DrawDebugUnits(tile_screen_position.x, tile_screen_position.y, tile_screen_size, tile_screen_size, tile_size, "Tile [0][0]");
        }

        EndTextureMode();

        // --- [최종 화면 출력] ---
        BeginDrawing();
        ClearBackground(BLACK);

        Rectangle source_rect = { 0.0f, 0.0f, (float)target_texture.texture.width, (float)-target_texture.texture.height };
        Rectangle dest_rect = { offset_x, offset_y, virtual_screen_width * scale, virtual_screen_height * scale };
        DrawTexturePro(target_texture.texture, source_rect, dest_rect, { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadRenderTexture(target_texture);
    CloseWindow();
    return 0;
}