#include "GraphScene.h"
#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QGraphicsSceneMouseEvent>
#include <QResizeEvent>
#include <QtMath>

// ═══════════════════════════════════════════════════════════════════════════
// EdgeItem
// ═══════════════════════════════════════════════════════════════════════════

EdgeItem::EdgeItem(int eid, VertexItem* from, VertexItem* to,
                   int weight, bool oriented, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_eid(eid), m_from(from), m_to(to)
    , m_weight(weight), m_oriented(oriented)
{
    setZValue(-1);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void EdgeItem::setHighlighted(bool h)
{
    if (m_highlighted == h) return;
    m_highlighted = h;
    update();
}

QRectF EdgeItem::boundingRect() const
{
    if (!m_from || !m_to) return {};
    return QRectF(m_from->pos(), m_to->pos()).normalized().adjusted(-30, -30, 30, 30);
}

QPainterPath EdgeItem::shape() const
{
    if (!m_from || !m_to) return {};
    QPainterPath p;
    p.moveTo(m_from->pos());
    p.lineTo(m_to->pos());
    QPainterPathStroker s;
    s.setWidth(16);
    return s.createStroke(p);
}

void EdgeItem::arrowHead(QPainter* painter,
                          const QPointF& tip, const QPointF& dir, qreal sz)
{
    QPointF perp(-dir.y(), dir.x());
    QPointF base = tip - dir * sz;
    QPolygonF poly;
    poly << tip << (base + perp * sz * 0.45) << (base - perp * sz * 0.45);
    painter->drawPolygon(poly);
}

void EdgeItem::paint(QPainter* painter,
                     const QStyleOptionGraphicsItem*, QWidget*)
{
    if (!m_from || !m_to) return;
    painter->setRenderHint(QPainter::Antialiasing);

    const QPointF fc    = m_from->pos();
    const QPointF tc    = m_to->pos();
    const QPointF delta = tc - fc;
    const qreal   len   = QLineF(fc, tc).length();
    if (len < 1.0) return;

    const QPointF dir = delta / len;
    const QPointF p1  = fc + dir * VertexItem::R;
    const QPointF p2  = tc - dir * (m_oriented ? VertexItem::R + 10.0 : VertexItem::R);

    const QColor col = m_highlighted ? QColor(20, 20, 20) : QColor(90, 90, 90);
    const qreal  sw  = m_highlighted ? 2.5 : 1.5;

    painter->setPen(QPen(col, sw, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(p1, p2);

    if (m_oriented) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(col);
        arrowHead(painter, tc - dir * VertexItem::R, dir, 9.0);
    }

    // Weight label
    const QPointF mid  = (fc + tc) * 0.5;
    const QPointF norm = QPointF(-dir.y(), dir.x()) * 15.0;
    const QPointF lpos = mid + norm;

    const QString ws = QString::number(m_weight);
    QFont fnt("Courier New", 10);
    painter->setFont(fnt);
    QFontMetricsF fm(fnt);
    QRectF br = fm.boundingRect(ws);
    br.moveCenter(lpos);
    br.adjust(-4, -2, 4, 2);

    painter->setBrush(QColor(244, 243, 239, 215));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(br, 2, 2);

    painter->setPen(m_highlighted ? QColor(20, 20, 20) : QColor(110, 110, 110));
    painter->drawText(br, Qt::AlignCenter, ws);
}

void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit edgeClicked(m_eid);
    QGraphicsObject::mousePressEvent(event);
}

// ═══════════════════════════════════════════════════════════════════════════
// VertexItem
// ═══════════════════════════════════════════════════════════════════════════

VertexItem::VertexItem(const QString& id, qreal x, qreal y,
                       QGraphicsItem* parent)
    : QGraphicsObject(parent), m_id(id)
{
    setPos(x, y);
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable);
    setZValue(1);
    setCursor(Qt::OpenHandCursor);
}

QRectF VertexItem::boundingRect() const
{
    return QRectF(-R - 10, -R - 10, (R + 10) * 2, (R + 10) * 2);
}

void VertexItem::paint(QPainter* painter,
                       const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    const bool dark = m_highlighted || isSelected();

    if (m_highlighted) {
        painter->setPen(QPen(QColor(0, 0, 0, 22), 1.0));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QRectF(-R - 8, -R - 8, (R + 8) * 2, (R + 8) * 2));
    }

    painter->setPen(QPen(QColor(20, 20, 20), dark ? 2.5 : 1.5));
    painter->setBrush(dark ? QColor(18, 18, 18) : Qt::white);
    painter->drawEllipse(QRectF(-R, -R, R * 2, R * 2));

    QFont fnt("Segoe UI", 13, QFont::DemiBold);
    painter->setFont(fnt);
    painter->setPen(dark ? Qt::white : QColor(18, 18, 18));
    painter->drawText(QRectF(-R, -R, R * 2, R * 2), Qt::AlignCenter, m_id);

    if (!m_distLabel.isEmpty()) {
        QFont df("Courier New", 10);
        painter->setFont(df);
        painter->setPen(QColor(140, 140, 140));
        painter->drawText(QPointF(R + 4, -R + 4), m_distLabel);
    }
}

