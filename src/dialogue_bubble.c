#include "dialogue_bubble.h"

DialogueBubble* createDialogueBubble(Rectangle rect, const char* text) {
    DialogueBubble* bubble = (DialogueBubble*)malloc(sizeof(DialogueBubble));
    if (bubble != NULL) {
        bubble->rect = rect;
        bubble->text = (char*)malloc((strlen(text) + 1) * sizeof(char));
        if (bubble->text != NULL) {
            strcpy(bubble->text, text);
        }
        bubble->isVisible = true;
    }
    return bubble;
}

void renderDialogueBubble(DialogueBubble* bubble) {
    
    //Todo: could use MeasureText to center the text and adjust text lines accordingly.
    
    if (bubble != NULL && bubble->isVisible) {
        DrawRectangleRec(bubble->rect, LIGHTGRAY);
        DrawRectangleLinesEx(bubble->rect, 2, DARKGRAY);
        // Draw speech bubble pointer triangle
        Vector2 triangle[3];
        triangle[0] = (Vector2){bubble->rect.x + 20, bubble->rect.y + bubble->rect.height};
        triangle[1] = (Vector2){bubble->rect.x + 35, bubble->rect.y + bubble->rect.height};
        triangle[2] = (Vector2){bubble->rect.x + 27.5f, bubble->rect.y + bubble->rect.height + 15};
        DrawTriangle(triangle[0], triangle[1], triangle[2], LIGHTGRAY);
        DrawTriangleLines(triangle[0], triangle[1], triangle[2], DARKGRAY);
        int padding = 10;
        DrawText(bubble->text, bubble->rect.x + padding, bubble->rect.y + padding, 20, BLACK);
    }
}

DialogueBubble* destroyDialogueBubble(DialogueBubble* bubble) {
    if (bubble != NULL) {
        if (bubble->text != NULL) {
            free(bubble->text);
        }
        free(bubble);
    }
    return NULL;
}