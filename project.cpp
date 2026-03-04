#include "project.h"

#include <vector>
#include <iostream>

#include "transform2Dproject.h"
#include "object2Dproject.h"

using namespace std;
using namespace m1;

Project::Project()
{
}

Project::~Project()
{
}

void Project::Init()
{

    // initialize the logical width of the scene to 1280 pixels
    logicalSceneWidth = 1280;
    // initialize the logical height of the scene to 720 pixels
    logicalSceneHeight = 720;

    leftPaddleAnimTime = 0.0f;
    rightPaddleAnimTime = 0.0f;

    // build the logical space of the scene using the initialized dimensions
    logicSpace = {0, 0, logicalSceneWidth, logicalSceneHeight};

    // get the current window resolution
    glm::ivec2 res = window->GetResolution();
    // initialize the viewport space with the screen resolution values
    viewSpace = {0, 0, res.x, res.y};

    // get the scene's default camera
    auto camera = GetSceneCamera();
    // set the camera projection to orthographic mode
    camera->SetOrthographic(0.0f, (float)res.x, 0.0f, (float)res.y, 0.01f, 400.0f);
    // position the camera in space at coords (0,0,50)
    camera->SetPosition(glm::vec3(0, 0, 50));
    // reset the camera rotation to zero on all axes
    camera->SetRotation(glm::vec3(0));
    // update the camera parameters after the modifications
    camera->Update();
    // disable camera input control to work in 2D
    GetCameraInput()->SetActive(false);

    // compute the uniform visualization matrix for scaling and centering the scene in the window
    visMatrix = VisualizationTransf2DUnif(logicSpace, viewSpace);

    // set the size of a grid cell
    cell_size_grid = 35.0f;

    // add base meshes for border and colored squares to the list
    AddMeshToList(object2D_project::CreateSquare("square_red", glm::vec3(0, 0, 0), 100, glm::vec3(1, 0, 0), false));
    AddMeshToList(object2D_project::CreateSquare("square_red2", glm::vec3(0, 0, 0), 100, glm::vec3(1, 0, 0), true));
    AddMeshToList(object2D_project::CreateSquare("square_grey", glm::vec3(0, 0, 0), 100, glm::vec3(0.6f, 0.6f, 0.6f), true));
    AddMeshToList(object2D_project::CreateSquare("square_grey2", glm::vec3(0, 0, 0), 100, glm::vec3(0.6f, 0.6f, 0.6f), false));
    AddMeshToList(object2D_project::CreateSquare("square_green", glm::vec3(0, 0, 0), 100, glm::vec3(0.1f, 0.9f, 0.1f), true));
    AddMeshToList(object2D_project::CreateSquare("square_blue_fill", glm::vec3(0, 0, 0), 100, glm::vec3(0.2f, 0.3f, 0.8f), true));
    AddMeshToList(object2D_project::CreateSquare("square_blue_border", glm::vec3(0, 0, 0), 100, glm::vec3(0.2f, 0.3f, 0.8f), false));
    AddMeshToList(object2D_project::CreateCircle("ball_orange", 50, glm::vec3(1.0f, 0.5f, 0.0f)));

    // add meshes that define the vehicle components
    AddMeshToList(object2D_project::CreateSquare("engine_base_square", glm::vec3(0, 0, 0), 100, glm::vec3(0.95f, 0.7f, 0.25f), true));
    AddMeshToList(object2D_project::CreateFlame("engine_flame", glm::vec3(0, 0, 0), 100, 50, glm::vec3(0.95f, 0.45f, 0.15f)));
    AddMeshToList(object2D_project::CreateSquare("gun_base_square", glm::vec3(0, 0, 0), 100, glm::vec3(0.4f, 0.4f, 0.4f), true));
    AddMeshToList(object2D_project::CreateSemicircle("gun_base_semi", 50, glm::vec3(0.85f, 0.85f, 0.85f)));
    AddMeshToList(object2D_project::CreateRectangle("gun_pipe", glm::vec3(0, 0, 0), 100, 200, glm::vec3(0.1f, 0.1f, 0.1f), true));
    AddMeshToList(object2D_project::CreateSquare("bumper_base_square", glm::vec3(0, 0, 0), 100, glm::vec3(0.8f, 0.8f, 0.8f), true));
    AddMeshToList(object2D_project::CreateSemicircle("bumper_top_semi", 50, glm::vec3(0.95f, 0.93f, 0.75f)));

    // add green and red arrows for the ship validation button
    AddMeshToList(object2D_project::CreateArrow("arrow_green", glm::vec3(0, 0, 0), 100, 100, glm::vec3(0.1f, 0.9f, 0.1f), true));
    AddMeshToList(object2D_project::CreateArrow("arrow_red", glm::vec3(0, 0, 0), 100, 100, glm::vec3(0.9f, 0.1f, 0.1f), true));
}

glm::mat3 Project::VisualizationTransf2D(const LogicSpace &logicSpace, const ViewportSpace &viewSpace)
{
    // declare variables for scaling and translation
    float sx, sy, tx, ty;

    // compute the scaling factor on the X axis by relating viewport width to space width
    sx = (float)viewSpace.width / logicSpace.width;
    // compute the scaling factor on the Y axis by relating viewport height to space height
    sy = (float)viewSpace.height / logicSpace.height;

    // compute translation on X to move the logicSpace origin to viewport coordinates
    tx = viewSpace.x - sx * logicSpace.x;
    // compute translation on Y for vertical position
    ty = viewSpace.y - sy * logicSpace.y;

    // return the 2D transformation matrix
    return glm::transpose(glm::mat3(
        sx, 0, tx,
        0, sy, ty,
        0, 0, 1));
}

