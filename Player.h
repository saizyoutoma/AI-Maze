#pragma once
#include "./Library/GameObject.h"
#include "./globals.h"

class Player :
    public GameObject
{
    int playerImage_;
    Point pos_;
    Rect playerRect;
public:
	Player();
    ~Player();
    void Update() override;
    void Draw() override;
	bool CheckHit(const Rect& me,const Rect& other);
    Rect GetRect() { return  playerRect; }
    Point GetPos() { return pos_; }
};

