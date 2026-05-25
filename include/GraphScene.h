#pragma once
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QGraphicsView>
#include <QSet>
#include <QMap>

class VertexItem;

// ── EdgeItem ──────────────────────────────────────────────────────────────────
class EdgeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    EdgeItem(int eid, VertexItem* from, VertexItem* to,
             int weight, bool oriented, QGraphicsItem* parent = nullptr);

    int  edgeId() const { return m_eid; }
    void setHighlighted(bool h);
    void setOriented(bool o)  { m_oriented = o; prepareGeometryChange(); update(); }
    void setParallel(bool p)  { m_parallel = p; prepareGeometryChange(); update(); }
    void updateGeom()         { prepareGeometryChange(); update(); }

    VertexItem* fromItem() const { return m_from; }
    VertexItem* toItem()   const { return m_to; }

    QRectF       boundingRect() const override;
    QPainterPath shape()        const override;
    void         paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

signals:
    void edgeClicked(int eid);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;

private:
    static void arrowHead(QPainter*, const QPointF& tip, const QPointF& dir, qreal size);

    int         m_eid;
    VertexItem* m_from;
    VertexItem* m_to;
    int         m_weight;
    bool        m_oriented;
    bool        m_highlighted = false;
    bool        m_parallel    = false;
};

// ── VertexItem ────────────────────────────────────────────────────────────────
class VertexItem : public QGraphicsObject
{
    Q_OBJECT
public:
    VertexItem(const QString& id, qreal x, qreal y,
               QGraphicsItem* parent = nullptr);

    QString vertexId()    const { return m_id; }
    void setHighlighted(bool h) { m_highlighted = h; update(); }
    void setDistLabel(const QString& s) { m_distLabel = s; update(); }

    QRectF boundingRect() const override;
    void   paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;

    static constexpr qreal R = 23.0;

signals:
    void vertexMoved(const QString& id, qreal x, qreal y);
    void vertexClicked(const QString& id);

protected:
    QVariant itemChange(GraphicsItemChange, const QVariant&) override;
    void     mousePressEvent(QGraphicsSceneMouseEvent*) override;

private:
    QString m_id;
    bool    m_highlighted = false;
    QString m_distLabel;
};

// ── GraphView ─────────────────────────────────────────────────────────────────
class GraphView : public QGraphicsView
{
public:
    explicit GraphView(QGraphicsScene* scene, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent*) override;
    void drawBackground(QPainter*, const QRectF&) override;
};

// ── GraphScene ────────────────────────────────────────────────────────────────
class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit GraphScene(QObject* parent = nullptr);

    void setOriented(bool o);
    void reset();
    void addVertex(const QString& id, qreal x, qreal y);
    void removeVertex(const QString& id);
    void addEdge(int eid, const QString& from, const QString& to, int weight);
    void removeEdge(int eid);
    void clearHighlights();
    void highlightVertices(const QSet<QString>& ids);
    void highlightEdges(const QSet<int>& ids);
    void showBFSDist(const QMap<QString, int>& dist);
    void clearDist();

signals:
    void vertexMoved(const QString& id, qreal x, qreal y);
    void vertexSelected(const QString& id);
    void edgeSelected(int eid);

private slots:
    void onAnyVertexMoved();

private:
    bool m_oriented = false;
    QMap<QString, VertexItem*> m_vItems;
    QMap<int,     EdgeItem*>   m_eItems;
};