glm::mat3 Project::VisualizationTransf2DUnif(const LogicSpace &logicSpace, const ViewportSpace &viewSpace)
{
    // declare variables for scaling, translation and minimum
    float sx, sy, tx, ty, smin;
    // compute scaling factors on axes
    sx = (float)viewSpace.width / logicSpace.width;
    sy = (float)viewSpace.height / logicSpace.height;
    // choose the smallest factor to preserve aspect ratio
    smin = std::min(sx, sy);

    // compute translation on X to center the scene in the viewport
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2.0f;
    // compute translation on Y for vertical centering
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2.0f;

    // return the uniform 2D transformation matrix
    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

glm::vec2 Project::ScreenToWorld(int mouseX, int mouseY)
{
    // get the window resolution
    glm::ivec2 res = window->GetResolution();
    // build a vector with mouse coordinates in screen space
    glm::vec3 screenCoords((float)mouseX, (float)(res.y - mouseY), 1.0f);

    // compute the inverse of the visualization matrix to convert from screen to world coordinates
    glm::mat3 invVis = glm::inverse(visMatrix);
    // multiply screen coordinates by the inverse to get coordinates in logical space
    glm::vec3 worldCoords = invVis * screenCoords;

    // return only the X and Y components as a 2D vector
    return glm::vec2(worldCoords.x, worldCoords.y);
}

void Project::SetViewportArea(const ViewportSpace &viewSpace, glm::vec3 color, bool clear)
{
    // set the active drawing area
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // enable scissor test
    glEnable(GL_SCISSOR_TEST);
    // define the scissor region
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    // clear the color and depth buffers
    if (clear)
    {
        glClearColor(color.r, color.g, color.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glDisable(GL_SCISSOR_TEST);

    // get the scene camera to update orthographic projection
    auto camera = GetSceneCamera();
    // set the new orthographic limits based on the current viewport
    camera->SetOrthographic(
        (float)viewSpace.x,
        (float)(viewSpace.x + viewSpace.width),
        (float)viewSpace.y,
        (float)(viewSpace.y + viewSpace.height),
        0.01f, 400);
    // update the camera after parameter changes
    camera->Update();
}

void Project::FrameStart()
{
    // clear the screen each frame
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // get the current window resolution
    glm::ivec2 resolution = window->GetResolution();

    // update the viewport area to cover the entire window
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);

    // apply the uniform scaling transformation and center the logical space in the viewport
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    // update OpenGL settings for the new viewport and position the camera
    SetViewportArea(viewSpace, glm::vec3(0), false);
}

void Project::Update(float deltaTimeSeconds)
{

    // set the background color to black
    glClearColor(0, 0, 0, 1);

    // clear color and depth buffers before drawing the scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // disable depth test to ensure all 2D objects are visible
    glDisable(GL_DEPTH_TEST);

    if (currentState == STATE_EDITOR)
    {
        // draw the main red frame
        DrawMainFrame();
        // draw the left panel containing available block types
        DrawLeftPanelOfGrid();
        // draw the main grid where objects will be placed
        DrawGrid();

        // define the number of rows and columns of the grid
        const int rows_grid = 7;
        const int cols_grid = 15;
        // define the spacing between cells
        const float padding = 6.0f;
        // define the size of each cell
        const float cell_size = 55.0f;

        // calculate the total width of the grid
        const float total_w = cols_grid * (cell_size + padding) - padding;
        // define the total height of the grid
        const float total_h = rows_grid * (cell_size + padding) - padding;

        // calculate the starting X coordinate of the grid in the scene
        const float gridX = logicSpace.width * 0.22f;
        // calculate the starting Y coordinate of the grid in the scene
        const float gridY = logicSpace.height * 0.12f;

        // iterate through all objects placed in the grid
        for (const auto &obj : placedObjects)
        {
            // calculate the X position of the current cell
            const float x = gridX + obj.col * (cell_size + padding);
            // calculate the Y position of the current cell
            const float y = gridY + obj.row * (cell_size + padding);

            // build the transformation matrix for an object
            glm::mat3 M = glm::mat3(1);
            M *= transform2Dproject::Translate(x, y);
            M *= transform2Dproject::Scale(cell_size / 100.0f, cell_size / 100.0f);

            // check if the object is a solid block
            if (obj.type == "solid")
            {
                // draw the grey square corresponding to a solid block
                RenderMesh2D(meshes["square_grey"], shaders["VertexColor"], visMatrix * M);
            }

            // check if the object is an engine
            if (obj.type == "engine")
            {
                // draw the engine base
                RenderMesh2D(meshes["engine_base_square"], shaders["VertexColor"], visMatrix * M);
                // draw the engine flame
                glm::mat3 F = glm::mat3(1);
                F *= transform2Dproject::Translate(x, y);
                F *= transform2Dproject::Scale(cell_size / 100.0f, cell_size / 100.0f);
                RenderMesh2D(meshes["engine_flame"], shaders["VertexColor"], visMatrix * F);
            }

            // check if the object is a cannon
            if (obj.type == "gun")
            {
                // define the width and height of a cell
                float w = cell_size;
                float h = cell_size;
                // store the base position of the cannon
                float cx = x;
                float cy = y;

                // build the matrix for the cannon pipe
                glm::mat3 pipe = glm::mat3(1);
                pipe *= transform2Dproject::Translate(cx, cy + h);
                pipe *= transform2Dproject::Scale(w / 100.0f, (1.15 * h) / 100.0f);
                RenderMesh2D(meshes["gun_pipe"], shaders["VertexColor"], visMatrix * pipe);

                // draw the grey base of the cannon
                RenderMesh2D(meshes["gun_base_square"], shaders["VertexColor"], visMatrix * M);

                // draw the semicircle above the base
                {
                    glm::mat3 S = glm::mat3(1);
                    S *= transform2Dproject::Translate(x + 0.5f * cell_size, y + cell_size);
                    S *= transform2Dproject::Scale(cell_size / 100.0f, cell_size / 100.0f);
                    RenderMesh2D(meshes["gun_base_semi"], shaders["VertexColor"], visMatrix * S);
                }
            }

            // check if the object is a bumper
            if (obj.type == "bumper")
            {
                // draw the square base of the bumper
                RenderMesh2D(meshes["bumper_base_square"], shaders["VertexColor"], visMatrix * M);

                // build the large semicircle above
                glm::mat3 B = glm::mat3(1);
                B *= transform2Dproject::Translate(x + 0.5f * cell_size, y + cell_size);
                B *= transform2Dproject::Scale((3.15f * cell_size) / 100.0f, (1.15f * cell_size) / 100.0f);
                RenderMesh2D(meshes["bumper_top_semi"], shaders["VertexColor"], visMatrix * B);
            }
        }

        // draw the top bar over all elements
        DrawTopBar();

        // check if left click is held and an object is being dragged
        if (isDragging && draggedType != "" && leftMouseHeld)
        {
            // initialize a matrix for the dragged object
            glm::mat3 dragMatrix = glm::mat3(1);
            float halfCell = cell_size / 2.0f;

            // apply translation to position the object under the cursor
            dragMatrix *= transform2Dproject::Translate(mouseX_world - halfCell, mouseY_world - halfCell);
            // apply scaling to bring the object to the correct size
            dragMatrix *= transform2Dproject::Scale(cell_size / 100.0f, cell_size / 100.0f);

            // if dragging a solid block, draw it
            if (draggedType == "solid")
                RenderMesh2D(meshes["square_grey"], shaders["VertexColor"], visMatrix * dragMatrix);

            // if dragging an engine, draw its base and flame
            if (draggedType == "engine")
            {
                RenderMesh2D(meshes["engine_base_square"], shaders["VertexColor"], visMatrix * dragMatrix);
                glm::mat3 flame = glm::mat3(1);
                flame *= transform2Dproject::Translate(mouseX_world - halfCell, mouseY_world - halfCell);
                flame *= transform2Dproject::Scale(cell_size / 100.0f, cell_size / 100.0f);
                RenderMesh2D(meshes["engine_flame"], shaders["VertexColor"], visMatrix * flame);
            }

            // if dragging a cannon
            if (draggedType == "gun")
            {

                float w = cell_size;
                float h = cell_size;
                float cx = mouseX_world - halfCell;
                float cy = mouseY_world - halfCell;

                // draw the black pipe above the base
                glm::mat3 pipe = glm::mat3(1);
                pipe *= transform2Dproject::Translate(cx, cy + h);
                pipe *= transform2Dproject::Scale(w / 100.0f, (1.15f * h) / 100.0f);
                RenderMesh2D(meshes["gun_pipe"], shaders["VertexColor"], visMatrix * pipe);

                // draw the grey base
                glm::mat3 base = glm::mat3(1);
                base *= transform2Dproject::Translate(cx, cy);
                base *= transform2Dproject::Scale(w / 100.0f, h / 100.0f);
                RenderMesh2D(meshes["gun_base_square"], shaders["VertexColor"], visMatrix * base);

                // draw the semicircle above the base
                glm::mat3 semi = glm::mat3(1);
                semi *= transform2Dproject::Translate(cx + 0.5f * w, cy + h);
                semi *= transform2Dproject::Scale(w / 100.0f, h / 100.0f);
                RenderMesh2D(meshes["gun_base_semi"], shaders["VertexColor"], visMatrix * semi);
            }

            // if dragging a bumper, draw base and large semicircle
            if (draggedType == "bumper")
            {
                RenderMesh2D(meshes["bumper_base_square"], shaders["VertexColor"], visMatrix * dragMatrix);
                glm::mat3 semi = glm::mat3(1);
                semi *= transform2Dproject::Translate(
                    mouseX_world - halfCell + cell_size * 0.5f,
                    mouseY_world - halfCell + cell_size);
                semi *= transform2Dproject::Scale((cell_size * 3.25f) / 100.0f, (cell_size * 1.15f) / 100.0f);
                RenderMesh2D(meshes["bumper_top_semi"], shaders["VertexColor"], visMatrix * semi);
            }
        }
    }

    else if (currentState == STATE_GAME)
    {
        // draw the complete scene
        DrawGame();

        // if the game has ended, stop updating logic
        if (gameOver)
            return;

        // update the timers used for paddle animation
        if (leftPaddleAnimTime > 0.0f)
            leftPaddleAnimTime = std::max(0.0f, leftPaddleAnimTime - deltaTimeSeconds);
        if (rightPaddleAnimTime > 0.0f)
            rightPaddleAnimTime = std::max(0.0f, rightPaddleAnimTime - deltaTimeSeconds);

        // define horizontal and vertical margins of the frame
        float marginX = 200.0f;
        float marginY = 80.0f;

        // define the position of the bottom margin of the frame
        float frameBottom = marginY;

        // define the position of the top margin of the frame
        float frameTop = logicalSceneHeight - marginY;

        // calculate vertical limits for the ball
        float ballTopLimit = frameTop - ballRadius;
        float ballBottomLimit = frameBottom + ballRadius;

        // calculate the height of the left ship
        float leftShipHeight = ComputeShipHeight(playerShipLeft);

        // calculate the height of the right ship
        float rightShipHeight = ComputeShipHeight(playerShipRight);

        // offset needed because ships are rotated 90 degrees
        float rotationOffset = (cell_size_grid * 3.0f) * 0.7f;

        // calculate min and max limits for vertical paddle movement
        float paddleMinY = frameBottom;
        float paddleMaxY_Left = frameTop - (leftShipHeight + rotationOffset);
        float paddleMaxY_Right = frameTop - (rightShipHeight + rotationOffset);

        // force paddles to stay inside the frame
        paddleLeftY = std::max(paddleMinY, std::min(paddleLeftY, paddleMaxY_Left));
        paddleRightY = std::max(paddleMinY, std::min(paddleRightY, paddleMaxY_Right));

        // size of a cell in the ship
        float cell = cell_size_grid;

        // create an identity matrix
        glm::mat3 leftBase = glm::mat3(1);

        // translate the left ship to its position in the scene
        leftBase *= transform2Dproject::Translate(leftShipX, paddleLeftY);

        // move the ship origin to center for correct rotation
        leftBase *= transform2Dproject::Translate(cell * 1.5f, cell * 3.0f);

        // rotate the ship -90 degrees to orient it to the right
        leftBase *= transform2Dproject::Rotate(glm::radians(-90.0f));

        // scale the ship by 0.7 on OX and OY to shrink it and fit in the game
        leftBase *= transform2Dproject::Scale(0.7f, 0.7f);

        // move the ship origin back after rotation and scaling to maintain correct structure
        leftBase *= transform2Dproject::Translate(-cell * 1.5f, -cell * 3.0f);

        // create an identity matrix
        glm::mat3 rightBase = glm::mat3(1);

        // translate the right ship to its position in the scene
        rightBase *= transform2Dproject::Translate(rightShipX, paddleRightY);

        // move the ship origin to center for correct model rotation
        rightBase *= transform2Dproject::Translate(cell * 1.5f, cell * 3.0f);

        // rotate the ship +90 degrees to orient it to the left
        rightBase *= transform2Dproject::Rotate(glm::radians(90.0f));

        // scale the ship with factor -0.7 on OX
        rightBase *= transform2Dproject::Scale(-0.7f, 0.7f);

        // move the origin back after rotation to reposition the model correctly
        rightBase *= transform2Dproject::Translate(-cell * 1.5f, -cell * 3.0f);

        // variables for AABB limits of the left ship
        float minLX, maxLX, minLY, maxLY;

        // variables for AABB limits of the right ship
        float minRX, maxRX, minRY, maxRY;

        // calculate AABB for left ship based on ship structure and leftBase transform
        ComputeShipAABB(playerShipLeft, leftBase, minLX, maxLX, minLY, maxLY);

        // calculate AABB for right ship based on ship structure and rightBase transform
        ComputeShipAABB(playerShipRight, rightBase, minRX, maxRX, minRY, maxRY);

        // iterate through the ball vector
        for (auto &b : balls)
        {
            // ignore balls that are no longer active
            if (!b.alive)
                continue; // skip to the next ball

            // update ball position based on velocity
            b.x += b.vx * deltaTimeSeconds;
            b.y += b.vy * deltaTimeSeconds;

            // ball collision with top or bottom of the frame
            if (b.y >= ballTopLimit || b.y <= ballBottomLimit) // check if ball crosses vertical boundaries
                b.vy = -b.vy;                                  // invert vertical velocity

            // calculate total speed
            float speed = sqrt(b.vx * b.vx + b.vy * b.vy);

            if (b.vx < 0 &&                   // if the ball is moving to the left
                b.x - b.radius <= maxLX &&    // left edge of ball touches left ship AABB on OX
                b.y >= minLY && b.y <= maxLY) // ball is between the vertical limits of the ship
            {
                // calculate center and half height of AABB
                float centerY = (minLY + maxLY) * 0.5f;
                // half of the effective paddle height
                float halfH = (maxLY - minLY) * 0.5f;

                // relative position on paddle (-1 bottom, 0 middle, +1 top)
                float unghiNorm = (b.y - centerY) / halfH;

                // calculate the impact zone angle
                float theta = unghiNorm * glm::radians(75.0f);

                // set the new velocity components
                // horizontal component becomes positive
                b.vx = speed * cos(theta);
                // vertical component becomes a function of the impact point
                b.vy = speed * sin(theta);

                // move the ball slightly to avoid continuous collision
                b.x = maxLX + b.radius + 1.0f;

                // start animation for the left paddle
                leftPaddleAnimTime = paddleAnimDuration;
            }

            if (b.vx > 0 &&                   // if the ball is moving to the right
                b.x + b.radius >= minRX &&    // right edge of ball touches right paddle
                b.y >= minRY && b.y <= maxRY) // ball is in the vertical zone of the paddle
            {
                float centerY = (minRY + maxRY) * 0.5f;
                float halfH = (maxRY - minRY) * 0.5f;

                // normalized relative position
                float unghiNorm = (b.y - centerY) / halfH;
                float theta = unghiNorm * glm::radians(75.0f);

                // horizontal component becomes negative
                b.vx = -speed * cos(theta);
                b.vy = speed * sin(theta);

                // push the ball out of the collision
                b.x = minRX - b.radius - 1.0f;

                // trigger the paddle animation
                rightPaddleAnimTime = paddleAnimDuration;
            }

            // check collision with bumpers of left/right ship
            ApplyBumperSpeedBoost(playerShipLeft, leftBase, b);
            ApplyBumperSpeedBoost(playerShipRight, rightBase, b);

            // if the ball passes completely past the left edge
            if (b.x < marginX)
            {
                // increase the right player's score
                rightScore++;
                // ball disappears from the scene
                b.alive = false;
            }
            else if (b.x > logicalSceneWidth - marginX) // if the ball passes completely past the right edge
            {
                // increase the left player's score
                leftScore++;
                // ball disappears from the scene
                b.alive = false;
            }
        }

        // if W key is pressed
        if (window->KeyHold(GLFW_KEY_W))
            paddleLeftY += paddleSpeed * deltaTimeSeconds; // move up

        // if S key is pressed
        if (window->KeyHold(GLFW_KEY_S))
            paddleLeftY -= paddleSpeed * deltaTimeSeconds; // move down

        // if up arrow is pressed
        if (window->KeyHold(GLFW_KEY_UP))
            paddleRightY += paddleSpeed * deltaTimeSeconds; // move up

        // if down arrow is pressed
        if (window->KeyHold(GLFW_KEY_DOWN))
            paddleRightY -= paddleSpeed * deltaTimeSeconds; // move down

        // apply limits after input
        paddleLeftY = std::max(paddleMinY, std::min(paddleLeftY, paddleMaxY_Left));
        paddleRightY = std::max(paddleMinY, std::min(paddleRightY, paddleMaxY_Right));

        // check if someone reached the maximum score
        if (leftScore >= maxScore || rightScore >= maxScore)
            gameOver = true; // mark end of game

        // assume all balls are dead
        bool allDead = true;
        // iterate through the ball vector
        for (auto &b : balls)
            if (b.alive) // if we find at least one alive
            {
                // round has not ended
                allDead = false;
                break;
            }

        // if all balls have been marked as dead, the round restarts
        if (allDead && !gameOver)
            StartNewRound(); // launch new balls from ship cannons
    }
}

void Project::FrameEnd()
{
}

void Project::OnInputUpdate(float deltaTime, int mods)
{
}

void Project::OnKeyPress(int key, int mods)
{
}

void Project::OnKeyRelease(int key, int mods)
{
}

void Project::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // convert mouse coordinates from screen space to world coordinates
    glm::vec2 w = ScreenToWorld(mouseX, mouseY);
    // store mouse X position in scene coordinates
    mouseX_world = w.x;
    // store mouse Y position in scene coordinates
    mouseY_world = w.y;
}

void Project::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // convert mouse coordinates to logical scene coordinates
    glm::vec2 w = ScreenToWorld(mouseX, mouseY);
    // store X position
    mouseX_world = w.x;
    // store Y position
    mouseY_world = w.y;

    // check if left click is pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT || button == 1)
    {
        // mark that left mouse button is held
        leftMouseHeld = true;

        // define the number of rows in the left panel
        int rows_panel = 4;
        // define the spacing between panel elements
        float spacing = 6.0f;
        // calculate the X margin of the panel
        float marginX = logicSpace.width * 0.015f;
        // calculate the Y margin of the panel
        float marginY = logicSpace.height * 0.03f;
        // calculate total height available for panel
        float total_height = logicSpace.height - 2 * marginY;
        // calculate the size of a block in the panel
        float block_size = total_height / rows_panel;

        // calculate the starting X position of the panel
        float panelX = marginX;
        // calculate the starting Y position of the panel
        float panelY = marginY;

        // check if mouse is inside the left panel
        if (mouseX_world >= panelX && mouseX_world <= panelX + block_size)
        {
            // iterate through each row in the panel
            for (int i = 0; i < rows_panel; i++)
            {
                // calculate Y coordinate of current row
                float currentY = panelY + i * block_size;
                // check if mouse is on the current row
                if (mouseY_world >= currentY && mouseY_world <= currentY + block_size)
                {
                    // enable drag and drop mode
                    isDragging = true;
                    // set the type of the selected object
                    if (i == 0)
                        draggedType = "solid";
                    else if (i == 1)
                        draggedType = "engine";
                    else if (i == 2)
                        draggedType = "gun";
                    else if (i == 3)
                        draggedType = "bumper";
                }
            }
        }
    }

    // check if right click is pressed
    if (button == GLFW_MOUSE_BUTTON_RIGHT || button == 2)
    {
        // define the number of rows and columns of the grid
        int rows = 7, cols = 15;
        // define the spacing between cells and their size
        float padding = 6.0f, cell_size = 55.0f;
        // calculate the total width of the grid
        float totalW = cols * (cell_size + padding) - padding;
        // calculate total height of the grid
        float totalH = rows * (cell_size + padding) - padding;
        // calculate grid position in the scene
        float gridX = logicSpace.width * 0.22f;
        float gridY = logicSpace.height * 0.12f;

        // check if mouse is inside the grid
        if (mouseX_world >= gridX && mouseX_world <= gridX + totalW &&
            mouseY_world >= gridY && mouseY_world <= gridY + totalH)
        {

            // determine the column and row where the click occurred
            int col = (mouseX_world - gridX) / (cell_size + padding);
            int row = (mouseY_world - gridY) / (cell_size + padding);

            // iterate through the vector of placed objects
            for (int i = 0; i < placedObjects.size(); i++)
            {
                // check if current object is exactly on the selected cell
                if (placedObjects[i].row == row && placedObjects[i].col == col)
                {
                    cout << "Removed block " << placedObjects[i].type << " from [" << row << "," << col << "]\n";
                    // remove the object from the placed objects vector
                    placedObjects.erase(placedObjects.begin() + i);
                    // decrement the total placed blocks counter
                    blocksPlaced--;
                    // verify again that all constraints are met
                    constraintsValid = CheckConstraints();
                    return;
                }
            }
        }
    }

    // calculate the position of the Start button in the top bar
    float startX = logicSpace.width * 0.26f + 10 * (55.0f + 20.0f) + 25;
    float startY = logicSpace.height * 0.84f;
    float size = 55.0f;

    // check if mouse position is inside the button
    if (mouseX_world >= startX && mouseX_world <= startX + size &&
        mouseY_world >= startY && mouseY_world <= startY + size)
    {

        // check if the ship respects all rules
        if (constraintsValid)
        {
            // print a message to the console
            std::cout << ">>> START GAME <<<" << std::endl;

            // save the built ship as the left ship
            playerShipLeft = placedObjects;

            // copy the same ship for the right player
            playerShipRight = playerShipLeft;

            // set the cell size of the ship
            cell_size_grid = 35.0f;

            // set the approximate paddle width
            paddleWidth = 20.0f;

            // calculate estimated paddle height
            paddleHeight = 7 * (cell_size_grid + 6.0f);

            // set the paddle movement speed
            paddleSpeed = 400.0f;

            // position the left ship vertically at the center
            paddleLeftY = logicalSceneHeight / 2.0f - paddleHeight / 2.0f;

            // position the right ship vertically at the center
            paddleRightY = logicalSceneHeight / 2.0f - paddleHeight / 2.0f;

            // set the standard radius for balls
            ballRadius = 10.0f;

            // reset scores at the start of the game
            leftScore = rightScore = 0;

            // mark that the game is not in finished state
            gameOver = false;

            // remove all existing balls
            balls.clear();

            // start a new round and launch one ball from each cannon
            StartNewRound();

            // enter the actual game mode
            currentState = STATE_GAME;
        }

        else
        {
            std::cout << "Invalid vehicle!" << std::endl;
        }
    }
}

