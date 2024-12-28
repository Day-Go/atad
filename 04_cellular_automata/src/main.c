#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

const int WND_H = 900;
const int WND_W = 1600;
const int GRID_W = 32;
const int GRID_H = 32;

int draw_grid(float cell_size, float offset) {
    int pos_y = 8;
    for (int i = 0; i < GRID_W; i++) {
        int pos_x = 8;
        for (int j = 0; j < GRID_H; j++) {
            DrawRectangle(pos_x, pos_y, cell_size, cell_size, BLACK);
            pos_x += cell_size + offset * 2;
        }
        pos_y += cell_size + offset * 2;
    }
    return 0;
}

int main(void) {
    InitWindow(WND_W, WND_H, "Falling Sand");

    int *grid = (int *)malloc(GRID_H * GRID_W * sizeof(int));
    // Access: array[i * cols + j]

    float cell_size = WND_H / GRID_H * 0.90; 
    int offset = ceilf(cell_size * 0.01);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_grid(cell_size, offset);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}

