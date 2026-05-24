#pragma once

// Дескриптор ребра графа
template<typename VD, typename WeightType, typename DataType>
class EdgeDescriptor {
public:
    VD* ptr_v1;
    VD* ptr_v2;
    WeightType w;
    DataType data;

    EdgeDescriptor() : ptr_v1(nullptr), ptr_v2(nullptr), w(WeightType{}) {}
    EdgeDescriptor(VD* v1, VD* v2, WeightType weight = WeightType{}, DataType d = DataType{})
        : ptr_v1(v1), ptr_v2(v2), w(weight), data(d) {}

    VD* v1() const { return ptr_v1; }
    VD* v2() const { return ptr_v2; }
    void SetW(WeightType weight) { w = weight; }
    void SetData(const DataType& d) { data = d; }
    WeightType GetW() const { return w; }
    DataType GetData() const { return data; }
};