void Project::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // check if the released button is the left one
    if (button == GLFW_MOUSE_BUTTON_LEFT || button == 1)
    {
        // mark that the left button is no longer held
        leftMouseHeld = false;
        // if we had drag and drop
        if (isDragging)
        {
            // stop the dragging state
            isDragging = false;

            // define the number of rows and columns of the grid
            int rows = 7, cols = 15;
            // define the spacing between cells and their size
            float padding = 6.0f, cell_size = 55.0f;
            // calculate the total width of the grid
            float totalW = cols * (cell_size + padding) - padding;
            // calculate total height of the grid
            float totalH = rows * (cell_size + padding) - padding;
            // calculate starting X coordinate of the grid
            float gridX = logicSpace.width * 0.22f;
            // calculate starting Y coordinate of the grid
            float gridY = logicSpace.height * 0.12f;

            // convert mouse coordinates from screen space to logical scene coordinates
            glm::vec2 w = ScreenToWorld(mouseX, mouseY);
            mouseX_world = w.x;
            mouseY_world = w.y;

            // check if the final mouse position is inside the grid
            if (mouseX_world >= gridX && mouseX_world <= gridX + totalW &&
                mouseY_world >= gridY && mouseY_world <= gridY + totalH)
            {
                // calculate the column and row in the grid where the block was released
                int col = (mouseX_world - gridX) / (cell_size + padding);
                int row = (mouseY_world - gridY) / (cell_size + padding);

                // check if a block is already placed in this cell
                for (auto &o : placedObjects)
                    if (o.row == row && o.col == col)
                        return;

                // add the new object to the list of placed objects
                placedObjects.push_back({draggedType, row, col});
                // increment the number of placed objects
                blocksPlaced++;
                // check if all constraints are satisfied
                constraintsValid = CheckConstraints();
            }
            // reset the dragged object type to finish drag and drop
            draggedType = "";
        }
    }
}

