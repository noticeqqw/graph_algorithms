#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QSpinBox>
#include <QLineEdit>
#include "AppTypes.h"

class GraphCanvas;
class ControlPanel;

class TaskPanel : public QWidget {
    Q_OBJECT
public:
    explicit TaskPanel(AppGraph** g, GraphCanvas* canvas,
                       ControlPanel* ctrl, QWidget* parent = nullptr);
    ~TaskPanel();

private slots:
    void onRunBFS();
    void onRunPrim();
    void onRestart();

private:
    AppGraph**    graphPtr;
    GraphCanvas*  canvas;
    ControlPanel* control;

    QLineEdit* bfsStartInput;
    QSpinBox*  bfsDSpin;
    QLineEdit* primStartInput;
    QTextEdit* resultOutput;

    AppBFS*  bfsTask  = nullptr;
    AppPrim* primTask = nullptr;

    enum class LastTask { None, BFS, Prim } lastTask = LastTask::None;
};
