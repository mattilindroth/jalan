#ifndef LIGHT_HH
#define LIGHT_HH

#include <raylib.h>
#include <stdlib.h>

typedef struct Light {
    Vector2 position;
    float radius;
    float lightingRadius;
    Color color;
    bool isOn;
    bool flicker;
    float flickerTimer;
    bool breakable;
    unsigned char dimness;  // 0 = bright, 255 = dark as fog
} Light;

Light* createLight(Vector2 position, float radius, float lightingRadius, Color color, bool flicker, bool breakable, bool isOn, unsigned char dimness);

void renderLight(Light* light);

void updateLight(Light* light);

void destroyLight(Light* light);

#endif // LIGHT_HH