void Project::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Project::OnWindowResize(int width, int height)
{
    // update the viewport space with the new window dimensions
    viewSpace = {0, 0, width, height};
    // recalculate the uniform visualization matrix to scale and center the scene in the new viewport
    visMatrix = VisualizationTransf2DUnif(logicSpace, viewSpace);

    // get the scene camera to update the projection
    auto camera = GetSceneCamera();
    // set the orthographic projection of the camera to the new viewport limits
    camera->SetOrthographic(0.0f, (float)width, 0.0f, (float)height, 0.01f, 400.0f);
    // update the camera state after parameter change
    camera->Update();

    std::cout << "Resize " << width << "x" << height << "\n";
}

void Project::DrawGrid()
{
    // set the number of grid rows to 7
    int rows_grid = 7;
    // set the number of grid columns to 15
    int cols_grid = 15;
    // define the spacing between cells
    float padding = 6.0f;

    // set the size of a cell in pixels
    float cell_size = 55.0f;

    // calculate total width of the grid including spacing
    float total_width = cols_grid * (cell_size + padding) - padding;
    // calculate total height of the grid including spacing
    float total_height = rows_grid * (cell_size + padding) - padding;

    // position the grid at a proportional distance from margins and top bar
    float marginX = logicSpace.width * 0.22f;
    float marginY = logicSpace.height * 0.12f;

    // store the starting coordinates of the grid
    float grid_coordX = marginX;
    float grid_coordY = marginY;

    // initialize the transformation matrix for the outer frame
    glm::mat3 borderMatrix = glm::mat3(1);
    // define offset to draw the frame with an outer margin
    float border_offset = 8.0f;
    // apply translation to position the frame around the grid
    borderMatrix *= transform2Dproject::Translate(grid_coordX - border_offset, grid_coordY - border_offset);
    // apply scaling to enlarge the frame to match total dimensions
    borderMatrix *= transform2Dproject::Scale(
        (total_width + 2 * border_offset) / 100.0f,
        (total_height + 2 * border_offset) / 100.0f);

    // set the frame draw mode to outline
    meshes["square_blue_border"]->SetDrawMode(GL_LINE_LOOP);
    // draw the grid frame in blue
    RenderMesh2D(meshes["square_blue_border"], shaders["VertexColor"], visMatrix * borderMatrix);

    // start iterating through each row of the grid
    for (int i = 0; i < rows_grid; i++)
    {
        // for each row, iterate through all columns
        for (int j = 0; j < cols_grid; j++)
        {
            // calculate X coordinate of current cell
            float currentX = grid_coordX + j * (cell_size + padding);
            // calculate Y coordinate of current cell
            float currentY = grid_coordY + i * (cell_size + padding);

            // initialize the model matrix for cell transformation
            glm::mat3 modelMatrix = glm::mat3(1);
            // apply translation to position the cell on screen
            modelMatrix *= transform2Dproject::Translate(currentX, currentY);
            // apply scaling to adjust size to cell_size
            modelMatrix *= transform2Dproject::Scale(cell_size / 100.0f, cell_size / 100.0f);

            // set the draw mode to triangles
            meshes["square_blue_fill"]->SetDrawMode(GL_TRIANGLES);
            // draw the current cell in the grid using blue color
            RenderMesh2D(meshes["square_blue_fill"], shaders["VertexColor"], visMatrix * modelMatrix);
        }
    }
}

