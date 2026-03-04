#include <iostream>
#include "MonteCarloSimulator.h"
#include "ExperimentRunner.h"


int main() {
    // 设置参数
    SimulationConfig config;
    config.width = 11;
    config.height = 11;
    config.cellSize = 1.0;
    config.obstacleMode = SimulationConfig::ObstacleMode::RANDOM_DENSITY;
    config.obstacleDensity = 0.1;  // 10%障碍物
    config.numWalkers = 1;
    config.targetCoverage = 0.8;    // 目标覆盖率80%
    config.maxSteps = 10;
    config.moveRule = SimulationConfig::MoveRule::RANDOM;
    config.collisionRule = SimulationConfig::CollisionRule::IGNORE;
    
    // 选择模式
    std::cout << "选择模式: 1-交互模式, 2-批处理实验: ";
    int mode;
    std::cin >> mode;
    
    if (mode == 1) {
        // 交互模式
        MonteCarloSimulator simulator(config);
        simulator.initialize();
        
        /*
        // 检查初始状态
        std::cout << "Initial walkers: " << simulator.getCurrentStep() << std::endl;
        std::cout << "Initial coverage: " << simulator.getCurrentCoverage() << std::endl;
        
        // 如果初始覆盖率为0，说明起点没被标记
        if (simulator.getCurrentCoverage() == 0.0) {
            std::cout << "ERROR: Initial coverage is 0! Start points not marked." << std::endl;
        }
        
        // 运行前几步并输出
        for (int i = 0; i < 10; i++) {
            simulator.simulateStep();
            std::cout << "Step " << i << " coverage: " 
                    << simulator.getCurrentCoverage() << std::endl;
        }
        */
        
        // 然后继续正常运行
        while (!simulator.isComplete()) {
            simulator.simulateStep();
        }
        
        return 0;
        
    } else {
        // 批处理模式
        ExperimentRunner runner(config);
        runner.runExperiments(30);  // 运行30次实验
        
        auto stats = runner.computeStatistics();
        std::cout << "\n30次实验结果统计:\n";
        std::cout << "平均步数: " << stats.meanSteps 
                  << " (标准差: " << stats.stdSteps << ")\n";
        std::cout << "平均覆盖率: " << stats.meanCoverage * 100 << "%"
                  << " (标准差: " << stats.stdCoverage * 100 << "%)\n";
        
        /*
        runner.exportSummary("experiment_results.csv");
        std::cout << "结果已保存到 experiment_results.csv\n";
        */
    }
    
    return 0;
}