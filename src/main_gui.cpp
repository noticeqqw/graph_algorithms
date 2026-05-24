#include <QApplication>
#include "MainWindow.h"

static const char* STYLESHEET = R"(
QMainWindow, QDialog {
    background: #0f0f0f;
}
QWidget {
    background: #111111;
    color: #ededed;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
    font-size: 14px;
}
QToolBar {
    background: #0f0f0f;
    border: none;
    border-bottom: 1px solid #1e1e1e;
    padding: 0 8px;
    spacing: 4px;
}
QToolBar QLabel {
    background: transparent;
    color: #8a8a8a;
    font-size: 13px;
    padding: 0 2px;
}
QToolBar QPushButton {
    background: #1e1e1e;
    border: 1px solid #2a2a2a;
    border-radius: 4px;
    color: #ededed;
    font-size: 13px;
    font-weight: 500;
    padding: 4px 11px;
    min-height: 24px;
}
QToolBar QPushButton:hover  { background: #252525; border-color: #3a3a3a; }
QToolBar QPushButton:checked { background: #ededed; color: #111111; border-color: #ededed; font-weight: 600; }
QToolBar QSpinBox {
    background: #1e1e1e;
    border: 1px solid #2a2a2a;
    border-radius: 4px;
    color: #ededed;
    font-family: "Courier New", monospace;
    font-size: 13px;
    padding: 2px 4px;
    min-width: 44px;
    max-width: 44px;
    min-height: 24px;
}
QToolBar QSpinBox::up-button, QToolBar QSpinBox::down-button { width: 0; border: none; }
QToolBar QSpinBox:focus { border-color: #3d3d3d; }

/* Graph canvas — keep light so dot-grid background shows through */
QGraphicsView { background: transparent; border: none; }

/* Panels */
QScrollArea { border: none; background: #111111; }
QScrollArea > QWidget > QWidget { background: #111111; }
QScrollBar:vertical   { background: transparent; width: 4px; border: none; }
QScrollBar::handle:vertical { background: #2a2a2a; border-radius: 2px; min-height: 24px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

/* Section labels */
QLabel#secTitle {
    color: #404040;
    font-size: 10px;
    font-weight: 700;
    letter-spacing: 2px;
    text-transform: uppercase;
    background: transparent;
}
QLabel#algoTitle {
    color: #ededed;
    font-size: 12px;
    font-weight: 600;
    background: transparent;
}
QLabel#algoSub {
    color: #8a8a8a;
    font-size: 12px;
    letter-spacing: 1px;
    background: transparent;
}

/* List widgets */
QListWidget {
    background: transparent;
    border: none;
    outline: none;
    font-family: "Courier New", monospace;
    font-size: 13px;
}
QListWidget::item {
    height: 24px;
    padding: 0 8px;
    border-radius: 3px;
    color: #ededed;
}
QListWidget::item:hover    { background: #1a1a1a; }
QListWidget::item:selected { background: #ededed; color: #111111; }

/* Inputs */
QLineEdit, QSpinBox {
    background: #1a1a1a;
    border: 1px solid #272727;
    border-radius: 4px;
    color: #ededed;
    font-size: 13px;
    padding: 3px 8px;
    min-height: 26px;
    selection-background-color: #ededed;
    selection-color: #111111;
}
QLineEdit:focus, QSpinBox:focus { border-color: #3a3a3a; }
QSpinBox::up-button, QSpinBox::down-button {
    width: 16px; border: none;
    background: #222222;
}
QSpinBox::up-button:hover, QSpinBox::down-button:hover { background: #2e2e2e; }

/* Panel buttons */
QPushButton {
    background: #1a1a1a;
    border: 1px solid #272727;
    border-radius: 4px;
    color: #ededed;
    font-size: 13px;
    font-weight: 500;
    padding: 4px 12px;
    min-height: 26px;
}
QPushButton:hover  { background: #222222; border-color: #3a3a3a; }
QPushButton#primary {
    background: #ededed;
    color: #111111;
    border-color: #ededed;
    font-weight: 600;
}
QPushButton#primary:hover { background: #d0d0d0; border-color: #d0d0d0; }
QPushButton#danger:hover  { background: #180e0e; border-color: #3a1f1f; color: #d16060; }

/* Text edit (result) */
QTextEdit {
    background: #161616;
    border: 1px solid #1e1e1e;
    border-radius: 4px;
    color: #ededed;
    font-family: "Courier New", monospace;
    font-size: 12px;
    padding: 6px 8px;
}

/* Matrix dialog table */
QTableWidget {
    background: #161616;
    border: 1px solid #272727;
    border-radius: 4px;
    gridline-color: #1e1e1e;
    font-family: "Courier New", monospace;
    font-size: 12px;
    color: #ededed;
}
QHeaderView::section {
    background: #0f0f0f;
    color: #5a5a5a;
    border: none;
    border-right: 1px solid #1e1e1e;
    border-bottom: 1px solid #1e1e1e;
    padding: 4px;
    font-weight: 400;
}
QTableWidget::item { padding: 4px; text-align: center; }
)";

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Graph RGZ");
    app.setStyleSheet(STYLESHEET);

    MainWindow w;
    w.resize(1280, 800);
    w.setWindowTitle("Graph RGZ");
    w.show();

    return app.exec();
}
