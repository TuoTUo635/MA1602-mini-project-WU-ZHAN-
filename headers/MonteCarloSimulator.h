#ifndef MONTECARLOSIMULATOR_H
#define MONTECARLOSIMULATOR_H

#include <iostream>
#include <vector>
#include <ctime>  // 需要包含这个 for time()
#include "GridWorld.h"
#include "Config.h"
#include "Walker.h"
// 注意：Config.h 应该已经被包含了，但这里不需要再包含，因为头文件保护会防止重复

// 前向声明（如果 Walker 在另一个头文件中）
// class Walker;

class MonteCarloSimulator {
private:
    GridWorld world;
    std::vector<Walker> walkers;
    SimulationConfig config;
    
    // 统计信息
    int currentStep;
    double currentCoverage;
    std::vector<int> stepsToCoverage;  // 每次实验达到目标覆盖率的步数
    
public:
    // 初始化统计信息和网格图
    MonteCarloSimulator(const SimulationConfig& cfg): config(cfg), world(cfg.width, cfg.height, cfg.cellSize),currentStep(0),currentCoverage(0.0) {
        if (config.seed == 0) { // 这里生成种子是为了后面随机move做准备
            config.seed = static_cast<unsigned int>(time(nullptr));
        }
    }
    
    int getCurrentStep() { return currentStep; };
    double getCurrentCoverage() { return currentCoverage; }
    
    // 初始化
    void initialize() {
        switch (config.obstacleMode) {
        case SimulationConfig::ObstacleMode::RANDOM_DENSITY:
            generateObstaclesByDensity();
            break;
        case SimulationConfig::ObstacleMode::LOAD_FROM_FILE:
            //loadObstaclesFromFile(config.mapFile); (可选)
            break;
        
        case SimulationConfig::ObstacleMode::NONE:
            initializeNoObstacles();
            break;
        }
    } 
    
    void generateWalkers(){
        walkers.clear();
        
        std::cout << "Generating " << config.numWalkers << " walkers..." << std::endl;
        
        for (int i = 0; i < config.numWalkers; i++) {
            int startX, startY;
            int attempts = 0;
            bool found = false;
            
            while (!found && attempts < 1000) {  // 防止无限循环
                startX = rand() % config.width;
                startY = rand() % config.height;
                attempts++;
                
                if (!world.isObstacle(startX, startY)) {
                    found = true;
                }
            }
            
            if (found) {
                walkers.emplace_back(startX, startY);
                world.markVisited(startX, startY);  // 标记起点
                std::cout << "  Walker " << i << " at (" << startX << "," << startY << ")" << std::endl;
            } else {
                std::cout << "  Warning: Could not find valid position for walker " << i 
                          << " after " << attempts << " attempts" << std::endl;
                // 备选方案：放在(0,0)如果不是障碍物
                if (!world.isObstacle(0, 0)) {
                    walkers.emplace_back(0, 0);
                    world.markVisited(0, 0);
                    std::cout << "  Walker " << i << " placed at (0,0) as fallback" << std::endl;
                }
            }
        }
        
        std::cout << "Total walkers created: " << walkers.size() << std::endl;
    };
    void generateObstaclesByDensity() {

    // 1. 生成障碍物
    int totalCells = config.width * config.height;
    int numObstacles = static_cast<int>(totalCells * config.obstacleDensity);
    
    std::cout << "Grid size: " << config.width << "x" << config.height 
              << " = " << totalCells << " cells" << std::endl;
    std::cout << "Obstacle density: " << config.obstacleDensity 
              << " -> " << numObstacles << " obstacles" << std::endl;
    
    for (int i = 0; i < numObstacles; i++) {
        int x = rand() % config.width;
        int y = rand() % config.height;
        world.setObstacle(x, y, true);
    }
    
    // 2. 验证障碍物生成
    int obstacleCount = 0;
    for (int x = 0; x < config.height; x++) {
        for (int y = 0; y < config.width; y++) {
            if (world.isObstacle(x, y)) obstacleCount++;
        }
    }
    std::cout << "Actual obstacles created: " << obstacleCount << std::endl;
    
    // 3. 生成 walkers
    generateWalkers();

    // 4. 显示初始覆盖率
    std::cout << "Initial coverage after marking starts: " << world.getCoverage() << std::endl;

        
    } 
    void initializeNoObstacles() {
        walkers.clear();
        
        std::cout << "Initializing with NO obstacles..." << std::endl;
        
        // 确保没有障碍物（可以重置所有障碍物）
        for (int x = 0; x < config.height; x++) {
            for (int y = 0; y < config.width; y++) {
                if (world.isObstacle(x, y)) {
                    world.setObstacle(x, y, false);
                }
            }
        }
        
        // 使用独立的函数生成 walkers
        generateWalkers();
        
        std::cout << "Initial coverage: " << world.getCoverage() << std::endl;
    }
    void loadObstaclesFromFile(const std::string& filename){
        walkers.clear();
        
        std::cout << "Loading obstacles from file: " << filename << std::endl;
        
        // TODO: 实现从文件加载障碍物的逻辑
        // loadObstaclesFromFile(filename);
        
        // 使用独立的函数生成 walkers
        generateWalkers();
        
        std::cout << "Initial coverage: " << world.getCoverage() << std::endl;
    };


