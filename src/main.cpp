#include <iostream>
#include <string>
#include "VertexDescriptor.h"
#include "EdgeDescriptor.h"
#include "GraphStorage.h"
#include "Graph.h"
#include "Task_BFS_Distance.h"
#include "Task_Prim_MST.h"

using VD = VertexDescriptor<std::string, int>;
using ED = EdgeDescriptor<VD, double, int>;
using G  = Graph<VD, ED>;

// Вывод структуры графа
void printGraph(G* g) {
    if (!g) { std::cout << "(граф не создан)\n"; return; }
    std::cout << "Вершин: " << g->V() << ", Рёбер: " << g->E()
              << ", " << (g->Directed() ? "ориентированный" : "неориентированный")
              << ", K=" << g->K() << "\n";
    for (VD* v : g->vertices) {
        std::cout << v->GetName() << ": [";
        bool first = true;
        for (ED* e : g->storage->GetOutEdges(v)) {
            if (!first) std::cout << ", ";
            std::cout << e->v2()->GetName() << "(w=" << e->GetW() << ")";
            first = false;
        }
        std::cout << "]\n";
    }
}

int main() {
    G* graph = nullptr;

    // Для хранения последней задачи
    enum class LastTask { None, BFS, Prim } lastTask = LastTask::None;
    Task_BFS_Distance<G>* bfsTask = nullptr;
    Task_Prim_MST<G>* primTask  = nullptr;

    auto runBFS = [&](const std::string& startName, int d) {
        VD* start = graph->FindVertex(startName);
        if (!start) { std::cout << "Вершина '" << startName << "' не найдена.\n"; return; }
        delete bfsTask;
        bfsTask = new Task_BFS_Distance<G>(graph, start, d);
        lastTask = LastTask::BFS;
        auto res = bfsTask->Result();
        std::cout << "Вершины на расстоянии " << d << " от " << startName << ": [";
        bool first = true;
        for (VD* v : res) { if (!first) std::cout << ", "; std::cout << v->GetName(); first = false; }
        std::cout << "]\n";
    };

    auto runPrim = [&](const std::string& startName) {
        VD* start = graph->FindVertex(startName);
        if (!start) { std::cout << "Вершина '" << startName << "' не найдена.\n"; return; }
        delete primTask;
        primTask = new Task_Prim_MST<G>(graph, start);
        lastTask = LastTask::Prim;
        auto res = primTask->Result();
        std::cout << "MST рёбра: ";
        bool first = true;
        for (ED* e : res.edges) {
            if (!first) std::cout << ", ";
            std::cout << e->v1()->GetName() << "-" << e->v2()->GetName()
                      << "(w=" << e->GetW() << ")";
            first = false;
        }
        std::cout << " | Суммарный вес: " << res.totalWeight << "\n";
    };

    while (true) {
        std::cout << "\n=== Меню ===\n"
                  << "1. Создать граф\n"
                  << "2. Добавить вершину\n"
                  << "3. Удалить вершину\n"
                  << "4. Добавить ребро\n"
                  << "5. Удалить ребро\n"
                  << "6. Вывести структуру\n"
                  << "7. Задача 2 — BFS Distance\n"
                  << "8. Задача 3 — Prim MST\n"
                  << "9. Restart (повторить задачу)\n"
                  << "0. Выход\n"
                  << "Выбор: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 0) break;

        if (choice == 1) {
            int v, e; bool dir, usemat;
            std::cout << "Вершин (<=20): "; std::cin >> v;
            std::cout << "Рёбер: "; std::cin >> e;
            std::cout << "Ориентированный? (1/0): "; std::cin >> dir;
            std::cout << "Матрица смежности? (1/0): "; std::cin >> usemat;
            std::cin.ignore();
            delete graph;
            graph = new G(v, e, dir, usemat);
            std::cout << "Граф создан.\n";
        }
        else if (choice == 2) {
            if (!graph) { std::cout << "Сначала создайте граф.\n"; continue; }
            std::string name;
            std::cout << "Имя вершины: "; std::getline(std::cin, name);
            graph->InsertV(name);
            std::cout << "Вершина '" << name << "' добавлена.\n";
        }
        else if (choice == 3) {
            if (!graph) { std::cout << "Сначала создайте граф.\n"; continue; }
            std::string name;
            std::cout << "Имя вершины: "; std::getline(std::cin, name);
            VD* v = graph->FindVertex(name);
            if (!v) { std::cout << "Не найдена.\n"; continue; }
            graph->DeleteV(v);
            std::cout << "Вершина удалена.\n";
        }
        else if (choice == 4) {
            if (!graph) { std::cout << "Сначала создайте граф.\n"; continue; }
            std::string n1, n2; double w;
            std::cout << "Вершина 1: "; std::getline(std::cin, n1);
            std::cout << "Вершина 2: "; std::getline(std::cin, n2);
            std::cout << "Вес (0 = без веса): "; std::cin >> w; std::cin.ignore();
            VD* v1 = graph->FindVertex(n1);
            VD* v2 = graph->FindVertex(n2);
            if (!v1 || !v2) { std::cout << "Одна из вершин не найдена.\n"; continue; }
            graph->InsertE(v1, v2, w);
            std::cout << "Ребро добавлено.\n";
        }
        else if (choice == 5) {
            if (!graph) { std::cout << "Сначала создайте граф.\n"; continue; }
            std::string n1, n2;
            std::cout << "Вершина 1: "; std::getline(std::cin, n1);
            std::cout << "Вершина 2: "; std::getline(std::cin, n2);
            VD* v1 = graph->FindVertex(n1);
            VD* v2 = graph->FindVertex(n2);
            if (!v1 || !v2) { std::cout << "Одна из вершин не найдена.\n"; continue; }
            graph->DeleteE(v1, v2);
            std::cout << "Ребро удалено.\n";
        }
        else if (choice == 6) {
            printGraph(graph);
        }
        else if (choice == 7) {
            if (!graph) { std::cout << "Сначала создайте граф.\n"; continue; }
            std::string name; int d;
            std::cout << "Стартовая вершина: "; std::getline(std::cin, name);
            std::cout << "Расстояние d: "; std::cin >> d; std::cin.ignore();
            runBFS(name, d);
        }
        else if (choice == 8) {
            if (!graph) { std::cout << "Сначала создайте граф.\n"; continue; }
            std::string name;
            std::cout << "Стартовая вершина: "; std::getline(std::cin, name);
            runPrim(name);
        }
        else if (choice == 9) {
            if (lastTask == LastTask::BFS && bfsTask) {
                bfsTask->Set(graph);
                auto res = bfsTask->Result();
                std::cout << "BFS Restart — вершин найдено: " << res.size() << "\n";
                for (VD* v : res) std::cout << "  " << v->GetName() << "\n";
            } else if (lastTask == LastTask::Prim && primTask) {
                primTask->Set(graph);
                auto res = primTask->Result();
                std::cout << "Prim Restart — MST рёбер: " << res.edges.size()
                          << ", вес: " << res.totalWeight << "\n";
            } else {
                std::cout << "Нет сохранённой задачи.\n";
            }
        }
    }

    delete bfsTask;
    delete primTask;
    delete graph;
    return 0;
}
