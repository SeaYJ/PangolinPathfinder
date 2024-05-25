#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <cstdlib>

#include "../deps/console-colors.h"

using namespace std;

// 定义表示符号
static const char pangolin_ID = '1';
static const char grandpa_ID = '2';
static const char monster_ID = '3';
static const char pangolin_route_ID = '>';

// 定义地图宽 高
static const int map_width = 40;
static const int map_height = 20;

// 定义地图矩阵
static char map[map_height][map_width] = { 0 };
typedef struct {
	int x;
	int y;
} POSITION;

static POSITION pangolin_pos;
static POSITION grandpa_pos;
static vector<POSITION> monsters_pos;

// 声明相关函数
void initCC(cc_color_t fg_color, cc_color_t bg_color);
void initMap();
void showMap();
void pause();

// 可以斜着走
//const int dx[] = { -1, 1, 0, 0, -1, -1, 1, 1 };
//const int dy[] = { 0, 0, -1, 1, -1, 1, -1, 1 };
// 只能上下左右走
const vector<int> dx = { -1, 1, 0, 0 };
const vector<int> dy = { 0, 0, -1, 1 };
const double INF = std::numeric_limits<double>::max();  // 用于表示无穷大
vector<pair<int, int>> findPath(int startX, int startY, int endX, int endY);
double calculateProbability(int x, int y);

struct Node {
	int x, y;
	double cost;
	bool operator<(const Node& other) const {
		return cost > other.cost;
	}
};