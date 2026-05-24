#include "GraphCanvas.h"
#include <QPainter>
#include <QPaintEvent>
#include <cmath>

static const double PI = 3.14159265358979323846;
static const int R = 22;

static const QColor BG_COLOR(18, 18, 18);
static const QColor EDGE_COLOR(100, 120, 150);
static const QColor EDGE_MST_COLOR(76, 175, 80);
static const QColor VERTEX_FILL(38, 38, 38);
static const QColor VERTEX_BORDER(100, 120, 150);
static const QColor VERTEX_BFS_BORDER(74, 144, 217);
static const QColor VERTEX_BFS_FILL(30, 50, 80);
static const QColor TEXT_COLOR(220, 220, 220);
static const QColor WEIGHT_COLOR(160, 160, 160);

GraphCanvas::GraphCanvas(QWidget* parent) : QWidget(parent) {
    setMinimumSize(400, 400);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void GraphCanvas::setGraph(AppGraph* g) {
    graph = g;
    bfsHighlight.clear();
    mstHighlight.clear();
    computePositions();
    update();
}

void GraphCanvas::setBFSResult(const std::vector<AppVD*>& res) {
    bfsHighlight.clear();
    mstHighlight.clear();
    for (AppVD* v : res) bfsHighlight.insert(v);
    update();
}

void GraphCanvas::setMSTResult(const std::vector<AppED*>& res) {
    mstHighlight.clear();
    bfsHighlight.clear();
    for (AppED* e : res) mstHighlight.insert(e);
    update();
}

void GraphCanvas::clearHighlight() {
    bfsHighlight.clear();
    mstHighlight.clear();
    update();
}

void GraphCanvas::computePositions() {
    vertexPos.clear();
    if (!graph || graph->V() == 0) return;
    double cx = width() / 2.0, cy = height() / 2.0;
    double r = std::min(cx, cy) - R - 20;
    int n = graph->V();
    for (int i = 0; i < n; ++i) {
        double angle = 2.0 * PI * i / n - PI / 2.0;
        vertexPos.push_back({(int)(cx + r * std::cos(angle)),
                             (int)(cy + r * std::sin(angle))});
    }
}

void GraphCanvas::resizeEvent(QResizeEvent*) {
    computePositions();
    update();
}

void GraphCanvas::drawArrow(QPainter& p, QPoint from, QPoint to, int radius) {
    double dx = to.x() - from.x(), dy = to.y() - from.y();
    double len = std::sqrt(dx*dx + dy*dy);
    if (len < 1) return;
    double ux = dx/len, uy = dy/len;
    QPoint tip(to.x() - (int)(ux*radius), to.y() - (int)(uy*radius));
    QPoint start(from.x() + (int)(ux*radius), from.y() + (int)(uy*radius));
    p.drawLine(start, tip);

    double al = 11, aw = 5;
    QPoint left( tip.x() - (int)(ux*al - uy*aw), tip.y() - (int)(uy*al + ux*aw));
    QPoint right(tip.x() - (int)(ux*al + uy*aw), tip.y() - (int)(uy*al - ux*aw));
    QPolygon poly;
    poly << tip << left << right;
    p.setBrush(p.pen().color());
    p.drawPolygon(poly);
    p.setBrush(Qt::NoBrush);
}

void GraphCanvas::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), BG_COLOR);

    if (!graph || graph->V() == 0) {
        p.setPen(QColor(80, 80, 80));
        p.setFont(QFont("SF Pro Display", 14));
        p.drawText(rect(), Qt::AlignCenter, "Граф не создан");
        return;
    }
    if ((int)vertexPos.size() != graph->V()) computePositions();

    auto& verts = graph->vertices;

    // Рёбра
    for (AppED* e : graph->storage->GetAllEdges()) {
        int i1 = -1, i2 = -1;
        for (int i = 0; i < (int)verts.size(); ++i) {
            if (verts[i] == e->v1()) i1 = i;
            if (verts[i] == e->v2()) i2 = i;
        }
        if (i1 < 0 || i2 < 0) continue;
        if (!graph->Directed() && i1 > i2) continue;

        bool isMST = mstHighlight.count(e) > 0;
        QColor edgeCol = isMST ? EDGE_MST_COLOR : EDGE_COLOR;
        p.setPen(QPen(edgeCol, isMST ? 2.5 : 1.5, Qt::SolidLine, Qt::RoundCap));
        p.setBrush(Qt::NoBrush);

        QPoint p1 = vertexPos[i1], p2 = vertexPos[i2];
        if (graph->Directed()) {
            drawArrow(p, p1, p2, R);
        } else {
            double dx = p2.x()-p1.x(), dy = p2.y()-p1.y();
            double len = std::sqrt(dx*dx+dy*dy);
            if (len < 1) continue;
            double ux = dx/len, uy = dy/len;
            p.drawLine(QPoint(p1.x()+(int)(ux*R), p1.y()+(int)(uy*R)),
                       QPoint(p2.x()-(int)(ux*R), p2.y()-(int)(uy*R)));
        }

        // Вес ребра
        if (e->GetW() != 0.0) {
            QPoint mid((p1.x()+p2.x())/2 + 6, (p1.y()+p2.y())/2 - 4);
            p.setPen(WEIGHT_COLOR);
            p.setFont(QFont("Monospace", 9));
            p.drawText(mid, QString::number((int)e->GetW()));
        }
    }

    // Вершины
    p.setFont(QFont("Monospace", 10, QFont::Bold));
    for (int i = 0; i < (int)verts.size(); ++i) {
        QPoint pt = vertexPos[i];
        bool hi = bfsHighlight.count(verts[i]) > 0;

        // Тень
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 80));
        p.drawEllipse(pt + QPoint(2, 3), R, R);

        // Заливка
        p.setBrush(hi ? VERTEX_BFS_FILL : VERTEX_FILL);
        p.setPen(QPen(hi ? VERTEX_BFS_BORDER : VERTEX_BORDER, hi ? 2.5 : 1.5));
        p.drawEllipse(pt, R, R);

        // Метка
        QString label = QString::fromStdString(verts[i]->GetName());
        QFontMetrics fm(p.font());
        QRect br = fm.boundingRect(label);
        p.setPen(TEXT_COLOR);
        p.drawText(pt.x() - br.width()/2, pt.y() + br.height()/4, label);
    }
}
