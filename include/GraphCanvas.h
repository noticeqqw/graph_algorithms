#pragma once
#include <QWidget>
#include <QPoint>
#include <vector>
#include <unordered_set>
#include "AppTypes.h"

class GraphCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GraphCanvas(QWidget* parent = nullptr);

    void setGraph(AppGraph* g);
    void setBFSResult(const std::vector<AppVD*>& res);
    void setMSTResult(const std::vector<AppED*>& res);
    void clearHighlight();

private:
    AppGraph* graph = nullptr;
    std::unordered_set<AppVD*> bfsHighlight;
    std::unordered_set<AppED*> mstHighlight;
    std::vector<QPoint> vertexPos;

    void computePositions();
    void drawArrow(QPainter& p, QPoint from, QPoint to, int radius);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
};
