#pragma once
#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QTimer>
#include "AppTypes.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class GraphScene;
class GraphView;

struct AlgoFrame {
    QSet<QString>      nodes;
    QSet<int>          edges;
    QMap<QString, int> dist;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOrientToggle(bool checked);
    void onShowMatrix();
    void onCreateGraph();
    void onAddVertex();
    void onRemoveVertex();
    void onAddEdge();
    void onRemoveEdge();
    void onRunBFS();
    void onRunPrim();
    void onRestart();
    void onVertexSelected(const QString& id);
    void onEdgeSelected(int eid);
    void onVertexMoved(const QString& id, qreal x, qreal y);
    void onAnimTick();

private:
    void     buildUI();
    void     buildToolbar();
    QWidget* buildLeftPanel();
    QWidget* buildRightPanel();

    void syncSceneFromGraph();
    void syncVertexList();
    void syncEdgeList();
    void updateStats();
    void startAnimation(const QVector<AlgoFrame>& frames, int ms = 350);

    // ── Graph data ────────────────────────────────────────────────────────
    AppGraph*  m_graph    = nullptr;
    bool       m_oriented = false;
    int        m_nextEid  = 0;

    QMap<AppED*, int>    m_edgeIds;   // AppED* → scene edge id
    QMap<QString, QPointF> m_pos;     // vertex name → last known position

    QString m_selV;
    int     m_selE = -1;

    // ── Scene ─────────────────────────────────────────────────────────────
    GraphScene* m_scene = nullptr;
    GraphView*  m_view  = nullptr;

    // ── Toolbar ───────────────────────────────────────────────────────────
    QLabel*   m_vLabel    = nullptr;
    QLabel*   m_eLabel    = nullptr;
    QSpinBox* m_vCountSpin = nullptr;
    QSpinBox* m_eCountSpin = nullptr;
    QSpinBox* m_wMinSpin  = nullptr;
    QSpinBox* m_wMaxSpin  = nullptr;

    // ── Left panel ────────────────────────────────────────────────────────
    QListWidget* m_vertList     = nullptr;
    QLineEdit*   m_vertNameEdit = nullptr;
    QListWidget* m_edgeListW    = nullptr;
    QLineEdit*   m_edgeFromEdit = nullptr;
    QLineEdit*   m_edgeToEdit   = nullptr;
    QSpinBox*    m_edgeWSpin    = nullptr;

    // ── Right panel ───────────────────────────────────────────────────────
    QLineEdit* m_bfsStartEdit  = nullptr;
    QSpinBox*  m_bfsDSpin      = nullptr;
    QLineEdit* m_primStartEdit = nullptr;
    QTextEdit* m_resultEdit    = nullptr;

    // ── Animation ─────────────────────────────────────────────────────────
    QTimer             m_animTimer;
    QVector<AlgoFrame> m_frames;
    int                m_frameIdx = 0;
    QMap<QString, int> m_finalDist;
};
