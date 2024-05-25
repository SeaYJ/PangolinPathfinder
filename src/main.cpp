#include "main.h"

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>

#include "../deps/console-colors.h"

/*
* �������ݣ�
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

	// ��ȡ·����Ϣ���Ҳ�����Ϊ��
	vector<pair<int, int>> path = findPath(pangolin_pos.x, pangolin_pos.y, grandpa_pos.x, grandpa_pos.y);

	/*
	$ ��ģ˼· $

	1. ÿ���ľ������ʣ�

	ʹ�� Pi = k / d �Ĺ�ʽ������ d �ǵ�ǰ�ڵ���������ֵľ��룬k ��һ�����������ڵ����������ʵ���������֤ 0 <= Pi <= 1/(��2)��

	2. ����·�����ۻ��������ʣ�

	���������ۼӵķ�ʽ����·���ܾ������ʣ�P_total = �� Pi��
	Ϊ��ȷ�� P_total �ں���Χ�ڣ���������һ����һ��ϵ�� alpha��ʹ�� P = 1 - exp(-alpha * P_total)������ alpha ���Ը���·�����Ƚ��е�����
	·���ɹ����ʣ�

	3. �ɹ����� SuccessProbability = P��
	*/
	if (!path.empty()) {
		double P_total = 0.0;
		double alpha = 0.215;  // ��һ��ϵ���������������

		for (const auto& p : path) {
			if (map[p.first][p.second] == '0') {
				map[p.first][p.second] = pangolin_route_ID;

				double stepProbability = calculateProbability(p.first, p.second);	// Pi
				P_total += stepProbability;	// P_total = �� Pi
			}
		}

		double successProbability = 1 - exp(-alpha * P_total); // SuccessProbability = P = 1 - exp(-alpha * P_total)
		cout << "\n�ҵ�·�����ɹ�����Ϊ:" << successProbability << '\n';
	}
	else {
		cout << "\nδ�ҵ�����·����\n";
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
	// ��һ��ʼ��
	for (int row = 0; row < map_height; row++) {
		for (int col = 0; col < map_width; col++) {
			map[row][col] = '0';
		}
	}

	// ��ʼ����ɽ��λ��
	map[0][0] = pangolin_ID;
	pangolin_pos = { 0, 0 };

	// ��ʾ��Ϣ
	cout << "��ͼ��СΪ " << map_height << "x" << map_width << '\n';

	// ����үүλ��
	cout << "������үү��λ��:", cin >> grandpa_pos.x >> grandpa_pos.y;
	map[grandpa_pos.x][grandpa_pos.y] = grandpa_ID;

	// ������������
	int num_monster = 0;
	cout << "��������������:", cin >> num_monster;

	// ��������λ��
	for (int i = 0; i < num_monster; i++) {
		POSITION monster_pos = { 0 };
		cout << "������� " << (i + 1) << " �����ֵ�λ��:", cin >> monster_pos.x >> monster_pos.y;
		map[monster_pos.x][monster_pos.y] = monster_ID;

		monsters_pos.push_back(monster_pos);	// ��¼
	}

	system("cls");
}

void showMap()
{
	// ��ʾ��ͼ
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

	// ����ע��
	cc_fprintf((cc_color_t)(CC_FG_MAGENTA | CC_BG_NONE), stdout, "\n[ %c = ��ɽ�� ", pangolin_ID);
	cc_fprintf((cc_color_t)(CC_FG_YELLOW | CC_BG_NONE), stdout, "| %c = үү ", grandpa_ID);
	cc_fprintf((cc_color_t)(CC_FG_RED | CC_BG_NONE), stdout, "| %c = ���� ]\n", monster_ID);
}

void pause()
{
	cout << "\n[Press to continue]", system("pause 1>nul 2>nul");
}