QVariant VertexItem::itemChange(GraphicsItemChange change,
                                const QVariant& value)
{
    if (change == ItemPositionHasChanged)
        emit vertexMoved(m_id, pos().x(), pos().y());
    return QGraphicsObject::itemChange(change, value);
}

void VertexItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit vertexClicked(m_id);
    QGraphicsObject::mousePressEvent(event);
}

// ═══════════════════════════════════════════════════════════════════════════
// GraphView
// ═══════════════════════════════════════════════════════════════════════════

GraphView::GraphView(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setDragMode(QGraphicsView::NoDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
}

void GraphView::resizeEvent(QResizeEvent* e)
{
    QGraphicsView::resizeEvent(e);
    if (scene())
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void GraphView::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->fillRect(rect, QColor(250, 250, 248));

    painter->setPen(QPen(QColor(200, 198, 194), 1.5, Qt::SolidLine, Qt::RoundCap));
    const int sp = 24;
    const int x0 = (static_cast<int>(rect.left())  / sp) * sp;
    const int y0 = (static_cast<int>(rect.top())   / sp) * sp;
    for (int x = x0; x <= static_cast<int>(rect.right());  x += sp)
        for (int y = y0; y <= static_cast<int>(rect.bottom()); y += sp)
            painter->drawPoint(x, y);
}

// ═══════════════════════════════════════════════════════════════════════════
// GraphScene
// ═══════════════════════════════════════════════════════════════════════════

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0, 0, 1060, 740);
}

void GraphScene::reset()
{
    for (auto* e : m_eItems) { removeItem(e); delete e; }
    m_eItems.clear();
    for (auto* v : m_vItems) { removeItem(v); delete v; }
    m_vItems.clear();
}

void GraphScene::setOriented(bool o)
{
    m_oriented = o;
    for (auto* e : m_eItems) e->setOriented(o);
}

void GraphScene::addVertex(const QString& id, qreal x, qreal y)
{
    if (m_vItems.contains(id)) return;
    auto* item = new VertexItem(id, x, y);
    connect(item, &VertexItem::vertexMoved,   this, &GraphScene::vertexMoved);
    connect(item, &VertexItem::vertexMoved,   this, &GraphScene::onAnyVertexMoved);
    connect(item, &VertexItem::vertexClicked, this, &GraphScene::vertexSelected);
    m_vItems[id] = item;
    addItem(item);
}

void GraphScene::removeVertex(const QString& id)
{
    if (auto* item = m_vItems.take(id)) {
        removeItem(item);
        delete item;
    }
}

void GraphScene::addEdge(int eid, const QString& from,
                          const QString& to, int weight)
{
    auto* fv = m_vItems.value(from);
    auto* tv = m_vItems.value(to);
    if (!fv || !tv) return;
    auto* item = new EdgeItem(eid, fv, tv, weight, m_oriented);
    connect(item, &EdgeItem::edgeClicked, this, &GraphScene::edgeSelected);
    m_eItems[eid] = item;
    addItem(item);
}

void GraphScene::removeEdge(int eid)
{
    if (auto* item = m_eItems.take(eid)) {
        removeItem(item);
        delete item;
    }
}

void GraphScene::clearHighlights()
{
    for (auto* v : m_vItems) { v->setHighlighted(false); v->setDistLabel(""); }
    for (auto* e : m_eItems)   e->setHighlighted(false);
}

void GraphScene::highlightVertices(const QSet<QString>& ids)
{
    for (auto it = m_vItems.cbegin(); it != m_vItems.cend(); ++it)
        it.value()->setHighlighted(ids.contains(it.key()));
}

void GraphScene::highlightEdges(const QSet<int>& ids)
{
    for (auto it = m_eItems.cbegin(); it != m_eItems.cend(); ++it)
        it.value()->setHighlighted(ids.contains(it.key()));
}

void GraphScene::showBFSDist(const QMap<QString, int>& dist)
{
    for (auto it = m_vItems.cbegin(); it != m_vItems.cend(); ++it) {
        if (dist.contains(it.key()))
            it.value()->setDistLabel(QString("d=%1").arg(dist[it.key()]));
        else
            it.value()->setDistLabel(QString());
    }
}

void GraphScene::clearDist()
{
    for (auto* v : m_vItems) v->setDistLabel(QString());
}

void GraphScene::onAnyVertexMoved()
{
    for (auto* e : m_eItems) e->updateGeom();
}
