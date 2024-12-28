#include <math.h>
#include <stdlib.h>
#include "raylib.h"

const int WND_H = 900;
const int WND_W = 1600;
const int GRID_W = 32;
const int GRID_H = 32;

int draw_grid(float cell_size, float offset, int *grid) {
    int pos_y = 8;
    for (int i = 0; i < GRID_H; i++) {
        int pos_x = 8;
        for (int j = 0; j < GRID_W; j++) {
            Color cell_color = grid[i * GRID_W + j] ? BLACK : RAYWHITE;
            DrawRectangle(pos_x, pos_y, cell_size, cell_size, cell_color);
            pos_x += cell_size + offset * 2;
        }
        pos_y += cell_size + offset * 2;
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

            // game of life rules for now
            if (grid[i * GRID_W + j] == 1) {
                new_grid[i * GRID_W + j] = (live_neighbors == 2 || live_neighbors == 3) ? 1 : 0;
            } else {
                new_grid[i * GRID_W + j] = (live_neighbors == 3) ? 1 : 0;
            }
        }
    }
}

int main(void) {
    InitWindow(WND_W, WND_H, "Falling Sand");

    int *grid = (int *)malloc(GRID_H * GRID_W * sizeof(int));
    int *new_grid = (int *)malloc(GRID_H * GRID_W * sizeof(int));

    for (int i = 0; i < GRID_H * GRID_W; i++) {
        grid[i] = GetRandomValue(0, 1);
    }

    float cell_size = WND_H / GRID_H * 0.90; 
    int offset = ceilf(cell_size * 0.01);

    float update_interval = 0.1f; 
    float time_since_last_update = 0.0f;

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        time_since_last_update += delta_time;

        if (time_since_last_update >= update_interval) {
            update_grid(grid, new_grid);

            int *temp = grid;
            grid = new_grid;
            new_grid = temp;

            time_since_last_update = 0.0f;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_grid(cell_size, offset, grid);
        EndDrawing();
    }

    free(grid);
    free(new_grid);
    CloseWindow();
    return 0;
}


