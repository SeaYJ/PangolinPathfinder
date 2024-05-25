#include "main.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>

#include "../deps/console-colors.h"

/*
* 测试数据：
19 39
10
5 1
9 2
17 3
3 5
12 8
8 16
12 19
18 24
6 34
16 36

*/

int main(int argc, char** argv) {
	initCC(CC_FG_GRAY, CC_BG_BLACK);
	initMap();

	// 获取路径信息，找不到则为空
	vector<pair<int, int>> path = findPath(pangolin_pos.x, pangolin_pos.y, grandpa_pos.x, grandpa_pos.y);

	/*
	$ 建模思路 $

	1. 每步的惊动概率：

	使用 Pi = k / d 的公式，其中 d 是当前节点与最近妖怪的距离，k 是一个常数，用于调整惊动概率的量级，可证 0 <= Pi <= 1/(√2)。

	2. 整条路径的累积惊动概率：

	采用线性累加的方式计算路径总惊动概率：P_total = Σ Pi。
	为了确保 P_total 在合理范围内，可以引入一个归一化系数 alpha，使得 P = 1 - exp(-alpha * P_total)，其中 alpha 可以根据路径长度进行调整。
	路径成功概率：

	3. 成功概率 SuccessProbability = P。
	*/
	if (!path.empty()) {
		double P_total = 0.0;
		double alpha = 0.215;  // 归一化系数，根据需求调整

		for (const auto& p : path) {
			if (map[p.first][p.second] == '0') {
				map[p.first][p.second] = pangolin_route_ID;

				double stepProbability = calculateProbability(p.first, p.second);	// Pi
				P_total += stepProbability;	// P_total = Σ Pi
			}
		}

		double successProbability = 1 - exp(-alpha * P_total); // SuccessProbability = P = 1 - exp(-alpha * P_total)
		cout << "\n找到路径，成功概率为:" << successProbability << '\n';
	}
	else {
		cout << "\n未找到可行路径。\n";
	}

	showMap();
	pause();

	return 0;
}

void initCC(cc_color_t fg_color, cc_color_t bg_color)
{
	cc_fprintf((cc_color_t)(fg_color | bg_color), stdout, "");
}

void initMap()
{
	// 归一初始化
	for (int row = 0; row < map_height; row++) {
		for (int col = 0; col < map_width; col++) {
			map[row][col] = '0';
		}
	}

	// 初始化穿山甲位置
	map[0][0] = pangolin_ID;
	pangolin_pos = { 0, 0 };

	// 提示信息
	cout << "地图大小为 " << map_height << "x" << map_width << '\n';

	// 设置爷爷位置
	cout << "请输入爷爷的位置:", cin >> grandpa_pos.x >> grandpa_pos.y;
	map[grandpa_pos.x][grandpa_pos.y] = grandpa_ID;

	// 设置妖怪数量
	int num_monster = 0;
	cout << "请输入妖怪数量:", cin >> num_monster;

	// 设置妖怪位置
	for (int i = 0; i < num_monster; i++) {
		POSITION monster_pos = { 0 };
		cout << "请输入第 " << (i + 1) << " 个妖怪的位置:", cin >> monster_pos.x >> monster_pos.y;
		map[monster_pos.x][monster_pos.y] = monster_ID;

		monsters_pos.push_back(monster_pos);	// 记录
	}

	system("cls");
}

void showMap()
{
	// 显示地图
	for (int row = 0; row < map_height; row++) {
		for (int col = 0; col < map_width; col++) {
			if (map[row][col] == pangolin_ID) {
				cc_fprintf((cc_color_t)(CC_FG_MAGENTA | CC_BG_NONE), stdout, "%c", map[row][col]);
			}
			else if (map[row][col] == grandpa_ID) {
				cc_fprintf((cc_color_t)(CC_FG_YELLOW | CC_BG_NONE), stdout, "%c", map[row][col]);
			}
			else if (map[row][col] == monster_ID) {
				cc_fprintf((cc_color_t)(CC_FG_RED | CC_BG_NONE), stdout, "%c", map[row][col]);
			}
			else if (map[row][col] == pangolin_route_ID) {
				cc_fprintf((cc_color_t)(CC_FG_GREEN | CC_BG_NONE), stdout, "%c", map[row][col]);
			}
			else {
				cout << map[row][col];
			}

			cout << (col != map_width - 1 ? " " : "\n");
		}
	}

	// 生成注释
	cc_fprintf((cc_color_t)(CC_FG_MAGENTA | CC_BG_NONE), stdout, "\n[ %c = 穿山甲 ", pangolin_ID);
	cc_fprintf((cc_color_t)(CC_FG_YELLOW | CC_BG_NONE), stdout, "| %c = 爷爷 ", grandpa_ID);
	cc_fprintf((cc_color_t)(CC_FG_RED | CC_BG_NONE), stdout, "| %c = 妖怪 ]\n", monster_ID);
}

