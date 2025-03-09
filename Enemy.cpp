#include "Enemy.h"
#include "./Stage.h"
#include "globals.h"
#include "ImGui/imgui.h"
#include "Player.h"
#include "time.h"
#include "ImGui/imgui.h"

namespace
{
	Point nDir[4] = { {0,-1},{0,1},{-1,0},{1,0} };
}

Enemy::Enemy()
    :pos_({ 0,0 }), isAlive_(true),nextPos_({0,0}),isRandom_(true),isRight_(true),
	 isDijkstra_(true)
{
   /* int rx = GetRand(STAGE_WIDTH * CHA_WIDTH);
    int ry = GetRand(STAGE_HEIGHT * CHA_HEIGHT);*/
	/*int rx = GetRand(STAGE_WIDTH - 2) * CHA_WIDTH;
	int ry = GetRand(STAGE_HEIGHT - 2) * CHA_HEIGHT;
    pos_ = { rx, ry };*/
	SRand((unsigned)time(NULL));
	int rx = 0;
	int ry = 0;
	while (rx % 2 == 0 || ry % 2 == 0)
	{
		rx = GetRand(STAGE_WIDTH - 1);
		ry = GetRand(STAGE_HEIGHT - 1);
	}
	pos_ = { rx * CHA_WIDTH, ry * CHA_HEIGHT };
	forward_ = RIGHT;
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
	Player* player = (Player*)FindGameObject<Player>();
	Point pPos = player->GetPos();
	Stage* stage = (Stage*)FindGameObject<Stage>();
	static bool stop = false;
\
	if (!stop) {
		int ex = pos_.x / CHA_WIDTH;
		int ey = pos_.y / CHA_HEIGHT;

		int px = pPos.x / CHA_WIDTH;
		int py = pPos.y / CHA_HEIGHT;

			if (isDijkstra_) {
				//ダイクストラ
				stage->Dijkstra({ ex, ey });
				route_ = stage->restore(px, py);
			}

			if (isDijkstra_==false) {
				//幅優先探索
				stage->BFS({ ex,ey }, { px,py });
				route_ = stage->restore({ ex,ey }, { px,py }); // 経路の最短ルートを取得
			}

		// プレイヤーの位置に基づいて経路を復元し、route_ に保存
		route_ = stage->restore({ex,ey}, {px,py}); // 経路の最短ルートを取得
		//route_ = stage->restore(px,py);
		stop = true;
	}

	//printfDx("%d,%d", route_[0].x, route_[0].y);

	if (!route_.empty()) {
		// 最初のターゲット地点に向かって移動
		Point nextTarget = route_.front();

		// 目標地点に向かって敵を移動させる
		Move(nextTarget);

		// 目的地に到達したら次のターゲットへ移動
		if (pos_.x == nextTarget.x * CHA_WIDTH && pos_.y == nextTarget.y * CHA_HEIGHT) {
			route_.erase(route_.begin()); // 目的地に到達したので、次のターゲットをセット
		}
	}
	else {
		stop = false;
	}
}

void Enemy::Draw()
{
    DrawBox(pos_.x, pos_.y, pos_.x + CHA_WIDTH, pos_.y + CHA_HEIGHT, GetColor(80, 89, 10), TRUE);
	Point tp[4][3] = {
		{{pos_.x + CHA_WIDTH / 2,pos_.y},             {pos_.x,pos_.y + CHA_HEIGHT / 2},{pos_.x + CHA_WIDTH,pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x + CHA_WIDTH / 2,pos_.y + CHA_HEIGHT},{pos_.x,pos_.y + CHA_HEIGHT / 2},{pos_.x + CHA_WIDTH,pos_.y + CHA_HEIGHT / 2}},
		{{pos_.x,pos_.y + CHA_HEIGHT / 2},            {pos_.x + CHA_WIDTH / 2,pos_.y}, {pos_.x + CHA_WIDTH / 2,pos_.y + CHA_HEIGHT}},
		{{pos_.x + CHA_WIDTH,pos_.y + CHA_HEIGHT / 2},{pos_.x + CHA_WIDTH / 2,pos_.y}, {pos_.x + CHA_WIDTH / 2,pos_.y + CHA_HEIGHT}}
	};
	DrawTriangle(tp[forward_][0].x, tp[forward_][0].y,
		         tp[forward_][1].x, tp[forward_][1].y,
		         tp[forward_][2].x, tp[forward_][2].y,
		         GetColor(255, 255, 255), TRUE);
	int Cr = GetColor(173, 255, 47);

	for (auto itr : route_)
	{
		DrawBox((int)itr.x * CHA_WIDTH, (int)itr.y * CHA_HEIGHT,
			(int)itr.x * CHA_WIDTH + CHA_WIDTH + 1,
			(int)itr.y * CHA_HEIGHT + CHA_HEIGHT + 1, Cr, false);
	}
}

bool Enemy::CheckHit(const Rect& me, const Rect& other)
{
	if (me.x < other.x + other.w &&
		me.x + me.w > other.x &&
		me.y < other.y + other.h &&
		me.y + me.h > other.y)
	{
		return true;
	}
	return false;
}

