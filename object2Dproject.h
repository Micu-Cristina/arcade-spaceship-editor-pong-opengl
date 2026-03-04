#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D_project
{
    Mesh* CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateArrow(const std::string& name, glm::vec3 leftCorner, float width, float height, glm::vec3 color, bool fill = true);
    Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = true);
    Mesh* CreateSemicircle(const std::string& name, float radius, glm::vec3 color);
    Mesh* CreateFlame(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color);
    Mesh* CreateCircle(const std::string& name, float radius, glm::vec3 color);
}
