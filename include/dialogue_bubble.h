#include <raylib.h>

#ifndef DIALOGUE_BUBBLE_H
#define DIALOGUE_BUBBLE_H

typedef struct DialogueBubble {
    Rectangle rect;
    char* text;
    bool isVisible;
} DialogueBubble;

DialogueBubble* createDialogueBubble(Rectangle rect, const char* text);

void renderDialogueBubble(DialogueBubble* bubble);

DialogueBubble* destroyDialogueBubble(DialogueBubble* bubble);

#endif // DIALOGUE_BUBBLE_H