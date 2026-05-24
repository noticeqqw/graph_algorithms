#pragma once
#include <vector>
#include <queue>
#include <unordered_set>

// Задача 3 (вар. №12): минимальное остовное дерево методом Прима
template<typename GraphType>
class Task_Prim_MST {
public:
    using VD = typename GraphType::VertexType;
    using ED = typename GraphType::EdgeType;

    struct MSTResult {
        std::vector<ED*> edges;
        double totalWeight;
    };

    GraphType* graph;
    VD* startVertex;
    std::vector<ED*> mstEdges;
    double totalWeight;

    Task_Prim_MST(GraphType* g, VD* start)
        : graph(g), startVertex(start), totalWeight(0.0) {
        Restart();
    }

    Task_Prim_MST(const Task_Prim_MST& t)
        : graph(t.graph), startVertex(t.startVertex),
          mstEdges(t.mstEdges), totalWeight(t.totalWeight) {}

    ~Task_Prim_MST() = default;

    void Set(GraphType* g) {
        graph = g;
        Restart();
    }

    // Prim с min-heap: O(E log V)
    void Restart() {
        mstEdges.clear();
        totalWeight = 0.0;
        if (!graph || !startVertex) return;

        // Компаратор по весу ребра (min-heap)
        auto cmp = [](ED* a, ED* b){ return a->GetW() > b->GetW(); };
        std::priority_queue<ED*, std::vector<ED*>, decltype(cmp)> pq(cmp);

        std::unordered_set<VD*> inMST;
        inMST.insert(startVertex);

        for (ED* e : graph->storage->GetOutEdges(startVertex))
            pq.push(e);

        while (!pq.empty() && (int)mstEdges.size() < graph->V() - 1) {
            ED* e = pq.top(); pq.pop();
            VD* u = e->v2();
            if (inMST.count(u)) continue;

            mstEdges.push_back(e);
            totalWeight += e->GetW();
            inMST.insert(u);

            for (ED* f : graph->storage->GetOutEdges(u))
                if (!inMST.count(f->v2())) pq.push(f);
        }
    }

    MSTResult Result() const { return {mstEdges, totalWeight}; }
};
