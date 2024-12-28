#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

const int WND_H = 900;
const int WND_W = 1600;
const int GRID_W = 32;
const int GRID_H = 32;

bool is_running = true;

void initialize_cell_layout(int *cell_x_positions, int *cell_y_positions, int rows, int cols, float cell_size, float offset) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cell_x_positions[i * cols + j] = 8 + j * (cell_size + offset * 2);
            cell_y_positions[i * cols + j] = 8 + i * (cell_size + offset * 2);
        }
    }
}

int draw_grid(float cell_size, float offset, int *grid, int *cell_x_positions, int *cell_y_positions) {
    for (int i = 0; i < GRID_H; i++) {
        for (int j = 0; j < GRID_W; j++) {
            Color cell_color = grid[i * GRID_W + j] ? BLACK : RAYWHITE;
            DrawRectangle(cell_x_positions[i * GRID_W + j], cell_y_positions[i * GRID_W + j], cell_size, cell_size, cell_color);
        }
    }
    return 0;
}

int count_live_neighbors(int *grid, int i, int j) {
    int live_neighbors = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            if (x == 0 && y == 0) continue;
            int ni = i + x;
            int nj = j + y;
            if (ni >= 0 && ni < GRID_H && nj >= 0 && nj < GRID_W) {
                live_neighbors += grid[ni * GRID_W + nj];
            }
        }
    }
    return live_neighbors;
}

void update_grid(int *grid, int *new_grid) {
    for (int i = 0; i < GRID_H; i++) {
        for (int j = 0; j < GRID_W; j++) {
            int live_neighbors = count_live_neighbors(grid, i, j);

            if (grid[i * GRID_W + j] == 1) {
                new_grid[i * GRID_W + j] = (live_neighbors == 2 || live_neighbors == 3) ? 1 : 0;
            } else {
                new_grid[i * GRID_W + j] = (live_neighbors == 3) ? 1 : 0;
            }
        }
    }
}

void handle_mouse_drag(int *grid, float cell_size, float offset, int *cell_x_positions, int *cell_y_positions) {
    static bool is_dragging = false;
    static int last_i = -1, last_j = -1;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        Vector2 mouse_pos = GetMousePosition();

        int i = -1, j = -1;
        for (int idx = 0; idx < GRID_H * GRID_W; idx++) {
            if (mouse_pos.x >= cell_x_positions[idx] && mouse_pos.x < cell_x_positions[idx] + cell_size &&
                mouse_pos.y >= cell_y_positions[idx] && mouse_pos.y < cell_y_positions[idx] + cell_size) {
                i = idx / GRID_W;
                j = idx % GRID_W;
                break;
            }
        }

        if (i >= 0 && i < GRID_H && j >= 0 && j < GRID_W) {
            if (i != last_i || j != last_j) {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    grid[i * GRID_W + j] = 1;
                } else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
                    grid[i * GRID_W + j] = 0;
                }

                last_i = i;
                last_j = j;
            }
            is_dragging = true;
        }
    } else {
        is_dragging = false;
        last_i = -1;
        last_j = -1;
    }
}

void sand_button_pressed() {
    TraceLog(LOG_INFO, "Sand button pressed!");
}

void water_button_pressed() {
    TraceLog(LOG_INFO, "Water button pressed!");
}

void rock_button_pressed() {
    TraceLog(LOG_INFO, "Rock button pressed!");
}

void fire_button_pressed() {
    TraceLog(LOG_INFO, "Fire button pressed!");
}


