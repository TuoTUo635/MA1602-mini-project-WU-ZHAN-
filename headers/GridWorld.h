#ifndef GRIDWORLD_H
#define GRIDWORLD_H

#include <iostream>
#include <vector>

class GridWorld {
private:
    int width, height;
    double cellSize;
    std::vector<std::vector<bool>> obstacles;  // true表示障碍物
    std::vector<std::vector<bool>> visited;    // 记录访问过的格子
    std::vector<std::vector<int>> visitCount;  // 可选：记录访问次数
    
public:
    // 修复1：构造函数不能有返回值类型
    GridWorld(int w, int h, double cell_size) {
        width = w;
        height = h;
        cellSize = cell_size;
        obstacles.resize(height, std::vector<bool>(width, false));
        visited.resize(height, std::vector<bool>(width, false));
        visitCount.resize(height, std::vector<int>(width, 0));
    }
    
    // 修复2：去掉 GridWorld:: 作用域
    void setObstacle(int x, int y, bool isObstacle) {
        if (x >= 0 && x < height && y >= 0 && y < width) {  // 添加边界检查
            obstacles[x][y] = isObstacle;
        }
    }

    // 这里统一标准是 x row and y col
    bool isObstacle(int x, int y) const {
        if (x >= 0 && x < height && y >= 0 && y < width) {
            return obstacles[x][y];
        }
        return true;  // 超出边界视为障碍物
    }
    
    // 修复3：正确调用 isObstacle 函数
    bool isValidPosition(int x, int y) const {
        if (x >= 0 && x < height && y >= 0 && y < width) {
            return !isObstacle(x, y);  // 这里只检查是否是障碍物
            // 注意：visited 应该在 walker 移动时检查，不是在这里
        }
        return false;
    }
    
    void resetVisited() {
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                visited[x][y] = false;
                visitCount[x][y] = 0;
            }
        }
    }
    
    void markVisited(int x, int y) {
        if (x >= 0 && x < height && y >= 0 && y < width && !obstacles[x][y]) {
            visited[x][y] = true;
            visitCount[x][y]++;
        }
    }
    
    // 修复4：添加返回值
    double getCoverage() const {
        int totalValidCells = 0;
        int visitedCells = 0;
    
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                if (!obstacles[x][y]) {  // 只考虑非障碍物格子
                    totalValidCells++;
                    if (visited[x][y]) {
                        visitedCells++;
                    }
                }
            }
        }
        
        if (totalValidCells == 0) return 0.0;  // 避免除以0
        return static_cast<double>(visitedCells) / totalValidCells;  // 返回覆盖率
    }
};

#endif