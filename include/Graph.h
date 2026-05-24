#pragma once
#include "GraphStorage.h"
#include <vector>
#include <list>
#include <string>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

// Основной класс «Простой граф»
template<typename VD, typename ED>
class Graph {
public:
    using VertexType = VD;
    using EdgeType = ED;

    std::vector<VD*> vertices;
    GraphStorage<VD, ED>* storage;
    bool directed;
    int edgeCount;

    // Вспомогательная функция инициализации хранилища
    void initStorage(bool useMatrix) {
        if (useMatrix)
            storage = new MatrixGraph<VD, ED>();
        else
            storage = new ListGraph<VD, ED>();
    }

    // Конструктор: пустой L-граф
    Graph() : storage(nullptr), directed(false), edgeCount(0) {
        storage = new ListGraph<VD, ED>();
    }

    // Конструктор: V вершин без рёбер
    Graph(int v, bool dir, bool useMatrix)
        : storage(nullptr), directed(dir), edgeCount(0) {
        initStorage(useMatrix);
        for (int i = 0; i < v; ++i)
            InsertV(std::string(1, char('A' + i)));
    }

    // Конструктор: V вершин, E случайных рёбер
    Graph(int v, int e, bool dir, bool useMatrix, int wmin = 1, int wmax = 20)
        : storage(nullptr), directed(dir), edgeCount(0) {
        initStorage(useMatrix);
        for (int i = 0; i < v; ++i)
            InsertV(std::string(1, char('A' + i)));
        srand((unsigned)time(nullptr));
        int range = (wmax > wmin) ? (wmax - wmin + 1) : 1;
        int attempts = 0;
        while (edgeCount < e && attempts < e * 10) {
            ++attempts;
            int i = rand() % v, j = rand() % v;
            if (i == j) continue;
            VD* a = vertices[i];
            VD* b = vertices[j];
            if (storage->GetEdge(a, b)) continue;
            double w = (rand() % range) + wmin;
            InsertE(a, b, w);
        }
    }

    // Конструктор копирования (поверхностный: те же дескрипторы, новое хранилище)
    Graph(const Graph& g) : directed(g.directed), edgeCount(g.edgeCount) {
        vertices = g.vertices;
        storage = g.storage->Clone();
    }

    ~Graph() {
        for (ED* e : storage->GetAllEdges()) delete e;
        delete storage;
        for (VD* v : vertices) delete v;
    }

    int V() const { return (int)vertices.size(); }
    int E() const { return edgeCount; }
    bool Directed() const { return directed; }
    bool Dense() const {
        int v = V();
        if (v <= 1) return false;
        double maxE = directed ? v * (v - 1) : v * (v - 1) / 2.0;
        return maxE > 0 && (edgeCount / maxE) > 0.5;
    }
    double K() const {
        int v = V();
        if (v <= 1) return 0.0;
        double maxE = directed ? v * (v - 1) : v * (v - 1) / 2.0;
        return maxE > 0 ? edgeCount / maxE : 0.0;
    }

    void ToListGraph() {
        if (dynamic_cast<ListGraph<VD,ED>*>(storage)) return;
        auto* ng = new ListGraph<VD, ED>();
        for (ED* e : storage->GetAllEdges()) ng->InsertEdge(e);
        delete storage;
        storage = ng;
    }

    void ToMatrixGraph() {
        if (dynamic_cast<MatrixGraph<VD,ED>*>(storage)) return;
        auto* ng = new MatrixGraph<VD, ED>();
        for (ED* e : storage->GetAllEdges()) ng->InsertEdge(e);
        delete storage;
        storage = ng;
    }

    VD* InsertV() {
        VD* v = new VD();
        vertices.push_back(v);
        return v;
    }

    template<typename NameType>
    VD* InsertV(const NameType& name) {
        VD* v = new VD();
        v->SetName(name);
        vertices.push_back(v);
        return v;
    }

    void DeleteV(VD* v) {
        // Удалить все рёбра инцидентные v и скорректировать счётчик
        auto allEdges = storage->GetAllEdges();
        int removed = 0;
        for (ED* e : allEdges) {
            if (e->v1() == v || e->v2() == v) {
                ++removed;
                delete e;
            }
        }
        edgeCount -= removed;
        storage->RemoveVertex(v);
        vertices.erase(std::remove(vertices.begin(), vertices.end(), v), vertices.end());
        delete v;
    }

    ED* InsertE(VD* v1, VD* v2) {
        if (storage->GetEdge(v1, v2)) return storage->GetEdge(v1, v2);
        ED* e = new ED(v1, v2);
        storage->InsertEdge(e);
        ++edgeCount;
        if (!directed) {
            ED* er = new ED(v2, v1);
            er->SetW(e->GetW());
            storage->InsertEdge(er);
        }
        return e;
    }

    ED* InsertE(VD* v1, VD* v2, double w) {
        if (storage->GetEdge(v1, v2)) return storage->GetEdge(v1, v2);
        ED* e = new ED(v1, v2, w);
        storage->InsertEdge(e);
        ++edgeCount;
        if (!directed) {
            ED* er = new ED(v2, v1, w);
            storage->InsertEdge(er);
        }
        return e;
    }

    void DeleteE(VD* v1, VD* v2) {
        if (!storage->GetEdge(v1, v2)) return;
        ED* e = storage->GetEdge(v1, v2);
        storage->DeleteEdge(v1, v2);
        delete e;
        --edgeCount;
        if (!directed) {
            ED* er = storage->GetEdge(v2, v1);
            storage->DeleteEdge(v2, v1);
            delete er;
        }
    }

    ED* GetEdge(VD* v1, VD* v2) const {
        return storage->GetEdge(v1, v2);
    }

    // Найти вершину по имени
    template<typename NameType>
    VD* FindVertex(const NameType& name) const {
        for (VD* v : vertices)
            if (v->GetName() == name) return v;
        return nullptr;
    }

    // ── Итераторы ────────────────────────────────────────────────────────────

    class VertexIterator {
        Graph* g;
        int idx;
    public:
        VertexIterator(Graph* g) : g(g), idx(0) {}
        void beg() { idx = 0; }
        bool end() const { return idx >= (int)g->vertices.size(); }
        void operator++() { ++idx; }
        VD* operator*() const { return g->vertices[idx]; }
    };

    class EdgeIterator {
        Graph* g;
        std::list<ED*> edges;
        typename std::list<ED*>::iterator it;
    public:
        EdgeIterator(Graph* g) : g(g) {
            edges = g->storage->GetAllEdges();
            it = edges.begin();
        }
        void beg() { it = edges.begin(); }
        bool end() const { return it == edges.end(); }
        void operator++() { ++it; }
        ED* operator*() const { return *it; }
    };

    class OutEdgeIterator {
        std::list<ED*> edges;
        typename std::list<ED*>::iterator it;
    public:
        OutEdgeIterator(Graph* g, VD* v) {
            edges = g->storage->GetOutEdges(v);
            it = edges.begin();
        }
        void beg() { it = edges.begin(); }
        bool end() const { return it == edges.end(); }
        void operator++() { ++it; }
        ED* operator*() const { return *it; }
    };
};
