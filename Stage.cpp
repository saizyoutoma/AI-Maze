#include "Stage.h"
#include "./globals.h"
#include <stack>
#include <time.h>
#include <queue>
#include <numeric>
#include <map>

namespace {
	std::stack<Point> prStack;
	//Point Start, Goal;

	void DigDug(int x, int y, vector<vector<STAGE_OBJ>>& _stage)
	{
		_stage[y][x] = STAGE_OBJ::EMPTY;
		Point Dir[]{ {0,-1},{1, 0},{0, 1},{-1,0} };
		std::vector<int> dList;
		for (int i = 0; i < 4; i++) {
			//nextを0~3まで回してデータを取得
			Point next = Point{ x + Dir[i].x, y + Dir[i].y };
			Point nextNext = { next.x + Dir[i].x, next.y + Dir[i].y };
			if (nextNext.x < 0 || nextNext.y < 0 || nextNext.x > STAGE_WIDTH - 1 || nextNext.y > STAGE_HEIGHT - 1)
				continue;

			if (_stage[nextNext.y][nextNext.x] == STAGE_OBJ::WALL)
			{
				dList.push_back(i);
			}
		}
		if (dList.empty())
		{
			return;
		}
		int nrand = rand() % dList.size();
		int tmp = dList[nrand];

		Point next = { x + Dir[tmp].x, y + Dir[tmp].y };
		Point nextNext = { next.x + Dir[tmp].x, next.y + Dir[tmp].y };

		_stage[next.y][next.x] = STAGE_OBJ::EMPTY;
		_stage[nextNext.y][nextNext.x] = STAGE_OBJ::EMPTY;

		prStack.push(nextNext);
		DigDug(nextNext.x, nextNext.y, _stage);
	}


	void AllWall(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++) {
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::EMPTY;
				else
					_stage[j][i] = STAGE_OBJ::WALL;
			}
		}
	}

	void MakeMazeDigDug(int w, int h, vector<vector<STAGE_OBJ>>& _stage)
	{
		AllWall(w, h, _stage);
		Point sp{ 1, 1 };
		prStack.push(sp);
		while (!prStack.empty())
		{
			sp = prStack.top();
			prStack.pop();
			DigDug(sp.x, sp.y, _stage);
		}
		for (int j = 0; j < h; j++)
		{
			for (int i = 0; i < w; i++)
			{
				if (i == 0 || j == 0 || i == w - 1 || j == h - 1)
					_stage[j][i] = STAGE_OBJ::WALL;
				continue;
			}
		}
	}
}

Stage::Stage()
{
	stageData = vector(STAGE_HEIGHT, vector<STAGE_OBJ>(STAGE_WIDTH, STAGE_OBJ::EMPTY));
    stageDataForSearch = vector(STAGE_HEIGHT, vector<int>(STAGE_WIDTH, 0));

	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (y == 0 || y == STAGE_HEIGHT - 1 || x == 0 || x == STAGE_WIDTH - 1)
			{
				stageData[y][x] = STAGE_OBJ::WALL;
			}
			else
			{
				if (x % 2 == 0 && y % 2 == 0)
					stageData[y][x] = STAGE_OBJ::WALL;
				else
					stageData[y][x] = STAGE_OBJ::EMPTY;
			}

		}
	}
	srand((unsigned)time(NULL));
	MakeMazeDigDug(STAGE_WIDTH,STAGE_HEIGHT,stageData);
	//setHole();
	setStageRects();
}

Stage::~Stage()
{
}

void Stage::Update()
{
}

void Stage::Draw()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			switch (stageData[y][x])
			{
			case STAGE_OBJ::EMPTY:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(102, 205, 170), TRUE);
				break;
			case STAGE_OBJ::WALL:
				DrawBox(x * CHA_WIDTH, y * CHA_HEIGHT, x * CHA_WIDTH + CHA_WIDTH, y * CHA_HEIGHT + CHA_HEIGHT, GetColor(119, 136, 153), TRUE);
				break;
			case STAGE_OBJ::GOAL:
			
				break;
			default:
				break;
			}
		}
	}
}

void Stage::setStageRects()
{
	for (int y = 0; y < STAGE_HEIGHT; y++)
	{
		for (int x = 0; x < STAGE_WIDTH; x++)
		{
			if (stageData[y][x] == STAGE_OBJ::WALL)
			{
				stageRects.push_back(Rect(x * CHA_WIDTH, y * CHA_HEIGHT,  CHA_WIDTH, CHA_HEIGHT));
			}
		}
	}

}

void Stage::setHole()
{
	for (int i = 0; i < 100; i++) {
		int w = GetRand(STAGE_WIDTH - 3) + 1;
		int h = GetRand(STAGE_HEIGHT - 3) + 1;
		if (stageData[h][w] == STAGE_OBJ::WALL) {
			stageData[h][w] = STAGE_OBJ::EMPTY;
		}
		else {
			i--;
		}
	}
}