    // 模拟步骤 - 修复2：完善这个函数的实现
    void simulateStep() {
    std::cout << "\n=== Step " << currentStep << " ===" << std::endl;
    std::cout << "Current coverage before step: " << currentCoverage << std::endl;
    
    int movedCount = 0;
    int markedCount = 0;
    
    for (auto& walker : walkers) {
        int oldX = walker.getX();
        int oldY = walker.getY();

        int dx = 0;
        int dy = 0;
        
        std::cout << "Walker at (" << oldX << "," << oldY << ")" << std::endl;
        
        switch (config.moveRule) {
            case SimulationConfig::RANDOM:
            {
                // 等概率四个方向
                int dir = rand() % 4; //0到3
                dx = (dir == 0) ? 1 : (dir == 1) ? -1 : 0; 
                dy = (dir == 2) ? 1 : (dir == 3) ? -1 : 0;
            }
                break;
            case SimulationConfig::BIASED:
            {
                //某个方向概率更高
                int r = rand() % 100;
                if (r < 40) { // 40% 概率向右
                    dx = 1; dy = 0;
                } else if (r < 60) { // 20% 概率向左
                    dx = -1; dy = 0;
                } else if (r < 80) { // 20% 概率向下
                    dx = 0; dy = 1;
                } else { // 20% 概率向上
                    dx = 0; dy = -1;
                }
            }
                break;
            case SimulationConfig::AVOID_OBSTACLES:
            {
                //避障模式
                // 尝试四个方向，选择第一个不是障碍物的方向
                int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
                for (int d = 0; d < 4; d++) {
                    int testX = walker.getX() + directions[d][0];
                    int testY = walker.getY() + directions[d][1];
                    if (world.isValidPosition(testX, testY)) {
                        dx = directions[d][0];
                        dy = directions[d][1];
                        break;
                    }
                }
            }
        }
        
        int newX = oldX + dx;
        int newY = oldY + dy;
        
        std::cout << "  Trying to move to (" << newX << "," << newY << ")" << std::endl;
        
        // 检查 isValidPosition
        bool valid = world.isValidPosition(newX, newY); //是否在Gridmap范围内行走
        std::cout << "  isValidPosition: " << (valid ? "true" : "false") << std::endl;
        
        if (valid) {
            // 检查是否是障碍物
            bool obstacle = world.isObstacle(newX, newY);
            std::cout << "  isObstacle: " << (obstacle ? "true" : "false") << std::endl;
            
            
            if (!obstacle) {
                // 尝试移动
                walker.move(dx, dy, world);
                movedCount++;
                
                // 检查移动后位置
                std::cout << "  Moved to (" << walker.getX() << "," << walker.getY() << ")" << std::endl;
                
                // 标记
                world.markVisited(newX, newY);
                markedCount++;
                
                // 验证标记是否成功
                // 这里需要添加一个方法检查 visited 状态
                // std::cout << "  Marked as visited" << std::endl;
            }
        } else {
            // 碰撞处理

        switch (config.collisionRule) {
            case SimulationConfig::STOP:
                // 不动，保持原位
                break;
                
            case SimulationConfig::BOUNCE:
            {
                // 尝试相反方向
                int bounceX = walker.getX() - dx;
                int bounceY = walker.getY() - dy;
                if (world.isValidPosition(bounceX, bounceY) && !world.isObstacle(bounceX, bounceY)) {
                    walker.move(-dx, -dy, world);
                    world.markVisited(bounceX, bounceY);
                }
                break;
            }
            
            case SimulationConfig::IGNORE:
                // 忽略碰撞，强行移动到新位置（即使无效或障碍物）
                walker.move(dx, dy, world);
                // 注意：这里不标记为已访问，因为位置可能无效或是障碍物
                // 如果也想标记，可以取消下面的注释
                // world.markVisited(newX, newY);
                break;
            }
        }
    }
    
    currentStep++;
    currentCoverage = world.getCoverage();
    
    std::cout << "Moved: " << movedCount << "/" << walkers.size() << std::endl;
    std::cout << "Marked: " << markedCount << "/" << walkers.size() << std::endl;
    std::cout << "Coverage after step: " << currentCoverage << std::endl;
}
    
    void simulateFullRun();
    
    // 运动规则（未做）
    void applyMoveRule(Walker& walker);
    bool checkCollision(const Walker& walker, int dx, int dy);
    
    // 结果处理 - 修复3：去掉类名作用域
    bool isComplete() const {
        return (currentCoverage >= config.targetCoverage) || 
               (currentStep >= config.maxSteps);
    }

    
    SimulationResult getResults() {
        SimulationResult result;
        result.stepsToComplete = currentStep;
        result.finalCoverage = currentCoverage;
        return result;
     }

    void exportToCSV(const std::string& filename); //输出成excel表格形式（可选）
};

#endif