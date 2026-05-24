#include <cassert>
#include <iostream>
#include "VertexDescriptor.h"
#include "EdgeDescriptor.h"
#include "GraphStorage.h"
#include "Graph.h"
#include "Task_BFS_Distance.h"
#include "Task_Prim_MST.h"

using VD = VertexDescriptor<std::string, int>;
using ED = EdgeDescriptor<VD, double, int>;
using G  = Graph<VD, ED>;

void test_graph_structure() {
    G g;
    auto* a = g.InsertV(std::string("A"));
    auto* b = g.InsertV(std::string("B"));
    auto* c = g.InsertV(std::string("C"));
    g.InsertE(a, b, 3.0);
    g.InsertE(b, c, 5.0);

    assert(g.V() == 3);
    assert(g.E() == 2);
    assert(!g.Directed());

    auto* e = g.GetEdge(a, b);
    assert(e != nullptr);
    assert(e->GetW() == 3.0);

    // Переключение хранилища
    g.ToMatrixGraph();
    assert(g.GetEdge(b, a) != nullptr);
    g.ToListGraph();
    assert(g.GetEdge(a, b) != nullptr);

    // Удаление ребра
    g.DeleteE(a, b);
    assert(g.E() == 1);
    assert(g.GetEdge(a, b) == nullptr);

    std::cout << "[PASS] test_graph_structure\n";
}

void test_bfs() {
    // Линейный граф: A-B-C-D-E
    G g;
    auto* a = g.InsertV(std::string("A"));
    auto* b = g.InsertV(std::string("B"));
    auto* c = g.InsertV(std::string("C"));
    auto* d = g.InsertV(std::string("D"));
    auto* e = g.InsertV(std::string("E"));
    g.InsertE(a, b, 1); g.InsertE(b, c, 1);
    g.InsertE(c, d, 1); g.InsertE(d, e, 1);

    Task_BFS_Distance<G> bfs(&g, a, 2);
    auto res = bfs.Result();
    assert(res.size() == 1 && res[0] == c);

    Task_BFS_Distance<G> bfs3(&g, a, 3);
    auto res3 = bfs3.Result();
    assert(res3.size() == 1 && res3[0] == d);

    std::cout << "[PASS] test_bfs\n";
}

void test_prim() {
    // Граф: A-B(5), B-C(3), C-D(4), D-E(2), A-D(8)
    G g;
    auto* a = g.InsertV(std::string("A"));
    auto* b = g.InsertV(std::string("B"));
    auto* c = g.InsertV(std::string("C"));
    auto* d = g.InsertV(std::string("D"));
    auto* e = g.InsertV(std::string("E"));
    g.InsertE(a, b, 5); g.InsertE(b, c, 3);
    g.InsertE(c, d, 4); g.InsertE(d, e, 2);
    g.InsertE(a, d, 8);

    Task_Prim_MST<G> prim(&g, a);
    auto res = prim.Result();
    // MST должен содержать V-1 = 4 ребра
    assert(res.edges.size() == 4);
    // Суммарный вес: 5+3+4+2 = 14
    assert(res.totalWeight == 14.0);

    std::cout << "[PASS] test_prim\n";
}

void test_iterators() {
    G g;
    auto* a = g.InsertV(std::string("A"));
    auto* b = g.InsertV(std::string("B"));
    g.InsertE(a, b, 1.0);

    int vcount = 0;
    G::VertexIterator vi(&g);
    for (vi.beg(); !vi.end(); ++vi) ++vcount;
    assert(vcount == 2);

    int ecount = 0;
    G::EdgeIterator ei(&g);
    for (ei.beg(); !ei.end(); ++ei) ++ecount;
    assert(ecount == 2); // неориентированный — два направления

    std::cout << "[PASS] test_iterators\n";
}

int main() {
    test_graph_structure();
    test_bfs();
    test_prim();
    test_iterators();
    std::cout << "\nВсе тесты пройдены.\n";
    return 0;
}
