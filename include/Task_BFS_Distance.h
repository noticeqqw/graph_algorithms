#pragma once
#include <vector>
#include <queue>
#include <unordered_map>

// Задача 2 (вар. №13): найти все вершины на расстоянии ровно d рёбер от стартовой
template<typename GraphType>
class Task_BFS_Distance {
public:
    using VD = typename GraphType::VertexType;

    GraphType* graph;
    VD* startVertex;
    int distance;
    std::vector<VD*> result;

    Task_BFS_Distance(GraphType* g, VD* start, int d)
        : graph(g), startVertex(start), distance(d) {
        Restart();
    }

    Task_BFS_Distance(const Task_BFS_Distance& t)
        : graph(t.graph), startVertex(t.startVertex),
          distance(t.distance), result(t.result) {}

    ~Task_BFS_Distance() = default;

    void Set(GraphType* g) {
        graph = g;
        Restart();
    }

    // BFS из одной стартовой вершины; заполняет dist
    void bfsFrom(VD* src, std::unordered_map<VD*, int>& dist) {
        std::queue<VD*> q;
        dist[src] = 0;
        q.push(src);
        while (!q.empty()) {
            VD* v = q.front(); q.pop();
            int dv = dist[v];
            if (dv >= distance) continue;
            for (auto* e : graph->storage->GetOutEdges(v)) {
                VD* u = e->v2();
                if (dist.count(u)) continue;
                dist[u] = dv + 1;
                if (dist[u] == distance)
                    result.push_back(u);
                else
                    q.push(u);
            }
        }
    }

    // BFS: O(V + E), обходит все компоненты связности
    void Restart() {
        result.clear();
        if (!graph || !startVertex) return;

        std::unordered_map<VD*, int> dist;

        // Основная компонента — стартуем из startVertex
        bfsFrom(startVertex, dist);

        // Остальные компоненты — запускаем BFS из каждой непосещённой вершины
        for (VD* v : graph->vertices) {
            if (!dist.count(v))
                bfsFrom(v, dist);
        }
    }

    std::vector<VD*> Result() const { return result; }
};
