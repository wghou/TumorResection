#ifndef BRAIN_TUMOR_H
#define BRAIN_TUMOR_H
#include <filament/Color.h>
#include <filament/Engine.h>
#include <filament/LightManager.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>

#include <utils/Entity.h>
#include <utils/EntityManager.h>

#include <math/vec3.h>

#include "resources/resources.h"

constexpr uint8_t MATERIAL_MODEL_UNLIT = 0;
constexpr uint8_t MATERIAL_MODEL_LIT = 1;
constexpr uint8_t MATERIAL_MODEL_SUBSURFACE = 2;
constexpr uint8_t MATERIAL_MODEL_CLOTH = 3;

constexpr uint8_t MATERIAL_UNLIT = 0;
constexpr uint8_t MATERIAL_LIT = 1;
constexpr uint8_t MATERIAL_SUBSURFACE = 2;
constexpr uint8_t MATERIAL_CLOTH = 3;
constexpr uint8_t MATERIAL_TRANSPARENT = 4;
constexpr uint8_t MATERIAL_FADE = 5;
constexpr uint8_t MATERIAL_COUNT = 6;
#endif // !BRAIN_TUMOR_H

