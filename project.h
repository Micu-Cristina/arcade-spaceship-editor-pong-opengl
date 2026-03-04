#pragma once

#include "components/simple_scene.h"


#include <string>
#include <vector>

namespace m1
{
    // Structure for an object placed in the grid
    struct PlacedObject {
        std::string type; // "gun", "engine", "bumper", "solid"
        int row;
        int col;
    };

    struct Object {
        std::string type;
        int row, col;
    };


    class Project : public gfxc::SimpleScene
    {
    public:
        
        Project();
        ~Project();

        void Init() override;
        struct ViewportSpace
        {

            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {
            }
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {
            }
            float x;
            float y;
            float width;
            float height;
        };

        enum GameState {
            STATE_EDITOR,
            STATE_GAME
        };

        GameState currentState = STATE_EDITOR; 


    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void DrawGrid();
        void DrawLeftPanelOfGrid();
        void DrawTopBar();
        void DrawMainFrame();
        glm::vec2 ScreenToWorld(int mouseX, int mouseY);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logic, const ViewportSpace& view);
        glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 color, bool clear);
        void ResetBall();
        void ComputeShipAABB(const std::vector<PlacedObject>& ship,
            const glm::mat3& baseTransform,
            float& minX, float& maxX,
            float& minY, float& maxY);
        float ComputeShipHeight(const std::vector<PlacedObject>& ship);
        void StartNewRound();
        void SpawnBallFromRight();
        void SpawnBallFromLeft();
        


        struct Ball {
            float x, y;
            float vx, vy;
            float radius;
            bool  active;
            bool alive;
        };

        std::vector<Ball> balls;

        // editor variables
        float cell_size_grid;
        std::vector<PlacedObject> placedObjects;

        bool isDragging = false;      
        std::string draggedType = "";     
        float mouseX_world = 0;          
        float mouseY_world = 0;         
        bool leftMouseHeld = false;
        std::vector<glm::vec2> pendingSemicircles;

        bool CheckConstraints();
        bool IsConnected();

        void ApplyBumperSpeedBoost(const std::vector<PlacedObject>& ship,
            const glm::mat3& baseTransform,
            Ball& b);


        float leftShipX;
        float rightShipX;
        int maxBlocks = 10;         
        int blocksPlaced = 0;       
        bool constraintsValid = true; 

        float logicalSceneWidth;  
        float logicalSceneHeight;  

        LogicSpace logicSpace;
        ViewportSpace viewSpace;
        glm::mat3 visMatrix;
        float length;



        // Pong game variables
        float paddleSpeed = 400.0f;     
        float ballSpeed = 300.0f;     
        float paddleWidth = 20.0f;      
        float paddleHeight = 120.0f;    

        float paddleLeftY = 0.0f;    
        float paddleRightY = 0.0f;  

        float ballX = 0.0f; 
        float ballY = 0.0f;    
        float ballVX = 0.0f;            
        float ballVY = 0.0f;           
        float ballRadius = 10.0f;

  
        float marginX = 200.0f;
        float marginY = 80.0f;
        int leftScore = 0;
        int rightScore = 0;

        bool gameOver = false;
        std::string winnerText = "";
        int maxScore = 5;

        // ship (paddle) animation on ball impact - lasts 1 second
        float leftPaddleAnimTime = 0.0f;
        float rightPaddleAnimTime = 0.0f;

        // maximum scale factor for swell/shrink in animation
        const float paddleAnimDuration = 1.0f;   // 1 second
        const float paddleAnimAmplitude = 0.2f;  // +20% / -20% of scale
        bool roundActive = false;

        std::vector<PlacedObject> playerShipLeft;
        std::vector<PlacedObject> playerShipRight;


        const float arenaMarginX = 200.0f;
        const float arenaMarginY = 80.0f;

        void DrawPlayerShip(std::vector<PlacedObject>& ship,
            float startX, float startY,
            bool isRight,
            glm::mat3 baseTransform);
        void DrawGame();
    };
}   // namespace m1
