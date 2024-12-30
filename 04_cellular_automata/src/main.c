#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

const int UI_PANEL_W = 400;
const int WND_H = 900;
const int WND_W = 1600;
const int GRID_H = 880;
const int GRID_W = WND_W - UI_PANEL_W;
const int GRID_PADDING = 10;  
const int CELL_SIZE = 8;

bool is_running = true;

#define MAX_TEMPERATURE 1000
#define MIN_TEMPERATURE 0

typedef enum {
    NONE,
    SAND,
    WATER,
    ROCK,
    FIRE
} Element; 

typedef struct {
    Element type;
    int temperature;
    float velocity_x;
    float velocity_y;
    bool updated_this_frame;
} Cell;

Element selected_element = SAND;
float brush_radius = 20.0f;

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

int draw_grid(Cell *grid, int *cell_x_positions, int *cell_y_positions, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            Element current_cell = grid[idx].type;

            Color cell_color = PURPLE;
            switch (current_cell) {
                case NONE:
                    cell_color = WHITE;
                    break;
                case SAND:
                    cell_color = BEIGE;
                    // Darken color based on temperature
                    if (grid[idx].temperature > 400) {
                        cell_color.r = min(255, cell_color.r + (grid[idx].temperature - 400) / 2);
                    }
                    break;
                case WATER:
                    cell_color = BLUE;
                    break;
                case ROCK:
                    cell_color = GRAY;
                    // Redden color based on temperature
                    if (grid[idx].temperature > 600) {
                        cell_color.r = min(255, cell_color.r + (grid[idx].temperature - 600) / 2);
                    }
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

void swap_cells(Cell *a, Cell *b) {
    Cell temp = *a;
    *a = *b;
    *b = temp;
}

float calculate_water_pressure(Cell *grid, int idx, int rows, int cols, int *neighbor_array) {
    int water_column = 0;
    int current = idx;
    
    while (current >= cols) {
        int above = neighbor_array[current * 8 + 1];
        if (above == -1 || grid[above].type != WATER) break;
        water_column++;
        current = above;
    }
    
    return 1.0f + (water_column * 0.2f);
}

void update_sand(Cell *grid, Cell *new_grid, int idx, int rows, int cols, int *neighbor_array) {
    int below = neighbor_array[idx * 8 + 6];
    int below_left = neighbor_array[idx * 8 + 5];
    int below_right = neighbor_array[idx * 8 + 7];

    if (below != -1) {
        if (grid[below].type == NONE) {
            swap_cells(&new_grid[idx], &new_grid[below]);
            new_grid[below].updated_this_frame = true;
            new_grid[below].velocity_y = 1.0f;
            return;
        }

        // interaction with water (sand sinks)
        if (grid[below].type == WATER) {
            swap_cells(&new_grid[idx], &new_grid[below]);
            new_grid[below].updated_this_frame = true;
            new_grid[below].velocity_y = 0.5f;
            return;
        }

        if (grid[below].type == SAND && grid[idx].velocity_y > 0.0f) {
            swap_cells(&new_grid[idx], &new_grid[below]);
            return;
        }
        // check if stationary
        bool can_fall_left = (below_left != -1 && grid[below_left].type == NONE);
        bool can_fall_right = (below_right != -1 && grid[below_right].type == NONE);

        if (can_fall_left && can_fall_right) {
            int target = (rand() % 2 == 0) ? below_left : below_right;
            swap_cells(&new_grid[idx], &new_grid[target]);
            new_grid[target].updated_this_frame = true;
        } else if (can_fall_left) {
            swap_cells(&new_grid[idx], &new_grid[below_left]);
            new_grid[below_left].updated_this_frame = true;
        } else if (can_fall_right) {
            swap_cells(&new_grid[idx], &new_grid[below_right]);
            new_grid[below_right].updated_this_frame = true;
        }
    }
}

void update_water(Cell *grid, Cell *new_grid, int idx, int rows, int cols, int *neighbor_array) {
    int below = neighbor_array[idx * 8 + 6];
    int below_left = neighbor_array[idx * 8 + 5];
    int below_right = neighbor_array[idx * 8 + 7];
    int left = neighbor_array[idx * 8 + 3];
    int right = neighbor_array[idx * 8 + 4];

    // Calculate water pressure (affects spread rate)
    float pressure = calculate_water_pressure(grid, idx, rows, cols, neighbor_array);

    // Primary downward movement
    if (below != -1 && grid[below].type == NONE) {
        swap_cells(&new_grid[idx], &new_grid[below]);
        new_grid[below].updated_this_frame = true;
        return;
    }

    // Diagonal falling with randomization
    bool can_fall_left = (below_left != -1 && grid[below_left].type == NONE);
    bool can_fall_right = (below_right != -1 && grid[below_right].type == NONE);

    if (can_fall_left || can_fall_right) {
        int target;
        if (can_fall_left && can_fall_right) {
            target = (rand() % 2 == 0) ? below_left : below_right;
        } else {
            target = can_fall_left ? below_left : below_right;
        }
        swap_cells(&new_grid[idx], &new_grid[target]);
        new_grid[target].updated_this_frame = true;
        return;
    }

    // Horizontal spread with pressure
    if (below != -1 && grid[below].type != NONE) {
        bool can_move_left = (left != -1 && grid[left].type == NONE);
        bool can_move_right = (right != -1 && grid[right].type == NONE);

        // Apply pressure to spreading
        if (rand() % 100 < pressure * 80) {  // Higher pressure = more likely to spread
            if (can_move_left && can_move_right) {
                int target = (rand() % 2 == 0) ? left : right;
                swap_cells(&new_grid[idx], &new_grid[target]);
                new_grid[target].updated_this_frame = true;
            } else if (can_move_left) {
                swap_cells(&new_grid[idx], &new_grid[left]);
                new_grid[left].updated_this_frame = true;
            } else if (can_move_right) {
                swap_cells(&new_grid[idx], &new_grid[right]);
                new_grid[right].updated_this_frame = true;
            }
        }
    }

    // Temperature effects
    if (new_grid[idx].temperature >= 100) {
        new_grid[idx].type = NONE;  // Water evaporates
    }
}

void update_fire(Cell *grid, Cell *new_grid, int idx, int rows, int cols, int *neighbor_array) {
    int heat_radius = 2;
    
    // Natural fire decay
    if (rand() % 100 < 5) {
        new_grid[idx].type = NONE;
        return;
    }

    // Heat propagation within radius
    for (int n = 0; n < 8; n++) {
        int neighbor_idx = neighbor_array[idx * 8 + n];
        if (neighbor_idx == -1) continue;

        // Heat transfer to neighbors
        new_grid[neighbor_idx].temperature += (50 / (heat_radius * heat_radius));

        // Interaction with other elements
        switch (grid[neighbor_idx].type) {
            case WATER:
                new_grid[idx].type = NONE;
                return;
            case SAND:
                if (new_grid[neighbor_idx].temperature > 800 && rand() % 100 < 10) {
                    new_grid[neighbor_idx].type = NONE;
                }
                break;
            case ROCK:
                if (new_grid[neighbor_idx].temperature > 900) {
                    new_grid[neighbor_idx].type = FIRE;
                }
                break;
            case FIRE:
            case NONE:
                break;
        }
    }

    // Fire movement
    int above = neighbor_array[idx * 8 + 1];
    if (above != -1 && grid[above].type == NONE && rand() % 100 < 70) {
        swap_cells(&new_grid[idx], &new_grid[above]);
        new_grid[above].updated_this_frame = true;
    }
}

void update_rock(Cell *grid, Cell *new_grid, int idx, int rows, int cols, int *neighbor_array) {
    // Rocks only move if unsupported
    int below = neighbor_array[idx * 8 + 4];
    if (below == -1) return;

    bool is_supported = false;
    
    // Check for support (including diagonals)
    for (int n = 3; n <= 5; n++) {  // Check bottom-left, bottom, bottom-right
        int support_idx = neighbor_array[idx * 8 + n];
        if (support_idx != -1 && (grid[support_idx].type == ROCK || grid[support_idx].type == SAND)) {
            is_supported = true;
            break;
        }
    }

    if (!is_supported && grid[below].type == NONE) {
        swap_cells(&new_grid[idx], &new_grid[below]);
        new_grid[below].updated_this_frame = true;
    }

    // Temperature effects - rocks melt at very high temperatures
    if (new_grid[idx].temperature > 900) {
        new_grid[idx].type = FIRE;
    }
}

void update_grid(Cell *grid, Cell *new_grid, int rows, int cols, int *neighbor_array) {
    // Reset update flags
    for (int i = 0; i < rows * cols; i++) {
        grid[i].updated_this_frame = false;
        new_grid[i] = grid[i];
    }

    // Update from bottom to top for gravity-based elements
    for (int i = rows - 1; i >= 0; i--) {
        for (int j = 0; j < cols; j++) {
            int idx = i * cols + j;
            if (grid[idx].updated_this_frame) continue;

            switch (grid[idx].type) {
                case SAND: {
                    update_sand(grid, new_grid, idx, rows, cols, neighbor_array);
                    break;
                }
                case WATER: {
                    update_water(grid, new_grid, idx, rows, cols, neighbor_array);
                    break;
                }
                case FIRE: {
                    update_fire(grid, new_grid, idx, rows, cols, neighbor_array);
                    break;
                }
                case ROCK: {
                    update_rock(grid, new_grid, idx, rows, cols, neighbor_array);
                    break;
                }
                case NONE: {
                    break; // Empty cells don't need updating
                }
            }
        }
    }
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~    UI    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void handle_mouse_drag(Cell *grid, int *cell_x_positions, int *cell_y_positions, int rows, int cols) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse_pos = GetMousePosition();

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                int idx = i * cols + j;
                float cell_center_x = cell_x_positions[idx] + CELL_SIZE / 2.0f;
                float cell_center_y = cell_y_positions[idx] + CELL_SIZE / 2.0f;

                float distance = sqrtf((mouse_pos.x - cell_center_x) * (mouse_pos.x - cell_center_x) +
                                    (mouse_pos.y - cell_center_y) * (mouse_pos.y - cell_center_y));

                if (distance <= brush_radius) {
                    grid[idx].type = selected_element;
                    grid[idx].temperature = 20;
                    grid[idx].velocity_x = 0;
                    grid[idx].velocity_y = 0;
                    grid[idx].updated_this_frame = false;
                }
            }
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

void draw_brush_outline() {
    Vector2 mouse_pos = GetMousePosition();
    DrawCircleLines(mouse_pos.x, mouse_pos.y, brush_radius, RED);
}

void draw_brush_slider() {
    Rectangle slider = { WND_W - UI_PANEL_W + 50, WND_H - 50, UI_PANEL_W - 100, 20 };
    DrawRectangleRec(slider, LIGHTGRAY);
    float slider_value = (brush_radius - 4.0f) / 90.0f; // normalize radius
    DrawRectangle(slider.x + slider_value * (slider.width - 10), slider.y, 10, slider.height, DARKGRAY);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse_pos = GetMousePosition();
        if (CheckCollisionPointRec(mouse_pos, slider)) {
            float value = (mouse_pos.x - slider.x) / slider.width;
            brush_radius = 4.0f + value * 90.0f; // scale back to 10-100
        }
    }
}

int main(void) {
    InitWindow(WND_W, WND_H, "Falling Sand");

    int fps = 5;
    float update_interval = 1.0f / fps;
    float time_since_last_update = 0.0f; 

    // setup CA grid
    int cols = (int)(GRID_W / CELL_SIZE);
    int rows = (int)(GRID_H / CELL_SIZE);

    Cell *grid = (Cell *)malloc(rows * cols * sizeof(Cell));
    Cell *new_grid = (Cell *)malloc(rows * cols * sizeof(Cell));

    // Initialize all cells
    for (int i = 0; i < rows * cols; i++) {
        grid[i] = (Cell){
            .type = NONE,
            .temperature = 20, // room temperature
            .velocity_x = 0,
            .velocity_y = 0,
            .updated_this_frame = false
        };
        new_grid[i] = grid[i];
    }

    int *cell_x_positions = (int *)malloc(rows * cols * sizeof(int));
    int *cell_y_positions = (int *)malloc(rows * cols * sizeof(int));

    initialize_cell_layout(cell_x_positions, cell_y_positions, rows, cols);

    int *neighbor_array = generate_neighbor_array(rows, cols);

    while (!WindowShouldClose()) {
        if (IsKeyReleased(KEY_SPACE)) {
            is_running = !is_running;
        }

        float delta_time = GetFrameTime();
        time_since_last_update += delta_time;
        if (time_since_last_update >= update_interval && is_running) {
            update_grid(grid, new_grid, rows, cols, neighbor_array);

            Cell *temp = grid;
            grid = new_grid;
            new_grid = temp;

            time_since_last_update = 0.0f;
            handle_mouse_drag(grid, cell_x_positions, cell_y_positions, rows, cols);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_grid(grid, cell_x_positions, cell_y_positions, rows, cols);

        draw_buttons(GRID_W, UI_PANEL_W);
        draw_brush_outline();
        draw_brush_slider();
        EndDrawing();

        handle_button_input(GRID_W, UI_PANEL_W);
    }

    free(grid);
    free(new_grid);
    free(cell_x_positions);
    free(cell_y_positions);
    CloseWindow();
    return 0;
}

