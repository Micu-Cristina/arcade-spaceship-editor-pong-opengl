#include "object2Dproject.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D_project::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    // vertex list for the square defined in the XY plane
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),                               // bottom-left corner
        VertexFormat(corner + glm::vec3(length, 0, 0), color),     // bottom-right corner
        VertexFormat(corner + glm::vec3(length, length, 0), color),// top-right corner
        VertexFormat(corner + glm::vec3(0, length, 0), color)      // top-left corner
    };

    // resulting Mesh object
    Mesh* square = new Mesh(name);

    // indices for drawing the square
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        // if not filled, draw outline only
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // if filled, add indices for two triangles
        indices.push_back(0);
        indices.push_back(2);
    }

    // initialize mesh with vertices and indices
    square->InitFromData(vertices, indices);
    return square;
}



Mesh* object2D_project::CreateArrow(const std::string& name, glm::vec3 leftCorner, float width, float height, glm::vec3 color, bool fill)
{
    // store the bottom-left corner of the shape
    glm::vec3 corner = leftCorner;

    // build the vertex list for the desired shape
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(0, 0, 0), color),                        // bottom-left corner
        VertexFormat(corner + glm::vec3(width, 0, 0), color),                    // bottom-right corner
        VertexFormat(corner + glm::vec3(width * 0.75f, height / 2, 0), color),   // notch point
        VertexFormat(corner + glm::vec3(width, height, 0), color),               // top-right corner
        VertexFormat(corner + glm::vec3(0, height, 0), color)                    // top-left corner
    };

    // create a new Mesh object representing this shape
    Mesh* arrow = new Mesh(name);

    // index vector defining the triangles inside the shape
    std::vector<unsigned int> indices;

    // check if we want to draw filled shape or outline
    if (fill)
    {
        indices = { 0, 1, 2, 0, 2, 4, 2, 3, 4 };
    }
    else
    {
        // set draw mode to connect points with a continuous line
        arrow->SetDrawMode(GL_LINE_LOOP);

        // point connection order for the complete shape outline
        indices = { 0, 1, 2, 3, 4 };
    }

    // initialize mesh with the previously defined vertices and indices
    arrow->InitFromData(vertices, indices);

    // return the built mesh
    return arrow;
}

Mesh* object2D_project::CreateRectangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    // define the four corners of the rectangle
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),                                 // bottom-left
        VertexFormat(corner + glm::vec3(width, 0, 0), color),        // bottom-right
        VertexFormat(corner + glm::vec3(width, height, 0), color),   // top-right
        VertexFormat(corner + glm::vec3(0, height, 0), color)        // top-left
    };

    // indices for drawing the rectangle
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    // create mesh object
    Mesh* rectangle = new Mesh(name);

    if (fill) {
        // if rectangle is filled, add indices for two triangles
        indices.push_back(0);
        indices.push_back(2);
    }
    else {
        // if not filled, draw outline only
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }

    // initialize mesh
    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* object2D_project::CreateSemicircle(
    const std::string& name,
    float radius,
    glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // add the center of the semicircle
    vertices.emplace_back(glm::vec3(0, 0, 0), color);

    // number of segments for the curve
    int num_segments = 30;
    for (int i = 0; i <= num_segments; i++) {
        // calculate each point on the arc
        float angle = glm::pi<float>() * i / num_segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, y, 0), color);
    }

    // connect triangles for each segment
    for (int i = 1; i <= num_segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // initialize semicircle mesh
    Mesh* semicircle = new Mesh(name);
    semicircle->InitFromData(vertices, indices);
    return semicircle;
}


// ===========================================
Mesh* object2D_project::CreateFlame(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float width,
    float height,
    glm::vec3 color)
{
    glm::vec3 corner = leftBottomCorner;

    // define flame vertices as a zigzag line
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner + glm::vec3(0, 0, 0), color),                        // bottom-left
        VertexFormat(corner + glm::vec3(width * 0.15f, -height * 0.9f, 0), color), // zigzag 1
        VertexFormat(corner + glm::vec3(width * 0.30f, -height * 0.5f, 0), color), // zigzag 2
        VertexFormat(corner + glm::vec3(width * 0.45f, -height * 1.1f, 0), color), // zigzag 3
        VertexFormat(corner + glm::vec3(width * 0.60f, -height * 0.6f, 0), color), // zigzag 4
        VertexFormat(corner + glm::vec3(width * 0.75f, -height * 0.95f, 0), color), // zigzag 5
        VertexFormat(corner + glm::vec3(width, 0, 0), color)                      // bottom-right
    };

    // indices for triangles forming the flame surface
    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6
    };

    // initialize flame mesh
    Mesh* flame = new Mesh(name);
    flame->InitFromData(vertices, indices);
    return flame;
}

Mesh* object2D_project::CreateCircle(const std::string& name, float radius, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    // add the center of the circle
    vertices.emplace_back(glm::vec3(0, 0, 0), color);

    int num_segments = 40; // more segments = smoother circle
    for (int i = 0; i <= num_segments; i++) {
        float angle = 2 * glm::pi<float>() * i / num_segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.emplace_back(glm::vec3(x, y, 0), color);
    }

    for (int i = 1; i <= num_segments; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* circle = new Mesh(name);
    circle->InitFromData(vertices, indices);
    return circle;
}
