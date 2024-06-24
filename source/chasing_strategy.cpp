//追逐策略函数实现
#include "game.h"

//最基本的追逐策略，直接追逐玩家
// 获取玩家 Pacman 的位置。
// 计算鬼魂与玩家之间的水平和垂直距离。
// 返回这两个距离作为目标方向。
QPair<int, int> strategy1(Ghost* ghost) {
    Pacman *pacman = ghost->game->pacman;
    int dist_x = pacman->get_x() - ghost->get_x();
    int dist_y = pacman->get_y() - ghost->get_y();
    return QPair<int, int>(dist_x, dist_y);
}


//基于玩家的当前位置和移动方向进行预测
//获取玩家 Pacman 的位置和移动方向。
//计算鬼魂与玩家之间的距离。
//根据玩家的移动方向，进一步预测玩家未来的位置（前方4个单位）。
//返回鬼魂到预测位置的距离
QPair<int, int> strategy2(Ghost* ghost) {
    Pacman *pacman = ghost->game->pacman;
    int dist_x = pacman->get_x() - ghost->get_x();
    int dist_y = pacman->get_y() - ghost->get_y();
    switch(pacman->get_dir()) {
    case GameObject::Up:
        dist_y-=4;
        break;
    case GameObject::Down:
        dist_y+=4;
        break;
    case GameObject::Left:
        dist_x-=4;
        break;
    case GameObject::Right:
        dist_x+=4;
        break;
    default:
        break;
    }
    return QPair<int, int>(dist_x, dist_y);
}


//结合了红色鬼魂和玩家的位置进行追逐
//获取玩家 Pacman 和红色鬼魂的位置信息。
//计算玩家相对于红色鬼魂的位置，并将其放大两倍，作为目标位置
//返回当前鬼魂到这个目标位置的距离
QPair<int, int> strategy3(Ghost* ghost) {
    Pacman *pacman = ghost->game->pacman;
    Ghost *red_ghost = ghost->game->ghost[Ghost::Red];
    int dist_x = (pacman->get_x() - red_ghost->get_x()) * 2 - ghost->get_x();
    int dist_y = (pacman->get_y() - red_ghost->get_y()) * 2 - ghost->get_y();
    return QPair<int, int>(dist_x, dist_y);
}


//根据鬼魂与玩家之间的距离决定追逐还是逃跑
//如果鬼魂与玩家的距离小于 64（即接近玩家），鬼魂将目标设置为地图的一个固定位置 (2, 2)，表示逃跑。
QPair<int, int> strategy4(Ghost* ghost) {
    Pacman *pacman = ghost->game->pacman;
    int dist_x = pacman->get_x() - ghost->get_x();
    int dist_y = pacman->get_y() - ghost->get_y();
    if (dist_x * dist_x + dist_y * dist_y < 64) {
        dist_x = 2 - ghost->get_x();
        dist_y = 2 - ghost->get_y();
    }
    return QPair<int, int>(dist_x, dist_y);
}
