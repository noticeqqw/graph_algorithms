cmake_minimum_required(VERSION 3.15)
project(RGR_Graph)
set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# ── Консольное приложение ──────────────────────────────────────────────────────
add_executable(rgr src/main.cpp)
target_compile_features(rgr PRIVATE cxx_std_17)

# ── GUI на wxWidgets ───────────────────────────────────────────────────────────
find_package(wxWidgets QUIET COMPONENTS core base)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    add_executable(rgr_gui WIN32 MACOSX_BUNDLE
        src/App.cpp
        src/MainFrame.cpp
        src/GraphCanvas.cpp
        src/ControlPanel.cpp
        src/TaskPanel.cpp
    )
    target_link_libraries(rgr_gui ${wxWidgets_LIBRARIES})
else()
    message(STATUS "wxWidgets не найден — GUI не будет собран")
endif()

# ── Тесты ─────────────────────────────────────────────────────────────────────
enable_testing()
if(EXISTS ${CMAKE_SOURCE_DIR}/tests/test_graph.cpp)
    add_executable(test_graph tests/test_graph.cpp)
    target_compile_features(test_graph PRIVATE cxx_std_17)
    add_test(NAME GraphTests COMMAND test_graph)
endif()
