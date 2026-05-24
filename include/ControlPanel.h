#pragma once
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include "AppTypes.h"

class GraphCanvas;

class ControlPanel : public QWidget {
    Q_OBJECT
public:
    explicit ControlPanel(AppGraph** g, GraphCanvas* canvas, QWidget* parent = nullptr);
    void refreshLists();

private slots:
    void onAddVertex();
    void onDelVertex();
    void onAddEdge();
    void onDelEdge();

private:
    AppGraph**   graphPtr;
    GraphCanvas* canvas;

    QListWidget* vertexList;
    QListWidget* edgeList;
    QLineEdit*   nameInput;
    QLineEdit*   v1Input;
    QLineEdit*   v2Input;
    QLineEdit*   weightInput;
};