void Project::DrawLeftPanelOfGrid()
{
    // set the number of rows in the side panel
    int rows_panel = 4;

    // define the outer margins of the left side panel
    float marginX = logicSpace.width * 0.015f;
    float marginY = logicSpace.height * 0.03f;

    // calculate total height available for the panel
    float total_height = logicSpace.height - 2 * marginY;
    // divide the height into 4 equal blocks, one for each component
    float block_size = total_height / rows_panel;

    // X coordinate of the panel, flush with the left margin
    float panelX = marginX;
    // Y coordinate of the panel, flush with the bottom margin
    float panelY = marginY;

    // iterate through each row
    for (int i = 0; i < rows_panel; i++)
    {
        // calculate Y position of current row in panel
        float y = panelY + i * block_size;

        // initialize the matrix for the frame around each component
        glm::mat3 outline = glm::mat3(1);
        // translation positions the frame in the appropriate place
        outline *= transform2Dproject::Translate(panelX, y);
        // scaling adapts the size to block_size
        outline *= transform2Dproject::Scale(block_size / 100.0f, block_size / 100.0f);

        // set drawing to outline mode
        meshes["square_red"]->SetDrawMode(GL_LINE_LOOP);
        // draw the red frame around each component
        RenderMesh2D(meshes["square_red"], shaders["VertexColor"], visMatrix * outline);

        // calculate center coordinates of current block to align component in the middle
        float cx = panelX + (block_size - cell_size_grid) / 2.0f;
        float cy = y + (block_size - cell_size_grid) / 2.0f;

        // cazul 0: bloc solid gri
        if (i == 0)
        {
            glm::mat3 solid = glm::mat3(1);
            solid *= transform2Dproject::Translate(cx, cy);
            solid *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["square_grey"], shaders["VertexColor"], visMatrix * solid);
        }

        // cazul 1: motor
        if (i == 1)
        {
            glm::mat3 base = glm::mat3(1);
            base *= transform2Dproject::Translate(cx, cy - 5); // lower the base slightly for aesthetic framing
            base *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["engine_base_square"], shaders["VertexColor"], visMatrix * base);

            // draw the engine flame below the base
            glm::mat3 flame = glm::mat3(1);
            flame *= transform2Dproject::Translate(cx, cy - 5);
            flame *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["engine_flame"], shaders["VertexColor"], visMatrix * flame);
        }

        // cazul 2: tun
        if (i == 2)
        {
            // enable depth test to correctly render overlapping elements
            glEnable(GL_DEPTH_TEST);

            // draw the cannon base
            glm::mat3 base = glm::mat3(1);
            base *= transform2Dproject::Translate(cx, cy - 25);
            base *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["gun_base_square"], shaders["VertexColor"], visMatrix * base);

            // draw the semicircle above the base
            glm::mat3 semi = glm::mat3(1);
            semi *= transform2Dproject::Translate(cx + cell_size_grid * 0.5f, cy - 25 + cell_size_grid);
            semi *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["gun_base_semi"], shaders["VertexColor"], visMatrix * semi);

            // draw the cannon pipe above the semicircle
            glm::mat3 pipe = glm::mat3(1);
            pipe *= transform2Dproject::Translate(cx, cy - 25 + cell_size_grid);
            pipe *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["gun_pipe"], shaders["VertexColor"], visMatrix * pipe);

            // dezactivam testul de adancime dupa desenare
            glDisable(GL_DEPTH_TEST);
        }

        // cazul 3: bumper
        if (i == 3)
        {
            // draw the bumper base
            glm::mat3 base = glm::mat3(1);
            base *= transform2Dproject::Translate(cx, cy - 8);
            base *= transform2Dproject::Scale(cell_size_grid / 100.0f, cell_size_grid / 100.0f);
            RenderMesh2D(meshes["bumper_base_square"], shaders["VertexColor"], visMatrix * base);

            // draw the large semicircle above
            glm::mat3 semi = glm::mat3(1);
            semi *= transform2Dproject::Translate(cx + cell_size_grid * 0.5f, cy - 8 + cell_size_grid);
            semi *= transform2Dproject::Scale((cell_size_grid * 3.0f) / 100.0f, (cell_size_grid * 1.0f) / 100.0f);
            RenderMesh2D(meshes["bumper_top_semi"], shaders["VertexColor"], visMatrix * semi);
        }
    }
}

void Project::DrawTopBar()
{
    // set the total number of slots
    int slots = 10;
    // define the spacing between slots
    float spacing = 20.0f;
    // set the size of each slot
    float size = 55.0f;

    // calculate the starting position on the X axis
    float startX = logicSpace.width * 0.26f;
    // calculate the starting position on the Y axis
    float startY = logicSpace.height * 0.84f;

    // iterate through each slot and draw it in the top bar
    for (int i = 0; i < slots; i++)
    {
        // initialize the transformation matrix for the current slot
        glm::mat3 m = glm::mat3(1);
        // apply translation to position the slot horizontally based on index
        m *= transform2Dproject::Translate(startX + i * (size + spacing), startY);
        // apply scaling to adjust size to the graphic unit
        m *= transform2Dproject::Scale(size / 100.0f, size / 100.0f);

        // draw only the free slots (those remaining until the maximum number)
        if (i >= blocksPlaced)
            RenderMesh2D(meshes["square_green"], shaders["VertexColor"], visMatrix * m);
    }

    // initialize the matrix for the validation button
    glm::mat3 button = glm::mat3(1);
    // position the button immediately after the last slots
    button *= transform2Dproject::Translate(startX + slots * (size + spacing) + 25, startY);
    // scaling ensures the same size as the slots
    button *= transform2Dproject::Scale(size / 100.0f, size / 100.0f);

    // if all constraints are valid, show the green arrow
    if (constraintsValid)
        RenderMesh2D(meshes["arrow_green"], shaders["VertexColor"], visMatrix * button);
    // otherwise show the red arrow
    else
        RenderMesh2D(meshes["arrow_red"], shaders["VertexColor"], visMatrix * button);
}

