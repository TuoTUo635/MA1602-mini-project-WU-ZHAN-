#include <iostream>
#include "MonteCarloSimulator.h"
#include "ExperimentRunner.h"


int main() {
    // 设置参数
    SimulationConfig config;
    config.width = 10;
    config.height = 10;
    config.cellSize = 1.0;
    config.obstacleMode = SimulationConfig::ObstacleMode::NONE;
    config.obstacleDensity = 0.1;  // 10%障碍物
    config.numWalkers = 1;
    config.targetCoverage = 0.8;    // 目标覆盖率80%
    config.maxSteps = 100;
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
        
        std::cout << "开始模拟...\n";
        
        while (!simulator.isComplete()) {
            simulator.simulateStep();
            
            // 每100步输出一次状态
            if (simulator.getCurrentStep() % 100 == 0) {
                std::cout << "Step: " << simulator.getCurrentStep() 
                          << ", Coverage: " << simulator.getCurrentCoverage() * 100 << "%\n";
            }
        }
        
        auto result = simulator.getResults();
        std::cout << "\n模拟完成!\n";
        std::cout << "总步数: " << result.stepsToComplete << "\n";
        std::cout << "最终覆盖率: " << result.finalCoverage * 100 << "%\n";
        
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