double calculateProbability(int x, int y) {
	double minDistance = INF;
	for (const auto& monster_pos : monsters_pos) {	// Ѱ�ҵ�������Ǹ����֣�����¼����ľ��� minDistance
		double distance = sqrt(pow(x - monster_pos.x, 2) + pow(y - monster_pos.y, 2));
		if (distance < minDistance) {
			minDistance = distance;
		}
	}
	double k = 1.0; // �趨���� k�������������
	return k / minDistance;
}

/**
 * @brief ʹ�����ȶ���ʵ�ֵ�Dijkstra�㷨�ڵ�ͼ�в��Ҵ���㵽�յ��·����ͬʱ���Ǿ������ֵĸ��ʡ�
 *
 * @param startX ����X���ꡣ
 * @param startY ����Y���ꡣ
 * @param endX �յ��X���ꡣ
 * @param endY �յ��Y���ꡣ
 * @param monsters ���ֵ�λ����Ϣ�б�
 * @return vector<pair<int, int>> ����㵽�յ��·������������ڿ���·���򷵻ؿ��б�
 *
 * @details
 * �ú���ʵ����һ����Ȩ�ص����·�������㷨��ʹ�����ȶ��н��������������ڼ���·��Ȩ��ʱ��
 * ������·����ÿһ���ľ������ʡ�ͨ������ÿһ���ľ������ʲ��ۼӣ����յõ�·������Ȩ�ء�
 */
vector<pair<int, int>> findPath(int startX, int startY, int endX, int endY) {
	// �������ȶ��У����ڴ洢������Ľڵ㣬���ȶ��а���cost��С��������
	priority_queue<Node> pq;

	// ���������󣬳�ʼ��ΪINF
	vector<vector<double>> dist(map_height, vector<double>(map_width, INF));

	// ����ǰ���������ڼ�¼·���ϵ�ǰһ���ڵ㣬��ʼ��Ϊ(-1, -1)
	vector<vector<pair<int, int>>> prev(map_height, vector<pair<int, int>>(map_width, { -1, -1 }));

	// �����������ȶ��У�����costΪ0
	pq.push({ startX, startY, 0.0 });

	// �������ľ���Ϊ0
	dist[startX][startY] = 0.0;

	// ��ʼ�������ȶ���
	while (!pq.empty()) {
		// ȡ�����ȶ����е���Сcost�ڵ�
		Node current = pq.top();
		pq.pop();

		// ��ȡ��ǰ�ڵ�������cost
		int x = current.x;
		int y = current.y;
		double currentCost = current.cost;

		// �����ǰ�ڵ����յ㣬���˳�ѭ��
		if (x == endX && y == endY) {
			break;
		}

		// ������ǰ�ڵ���ĸ��ھӽڵ�
		for (int i = 0; i < 4; ++i) {  // ���������������ĸ�����
			int nx = x + dx[i];
			int ny = y + dy[i];

			// ����ھӽڵ��Ƿ��ڵ�ͼ��Χ�ڣ��Ҳ������ֵ�λ��
			if (nx >= 0 && ny >= 0 && nx < map_height && ny < map_width && map[nx][ny] != monster_ID) {
				// �����ƶ����ھӽڵ��cost
				double nextCost = currentCost + calculateProbability(nx, ny);

				// ����ƶ����ھӽڵ��costС�ڵ�ǰ��¼��cost������¾�������ǰ������
				if (nextCost < dist[nx][ny]) {
					dist[nx][ny] = nextCost;
					pq.push({ nx, ny, nextCost });  // ���ھӽڵ�������ȶ���
					prev[nx][ny] = { x, y };  // ��¼ǰ���ڵ�
				}
			}
		}
	}

	// ����·��
	vector<pair<int, int>> path;
	for (pair<int, int> at = { endX, endY }; at != make_pair(-1, -1); at = prev[at.first][at.second]) {
		path.push_back(at);
	}
	// ��ת·����ʹ�����㵽�յ�
	reverse(path.begin(), path.end());
	return path;
}