void Project::DrawMainFrame()
{
    // define the left margin proportional to the logical window width
    float marginX = logicSpace.width * 0.015f;
    // define the bottom margin proportional to the logical window height
    float marginY = logicSpace.height * 0.03f;

    // calculate the left coordinate of the large frame
    float leftX = marginX;
    // calculate the bottom coordinate of the large frame
    float bottomY = marginY;
    // calculate the right coordinate of the large frame
    float rightX = logicSpace.width - marginX;
    // calculate the top coordinate of the large frame
    float topY = logicSpace.height - marginY;

    // determine the total width of the main red frame
    float total_width = rightX - leftX;
    // determine the total height of the main red frame
    float total_height = topY - bottomY;

    // initialize the model matrix for frame transformation
    glm::mat3 frameMatrix = glm::mat3(1);
    // apply translation to position the frame in the bottom-left corner
    frameMatrix *= transform2Dproject::Translate(leftX, bottomY);
    // apply scaling to adapt the frame to the calculated dimensions
    frameMatrix *= transform2Dproject::Scale(total_width / 100.0f, total_height / 100.0f);

    // set the line width of the main frame outline
    glLineWidth(2.0f);
    // configure the frame mesh to be drawn only as outline
    meshes["square_red"]->SetDrawMode(GL_LINE_LOOP);
    // draw the large red frame that surrounds the entire logical area of the editor
    RenderMesh2D(meshes["square_red"], shaders["VertexColor"], visMatrix * frameMatrix);
}

bool Project::CheckConstraints()
{
    // check if the vehicle respects all imposed construction rules

    // check if there is at least one block in the vehicle
    if (placedObjects.empty())
    {
        cout << "Error: the vehicle contains no blocks!\n";
        return false;
    }

    // check if all blocks are connected to each other
    if (!IsConnected())
    {
        cout << "Error: the vehicle is not connected!\n";
        return false;
    }

    // check if there are overlapping blocks in the same cell
    for (int i = 0; i < placedObjects.size(); i++)
    {
        for (int j = i + 1; j < placedObjects.size(); j++)
        {
            // daca doua blocuri ocupa acelasi rand si aceeasi coloana, avem o suprapunere
            if (placedObjects[i].row == placedObjects[j].row &&
                placedObjects[i].col == placedObjects[j].col)
            {
                cout << "Error: two blocks overlap at (" << placedObjects[i].row
                     << ", " << placedObjects[i].col << ")\n";
                return false;
            }
        }
    }

    // check that the engine is the lowest block in the vehicle
    for (auto &e : placedObjects)
    {
        // find all engine-type blocks
        if (e.type == "engine")
        {
            for (auto &other : placedObjects)
            {
                // if we find another block below the engine
                if (other.row < e.row)
                {
                    cout << "Error: a block is below the engine!\n";
                    return false;
                }
            }
        }
    }

    // check that there are no other blocks above the cannon
    for (auto &g : placedObjects)
    {
        // find cannon-type blocks
        if (g.type == "gun")
        {
            for (auto &o : placedObjects)
            {
                // if a block exists on the same column and a higher row
                if (o.col == g.col && o.row > g.row)
                {
                    cout << "Error: block above the cannon!\n";
                    return false;
                }
            }
        }
    }

    // check that the bumper has no blocks above it in the three columns
    for (auto &b : placedObjects)
    {
        // find bumper-type blocks
        if (b.type == "bumper")
        {
            for (auto &o : placedObjects)
            {
                // if we find a block above the bumper in the center column or adjacent ones
                if (abs(o.col - b.col) <= 1 && o.row > b.row)
                {
                    cout << "Error: block above the bumper!\n";
                    return false;
                }
            }
        }
    }

    // check that there are no two adjacent bumpers
    for (auto &b : placedObjects)
    {
        // if the current object is a bumper
        if (b.type == "bumper")
        {
            for (auto &o : placedObjects)
            {
                // check if another bumper is at distance 1 horizontally or vertically
                if (o.type == "bumper" && abs(o.col - b.col) <= 1 && abs(o.row - b.row) <= 1 && &o != &b)
                {
                    cout << "Error: two adjacent bumpers!\n";
                    return false;
                }
            }
        }
    }

    // check that the cannon is not too close horizontally to a bumper
    for (auto &g : placedObjects)
    {
        // if the object is a cannon
        if (g.type == "gun")
        {
            for (auto &b : placedObjects)
            {
                // if the bumper is on the same line and column at ±1 from the cannon
                if (b.type == "bumper" && abs(b.col - g.col) <= 1)
                {
                    cout << "Error: cannon too close to bumper!\n";
                    return false;
                }
            }
        }
    }

    // check that we do not exceed the maximum allowed number of blocks
    if (placedObjects.size() > 10)
    {
        cout << "Error: the vehicle contains more than 10 blocks!\n";
        return false;
    }

    // if all rules are met, confirm that the vehicle is valid
    cout << "Valid vehicle! All constraints satisfied.\n";
    return true;
}

bool Project::IsConnected()
{
    // check if all placed blocks are connected to each other in a single component

    // if there are no blocks, the vehicle cannot be considered connected
    if (placedObjects.empty())
        return false;

    // declare two boolean matrices of size 20x20:
    // visited stores whether a cell has been visited
    bool visited[20][20] = {false};
    // exists stores whether a block is placed on that cell
    bool exists[20][20] = {false};

    // initialize min row and column values to detect the occupied area
    int minRow = 100, minCol = 100;

    // iterate through the list of all placed blocks
    for (auto &obj : placedObjects)
    {
        // mark the current position as occupied by a block
        exists[obj.row][obj.col] = true;
        // update the min on rows and columns for reference
        minRow = min(minRow, obj.row);
        minCol = min(minCol, obj.col);
    }

    // initialize a stack for the DFS algorithm
    vector<pair<int, int>> stack;
    // add the position of the first block in the list as the starting point for DFS
    stack.push_back({placedObjects[0].row, placedObjects[0].col});
    // mark the first block as visited
    visited[placedObjects[0].row][placedObjects[0].col] = true;

    // initialize a counter for the number of connected blocks found in DFS
    int connectedCount = 0;

    // while we have elements in the stack, continue exploration
    while (!stack.empty())
    {
        // extract the coordinates of the last element from the stack
        int r = stack.back().first;
        int c = stack.back().second;
        // pop the element from the stack after processing it
        stack.pop_back();

        // increment the total number of connected blocks discovered
        connectedCount++;

        // define the direction vectors for movement up, down, left, right
        int dr[] = {1, -1, 0, 0};
        int dc[] = {0, 0, 1, -1};

        // iterate through the 4 possible directions
        for (int k = 0; k < 4; k++)
        {
            // calculate the row and column of the current neighbor
            int nr = r + dr[k];
            int nc = c + dc[k];

            // check if the neighbor is within the grid bounds
            if (nr >= 0 && nc >= 0 && nr < 20 && nc < 20)
            {
                // check if a block exists in that cell and it has not been visited
                if (exists[nr][nc] && !visited[nr][nc])
                {
                    // mark the neighbor as visited
                    visited[nr][nc] = true;
                    // add the neighbor to the stack to continue exploration
                    stack.push_back({nr, nc});
                }
            }
        }
    }

    // the vehicle is considered connected if we visited exactly the same number of blocks as exist in total
    return connectedCount == placedObjects.size();
}

