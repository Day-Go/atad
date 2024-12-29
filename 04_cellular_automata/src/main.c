#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

const int UI_PANEL_W = 400;
const int WND_H = 900;
const int WND_W = 1600;
const int GRID_H = 880;
const int GRID_W = WND_W - UI_PANEL_W;
const int GRID_PADDING = 10;  
const int CELL_SIZE = 20;

bool is_running = true;

typedef enum {
    NONE,
    SAND,
    WATER,
    ROCK,
    FIRE
} Element; 

Element selected_element = SAND;


// ~~~~~~~~~~~~~~~~~~~~~~~~~    RENDERING    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initialize_cell_layout(int *cell_x_positions, int *cell_y_positions, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cell_x_positions[i * cols + j] = GRID_PADDING + j * CELL_SIZE;
            cell_y_positions[i * cols + j] = GRID_PADDING + i * CELL_SIZE;
        }
    }
}

int* generate_neighbor_array(int rows, int cols) {
    int *neighbor_array = (int *)malloc(8 * rows * cols * sizeof(int));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            int neighbor_idx = 0;
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if (x == 0 && y == 0) continue;
                    int ni = i + x;
                    int nj = j + y;
                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                        neighbor_array[idx * 8 + neighbor_idx++] = ni * cols + nj;
                    } else {
                        neighbor_array[idx * 8 + neighbor_idx++] = -1; // Invalid neighbor
                    }
                }
            }
        }
    }
    return neighbor_array;
}

int draw_grid(int *grid, int *cell_x_positions, int *cell_y_positions, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            Element current_cell = grid[idx];

            Color cell_color = PURPLE;
            switch (current_cell) {
                case NONE:
                    cell_color = WHITE;
                    break;
                case SAND:
                    cell_color = BEIGE;
                    break;
                case WATER:
                    cell_color = BLUE;
                    break;
                case ROCK:
                    cell_color = GRAY;
                    break;
                case FIRE:
                    cell_color = RED;
                    break;
            }

            DrawRectangle(
                cell_x_positions[idx], 
                cell_y_positions[idx], 
                CELL_SIZE, CELL_SIZE, 
                cell_color
            );
        }
    }
    return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~    PHYSICS    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

void update_grid(int *grid, int *new_grid, int rows, int cols, int *neighbor_array) {
    // initialize the new grid with the current grid state
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            new_grid[idx] = grid[idx];
        }
    }

    // update the grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            Element current_cell = grid[idx];

            switch (current_cell) {
                case SAND:
                    // sand on the bottom row sits
                    if (i == rows - 1) {
                        new_grid[idx] = SAND;
                        break;
                    }
                    // sand with nothing below moves directly down
                    int bottom_idx = neighbor_array[idx * 8 + 6];
                    if (grid[bottom_idx] == NONE) {
                        new_grid[idx] = NONE;
                        new_grid[bottom_idx] = SAND;
                    }

                    break;

                case WATER:
                    break;

                case ROCK:
                    break;

                case FIRE:
                    break;

                case NONE:
                    break;
            }
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            grid[idx] = new_grid[idx];
        }
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~    UI    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void handle_mouse_drag(int *grid, int *cell_x_positions, int *cell_y_positions, int rows, int cols) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse_pos = GetMousePosition();

        int i = -1, j = -1;
        for (int idx = 0; idx < rows * cols; idx++) {
            if (mouse_pos.x >= cell_x_positions[idx] && mouse_pos.x < cell_x_positions[idx] + CELL_SIZE &&
                mouse_pos.y >= cell_y_positions[idx] && mouse_pos.y < cell_y_positions[idx] + CELL_SIZE) {
                i = idx / cols;
                j = idx % cols;
                break;
            }
        }

        if (i >= 0 && i < rows && j >= 0 && j < cols){
            grid[i * cols + j] = selected_element;

        }
    }
}

void sand_button_pressed() {
    selected_element = SAND;
    TraceLog(LOG_INFO, "Sand button pressed!");
}

