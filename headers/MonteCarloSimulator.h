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
    MonteCarloSimulator(const SimulationConfig& cfg): config(cfg), 
      world(cfg.width, cfg.height, cfg.cellSize),
      currentStep(0),
      currentCoverage(0.0) {
        if (config.seed == 0) { // 这里生成种子是为了后面随机move做准备
            config.seed = static_cast<unsigned int>(time(nullptr));
        }
    }
    
    int getCurrentStep() { return currentStep; }
    double getCurrentCoverage() { return currentCoverage; }
    
    // 初始化 - 修复1：添加缺失的分号
    void initialize() {
        switch (config.obstacleMode) {
        case SimulationConfig::ObstacleMode::RANDOM_DENSITY:
            generateObstaclesByDensity();
            break;
        case SimulationConfig::ObstacleMode::LOAD_FROM_FILE:
            // loadObstaclesFromFile(config.mapFile); (可选)
            break;
        
        case SimulationConfig::ObstacleMode::NONE:
            break;
        }
    }  // 这里加上了分号
    
    void generateObstaclesByDensity() {
        walkers.clear();
        
        for (int i = 0; i < config.numWalkers; i++) {
            // 随机选择有效起始位置（非障碍物）
            int startX, startY;
            do {
                startX = rand() % config.width;
                startY = rand() % config.height;
            } while (world.isObstacle(startX, startY)); //先生成一次再看位置是否有障碍，如果有就继续生成直到没有障碍为止
            
            walkers.emplace_back(startX, startY); //与push_back类似，把这个vector直接放到末尾
            world.markVisited(startX, startY);
        }
    }  // 这里加上了分号
    
    void loadObstaclesFromFile(const std::string& filename);
    void generateWalkers();
    
    // 模拟步骤 - 修复2：完善这个函数的实现
    void simulateStep() {
        for (auto& walker : walkers) {
            // 1. 确定移动方向（四个方向随机）
            int dx = 0, dy = 0;
            
            switch (config.moveRule) {
                case SimulationConfig::RANDOM:
                    {
                        // 等概率四个方向
                        int dir = rand() % 4; //0到3
                        // 运算三元符，因为dir只能是0，1，2，3，可以对应左右上下
                        dx = (dir == 0) ? 1 : (dir == 1) ? -1 : 0; 
                        dy = (dir == 2) ? 1 : (dir == 3) ? -1 : 0;
                    }
                    break;
                case SimulationConfig::BIASED:
                    // 某个方向概率更高
                    break;
                case SimulationConfig::AVOID_OBSTACLES:
                    // 避障模式
                    // TODO: 实现避障规则
                    break;
            }
            
            // 2. 检查新位置是否有效
            int newX = walker.getX() + dx;
            int newY = walker.getY() + dy;
            
            if (world.isValidPosition(newX, newY) && !world.isObstacle(newX, newY)) {
                // 可以移动
                walker.move(dx, dy, world);
                world.markVisited(newX, newY);
            } else {
                // 碰撞处理
                switch (config.collisionRule) {
                    case SimulationConfig::STOP:
                        // 不动，保持原位
                        break;
                    case SimulationConfig::BOUNCE:
                        // 尝试相反方向
                        // TODO: 实现反弹
                        break;
                    case SimulationConfig::IGNORE:
                        // 忽略碰撞，尝试移动
                        break;
                }
            }
        }

    static int stepCount = 0;
    stepCount++;
    
    if (stepCount % 100 == 0) {  // 每100步输出一次
        std::cout << "Step " << currentStep 
                  << ", Coverage: " << currentCoverage 
                  << ", Walkers: " << walkers.size() 
                  << std::endl;
        
        // 输出第一个walker的位置
        if (!walkers.empty()) {
            std::cout << "  Walker0 at (" << walkers[0].getX() 
                      << "," << walkers[0].getY() << ")" << std::endl;
        }
    }
        
        // 更新步数和覆盖率
        currentStep++;
        currentCoverage = world.getCoverage();
    }  // 修复：添加了闭合大括号和分号
    
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