
#ifndef EXPERIMENTRUNNER_H
#define EXPERIMENTRUNNER_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>  // 需要这个 for time()
#include <cmath>
#include "GridWorld.h"
#include "Config.h"
#include "MonteCarloSimulator.h"

class ExperimentRunner {
private:
    SimulationConfig baseConfig;
    std::vector<SimulationResult> allResults;
    
public:
    // 构造函数
    ExperimentRunner(const SimulationConfig& cfg) : baseConfig(cfg) {
        // 可以在这里做一些初始化
    }
    
    // 运行多次实验
    void runExperiments(int numRuns) {
        // 初始化 allResults
        allResults.clear();
        
        for (int exp = 0; exp < numRuns; exp++) {
            // 每次实验使用不同的随机种子
            SimulationConfig expConfig = baseConfig;
            expConfig.seed = static_cast<unsigned int>(time(nullptr) + exp);
            
            MonteCarloSimulator simulator(expConfig);
            simulator.initialize();
            
            // 运行直到完成
            while (!simulator.isComplete()) {
                simulator.simulateStep();
            }
            
            // 保存结果
            allResults.push_back(simulator.getResults());
            
            // 可选：导出单次运行的详细数据
            /*
            if (baseConfig.exportIndividualRuns) {
                std::string filename = "run_" + std::to_string(exp) + ".csv";
                simulator.exportToCSV(filename);
            }
            */
        }  // for 循环结束
    }  // runExperiments 函数结束
    
    // 计算统计信息
    Statistics computeStatistics() {
        Statistics stats;
        
        if (allResults.empty()) {
            stats.meanSteps = 0;
            stats.stdSteps = 0;
            stats.meanCoverage = 0;
            stats.stdCoverage = 0;
            return stats;
        }
        
        // 计算平均值
        double sumSteps = 0;
        double sumCoverage = 0;
        
        for (const auto& result : allResults) {
            sumSteps += result.stepsToComplete;
            sumCoverage += result.finalCoverage;
        }
        
        stats.meanSteps = sumSteps / allResults.size();
        stats.meanCoverage = sumCoverage / allResults.size();
        
        // 计算标准差
        double sumSqSteps = 0;
        double sumSqCoverage = 0;
        
        for (const auto& result : allResults) {
            sumSqSteps += (result.stepsToComplete - stats.meanSteps) * 
                          (result.stepsToComplete - stats.meanSteps);
            sumSqCoverage += (result.finalCoverage - stats.meanCoverage) * 
                             (result.finalCoverage - stats.meanCoverage);
        }
        
        stats.stdSteps = sqrt(sumSqSteps / allResults.size());
        stats.stdCoverage = sqrt(sumSqCoverage / allResults.size());
        
        return stats;
    }
    
    // 导出汇总结果到文件
    void exportSummary(const std::string& filename) {
        // 这里实现导出功能
        std::cout << "Exporting summary to " << filename << std::endl;
        // TODO: 实现文件写入逻辑
    }
    
    // 参数扫描功能
    void runParameterSweep(const std::string& paramName, 
                           const std::vector<double>& values) {
        std::cout << "Running parameter sweep for " << paramName << std::endl;
        
        for (double value : values) {
            std::cout << "Testing value: " << value << std::endl;
            
            // 修改参数
            SimulationConfig sweepConfig = baseConfig;
            
            // 根据参数名修改对应的参数
            if (paramName == "obstacleDensity") {
                sweepConfig.obstacleDensity = value;
            } else if (paramName == "numWalkers") {
                sweepConfig.numWalkers = static_cast<int>(value);
            } else if (paramName == "targetCoverage") {
                sweepConfig.targetCoverage = value;
            }
            // 添加更多参数...
            
            // 运行实验
            ExperimentRunner runner(sweepConfig);
            runner.runExperiments(5);  // 每个参数值运行5次实验
            
            // 处理结果...
        }
    }
    
    // 获取结果
    const std::vector<SimulationResult>& getResults() const {
        return allResults;
    }
};

// 如果需要，可以在类外实现统计函数
// Statistics ExperimentRunner::computeStatistics() {
//     // 实现...
// }

#endif