void draw_buttons(float grid_width, float remaining_width) {
    Rectangle buttonSand = { grid_width, 50, remaining_width, 50 };
    Rectangle buttonWater = { grid_width, 110, remaining_width, 50 };
    Rectangle buttonRock = { grid_width, 170, remaining_width, 50 };
    Rectangle buttonFire = { grid_width, 230, remaining_width, 50 };


    DrawRectangleRec(buttonSand, SKYBLUE);
    DrawText("sand", buttonSand.x + (buttonSand.width - MeasureText("sand", 20)) / 2, buttonSand.y + (buttonSand.height - 20) / 2, 20, DARKGRAY);

    DrawRectangleRec(buttonWater, SKYBLUE);
    DrawText("water", buttonWater.x + (buttonWater.width - MeasureText("water", 20)) / 2, buttonWater.y + (buttonWater.height - 20) / 2, 20, DARKGRAY);

    DrawRectangleRec(buttonRock, SKYBLUE);
    DrawText("rock", buttonRock.x + (buttonRock.width - MeasureText("rock", 20)) / 2, buttonRock.y + (buttonRock.height - 20) / 2, 20, DARKGRAY);

    DrawRectangleRec(buttonFire, SKYBLUE);
    DrawText("fire", buttonFire.x + (buttonFire.width - MeasureText("fire", 20)) / 2, buttonFire.y + (buttonFire.height - 20) / 2, 20, DARKGRAY);
}


void handle_button_input(float grid_width, float remaining_width) {
    Rectangle buttonSand = { grid_width, 50, remaining_width, 50 };
    Rectangle buttonWater = { grid_width, 110, remaining_width, 50 };
    Rectangle buttonRock = { grid_width, 170, remaining_width, 50 };
    Rectangle buttonFire = { grid_width, 230, remaining_width, 50 };

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mousePos, buttonSand)) sand_button_pressed();
        if (CheckCollisionPointRec(mousePos, buttonWater)) water_button_pressed();
        if (CheckCollisionPointRec(mousePos, buttonRock)) rock_button_pressed();
        if (CheckCollisionPointRec(mousePos, buttonFire)) fire_button_pressed();
    }
}

int main(void) {
    InitWindow(WND_W, WND_H, "Falling Sand");

    int *grid = (int *)malloc(GRID_H * GRID_W * sizeof(int));
    int *new_grid = (int *)malloc(GRID_H * GRID_W * sizeof(int));

    int *cell_x_positions = (int *)malloc(GRID_H * GRID_W * sizeof(int));
    int *cell_y_positions = (int *)malloc(GRID_H * GRID_W * sizeof(int));

    float cell_size = WND_H / GRID_H * 0.9;
    int offset = ceilf(cell_size * 0.01);

    initialize_cell_layout(cell_x_positions, cell_y_positions, GRID_H, GRID_W, cell_size, offset);

    for (int i = 0; i < GRID_H * GRID_W; i++) {
        grid[i] = GetRandomValue(0, 1);
    }

    float update_interval = 0.1f;
    float time_since_last_update = 0.0f;

    while (!WindowShouldClose()) {
        if (IsKeyReleased(KEY_SPACE)) {
            is_running = !is_running;
        }

        handle_mouse_drag(grid, cell_size, offset, cell_x_positions, cell_y_positions);

        float delta_time = GetFrameTime();
        time_since_last_update += delta_time;
        if (time_since_last_update >= update_interval && is_running) {
            update_grid(grid, new_grid);

            int *temp = grid;
            grid = new_grid;
            new_grid = temp;

            time_since_last_update = 0.0f;
        }

        float grid_width = 8 + GRID_W * (cell_size + 2 * offset);
        float remaining_width = WND_W - grid_width;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_grid(cell_size, offset, grid, cell_x_positions, cell_y_positions);
        draw_buttons(grid_width, remaining_width);
        EndDrawing();

        handle_button_input(grid_width, remaining_width);

        if (IsWindowResized()) {
            cell_size = GetScreenHeight() / GRID_H * 0.9;
            offset = ceilf(cell_size * 0.01);
            initialize_cell_layout(cell_x_positions, cell_y_positions, GRID_H, GRID_W, cell_size, offset);
        }
    }

    free(grid);
    free(new_grid);
    free(cell_x_positions);
    free(cell_y_positions);
    CloseWindow();
    return 0;
}