void water_button_pressed() {
    selected_element = WATER;
    TraceLog(LOG_INFO, "Water button pressed!");
}

void rock_button_pressed() {
    selected_element = ROCK;
    TraceLog(LOG_INFO, "Rock button pressed!");
}

void fire_button_pressed() {
    selected_element = FIRE;
    TraceLog(LOG_INFO, "Fire button pressed!");
}


void draw_buttons(float grid_width, float remaining_width) {
    Rectangle buttonSand = { grid_width, 50, remaining_width, 50 };
    Rectangle buttonWater = { grid_width, 110, remaining_width, 50 };
    Rectangle buttonRock = { grid_width, 170, remaining_width, 50 };
    Rectangle buttonFire = { grid_width, 230, remaining_width, 50 };

    DrawRectangleRec(buttonSand, selected_element == SAND ? BLUE : SKYBLUE);
    DrawText("sand", buttonSand.x + (buttonSand.width - MeasureText("sand", 20)) / 2, 
                     buttonSand.y + (buttonSand.height - 20) / 2, 20, DARKGRAY);

    DrawRectangleRec(buttonWater, selected_element == WATER ? BLUE : SKYBLUE);
    DrawText("water", buttonWater.x + (buttonWater.width - MeasureText("water", 20)) / 2, 
                      buttonWater.y + (buttonWater.height - 20) / 2, 20, DARKGRAY);

    DrawRectangleRec(buttonRock, selected_element == ROCK ? BLUE : SKYBLUE);
    DrawText("rock", buttonRock.x + (buttonRock.width - MeasureText("rock", 20)) / 2, 
                     buttonRock.y + (buttonRock.height - 20) / 2, 20, DARKGRAY);

    DrawRectangleRec(buttonFire, selected_element == FIRE ? BLUE : SKYBLUE);
    DrawText("fire", buttonFire.x + (buttonFire.width - MeasureText("fire", 20)) / 2, 
                     buttonFire.y + (buttonFire.height - 20) / 2, 20, DARKGRAY);
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

    int fps = 20;
    float update_interval = 1.0f / fps;
    float time_since_last_update = 0.0f; 

    // setup CA grid
    int cols = (int)(GRID_W / CELL_SIZE);
    int rows = (int)(GRID_H / CELL_SIZE);
    printf("%i\n", cols);
    printf("%i\n", rows);
    int *grid = (int *)malloc(rows * cols * sizeof(int));

    int *new_grid = (int *)malloc(rows* cols * sizeof(int));

    int *cell_x_positions = (int *)malloc(rows * cols * sizeof(int));
    int *cell_y_positions = (int *)malloc(rows * cols * sizeof(int));

    initialize_cell_layout(cell_x_positions, cell_y_positions, rows, cols);

    int *neighbor_array = generate_neighbor_array(rows, cols);

    for (int i = 0; i < rows * cols; i++) {
        grid[i] = NONE;
    }

    while (!WindowShouldClose()) {
        if (IsKeyReleased(KEY_SPACE)) {
            is_running = !is_running;
        }

        float delta_time = GetFrameTime();
        time_since_last_update += delta_time;
        if (time_since_last_update >= update_interval && is_running) {
            handle_mouse_drag(grid, cell_x_positions, cell_y_positions, rows, cols);

            update_grid(grid, new_grid, rows, cols, neighbor_array);

            int *temp = grid;
            grid = new_grid;
            new_grid = temp;

            time_since_last_update = 0.0f;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_grid(grid, cell_x_positions, cell_y_positions, rows, cols);
        
        draw_buttons(GRID_W, UI_PANEL_W);
        EndDrawing();

        handle_button_input(GRID_W, UI_PANEL_W);

        //if (IsWindowResized()) {
        //    initialize_cell_layout(cell_x_positions, cell_y_positions, rows, cols);
        //}
    }

    free(grid);
    free(new_grid);
    free(cell_x_positions);
    free(cell_y_positions);
    CloseWindow();
    return 0;
}