void Project::DrawGame()
{
    // set the background color to dark blue
    glClearColor(0.05f, 0.05f, 0.2f, 1);

    // clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // disable depth test for 2D scene
    glDisable(GL_DEPTH_TEST);

    // define the left/right margin of the arena
    float marginX = 200.0f;

    // define the top/bottom margin of the arena
    float marginY = 80.0f;

    // X coordinate of the arena frame
    float frameX = marginX;

    // Y coordinate of the arena frame
    float frameY = marginY;

    // inner width of the arena frame
    float frameWidth = logicalSceneWidth - 2 * marginX;

    // inner height of the arena frame
    float frameHeight = logicalSceneHeight - 2 * marginY;

    // initialize the model matrix for drawing the frame
    glm::mat3 frameMatrix = glm::mat3(1);

    // translation to position the frame
    frameMatrix *= transform2Dproject::Translate(frameX + 2, frameY + 2);

    // scalare astfel incat patratul sa devina chenarul complet
    frameMatrix *= transform2Dproject::Scale((frameWidth - 4) / 100.0f, (frameHeight - 4) / 100.0f);

    // set the line width
    glLineWidth(2.0f);

    // draw the arena frame
    RenderMesh2D(meshes["square_grey2"], shaders["VertexColor"], visMatrix * frameMatrix);

    // bucla pentru desenarea liniei punctate centrale
    for (int i = 0; i < 20; i++)
    {
        // skip the even segments
        if (i % 2 == 0)
            continue;

        // initialize the model matrix for the segment
        glm::mat3 mid = glm::mat3(1);

        // calculate the vertical position of the segment
        float segmentY = frameY + i * (frameHeight / 20.0f);

        // translation to position the segment
        mid *= transform2Dproject::Translate(logicalSceneWidth / 2.0f - 5, segmentY);

        // scaling for the dashed segment size
        mid *= transform2Dproject::Scale(10 / 100.0f, 20 / 100.0f);

        // set the draw mode to triangles
        meshes["square_grey"]->SetDrawMode(GL_TRIANGLES);

        // draw the segment
        RenderMesh2D(meshes["square_grey"], shaders["VertexColor"], visMatrix * mid);
    }

    // radius of a score point
    float scoreRadius = 12.0f;

    // distance between score balls
    float spacing = 25.0f;

    // vertical position of the score
    float scoreY = frameY + frameHeight + 40;

    // draw the score balls for the left player
    for (int i = 0; i < leftScore; i++)
    {
        // initialize the model matrix
        glm::mat3 scoreMatrix = glm::mat3(1);

        // calculate the position of each ball
        float posX = frameX + 20 + i * (2 * scoreRadius + spacing);

        // translation to ball position
        scoreMatrix *= transform2Dproject::Translate(posX, scoreY);

        // scaling for the ball size
        scoreMatrix *= transform2Dproject::Scale(scoreRadius / 50.0f, scoreRadius / 50.0f);

        // draw the ball
        RenderMesh2D(meshes["ball_orange"], shaders["VertexColor"], visMatrix * scoreMatrix);
    }

    // draw the score balls for the right player
    for (int i = 0; i < rightScore; i++)
    {
        glm::mat3 scoreMatrix = glm::mat3(1);

        // position of balls at the right edge
        float posX = frameX + frameWidth - (i + 1) * (2 * scoreRadius + spacing);

        scoreMatrix *= transform2Dproject::Translate(posX, scoreY);
        scoreMatrix *= transform2Dproject::Scale(scoreRadius / 50.0f, scoreRadius / 50.0f);
        RenderMesh2D(meshes["ball_orange"], shaders["VertexColor"], visMatrix * scoreMatrix);
    }

    // size of a cell in the ship structure
    float cell = cell_size_grid;

    // pozitia navei stanga pe axa X
    leftShipX = 210.0f;

    // pozitia navei dreapta pe axa X
    rightShipX = 980.0f;

    // animation factor for the left ship
    float animScaleL = 1.0f;

    // check if the animation is active
    if (leftPaddleAnimTime > 0.0f)
    {
        // normalize animation time between 0 and 1
        float t = 1.0f - (leftPaddleAnimTime / paddleAnimDuration);

        // animatie pulsatorie
        animScaleL = 1.0f + paddleAnimAmplitude * sin(t * 2.0f * glm::pi<float>());
    }

    // drawing block for the left ship
    {
        // initialize the model matrix
        glm::mat3 modelMatrix = glm::mat3(1);

        // translation to ship position
        modelMatrix *= transform2Dproject::Translate(leftShipX, paddleLeftY);

        // translatie la centrul navei pentru rotatie
        modelMatrix *= transform2Dproject::Translate(cell * 1.5f, cell * 3.0f);

        // rotate the left ship toward the right
        modelMatrix *= transform2Dproject::Rotate(glm::radians(-90.0f));

        // apply vertical scaling with animation
        modelMatrix *= transform2Dproject::Scale(0.7f, 0.7f * animScaleL);

        // return to the model origin
        modelMatrix *= transform2Dproject::Translate(-cell * 1.5f, -cell * 3.0f);

        // draw the left ship
        DrawPlayerShip(playerShipLeft, leftShipX, paddleLeftY, false, modelMatrix);
    }

    // animation factor for the right ship
    float animScaleR = 1.0f;

    if (rightPaddleAnimTime > 0.0f)
    {
        float t = 1.0f - (rightPaddleAnimTime / paddleAnimDuration);
        animScaleR = 1.0f + paddleAnimAmplitude * sin(t * 2.0f * glm::pi<float>());
    }

    // drawing block for the right ship
    {
        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix *= transform2Dproject::Translate(rightShipX, paddleRightY);
        modelMatrix *= transform2Dproject::Translate(cell * 1.5f, cell * 3.0f);
        modelMatrix *= transform2Dproject::Rotate(glm::radians(90.0f));

        // mirror on OX and apply animation on OY
        modelMatrix *= transform2Dproject::Scale(-0.7f, 0.7f * animScaleR);

        modelMatrix *= transform2Dproject::Translate(-cell * 1.5f, -cell * 3.0f);
        DrawPlayerShip(playerShipRight, rightShipX, paddleRightY, true, modelMatrix);
    }

    // draw all active balls
    for (auto &b : balls)
    {
        // skip inactive balls
        if (!b.alive)
            continue;

        // initialize the model matrix
        glm::mat3 M = glm::mat3(1);

        // translation to ball position
        M *= transform2Dproject::Translate(b.x, b.y);

        // scale to the correct size
        M *= transform2Dproject::Scale(ballRadius / 50.0f, ballRadius / 50.0f);

        // draw the ball
        RenderMesh2D(meshes["ball_orange"], shaders["VertexColor"], visMatrix * M);
    }

    // if the game has ended
    if (gameOver)
    {
        // change background to black
        glClearColor(0, 0, 0, 1);

        // stergem tot ce era desenat
        glClear(GL_COLOR_BUFFER_BIT);
    }
}


void Project::ResetBall()
{
    // set the ball position to the center of the screen on OX
    ballX = logicalSceneWidth / 2.0f;

    // set the ball position to the center of the screen on OY
    ballY = logicalSceneHeight / 2.0f;

    // define the base speed of the ball after reset
    float baseSpeed = 260.0f; 

    // send the ball toward the player who received the goal
    ballVX = (ballVX > 0 ? -baseSpeed : baseSpeed);

    // generate a random vertical component between -100 and +100
    ballVY = (rand() % 200 - 100);

    // calculate the length of the velocity vector
    float len = sqrt(ballVX * ballVX + ballVY * ballVY);

    // normalize the OX component to base speed
    ballVX = baseSpeed * (ballVX / len);

    // normalize the OY component to base speed
    ballVY = baseSpeed * (ballVY / len);
}


