#ifndef GRID_H
#define GRID_H

#include <raylib.h>
#include <stdbool.h>

typedef struct Grid {
    float cellSize;         // Size of each grid cell (e.g., 32 pixels)
    Vector2 offset;         // Grid offset from world origin
    Color lineColor;        // Color of grid lines
    Color majorLineColor;   // Color of major grid lines (every 5th line, etc.)
    int majorLineInterval;  // Draw major line every N cells
    bool visible;           // Whether to render the grid
    float lineThickness;    // Thickness of grid lines
} Grid;

// Create a new grid with default settings
Grid* createGrid(float cellSize);

// Create a grid with custom settings
Grid* createGridCustom(float cellSize, Vector2 offset, Color lineColor, Color majorLineColor, int majorLineInterval);

// Destroy the grid and free memory
void destroyGrid(Grid* grid);

// Render the grid within the given bounds (usually camera view)
void renderGrid(const Grid* grid, Rectangle viewBounds);

// Snap a world position to the nearest grid point
Vector2 snapToGrid(const Grid* grid, Vector2 worldPos);

// Snap a rectangle to grid (top-left corner snapped)
Rectangle snapRectangleToGrid(const Grid* grid, Rectangle rect);

// Get the grid cell coordinates for a world position
Vector2 worldToGridCoords(const Grid* grid, Vector2 worldPos);

// Convert grid coordinates to world position
Vector2 gridToWorldCoords(const Grid* grid, Vector2 gridCoords);

// Check if a world position is on a grid intersection
bool isOnGridIntersection(const Grid* grid, Vector2 worldPos, float tolerance);

// Set grid visibility
void setGridVisible(Grid* grid, bool visible);

// Set grid cell size (useful for zooming)
void setGridCellSize(Grid* grid, float cellSize);

#endif // GRID_H