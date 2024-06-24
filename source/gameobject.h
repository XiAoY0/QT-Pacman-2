#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <QVector>

//前置声明Game类 避免头文件之间的循环依赖
class Game;
//ghost 和 pacman的基类
class GameObject : public QGraphicsPixmapItem
{
public:
    //枚举不同游戏对象类型
    enum ObjectType {Ball ,Wall ,Gate ,Pacman ,Ghost ,Blank ,PowerBall};

    //枚举移动方向
    enum Dir{Up =0,Down =1,Left =2,Right =3,Stop = 4};//注意有stop状态

    //游戏对象宽度 单位：像素
    static const int Width = 20;

    //构造函数 包括对象类型和图像类的初始化
    GameObject(ObjectType, QPixmap);

    ~GameObject();

    //获取对象类型
    ObjectType get_type();

    //获取对象的x,y坐标
    int get_x();
    int get_y();

    //获取对象分数
    int get_score();

    //设置分数
    void set_score(int);

    //设置方向
    void set_dir(Dir);

    //设置下一个移动方向
    void set_next_dir(Dir);

    //获取当前移动方向和下一个移动方向
    Dir get_dir();
    Dir get_next_dir();

    //友元声明
    friend class Game;
    friend class Ghost;
protected:
    int _x, _y;                 // 对象在地图中坐标
    Dir dir;                    // 当前移动方向
    Dir next_dir;               // 下一个移动方向
    ObjectType type;            //对象类型
    int score;                  //分数

};

//pacman类的声明
class Pacman:public GameObject
{
public:
    Pacman();
    void move();//移动函数
    Game *game;

    friend class Game;
private:
    void moveup();
    void movedown();//移动函数
    void moveleft();
    void moveright();
    void eat_ball(int, int);//吃掉豆子后的处理函数
    bool overlapable(int, int); // 检查是否可到指定位置
    QVector<QPixmap> anim[4];   // pacman的动画帧
    int anim_index;//帧索引
};

class Ghost : public GameObject
{
public:
    enum Color {Red = 0, Yellow = 1, Pink = 2, Green = 3};//枚举类型，表示幽灵的颜色
    enum Status {Normal, Panic, Running};//枚举类型，表示幽灵的状态
    const static int GhostNum = 4;//幽灵总数
    Game *game;                 // 指向游戏对象的指针 用来访问游戏状态和地图

    Ghost(int);//初始化幽灵的构造函数
    void move();//移动函数
    Color get_color();//获取幽灵颜色

    //声明友元类
    friend class Game;
    friend class Pacman;

private:
    //移动函数
    void moveup();
    void movedown();
    void moveleft();
    void moveright();

    //设置随机移动方向 用来对幽灵行为模式编辑
    void setdir_randomly();

    //幽灵出笼函数
    void go_out_cage();

    //追击函数
    void chase_pacman();

    //躲避函数
    void dodge_pacman();

    //受惊状态 回笼函数
    void go_to_cage();

    //指向追逐策略的指针
    QPair<int, int> (*chase_strategy)(Ghost*);

    //检查是否可以移动到指定位置
    bool overlapable(int, int);

    //幽灵颜色 状态 动画
    Color color;
    Status status;
    QVector<QPixmap> anim[4];
    QVector<QPixmap> panic_anim;//panic状态 动画帧储存
    QVector<QPixmap> running_anim;//running 状态 动画帧储存

    //帧索引
    int anim_index;
    int release_time; //出笼时间
    bool is_released;//判断出笼
    int panic_time;//panic持续时间
};

//四个追逐策略函数
QPair<int, int> strategy1(Ghost*);
QPair<int, int> strategy2(Ghost*);
QPair<int, int> strategy3(Ghost*);
QPair<int, int> strategy4(Ghost*);
#endif // GAMEOBJECT_H