void pause()
{
	cout << "\n[Press to continue]", system("pause 1>nul 2>nul");
}

double calculateProbability(int x, int y) {
	double minDistance = INF;
	for (const auto& monster_pos : monsters_pos) {	// 寻找到最近的那个妖怪，并记录与其的距离 minDistance
		double distance = sqrt(pow(x - monster_pos.x, 2) + pow(y - monster_pos.y, 2));
		if (distance < minDistance) {
			minDistance = distance;
		}
	}
	double k = 1.0; // 设定常数 k，根据需求调整
	return k / minDistance;
}

/**
 * @brief 使用优先队列实现的Dijkstra算法在地图中查找从起点到终点的路径，同时考虑惊动妖怪的概率。
 *
 * @param startX 起点的X坐标。
 * @param startY 起点的Y坐标。
 * @param endX 终点的X坐标。
 * @param endY 终点的Y坐标。
 * @param monsters 妖怪的位置信息列表。
 * @return vector<pair<int, int>> 从起点到终点的路径，如果不存在可行路径则返回空列表。
 *
 * @details
 * 该函数实现了一个带权重的最短路径搜索算法，使用优先队列进行搜索，并且在计算路径权重时，
 * 考虑了路径上每一步的惊动概率。通过计算每一步的惊动概率并累加，最终得到路径的总权重。
 */
vector<pair<int, int>> findPath(int startX, int startY, int endX, int endY) {
	// 定义优先队列，用于存储待处理的节点，优先队列按照cost从小到大排序
	priority_queue<Node> pq;

	// 定义距离矩阵，初始化为INF
	vector<vector<double>> dist(map_height, vector<double>(map_width, INF));

	// 定义前驱矩阵，用于记录路径上的前一个节点，初始化为(-1, -1)
	vector<vector<pair<int, int>>> prev(map_height, vector<pair<int, int>>(map_width, { -1, -1 }));

	// 将起点加入优先队列，起点的cost为0
	pq.push({ startX, startY, 0.0 });

	// 设置起点的距离为0
	dist[startX][startY] = 0.0;

	// 开始处理优先队列
	while (!pq.empty()) {
		// 取出优先队列中的最小cost节点
		Node current = pq.top();
		pq.pop();

		// 获取当前节点的坐标和cost
		int x = current.x;
		int y = current.y;
		double currentCost = current.cost;

		// 如果当前节点是终点，则退出循环
		if (x == endX && y == endY) {
			break;
		}

		// 遍历当前节点的四个邻居节点
		for (int i = 0; i < 4; ++i) {  // 仅考虑上下左右四个方向
			int nx = x + dx[i];
			int ny = y + dy[i];

			// 检查邻居节点是否在地图范围内，且不是妖怪的位置
			if (nx >= 0 && ny >= 0 && nx < map_height && ny < map_width && map[nx][ny] != monster_ID) {
				// 计算移动到邻居节点的cost
				double nextCost = currentCost + calculateProbability(nx, ny);

				// 如果移动到邻居节点的cost小于当前记录的cost，则更新距离矩阵和前驱矩阵
				if (nextCost < dist[nx][ny]) {
					dist[nx][ny] = nextCost;
					pq.push({ nx, ny, nextCost });  // 将邻居节点加入优先队列
					prev[nx][ny] = { x, y };  // 记录前驱节点
				}
			}
		}
	}

	// 生成路径
	vector<pair<int, int>> path;
	for (pair<int, int> at = { endX, endY }; at != make_pair(-1, -1); at = prev[at.first][at.second]) {
		path.push_back(at);
	}
	// 反转路径，使其从起点到终点
	reverse(path.begin(), path.end());
	return path;
}
