#pragma once
#include <list>
#include <unordered_map>
#include <map>

// Абстрактный базовый класс хранилища графа
template<typename VD, typename ED>
class GraphStorage {
public:
    virtual ~GraphStorage() = default;

    virtual void InsertEdge(ED* e) = 0;
    virtual void DeleteEdge(VD* v1, VD* v2) = 0;
    virtual ED* GetEdge(VD* v1, VD* v2) const = 0;
    virtual std::list<ED*> GetOutEdges(VD* v) const = 0;
    virtual std::list<ED*> GetAllEdges() const = 0;
    virtual void RemoveVertex(VD* v) = 0;
    virtual GraphStorage<VD, ED>* Clone() const = 0;
};

// Хранилище на основе списков смежности
template<typename VD, typename ED>
class ListGraph : public GraphStorage<VD, ED> {
public:
    std::unordered_map<VD*, std::list<ED*>> adjList;

    void InsertEdge(ED* e) override {
        adjList[e->v1()].push_back(e);
    }

    void DeleteEdge(VD* v1, VD* v2) override {
        auto it = adjList.find(v1);
        if (it != adjList.end()) {
            it->second.remove_if([v2](ED* e){ return e->v2() == v2; });
        }
    }

    ED* GetEdge(VD* v1, VD* v2) const override {
        auto it = adjList.find(v1);
        if (it != adjList.end()) {
            for (ED* e : it->second)
                if (e->v2() == v2) return e;
        }
        return nullptr;
    }

    std::list<ED*> GetOutEdges(VD* v) const override {
        auto it = adjList.find(v);
        if (it != adjList.end()) return it->second;
        return {};
    }

    std::list<ED*> GetAllEdges() const override {
        std::list<ED*> result;
        for (auto& [v, edges] : adjList)
            for (ED* e : edges) result.push_back(e);
        return result;
    }

    void RemoveVertex(VD* v) override {
        adjList.erase(v);
        for (auto& [key, edges] : adjList)
            edges.remove_if([v](ED* e){ return e->v2() == v; });
    }

    GraphStorage<VD, ED>* Clone() const override {
        auto* g = new ListGraph<VD, ED>();
        g->adjList = adjList;
        return g;
    }
};

// Хранилище на основе матрицы смежности (map пар указателей)
template<typename VD, typename ED>
class MatrixGraph : public GraphStorage<VD, ED> {
public:
    std::map<std::pair<VD*, VD*>, ED*> matrix;

    void InsertEdge(ED* e) override {
        matrix[{e->v1(), e->v2()}] = e;
    }

    void DeleteEdge(VD* v1, VD* v2) override {
        matrix.erase({v1, v2});
    }

    ED* GetEdge(VD* v1, VD* v2) const override {
        auto it = matrix.find({v1, v2});
        return it != matrix.end() ? it->second : nullptr;
    }

    std::list<ED*> GetOutEdges(VD* v) const override {
        std::list<ED*> result;
        for (auto& [key, e] : matrix)
            if (key.first == v) result.push_back(e);
        return result;
    }

    std::list<ED*> GetAllEdges() const override {
        std::list<ED*> result;
        for (auto& [key, e] : matrix) result.push_back(e);
        return result;
    }

    void RemoveVertex(VD* v) override {
        for (auto it = matrix.begin(); it != matrix.end(); ) {
            if (it->first.first == v || it->first.second == v)
                it = matrix.erase(it);
            else ++it;
        }
    }

    GraphStorage<VD, ED>* Clone() const override {
        auto* g = new MatrixGraph<VD, ED>();
        g->matrix = matrix;
        return g;
    }
};
