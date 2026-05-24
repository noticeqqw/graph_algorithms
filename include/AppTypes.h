#pragma once
#include "VertexDescriptor.h"
#include "EdgeDescriptor.h"
#include "GraphStorage.h"
#include "Graph.h"
#include "Task_BFS_Distance.h"
#include "Task_Prim_MST.h"

using AppVD    = VertexDescriptor<std::string, int>;
using AppED    = EdgeDescriptor<AppVD, double, int>;
using AppGraph = Graph<AppVD, AppED>;
using AppBFS   = Task_BFS_Distance<AppGraph>;
using AppPrim  = Task_Prim_MST<AppGraph>;