void Project::DrawPlayerShip(std::vector<PlacedObject> &ship,
                           float startX, float startY,
                           bool isRight,
                           glm::mat3 baseTransform)
{
    // distance between cells inside the ship
    float padding = 2.0f;

    // scaling factor for sprites
    float scale = cell_size_grid / 100.0f;

    // iterate through all components added to the ship
    for (auto &obj : ship)
    {
        // calculate horizontal translation based on object column
        float offsetX = obj.col * (cell_size_grid + padding);

        // calculate vertical translation based on object row
        float offsetY = obj.row * (cell_size_grid + padding);

        // start from the base matrix
        glm::mat3 M = baseTransform;

        // apply translation within the ship
        M *= transform2Dproject::Translate(offsetX, offsetY);

        // apply scaling of the individual cell
        M *= transform2Dproject::Scale(scale, scale);

      
        if (obj.type == "solid")
        {
            // draw the solid cell
            RenderMesh2D(meshes["square_grey"], shaders["VertexColor"], visMatrix * M);
        }

      
        else if (obj.type == "engine")
        {
            // draw the engine base
            RenderMesh2D(meshes["engine_base_square"], shaders["VertexColor"], visMatrix * M);

            // pregatim flacara motorului
            glm::mat3 flame = M;

            // move the flame below the engine
            flame *= transform2Dproject::Translate(50, -20);

            // shrink the flame
            flame *= transform2Dproject::Scale(0.5f, 0.5f);

            // draw the flame
            RenderMesh2D(meshes["engine_flame"], shaders["VertexColor"], visMatrix * flame);
        }

    
        else if (obj.type == "gun")
        {
            // draw the cannon base
            RenderMesh2D(meshes["gun_base_square"], shaders["VertexColor"], visMatrix * M);

            // draw the upper semicircle of the cannon
            glm::mat3 semi = M;
            semi *= transform2Dproject::Translate(50, 100);
            semi *= transform2Dproject::Scale(1.0f, 1.0f);
            RenderMesh2D(meshes["gun_base_semi"], shaders["VertexColor"], visMatrix * semi);

            // draw the cannon pipe
            glm::mat3 pipe = M;
            pipe *= transform2Dproject::Translate(0, 100);
            pipe *= transform2Dproject::Scale(1.0f, 1.15f);
            RenderMesh2D(meshes["gun_pipe"], shaders["VertexColor"], visMatrix * pipe);
        }


        else if (obj.type == "bumper")
        {
            // draw the bumper base
            RenderMesh2D(meshes["bumper_base_square"], shaders["VertexColor"], visMatrix * M);

            // draw the semicircular shape above the bumper
            glm::mat3 semi = M;
            semi *= transform2Dproject::Translate(50, 100);
            semi *= transform2Dproject::Scale(3.0f, 1.1f);
            RenderMesh2D(meshes["bumper_top_semi"], shaders["VertexColor"], visMatrix * semi);
        }
    }
}


void Project::ComputeShipAABB(const std::vector<PlacedObject> &ship,
                            const glm::mat3 &baseTransform,
                            float &minX, float &maxX,
                            float &minY, float &maxY)
{
    // if the ship has no objects, set limits to 0 and exit
    if (ship.empty())
    {
        minX = maxX = 0.0f;
        minY = maxY = 0.0f;
        return;
    }

    // distance between ship cells
    float padding = 2.0f;

    // scaling factor (100px -> cell_size_grid)
    float scale = cell_size_grid / 100.0f;

    // flag to identify the first iteration
    bool first = true;

    // iterate through each object placed in the ship
    for (const auto &obj : ship)
    {
        // calculate horizontal translation based on object column
        float offsetX = obj.col * (cell_size_grid + padding);

        // calculate vertical translation based on object row
        float offsetY = obj.row * (cell_size_grid + padding);

        // start from the general ship transform
        glm::mat3 M = baseTransform;

        // apply translation of the object within the ship
        M *= transform2Dproject::Translate(offsetX, offsetY);

        // apply scaling of a 100x100 block to cell size
        M *= transform2Dproject::Scale(scale, scale);

        // define the 4 corners of a 100x100 square in local space
        glm::vec3 corners[4] = {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(100.0f, 0.0f, 1.0f),
            glm::vec3(100.0f, 100.0f, 1.0f),
            glm::vec3(0.0f, 100.0f, 1.0f)
        };

        // transform each corner into world space
        for (int i = 0; i < 4; i++)
        {
            // apply the final ship transform + block position
            glm::vec3 w = M * corners[i];

            // on the first corner, initialize AABB limits
            if (first)
            {
                minX = maxX = w.x;
                minY = maxY = w.y;
                first = false;
            }
            else
            {
                // update the min and max limits
                minX = std::min(minX, w.x);
                maxX = std::max(maxX, w.x);
                minY = std::min(minY, w.y);
                maxY = std::max(maxY, w.y);
            }
        }
    }
}


float Project::ComputeShipHeight(const std::vector<PlacedObject> &ship)
{
    // if the ship is empty, height is zero
    if (ship.empty())
        return 0;

    // padding between cells inside the ship
    float padding = 2.0f;

    // scaling factor applied to the ship when drawing
    float scale = 0.7f;

    // initialize row limits with extreme values
    int minRow = 999, maxRow = -999;

    // determine the min and max row in the ship
    for (const auto &o : ship)
    {
        minRow = std::min(minRow, o.row);
        maxRow = std::max(maxRow, o.row);
    }

    // total number of rows occupied by the ship
    int totalRows = maxRow - minRow + 1;

    // calculate the real height of the ship from cells, padding and scale
    float height = totalRows * (cell_size_grid + padding) * scale;

    // return the final height
    return height;
}



void Project::ApplyBumperSpeedBoost(const std::vector<PlacedObject> &ship,
                                  const glm::mat3 &baseTransform,
                                  Ball &b)
{
    // if the ship has no components, exit
    if (ship.empty())
        return;

    // distance between cells
    float padding = 2.0f;

    // scaling factor for the 100x100 block
    float scale = cell_size_grid / 100.0f;

    // iterate through all components of the ship
    for (const auto &obj : ship)
    {
        // ignore components that are not bumpers
        if (obj.type != "bumper")
            continue;

        // horizontal translation of the bumper
        float offsetX = obj.col * (cell_size_grid + padding);

        // vertical translation of the bumper
        float offsetY = obj.row * (cell_size_grid + padding);

        // start from the general ship transform
        glm::mat3 M = baseTransform;

        // apply bumper translation
        M *= transform2Dproject::Translate(offsetX, offsetY);

        // apply cell scaling
        M *= transform2Dproject::Scale(scale, scale);

        // define the bumper shape by 4 custom corners
        glm::vec3 corners[4] = {
            glm::vec3(-50.0f, 0.0f, 1.0f),
            glm::vec3(150.0f, 0.0f, 1.0f),
            glm::vec3(150.0f, 120.0f, 1.0f),
            glm::vec3(-50.0f, 120.0f, 1.0f)
        };

        // initialize the bumper AABB limits
        float minX = 1e9f, maxX = -1e9f;
        float minY = 1e9f, maxY = -1e9f;

        // transform each corner to global space and update the AABB
        for (int i = 0; i < 4; i++)
        {
            glm::vec3 w = M * corners[i];
            minX = std::min(minX, w.x);
            maxX = std::max(maxX, w.x);
            minY = std::min(minY, w.y);
            maxY = std::max(maxY, w.y);
        }

        // determine the closest point in the AABB to the ball
        float closestX = std::max(minX, std::min(b.x, maxX));
        float closestY = std::max(minY, std::min(b.y, maxY));

        // vector between the ball and the closest point
        float dx = b.x - closestX;
        float dy = b.y - closestY;

        // check if the ball intersects the bumper AABB
        if (dx * dx + dy * dy <= b.radius * b.radius)
        {
            // calculate the current speed of the ball
            float speed = sqrt(b.vx * b.vx + b.vy * b.vy);

            // apply a speed boost
            speed *= 1.08f;

            // calculate the movement angle
            float angle = atan2(b.vy, b.vx);

            // recalculate velocity components after boost
            b.vx = speed * cos(angle);
            b.vy = speed * sin(angle);

            // exit - only one collision per frame
            return;
        }
    }
}


void Project::SpawnBallFromLeft()
{
    // create a new ball
    Ball b;

    // set the ball radius
    b.radius = ballRadius;

    // approximate the cannon position in the left ship
    b.x = leftShipX + cell_size_grid * 4;
    b.y = paddleLeftY + cell_size_grid * 3;

    // ball travels to the right
    b.vx = 250.0f;

    // random vertical component
    b.vy = (rand() % 150 - 75);

    // ball is active
    b.alive = true;

    // add the ball to the list
    balls.push_back(b);
}


void Project::SpawnBallFromRight()
{
    // create a new ball
    Ball b;

    // set the ball radius
    b.radius = ballRadius;

    // approximate the cannon position in the right ship
    b.x = rightShipX - cell_size_grid * 4;
    b.y = paddleRightY + cell_size_grid * 3;

    // ball travels to the left
    b.vx = -250.0f;

    // random vertical component
    b.vy = (rand() % 150 - 75);

    // ball is active
    b.alive = true;

    // add the ball to the ball vector
    balls.push_back(b);
}


void Project::StartNewRound()
{
    // clear all balls from the previous round
    balls.clear();

    // launch the ball from the left ship's cannon
    SpawnBallFromLeft();

    // launch the ball from the right ship's cannon
    SpawnBallFromRight();

    // mark that the round is active
    roundActive = true;
}
