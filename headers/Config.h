#include <iostream>
#include <vector>

#ifndef CONFIG_H
#define CONFIG_H

struct SimulationConfig {
    // 网格参数
    int width;
    int height;
    double cellSize;
    
    // 障碍物模式
    enum class ObstacleMode { NONE, RANDOM_DENSITY, LOAD_FROM_FILE };
    ObstacleMode obstacleMode;
    double obstacleDensity;  // 用于随机生成
    
    // 模拟参数
    int numWalkers;
    double targetCoverage;
    int maxSteps;
    int stepsPerFrame;
    unsigned int seed; //unsigned int代表只能存储非负整数
    
    // 运动规则
    enum MoveRule { RANDOM, BIASED, AVOID_OBSTACLES };
    MoveRule moveRule;
    
    // 碰撞处理规则
    enum CollisionRule { IGNORE, STOP, BOUNCE };
    CollisionRule collisionRule;
    
    // 实验参数
    int numExperiments;  // 批处理时的实验次数
};

struct SimulationResult {
    //统计完成所需步数和最后覆盖率
    int stepsToComplete;
    double finalCoverage;
    std::vector<int> coverageHistory;  // 可选
};

struct Statistics {
    double meanSteps;
    double stdSteps;
    double meanCoverage;
    double stdCoverage;
    // 分位数等
};

#endif // CONFIG_H