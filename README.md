# Arcade Machine - Spaceship Editor & Pong

A 2D game project implementing a **spaceship construction editor** and the classic **Pong** arcade game. Built with **OpenGL** and **C++** using the GFX Framework 


---

## 📋 Table of Contents

- [Project Overview](#project-overview)
- [Framework Structure](#framework-structure)
- [Project Architecture](#project-architecture)
- [File Structure](#file-structure)
- [Function Reference](#function-reference)
- [How to Build & Run](#how-to-build--run)
- [Controls](#controls)

---

## Project Overview

This project consists of two main phases:

### 1. **Spaceship Editor** (STATE_EDITOR)
- Design a custom spaceship using a grid-based editor
- **4 block types**: Solid, Engine, Cannon (Gun), Bumper
- **Drag & drop** construction with left mouse button
- **Right-click** to remove blocks from the grid
- **Constraints validation** before starting the game
- **Start button** (green when valid, red when invalid) to launch the game

### 2. **Pong Game** (STATE_GAME)
- Two-player Pong using the designed ships as paddles
- **Left player**: W / S keys
- **Right player**: ↑ / ↓ arrow keys
- **Balls spawn** from cannons on each ship at round start
- **Bumper blocks** increase ball speed on contact
- **Paddle animation** on ball impact
- First player to **5 goals** wins

---

## Framework Structure

The project uses the **GFX Framework** (gfx-framework-master), a C++/OpenGL framework for 2D and 3D graphics coursework.

### Framework Components

| Component | Location | Description |
|-----------|----------|-------------|
| **Engine** | `core/engine.cpp` | Main loop, window creation, OpenGL context |
| **SimpleScene** | `components/simple_scene.h` | Base class for 2D/3D scenes; provides `Init`, `Update`, `FrameStart`, `FrameEnd`, input callbacks |
| **Mesh** | `core/gpu/mesh.h` | Vertex/index buffers, draw modes (triangles, line loops) |
| **Shader** | `core/gpu/shader.h` | Vertex/fragment shaders (e.g., VertexColor) |
| **Camera** | `components/camera.h` | Orthographic projection for 2D |
| **Window** | `core/window/` | GLFW window, input (mouse, keyboard) |

### How the Project Uses the Framework

1. **Project** extends `gfxc::SimpleScene` and overrides:
   - `Init()` – create meshes, set up logic/view spaces
   - `FrameStart()` – clear buffers, compute view matrix
   - `Update(deltaTimeSeconds)` – game logic and rendering
   - `FrameEnd()` – (empty)
   - `OnMouseBtnPress/Release`, `OnMouseMove` – editor interaction
   - `OnWindowResize` – adapt to new window size

2. **Logic space** (1280×720) is mapped to the **viewport** via `VisualizationTransf2DUnif` for consistent scaling and centering.

3. **Meshes** are created with `object2D_project::CreateSquare`, `CreateCircle`, `CreateSemicircle`, `CreateFlame`, `CreateArrow`, etc., and added with `AddMeshToList`.

4. **Rendering** uses `RenderMesh2D(mesh, shader, modelMatrix)` with `shaders["VertexColor"]`.

---

## Project Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Project (main scene)                  │
├─────────────────────────────────────────────────────────────┤
│  STATE_EDITOR                    │  STATE_GAME               │
│  ├─ DrawMainFrame()              │  ├─ DrawGame()            │
│  ├─ DrawLeftPanelOfGrid()        │  │   ├─ Arena, score      │
│  ├─ DrawGrid()                   │  │   ├─ DrawPlayerShip()  │
│  ├─ DrawTopBar()                 │  │   └─ Balls             │
│  ├─ placedObjects                │  ├─ Ball physics          │
│  ├─ Drag & drop                  │  ├─ Collisions            │
│  └─ CheckConstraints()           │  │   ├─ Paddles (AABB)    │
│                                  │  │   ├─ Bumpers (speed)   │
│                                  │  │   └─ Goals             │
│                                  │  └─ StartNewRound()       │
└─────────────────────────────────────────────────────────────┘
         │                                    │
         └──────────────┬─────────────────────┘
                        │
         ┌──────────────┴──────────────┐
         │  object2D_project           │  transform2Dproject
         │  CreateSquare, CreateCircle │  Translate, Scale, Rotate
         │  CreateFlame, CreateArrow   │  (3×3 matrices)
         └─────────────────────────────┘
```

---

## Function Reference

### Core Lifecycle

#### `void Init()`
- Sets logical scene size (1280×720)
- Builds logic and viewport spaces
- Configures orthographic camera (2D)
- Disables camera input
- Computes visualization matrix
- Creates meshes: squares (red, grey, blue, green), circle (ball), engine (base + flame), gun (base, semicircle, pipe), bumper (base + semicircle), arrows (green/red)

#### `void FrameStart()`
- Clears color and depth buffers
- Updates viewport to window resolution
- Recomputes `visMatrix` with `VisualizationTransf2DUnif`
- Calls `SetViewportArea` for the new viewport

#### `void Update(float deltaTimeSeconds)`
- Disables depth test for 2D
- **EDITOR**: draws main frame, left panel, grid, top bar; handles drag preview; draws placed objects
- **GAME**: draws game scene; updates ball positions; handles paddle-ball collisions; bumper speed boost; goal detection; paddle movement (W/S, ↑/↓); round end and `StartNewRound` when all balls are gone

---

### Coordinate & View

#### `glm::vec2 ScreenToWorld(int mouseX, int mouseY)`
Converts screen coordinates to logic-space coordinates using the inverse of `visMatrix`. Used for mouse interactions in the editor.

#### `glm::mat3 VisualizationTransf2D(...)`  
Non-uniform 2D view transform (logic → viewport).

#### `glm::mat3 VisualizationTransf2DUnif(...)`
Uniform 2D view transform that keeps aspect ratio and centers the logic space in the viewport.

#### `void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 color, bool clear)`
Sets the OpenGL viewport and scissor region; optionally clears buffers. Updates the orthographic camera to the viewport bounds.

---

### Editor

#### `void DrawMainFrame()`
Draws the outer red frame around the editor.

#### `void DrawGrid()`
Draws the 7×15 construction grid: blue filled cells and blue border. Uses `transform2Dproject::Translate` and `Scale`.

#### `void DrawLeftPanelOfGrid()`
Draws the left panel with 4 block types (solid, engine, gun, bumper). Each type is drawn with its specific meshes.

#### `void DrawTopBar()`
Draws 10 green slots (remaining blocks) and the start/validation button (green or red arrow).

#### `bool CheckConstraints()`
Validates the vehicle:
- At least one block
- All blocks connected (via `IsConnected`)
- No overlapping blocks
- Engine is lowest
- No blocks above cannon
- No blocks above bumper (3 columns)
- No adjacent bumpers
- No cannon within 1 block of bumper
- At most 10 blocks

#### `bool IsConnected()`
Uses DFS on a 20×20 grid to check that all placed blocks form a single connected component (4-neighborhood).

---

### Game

#### `void DrawGame()`
Draws the Pong scene: arena frame, center dashed line, score balls (left and right), both ships (with animation), and all active balls.

#### `void DrawPlayerShip(ship, startX, startY, isRight, baseTransform)`
Renders a ship: iterates over `PlacedObject`s, applies `baseTransform` plus per-cell offset, draws solid/engine/gun/bumper meshes.

#### `void ComputeShipAABB(ship, baseTransform, minX, maxX, minY, maxY)`
Computes axis-aligned bounding box for a ship. Transforms the four corners of each 100×100 cell and takes min/max. Used for paddle-ball collision.

#### `float ComputeShipHeight(ship)`
Returns the vertical extent of the ship in logic units (rows × cell size × scale).

#### `void ApplyBumperSpeedBoost(ship, baseTransform, ball)`
For each bumper in the ship, computes its AABB, checks circle-AABB collision with the ball. On hit, increases ball speed by 8% and applies only once per frame.

#### `void StartNewRound()`
Clears balls and spawns one ball from each ship via `SpawnBallFromLeft` and `SpawnBallFromRight`.

#### `void SpawnBallFromLeft()` / `SpawnBallFromRight()`
Creates a ball at the cannon position with horizontal velocity toward the opponent. Vertical component is random.

#### `void ResetBall()`
Resets ball position to center and velocity toward the player who conceded (used in alternate designs; current design uses cannon spawn).

---

### Input

#### `OnMouseBtnPress(mouseX, mouseY, button, mods)`
- **Left click in left panel**: start drag, set `draggedType` (solid/engine/gun/bumper)
- **Right click in grid**: remove block at that cell
- **Click on Start button**: if `constraintsValid`, copy `placedObjects` to both ships, init Pong state, call `StartNewRound`, switch to `STATE_GAME`

#### `OnMouseBtnRelease(mouseX, mouseY, button, mods)`
- **Left release**: if dragging and cursor over grid, add block at that cell; update `constraintsValid`.

#### `OnMouseMove(mouseX, mouseY, ...)`
Updates `mouseX_world` and `mouseY_world` via `ScreenToWorld` for drag preview and hit testing.

#### `OnWindowResize(width, height)`
Updates `viewSpace`, recomputes `visMatrix`, and adjusts the orthographic camera.

---

### object2D_project (Mesh Creation)

| Function | Description |
|----------|-------------|
| `CreateSquare(name, corner, length, color, fill)` | Square; outline if `fill=false` |
| `CreateRectangle(name, corner, width, height, color, fill)` | Rectangle |
| `CreateCircle(name, radius, color)` | Circle (40 segments) |
| `CreateSemicircle(name, radius, color)` | Semicircle (30 segments) |
| `CreateFlame(name, corner, width, height, color)` | Zigzag flame shape |
| `CreateArrow(name, corner, width, height, color, fill)` | Arrow for Start button |

---

### transform2Dproject (2D Transforms)

| Function | Description |
|----------|-------------|
| `Translate(tx, ty)` | 3×3 translation matrix |
| `Scale(sx, sy)` | 3×3 scale matrix |
| `Rotate(radians)` | 3×3 rotation matrix |

---


---

## Controls

| Action | Input |
|--------|--------|
| Select block type | Left-click on block in left panel |
| Place block | Drag from panel and release on grid |
| Remove block | Right-click on grid cell |
| Start game | Left-click green Start button |
| Left paddle up | W |
| Left paddle down | S |
| Right paddle up | ↑ |
| Right paddle down | ↓ |

---


