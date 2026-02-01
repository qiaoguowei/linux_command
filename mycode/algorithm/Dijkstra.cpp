#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <algorithm>


/**
 * @brief Dijkstra算法求解单源最短路径（邻接表版）
 * @param graph 邻接表：graph[u] = {{v1, w1}, {v2, w2}} 表示u到v1权w1，u到v2权w2
 * @param n 图的节点总数（节点编号建议从0开始，也可改1开始）
 * @param start 起点节点编号
 * @param dist 输出参数：dist[i]表示起点到i的最短距离
 * @param pre 输出参数：pre[i]表示i在最短路径中的前驱节点，用于路径回溯
 */
void Dijkstra(const std::vector<std::vector<std::pair<int, int>>>& graph, int start,
    std::vector<int>& dist, std::vector<int>& pre) {

    // 1.初始化距离数组和前驱数组
    dist.assign(graph.size(), INT32_MAX); //所有节点初始距离为无穷大
    pre.assign(graph.size(), -1); // 前驱初始化为-1，无前驱
    dist[start] = 0; //起点到自身的距离为0

    // 小根堆：存储<当前距离，节点>, 按距离升序排列
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> minHeap;
    minHeap.emplace(0, start); //将起点加入堆中

    while (!minHeap.empty()) {
        // 2.取出当前距离起点最近的节点（堆顶）
        auto [cur_dist, u] = minHeap.top();
        minHeap.pop();

        // 若当前距离大于已记录的最短距离，说明是旧的无效数据，直接跳过
        if (cur_dist > dist[u]) {
            continue;
        }

        // 3.遍历当前节点u的所有邻近节点，做松弛操作
        for (auto [v, w] : graph[u]) {
            // 松弛条件：通过u到v的路径，比v当前的最短距离更短
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w; //更新最短距离
                pre[v] = u; //更新前驱节点
                minHeap.emplace(dist[v], v); //将更新后的节点加入堆中
            }
        }
    }
}

/**
 * @brief 路径回溯：从终点到起点的前驱数组，还原为起点到终点的最短路径
 * @param pre 前驱数组（Dijkstra输出）
 * @param start 起点节点
 * @param end 终点节点
 * @return 起点到终点的最短路径节点数组，空表示无路径
 */
std::vector<int> getShortestPath(const std::vector<int>& pre, int start, int end) {
    std::vector<int> path;
    // 从终端回溯到起点
    for (int cur = end; cur != -1; cur = pre[cur]) {
        path.push_back(cur);
        if (cur == start) break; //到达起点，停止回溯
    }

    if (path.back() != start) {
        return {}; //无路径
    }

    //反转数组，得到起点到终端的顺序
    std::reverse(path.begin(), path.end());
    return path;
}


int main() {
    /* 构建测试图（无向图，有向图只需要修改领接表构建）
       节点编号：1,2,3,4,5,6,7 
       边及权值：
       1-2(2), 1-4(1)
       2-4(3), 2-5(10)
       3-1(4), 3-6(5)
       4-3(2), 4-5(2), 4-6(8), 4-7(4)
       5-7(6)
       7-6(1)
       起点1，预期最短路径：
       1-2(2), 1-4-3(3), 1-4(1), 1-4-5(3), 1-4-7-6(6), 1-4-7(5) 
    */

    int n = 7; //节点数量
    std::vector<std::vector<std::pair<int, int>>> graph(n+1);
    graph[1].emplace_back(2, 2);
    graph[1].emplace_back(4, 1);
    graph[2].emplace_back(4, 3);
    graph[2].emplace_back(5, 10);
    graph[3].emplace_back(1, 4);
    graph[3].emplace_back(6, 5);
    graph[4].emplace_back(3, 2);
    graph[4].emplace_back(5, 2);
    graph[4].emplace_back(6, 8);
    graph[4].emplace_back(7, 4);
    graph[5].emplace_back(7, 6);
    graph[7].emplace_back(6, 1);

    int start = 1; //起点
    std::vector<int> dist, pre;
    Dijkstra(graph, start, dist, pre);

    // 输出起点到各节点的最短距离
    std::cout << "起点" << start << "到各节点的最短距离：" << std::endl;
    for (int i = 1; i <= n; ++i) {
        if (dist[i] == INT32_MAX) {
            std::cout << start << "->" << i << " : 无路径" << std::endl;
        } else {
            std::cout << start << "->" << i << " : " << dist[i] << std::endl;
        }
    }

    // 输出起点0到终端3的具体最短路径
    int end = 3;
    std::vector<int> path = getShortestPath(pre, start, end);
    std::cout << "起点" << start << "到终点" << end << "的最短路径：";
    if (path.empty()) {
        std::cout << "无路径" << std::endl;
    } else {
        for (int i = 0; i < path.size(); ++i) {
            if (i > 0) std::cout << "->";
            std::cout << path[i];
        }
        std::cout << std::endl;
    }

    return 0;
}
