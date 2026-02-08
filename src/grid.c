#include "grid.h"
#include <stdlib.h>
#include <math.h>

Grid* createGrid(float cellSize) {
    return createGridCustom(cellSize, (Vector2){0, 0}, GRAY, DARKGRAY, 5);
}

Grid* createGridCustom(float cellSize, Vector2 offset, Color lineColor, Color majorLineColor, int majorLineInterval) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (grid != NULL) {
        grid->cellSize = cellSize;
        grid->offset = offset;
        grid->lineColor = lineColor;
        grid->majorLineColor = majorLineColor;
        grid->majorLineInterval = majorLineInterval;
        grid->visible = true;
        grid->lineThickness = 1.0f;
    }
    return grid;
}

void destroyGrid(Grid* grid) {
    if (grid != NULL) {
        free(grid);
    }
}

void renderGrid(const Grid* grid, Rectangle viewBounds) {
    if (!grid || !grid->visible || grid->cellSize <= 0) return;
    
    float cellSize = grid->cellSize;
    Vector2 offset = grid->offset;
    
    // Calculate grid bounds based on view
    float startX = floorf((viewBounds.x - offset.x) / cellSize) * cellSize + offset.x;
    float startY = floorf((viewBounds.y - offset.y) / cellSize) * cellSize + offset.y;
    float endX = viewBounds.x + viewBounds.width + cellSize;
    float endY = viewBounds.y + viewBounds.height + cellSize;
    
    // Draw vertical lines
    for (float x = startX; x <= endX; x += cellSize) {
        int gridIndex = (int)roundf((x - offset.x) / cellSize);
        Color color = (grid->majorLineInterval > 0 && gridIndex % grid->majorLineInterval == 0) 
                     ? grid->majorLineColor : grid->lineColor;
        
        DrawLineEx((Vector2){x, viewBounds.y}, 
                  (Vector2){x, viewBounds.y + viewBounds.height}, 
                  grid->lineThickness, color);
    }
    
    // Draw horizontal lines
    for (float y = startY; y <= endY; y += cellSize) {
        int gridIndex = (int)roundf((y - offset.y) / cellSize);
        Color color = (grid->majorLineInterval > 0 && gridIndex % grid->majorLineInterval == 0) 
                     ? grid->majorLineColor : grid->lineColor;
        
        DrawLineEx((Vector2){viewBounds.x, y}, 
                  (Vector2){viewBounds.x + viewBounds.width, y}, 
                  grid->lineThickness, color);
    }
}

Vector2 snapToGrid(const Grid* grid, Vector2 worldPos) {
    if (!grid || grid->cellSize <= 0) return worldPos;
    
    float cellSize = grid->cellSize;
    Vector2 offset = grid->offset;
    
    float snappedX = roundf((worldPos.x - offset.x) / cellSize) * cellSize + offset.x;
    float snappedY = roundf((worldPos.y - offset.y) / cellSize) * cellSize + offset.y;
    
    return (Vector2){snappedX, snappedY};
}

Rectangle snapRectangleToGrid(const Grid* grid, Rectangle rect) {
    if (!grid || grid->cellSize <= 0) return rect;
    
    Vector2 snappedTopLeft = snapToGrid(grid, (Vector2){rect.x, rect.y});
    
    return (Rectangle){
        snappedTopLeft.x,
        snappedTopLeft.y,
        rect.width,
        rect.height
    };
}

Vector2 worldToGridCoords(const Grid* grid, Vector2 worldPos) {
    if (!grid || grid->cellSize <= 0) return (Vector2){0, 0};
    
    float cellSize = grid->cellSize;
    Vector2 offset = grid->offset;
    
    return (Vector2){
        (worldPos.x - offset.x) / cellSize,
        (worldPos.y - offset.y) / cellSize
    };
}

Vector2 gridToWorldCoords(const Grid* grid, Vector2 gridCoords) {
    if (!grid || grid->cellSize <= 0) return gridCoords;
    
    float cellSize = grid->cellSize;
    Vector2 offset = grid->offset;
    
    return (Vector2){
        gridCoords.x * cellSize + offset.x,
        gridCoords.y * cellSize + offset.y
    };
}

bool isOnGridIntersection(const Grid* grid, Vector2 worldPos, float tolerance) {
    if (!grid || grid->cellSize <= 0) return false;
    
    Vector2 snapped = snapToGrid(grid, worldPos);
    float dx = fabsf(worldPos.x - snapped.x);
    float dy = fabsf(worldPos.y - snapped.y);
    
    return (dx <= tolerance) && (dy <= tolerance);
}

void setGridVisible(Grid* grid, bool visible) {
    if (grid != NULL) {
        grid->visible = visible;
    }
}

void setGridCellSize(Grid* grid, float cellSize) {
    if (grid != NULL && cellSize > 0) {
        grid->cellSize = cellSize;
    }
}