bool Enemy::HitToChip(int x, int y)
{
	Rect me = { x,y,CHA_WIDTH,CHA_HEIGHT };
	Stage* stage = (Stage*)FindGameObject<Stage>();
	for (auto& obj : stage->GetStageRects())
	{
		if (CheckHit(me, obj)) {
			return true;
		}
	}
	return false;
}

void Enemy::XCloserMove()
{
	Player* player = (Player*)FindGameObject<Player>();
	if (pos_.x > player->GetPos().x)
	{
		forward_ = LEFT;
	}
	else if (pos_.x < player->GetPos().x)
	{
		forward_ = RIGHT;
	}
}

void Enemy::YCloserMove()
{
	Player* player = (Player*)FindGameObject<Player>();
	if (pos_.y > player->GetPos().y)
	{
		forward_ = UP;
	}
	else if (pos_.y < player->GetPos().y)
	{
		forward_ = DOWN;
	}
}

void Enemy::XYCloserMove()
{
	Player* player = (Player*)FindGameObject<Player>();
	int xdis = abs(pos_.x - player->GetPos().x);
	int ydis = abs(pos_.y - player->GetPos().y);

	if (xdis > ydis) {
		if (pos_.x > player->GetPos().x)
		{
			forward_ = LEFT;
		}
		else if (pos_.x < player->GetPos().x)
		{
			forward_ = RIGHT;
		}
	}
	else
	{
		if (pos_.y > player->GetPos().y)
		{
			forward_ = UP;
		}
		else if (pos_.y < player->GetPos().y)
		{
			forward_ = DOWN;
		}
	}
}

void Enemy::XYCloserMoveRandom()
{
	Player* player = (Player*)FindGameObject<Player>();
	int rnum = GetRand(2);
	if (rnum == 0)
	{
		XYCloserMove();
	}
	else if (rnum == 1)
	{
		forward_ = (DIR)GetRand(3);
	}
}

void Enemy::RightHandMove()
{
	DIR myRight[4] = { RIGHT,LEFT,UP,DOWN };
	DIR myLeft[4] = { LEFT,RIGHT,DOWN,UP };
	Point nposF = { pos_.x + nDir[forward_].x,pos_.y + nDir[forward_].y };
	Point nposR = { pos_.x + nDir[myRight[forward_]].x,pos_.y + nDir[myRight[forward_]].y };
	Rect myRectF{ nposF.x,nposF.y,CHA_WIDTH,CHA_HEIGHT };
	Rect myRectR{ nposR.x,nposR.y,CHA_WIDTH,CHA_HEIGHT };
	Stage* stage = (Stage*)FindGameObject<Stage>();
	bool isRightOpen = true;
	bool isForwardOpen = true;
	for (auto& obj : stage->GetStageRects()) {
		if (CheckHit(myRectF, obj)) {
			isForwardOpen = false;
		}
		if (CheckHit(myRectR, obj)) {
			isRightOpen = false;
		}
	}

	if (isRightOpen) {
		forward_ = myRight[forward_];
	}
	else if (isRightOpen == false && isForwardOpen == false) {
		forward_ = myLeft[forward_];
	}
}

void Enemy::LeftHandMove()
{
	DIR myRight[4] = { LEFT,RIGHT,DOWN,UP };
	DIR myLeft[4] = { RIGHT, LEFT, UP, DOWN };
	Point nposF = { pos_.x + nDir[forward_].x,pos_.y + nDir[forward_].y };
	Point nposR = { pos_.x + nDir[myRight[forward_]].x,pos_.y + nDir[myRight[forward_]].y };
	Rect myRectF{ nposF.x,nposF.y,CHA_WIDTH,CHA_HEIGHT };
	Rect myRectR{ nposR.x,nposR.y,CHA_WIDTH,CHA_HEIGHT };
	Stage* stage = (Stage*)FindGameObject<Stage>();
	bool isRightOpen = true;
	bool isForwardOpen = true;
	for (auto& obj : stage->GetStageRects()) {
		if (CheckHit(myRectF, obj)) {
			isForwardOpen = false;
		}
		if (CheckHit(myRectR, obj)) {
			isRightOpen = false;
		}
	}

	if (isRightOpen) {
		forward_ = myRight[forward_];
	}
	else if (isRightOpen == false && isForwardOpen == false) {
		forward_ = myLeft[forward_];
	}
}

void Enemy::Move(Point p)
{
	int dx = p.x * CHA_WIDTH;
	int dy = p.y * CHA_HEIGHT;

	// X軸とY軸にそれぞれ移動量を計算
	if (pos_.x != dx) {
		if (dx > pos_.x) {
			pos_.x += 2;
			forward_ = RIGHT;
		}
		else {
			pos_.x -= 2;
			forward_ = LEFT;
		}
	}
	else if (pos_.y != dy) {
		if (dy > pos_.y) {
			pos_.y += 2;
			forward_ = DOWN;
		}
		else {
			pos_.y -= 2;
			forward_ = UP;
		}
	}
}

void Enemy::SetDijkstra(bool useDijkstra)
{
	isDijkstra_ = useDijkstra;
}
