CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
BUILDDIR := build

# ── Консольное приложение ──────────────────────────────────────────────────────
.PHONY: all console test clean gui run-gui rebuild-gui

all: console

console:
	@rm -f $(BUILDDIR)/rgr
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(BUILDDIR)/rgr
	@$(BUILDDIR)/rgr

# ── Тесты ─────────────────────────────────────────────────────────────────────
test: $(BUILDDIR)/test_graph
	./$(BUILDDIR)/test_graph

$(BUILDDIR)/test_graph: tests/test_graph.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# ── GUI (Qt6) ─────────────────────────────────────────────────────────────────
QT_CXXFLAGS := $(shell pkg-config --cflags Qt6Widgets 2>/dev/null)
QT_LIBS     := $(shell pkg-config --libs   Qt6Widgets 2>/dev/null)
MOC         := /opt/homebrew/share/qt/libexec/moc

GUI_HEADERS := include/GraphScene.h include/MainWindow.h

GUI_SRC := src/GraphScene.cpp src/MainWindow.cpp src/main_gui.cpp

MOC_SRCS := $(patsubst include/%.h, $(BUILDDIR)/moc_%.cpp, $(GUI_HEADERS))

$(BUILDDIR)/moc_%.cpp: include/%.h | $(BUILDDIR)
	$(MOC) $< -o $@

gui:
	@pkill rgr_gui 2>/dev/null || true
	@rm -rf $(BUILDDIR)
	@mkdir -p $(BUILDDIR)
	@$(MOC) include/GraphScene.h -o $(BUILDDIR)/moc_GraphScene.cpp
	@$(MOC) include/MainWindow.h -o $(BUILDDIR)/moc_MainWindow.cpp
	$(CXX) $(CXXFLAGS) -Wno-implicit-function-declaration $(QT_CXXFLAGS) \
		$(GUI_SRC) $(BUILDDIR)/moc_GraphScene.cpp $(BUILDDIR)/moc_MainWindow.cpp \
		$(QT_LIBS) -o $(BUILDDIR)/rgr_gui
	@$(BUILDDIR)/rgr_gui &

run-gui: $(BUILDDIR)/rgr_gui
	$(BUILDDIR)/rgr_gui &

$(BUILDDIR)/rgr_gui: $(GUI_SRC) $(MOC_SRCS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -Wno-implicit-function-declaration $(QT_CXXFLAGS) $^ $(QT_LIBS) -o $@

# ── Служебные цели ────────────────────────────────────────────────────────────
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
