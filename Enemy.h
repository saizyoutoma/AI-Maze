#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"
#include <vector>

class Enemy :
    public GameObject
{
    Point pos_;
    bool isAlive_;
    float speed_;
    Point nextPos_;
    DIR forward_;
    bool isRandom_;
    bool isRight_;
    bool isDijkstra_;
    std::vector<Point> route_;
    /*Point move_;
    bool moveNow_;
    Point dis;*/
public:
    Enemy();
    ~Enemy();

    void Update() override;
    void Draw() override;
    bool CheckHit(const Rect& me, const Rect& other);
    bool HitToChip(int x, int y);
    void XCloserMove();  //Xで近づく
    void YCloserMove();  //Yで近づく
    void XYCloserMove(); //XYで近づく
    void XYCloserMoveRandom(); //XYで近づきつつランダムもある
    void RightHandMove(); //右手法
    void LeftHandMove();  //左手法
    void Move(Point p); //単なる移動
    void SetDijkstra(bool useDijkstra);
};