std::queue<std::pair<Point, int>>myQueue;
//vector<vector<int>> stageDataForSearch;
vector<vector<int>> dist(STAGE_HEIGHT, vector<int>(STAGE_WIDTH, INT_MAX));
vector<vector<Point>> pre(STAGE_HEIGHT, vector<Point>(STAGE_WIDTH, { -1,-1 }));
vector<vector<bool>> visited(STAGE_WIDTH, vector<bool>(STAGE_HEIGHT, false));

void Stage::BFS(Point Start,Point Goal)
{
	//探索用
	Point np[4]{ {0,-1},{1,0},{0,1},{-1,0} };
	int step = 0;
	//キュースタート地点とステップ数を記録
	myQueue.push({ Start,step });
	//スタート地点のステップ数をstepに設定
	stageDataForSearch[Start.y][Start.x] = step;
	//探索候補のキューが空ではない間探索を続行
	while (!myQueue.empty())
	{
		//キューから探索候補を取ってくる
		std::pair<Point, int> Crr = myQueue.front();
		myQueue.pop();

		//上のnp[]を使って自分の4近傍を探索
		for (int i = 0; i < 4; i++) {
			Point tmp = { Crr.first.x + np[i].x,Crr.first.y + np[i].y };
			if (tmp.x == 0 || tmp.x == STAGE_WIDTH - 1 ||
				tmp.y == 0 || tmp.y == STAGE_HEIGHT - 1)
				continue;

			//範囲外
			if (tmp.x < 0 || tmp.x >= STAGE_WIDTH || tmp.y < 0 || tmp.y >= STAGE_HEIGHT)
				continue;

			//訪問したか
			if (visited[tmp.x][tmp.y]) {
				continue;
			}

			//隣がEMPTY(床)だったら進める
			if (stageData[tmp.y][tmp.x] == STAGE_OBJ::EMPTY) {
				stageDataForSearch[tmp.y][tmp.x] = Crr.second + 1;
				pre[tmp.y][tmp.x] = Crr.first;  // 親ノードを記録
				myQueue.push({ tmp,stageDataForSearch[tmp.y][tmp.x] });
				visited[tmp.x][tmp.y] = true;
			}

			//stageData[Crr.first.y][Crr.first.x] = BAR;
		}
	}
}

using vecInt = std::pair<int, int>;
using Mdat = std::pair<int, vecInt>;

void Stage::Dijkstra(std::pair<int, int> sp)
{
	dist[sp.second][sp.first] = 0;
	std::priority_queue<Mdat, std::vector<Mdat>, std::greater<Mdat>> pq;
	pq.push(Mdat(0, { sp.first,sp.second }));

	/*if (pq.empty()) {
		return;
	}

	Mdat p = pq.top();
	pq.pop();

	int c = p.first;
	vecInt v = p.second;

	for (int i = 0; i < 4; i++)
	{
		vecInt np = { v.first + (int)dirs[i].x,v.second + (int)dirs[i].y };
		if (np.first < 0 || np.second < 0 || np.first >= STAGE_WIDTH || np.second >= STAGE_HEIGHT) continue;
		if (stageData[np.second][np.first] == STAGE_OBJ::WALL) continue;
		if (dist[np.second][np.first] <= 1 + c) continue;
		dist[np.second][np.first] = 1 + c;
		pre[np.second][np.first] = Point{ v.first, v.second };
		pq.push(Mdat(dist[np.second][np.first], np));
	}*/

	while (!pq.empty())
	{
		Mdat p = pq.top();
		pq.pop();

		int c = p.first;
		vecInt v = p.second;

		for (int i = 0; i < 4; i++)
		{
			vecInt np = { v.first + (int)dirs[i].x,v.second + (int)dirs[i].y };
			if (np.first < 0 || np.second < 0 || np.first >= STAGE_WIDTH || np.second >= STAGE_HEIGHT) continue;
			if (stageData[np.second][np.first] == STAGE_OBJ::WALL) continue;
			if (dist[np.second][np.first] <= 1 + c) continue;
			dist[np.second][np.first] = 1 + c;
			pre[np.second][np.first] = Point{ v.first, v.second };
			pq.push(Mdat(dist[np.second][np.first], np));
		}
	}
}

vector<Point> Stage::restore(int tx, int ty)
{
	vector<Point> path;
	int x = tx;
	int y = ty;
	for (; tx != -1 || ty != -1; tx = pre[y][x].x, ty = pre[y][x].y) {
		path.push_back(Point{tx, ty});
		x = (int)tx, y = (int)ty;
	}
	reverse(path.begin(), path.end());

	return path;
}

vector<Point> Stage::restore(Point start, Point goal)
{
	vector<Point> path;
	Point current = goal;

	//ゴールからスタートまで親ノードをたどって経路を復元
	while (current.x != start.x || current.y != start.y) {
		path.push_back(current);
		current = pre[current.y][current.x];
	}
	path.push_back(start);
	reverse(path.begin(), path.end());

	return path;
}
