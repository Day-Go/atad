#include "raylib.h"

int main(void) {
    InitWindow(800, 450, "Raylib Test");
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(100, 100, 200, 200, RED);
        DrawText("If you see this, Raylib works!", 190, 200, 20, BLACK);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
