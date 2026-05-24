#pragma once
#include <string>

// Дескриптор вершины графа
template<typename NameType, typename DataType>
class VertexDescriptor {
public:
    NameType name;
    DataType data;

    VertexDescriptor() = default;
    VertexDescriptor(const NameType& n, const DataType& d) : name(n), data(d) {}

    void SetName(const NameType& n) { name = n; }
    void SetData(const DataType& d) { data = d; }
    NameType GetName() const { return name; }
    DataType GetData() const { return data; }
};
