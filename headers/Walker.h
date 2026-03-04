
#ifndef WALKER_H
#define WALKER_H

#include <iostream>
#include "GridWorld.h"
#include "Config.h"

class Walker {
private:
    int x, y;  // 当前格子坐标
    int startX, startY;
    
public:
    Walker(int start_x, int start_y): x(start_x), y(start_y), startX(start_x), startY(start_y) {}; //初始化四个参数
    void move(int dx, int dy, const GridWorld& world){ 
        int newX = x + dx;
        int newY = y + dy;
    
        if (world.isValidPosition(newX, newY) && !world.isObstacle(newX, newY)) {
            x = newX;
            y = newY;
        }
        // 如果位置无效，保持原地不动
    };
    void resetToStart(){
        x = startX;
        y = startY;
    };
    int getX(){return x;};
    int getY(){return y;};
};

#endif