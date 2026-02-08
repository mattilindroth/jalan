#include "light.h"
#include <math.h>

Light* createLight(Vector2 position, float radius, float lightingRadius, Color color, bool flicker, bool breakable, bool isOn) {
    Light* light = (Light*)malloc(sizeof(Light));
    if (light != NULL) {
        light->position = position;
        light->radius = radius;
        light->lightingRadius = lightingRadius;
        light->color = color;
        light->flicker = flicker;
        light->breakable = breakable;
        light->isOn = isOn;
    }
    return light;
}

void renderLight(Light* light) {
    if (light != NULL) {
        // Placeholder rendering logic (e.g., draw a circle representing the light)
        DrawCircleV(light->position, light->radius, light->color);
    }
}

void updateLight(Light* light) {
    // Placeholder update logic (e.g., handle flickering or other dynamic behavior)
    if (light != NULL && light->flicker) {
        light->flickerTimer += GetFrameTime();
        
        // Flicker pattern: 1s on, 2s off, 2s on, 4s off (total 9s cycle)
        float cycleTime = fmod(light->flickerTimer, 9.0f);
        
        if (cycleTime < 1.0f) {
            light->isOn = true;  // 0-1s: on
        } else if (cycleTime < 3.0f) {
            light->isOn = false; // 1-3s: off
        } else if (cycleTime < 5.0f) {
            light->isOn = true;  // 3-5s: on
        } else {
            light->isOn = false; // 5-9s: off
        }
    }
}

void destroyLight(Light* light) {
    if (light != NULL) {
        free(light);
    }
}