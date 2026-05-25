#include "MainWindow.h"
#include "GraphScene.h"

#include <QToolBar>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QQueue>
#include <QtMath>
#include <algorithm>
#include <limits>
#include <tuple>
#include <cmath>

// ═══════════════════════════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════════════════════════

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUI();
    connect(&m_animTimer, &QTimer::timeout, this, &MainWindow::onAnimTick);
}

MainWindow::~MainWindow()
{
    delete m_graph;
}

// ═══════════════════════════════════════════════════════════════════════════
// UI construction
// ═══════════════════════════════════════════════════════════════════════════

static QWidget* makeSection(const QString& title, QVBoxLayout*& outLayout)
{
    auto* sec = new QWidget;
    sec->setStyleSheet("QWidget { border-bottom: 1px solid #1c1c1c; }");
    outLayout = new QVBoxLayout(sec);
    outLayout->setContentsMargins(10, 9, 10, 10);
    outLayout->setSpacing(4);
    auto* lbl = new QLabel(title.toUpper());
    lbl->setObjectName("secTitle");
    outLayout->addWidget(lbl);
    return sec;
}

void MainWindow::buildUI()
{
    buildToolbar();

    auto* central = new QWidget;
    setCentralWidget(central);
    auto* hlay = new QHBoxLayout(central);
    hlay->setContentsMargins(0, 0, 0, 0);
    hlay->setSpacing(0);

    hlay->addWidget(buildLeftPanel());

    m_scene = new GraphScene(this);
    connect(m_scene, &GraphScene::vertexSelected, this, &MainWindow::onVertexSelected);
    connect(m_scene, &GraphScene::edgeSelected,   this, &MainWindow::onEdgeSelected);
    connect(m_scene, &GraphScene::vertexMoved,    this, &MainWindow::onVertexMoved);

    m_view = new GraphView(m_scene, this);
    hlay->addWidget(m_view, 1);

    hlay->addWidget(buildRightPanel());
}

void MainWindow::buildToolbar()
{
    auto* tb = addToolBar("main");
    tb->setMovable(false);
    tb->setFloatable(false);

    m_vLabel = new QLabel("V: 0");
    m_eLabel = new QLabel("E: 0");
    tb->addWidget(m_vLabel);
    tb->addWidget(new QLabel("  "));
    tb->addWidget(m_eLabel);
    tb->addSeparator();

    // V / E count spinboxes for generation
    tb->addWidget(new QLabel("Вершин:"));
    m_vCountSpin = new QSpinBox;
    m_vCountSpin->setRange(1, 20);
    m_vCountSpin->setValue(6);
    tb->addWidget(m_vCountSpin);

    tb->addWidget(new QLabel("Рёбер:"));
    m_eCountSpin = new QSpinBox;
    m_eCountSpin->setRange(0, 190);
    m_eCountSpin->setValue(8);
    tb->addWidget(m_eCountSpin);
    tb->addSeparator();

    auto* orientBtn = new QPushButton("Неориент.");
    orientBtn->setCheckable(true);
    orientBtn->setChecked(false);
    connect(orientBtn, &QPushButton::toggled, this, [this, orientBtn](bool c) {
        orientBtn->setText(c ? "Ориент." : "Неориент.");
        onOrientToggle(c);
    });
    tb->addWidget(orientBtn);

    auto* matBtn = new QPushButton("Матрица");
    connect(matBtn, &QPushButton::clicked, this, &MainWindow::onShowMatrix);
    tb->addWidget(matBtn);
    tb->addSeparator();

    tb->addWidget(new QLabel("Вес ["));
    m_wMinSpin = new QSpinBox;
    m_wMinSpin->setRange(1, 999); m_wMinSpin->setValue(1);
    tb->addWidget(m_wMinSpin);
    tb->addWidget(new QLabel(".."));
    m_wMaxSpin = new QSpinBox;
    m_wMaxSpin->setRange(1, 999); m_wMaxSpin->setValue(10);
    tb->addWidget(m_wMaxSpin);
    tb->addWidget(new QLabel("]"));
    tb->addSeparator();

    auto* rndBtn = new QPushButton("Создать граф");
    rndBtn->setObjectName("primary");
    connect(rndBtn, &QPushButton::clicked, this, &MainWindow::onCreateGraph);
    tb->addWidget(rndBtn);
}

QWidget* MainWindow::buildLeftPanel()
{
    auto* scroll = new QScrollArea;
    scroll->setFixedWidth(210);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* inner = new QWidget;
    auto* vlay  = new QVBoxLayout(inner);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->setSpacing(0);

    {
        QVBoxLayout* ly;
        auto* sec = makeSection("Вершины", ly);
        m_vertList = new QListWidget;
        m_vertList->setMaximumHeight(160);
        connect(m_vertList, &QListWidget::itemClicked, this,
                [this](QListWidgetItem* item) {
                    onVertexSelected(item->text().split(' ').first());
                });
        ly->addWidget(m_vertList);
        vlay->addWidget(sec);
    }

    {
        QVBoxLayout* ly;
        auto* sec = makeSection("Добавить вершину", ly);
        m_vertNameEdit = new QLineEdit;
        m_vertNameEdit->setPlaceholderText("Имя вершины");
        connect(m_vertNameEdit, &QLineEdit::returnPressed,
                this, &MainWindow::onAddVertex);
        ly->addWidget(m_vertNameEdit);
        auto* addBtn = new QPushButton("+ Добавить");
        addBtn->setObjectName("primary");
        connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddVertex);
        ly->addWidget(addBtn);
        auto* delBtn = new QPushButton("− Удалить выбранную");
        delBtn->setObjectName("danger");
        connect(delBtn, &QPushButton::clicked, this, &MainWindow::onRemoveVertex);
        ly->addWidget(delBtn);
        vlay->addWidget(sec);
    }

    {
        QVBoxLayout* ly;
        auto* sec = makeSection("Рёбра", ly);
        m_edgeListW = new QListWidget;
        m_edgeListW->setMaximumHeight(220);
        connect(m_edgeListW, &QListWidget::itemClicked, this,
                [this](QListWidgetItem* item) {
                    int row = m_edgeListW->row(item);
                    if (m_graph && row >= 0) {
                        // find the edge by row
                        auto& verts = m_graph->vertices;
                        int cnt = 0;
                        for (AppED* e : m_graph->storage->GetAllEdges()) {
                            if (!m_oriented) {
                                int i1 = -1, i2 = -1;
                                for (int i = 0; i < (int)verts.size(); ++i) {
                                    if (verts[i] == e->v1()) i1 = i;
                                    if (verts[i] == e->v2()) i2 = i;
                                }
                                if (i1 > i2) continue;
                            }
                            if (cnt == row) {
                                if (m_edgeIds.contains(e))
                                    onEdgeSelected(m_edgeIds[e]);
                                break;
                            }
                            ++cnt;
                        }
                    }
                });
        ly->addWidget(m_edgeListW);
        vlay->addWidget(sec);
    }

    {
        QVBoxLayout* ly;
        auto* sec = makeSection("Добавить ребро", ly);
        auto* row = new QWidget;
        auto* rh  = new QHBoxLayout(row);
        rh->setContentsMargins(0, 0, 0, 0); rh->setSpacing(5);
        m_edgeFromEdit = new QLineEdit; m_edgeFromEdit->setPlaceholderText("От");
        m_edgeToEdit   = new QLineEdit; m_edgeToEdit->setPlaceholderText("До");
        rh->addWidget(m_edgeFromEdit);
        rh->addWidget(m_edgeToEdit);
        ly->addWidget(row);
        m_edgeWSpin = new QSpinBox;
        m_edgeWSpin->setRange(1, 9999);
        m_edgeWSpin->setValue(1);
        m_edgeWSpin->setPrefix("Вес: ");
        ly->addWidget(m_edgeWSpin);
        auto* addEBtn = new QPushButton("+ Добавить ребро");
        addEBtn->setObjectName("primary");
        connect(addEBtn, &QPushButton::clicked, this, &MainWindow::onAddEdge);
        ly->addWidget(addEBtn);
        auto* delEBtn = new QPushButton("− Удалить ребро");
        delEBtn->setObjectName("danger");
        connect(delEBtn, &QPushButton::clicked, this, &MainWindow::onRemoveEdge);
        ly->addWidget(delEBtn);
        vlay->addWidget(sec);
    }

    vlay->addStretch(1);
    scroll->setWidget(inner);

    auto* wrapper = new QWidget;
    wrapper->setFixedWidth(210);
    wrapper->setStyleSheet("border-right: 1px solid #1c1c1c;");
    auto* wl = new QVBoxLayout(wrapper);
    wl->setContentsMargins(0, 0, 0, 0);
    wl->addWidget(scroll);
    return wrapper;
}

QWidget* MainWindow::buildRightPanel()
{
    auto* panel = new QWidget;
    panel->setFixedWidth(252);
    panel->setStyleSheet("border-left: 1px solid #1c1c1c;");

    auto* vlay = new QVBoxLayout(panel);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->setSpacing(0);

    auto addSub = [](QVBoxLayout* ly, const QString& t) {
        auto* l = new QLabel(t);
        l->setObjectName("algoSub");
        ly->addWidget(l);
    };

    // BFS
    {
        auto* sec = new QWidget;
        sec->setStyleSheet("border-bottom: 1px solid #1c1c1c;");
        auto* ly = new QVBoxLayout(sec);
        ly->setContentsMargins(12, 14, 12, 14);
        ly->setSpacing(5);
        auto* title = new QLabel("Задача 2");
        title->setObjectName("algoTitle");
        ly->addWidget(title);
        addSub(ly, "Стартовая вершина");
        m_bfsStartEdit = new QLineEdit;
        m_bfsStartEdit->setPlaceholderText("Например А");
        connect(m_bfsStartEdit, &QLineEdit::returnPressed,
                this, &MainWindow::onRunBFS);
        ly->addWidget(m_bfsStartEdit);
        addSub(ly, "Расстояние d");
        m_bfsDSpin = new QSpinBox;
        m_bfsDSpin->setRange(0, 999);
        m_bfsDSpin->setValue(1);
        ly->addWidget(m_bfsDSpin);
        auto* runBtn = new QPushButton("Запустить BFS");
        runBtn->setObjectName("primary");
        connect(runBtn, &QPushButton::clicked, this, &MainWindow::onRunBFS);
        ly->addWidget(runBtn);
        vlay->addWidget(sec);
    }

    // Prim
    {
        auto* sec = new QWidget;
        sec->setStyleSheet("border-bottom: 1px solid #1c1c1c;");
        auto* ly = new QVBoxLayout(sec);
        ly->setContentsMargins(12, 14, 12, 14);
        ly->setSpacing(5);
        auto* title = new QLabel("Задача 3");
        title->setObjectName("algoTitle");
        ly->addWidget(title);
        addSub(ly, "Стартовая вершина");
        m_primStartEdit = new QLineEdit;
        m_primStartEdit->setPlaceholderText("Например А");
        connect(m_primStartEdit, &QLineEdit::returnPressed,
                this, &MainWindow::onRunPrim);
        ly->addWidget(m_primStartEdit);
        auto* runBtn = new QPushButton("Запустить Prim");
        runBtn->setObjectName("primary");
        connect(runBtn, &QPushButton::clicked, this, &MainWindow::onRunPrim);
        ly->addWidget(runBtn);
        auto* rstBtn = new QPushButton("Restart");
        connect(rstBtn, &QPushButton::clicked, this, &MainWindow::onRestart);
        ly->addWidget(rstBtn);
        vlay->addWidget(sec);
    }

    // Result
    {
        auto* sec = new QWidget;
        auto* ly  = new QVBoxLayout(sec);
        ly->setContentsMargins(12, 12, 12, 12);
        ly->setSpacing(6);
        addSub(ly, "Результат");
        m_resultEdit = new QTextEdit;
        m_resultEdit->setReadOnly(true);
        m_resultEdit->setMinimumHeight(80);
        m_resultEdit->setPlaceholderText("—");
        ly->addWidget(m_resultEdit);
        vlay->addWidget(sec);
    }

    vlay->addStretch(1);
    return panel;
}

// ═══════════════════════════════════════════════════════════════════════════
// Graph → Scene sync
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::syncSceneFromGraph()
{
    m_scene->reset();
    m_edgeIds.clear();

    if (!m_graph) return;

    const int n = m_graph->V();
    const double cx = 530, cy = 370, r = 270;

    // Add vertices
    for (int i = 0; i < n; ++i) {
        QString name = QString::fromStdString(m_graph->vertices[i]->GetName());
        QPointF pos;
        if (m_pos.contains(name)) {
            pos = m_pos[name];
        } else {
            double angle = 2.0 * M_PI * i / n - M_PI / 2.0;
            pos = {cx + r * std::cos(angle), cy + r * std::sin(angle)};
        }
        m_scene->addVertex(name, pos.x(), pos.y());
    }

    // Add edges (undirected: only i1 <= i2)
    auto& verts = m_graph->vertices;
    for (AppED* e : m_graph->storage->GetAllEdges()) {
        if (!m_oriented) {
            int i1 = -1, i2 = -1;
            for (int i = 0; i < (int)verts.size(); ++i) {
                if (verts[i] == e->v1()) i1 = i;
                if (verts[i] == e->v2()) i2 = i;
            }
            if (i1 > i2) continue;
        }
        int eid = m_nextEid++;
        m_edgeIds[e] = eid;
        m_scene->addEdge(eid,
            QString::fromStdString(e->v1()->GetName()),
            QString::fromStdString(e->v2()->GetName()),
            (int)e->GetW());
    }
}

void MainWindow::syncVertexList()
{
    const QString prev = m_selV;
    m_vertList->clear();
    if (!m_graph) return;
    for (auto* v : m_graph->vertices) {
        auto* item = new QListWidgetItem(QString::fromStdString(v->GetName()), m_vertList);
        if (item->text() == prev) item->setSelected(true);
    }
}

void MainWindow::syncEdgeList()
{
    m_edgeListW->clear();
    if (!m_graph) return;
    const QString arrow = m_oriented ? "→" : "—";
    auto& verts = m_graph->vertices;
    for (AppED* e : m_graph->storage->GetAllEdges()) {
        if (!m_oriented) {
            int i1 = -1, i2 = -1;
            for (int i = 0; i < (int)verts.size(); ++i) {
                if (verts[i] == e->v1()) i1 = i;
                if (verts[i] == e->v2()) i2 = i;
            }
            if (i1 > i2) continue;
        }
        QString text = QString("%1 %2 %3   w=%4")
            .arg(QString::fromStdString(e->v1()->GetName()),
                 arrow,
                 QString::fromStdString(e->v2()->GetName()))
            .arg((int)e->GetW());
        auto* item = new QListWidgetItem(text, m_edgeListW);
        if (m_edgeIds.contains(e) && m_edgeIds[e] == m_selE)
            item->setSelected(true);
    }
}

void MainWindow::updateStats()
{
    int v = m_graph ? m_graph->V() : 0;
    int e = m_graph ? m_graph->E() : 0;
    m_vLabel->setText(QString("V: %1").arg(v));
    m_eLabel->setText(QString("E: %1").arg(e));
}

// ═══════════════════════════════════════════════════════════════════════════
// Toolbar slots
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onOrientToggle(bool checked)
{
    m_oriented = checked;
    m_scene->setOriented(m_oriented);
    syncEdgeList();
}

void MainWindow::onShowMatrix()
{
    if (!m_graph) return;
    const int n = m_graph->V();
    auto& verts = m_graph->vertices;

    QDialog dlg(this);
    dlg.setWindowTitle("Матрица смежности");
    dlg.resize(std::max(300, 60 + n * 48), std::max(200, 80 + n * 36));

    auto* lay = new QVBoxLayout(&dlg);
    auto* tbl = new QTableWidget(n, n, &dlg);

    QStringList labels;
    QMap<QString, int> idx;
    for (int i = 0; i < n; ++i) {
        QString name = QString::fromStdString(verts[i]->GetName());
        labels << name;
        idx[name] = i;
    }
    tbl->setHorizontalHeaderLabels(labels);
    tbl->setVerticalHeaderLabels(labels);
    tbl->horizontalHeader()->setDefaultSectionSize(44);
    tbl->verticalHeader()->setDefaultSectionSize(34);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) {
            auto* cell = new QTableWidgetItem("0");
            cell->setTextAlignment(Qt::AlignCenter);
            cell->setForeground(QColor(55, 55, 55));
            tbl->setItem(i, j, cell);
        }

    for (AppED* e : m_graph->storage->GetAllEdges()) {
        QString from = QString::fromStdString(e->v1()->GetName());
        QString to   = QString::fromStdString(e->v2()->GetName());
        int i = idx.value(from, -1);
        int j = idx.value(to,   -1);
        if (i < 0 || j < 0) continue;
        auto set = [&](int r, int c) {
            auto* cell = new QTableWidgetItem(QString::number((int)e->GetW()));
            cell->setTextAlignment(Qt::AlignCenter);
            cell->setForeground(QColor(237, 237, 237));
            tbl->setItem(r, c, cell);
        };
        set(i, j);
        if (!m_oriented) set(j, i);
    }

    lay->addWidget(tbl);
    auto* closeBtn = new QPushButton("Закрыть");
    closeBtn->setObjectName("primary");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    lay->addWidget(closeBtn);
    dlg.exec();
}

void MainWindow::onCreateGraph()
{
    onRestart();
    m_pos.clear();

    delete m_graph;
    m_edgeIds.clear();
    m_nextEid = 0;

    int v    = m_vCountSpin->value();
    int e    = m_eCountSpin->value();
    int wmin = m_wMinSpin->value();
    int wmax = std::max(wmin, m_wMaxSpin->value());

    m_graph = new AppGraph(v, e, m_oriented, false, wmin, wmax);

    syncSceneFromGraph();
    syncVertexList();
    syncEdgeList();
    updateStats();
}

// ═══════════════════════════════════════════════════════════════════════════
// Left-panel slots
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onAddVertex()
{
    if (!m_graph) {
        QMessageBox::warning(this, "Граф не создан", "Сначала создайте граф.");
        return;
    }
    QString name = m_vertNameEdit->text().trimmed().toUpper();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя вершины.");
        return;
    }
    if (m_graph->FindVertex(name.toStdString())) {
        QMessageBox::warning(this, "Ошибка", QString("Вершина «%1» уже существует.").arg(name));
        return;
    }

    const int    n     = m_graph->V();
    const double angle = (n == 0) ? 0.0 : (2.0 * M_PI * n / std::max(n, 6));
    const qreal  x     = 530 + std::cos(angle) * 270;
    const qreal  y     = 370 + std::sin(angle) * 270;

    m_graph->InsertV(name.toStdString());
    m_scene->addVertex(name, x, y);
    m_pos[name] = {x, y};
    m_vertNameEdit->clear();
    syncVertexList();
    updateStats();
}

void MainWindow::onRemoveVertex()
{
    if (!m_graph) {
        QMessageBox::warning(this, "Граф не создан", "Сначала создайте граф.");
        return;
    }
    if (m_selV.isEmpty()) {
        QMessageBox::warning(this, "Не выбрана", "Выберите вершину в списке.");
        return;
    }

    // Remove incident edges from scene first
    AppVD* vd = m_graph->FindVertex(m_selV.toStdString());
    if (!vd) return;

    QVector<AppED*> toRemove;
    for (AppED* e : m_graph->storage->GetAllEdges())
        if (e->v1() == vd || e->v2() == vd) toRemove.append(e);
    for (AppED* e : toRemove) {
        if (m_edgeIds.contains(e)) {
            m_scene->removeEdge(m_edgeIds[e]);
            m_edgeIds.remove(e);
        }
    }

    m_graph->DeleteV(vd);
    m_scene->removeVertex(m_selV);
    m_pos.remove(m_selV);
    m_selV.clear();
    onRestart();
    syncVertexList();
    syncEdgeList();
    updateStats();
}

void MainWindow::onAddEdge()
{
    if (!m_graph) {
        QMessageBox::warning(this, "Граф не создан", "Сначала создайте граф.");
        return;
    }
    QString from = m_edgeFromEdit->text().trimmed().toUpper();
    QString to   = m_edgeToEdit->text().trimmed().toUpper();
    int w = m_edgeWSpin->value();
    if (from.isEmpty() || to.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поля «От» и «До».");
        return;
    }
    if (from == to) {
        QMessageBox::warning(this, "Ошибка", "Петли не поддерживаются.");
        return;
    }
    AppVD* v1 = m_graph->FindVertex(from.toStdString());
    AppVD* v2 = m_graph->FindVertex(to.toStdString());
    if (!v1 || !v2) {
        QMessageBox::warning(this, "Ошибка",
            QString("Вершина «%1» не найдена.").arg(!v1 ? from : to));
        return;
    }
    if (m_graph->GetEdge(v1, v2)) {
        QMessageBox::warning(this, "Ошибка",
            QString("Ребро %1→%2 уже существует.").arg(from, to));
        return;
    }

    m_graph->InsertE(v1, v2, w);

    // Find the newly added edge and register it
    AppED* newE = m_graph->GetEdge(v1, v2);
    if (newE) {
        int eid = m_nextEid++;
        m_edgeIds[newE] = eid;
        m_scene->addEdge(eid, from, to, w);
    }

    m_edgeFromEdit->clear();
    m_edgeToEdit->clear();
    syncEdgeList();
    updateStats();
}

void MainWindow::onRemoveEdge()
{
    if (!m_graph) {
        QMessageBox::warning(this, "Граф не создан", "Сначала создайте граф.");
        return;
    }
    if (m_selE < 0) {
        QMessageBox::warning(this, "Не выбрано", "Выберите ребро в списке.");
        return;
    }

    AppED* target = nullptr;
    for (auto it = m_edgeIds.begin(); it != m_edgeIds.end(); ++it) {
        if (it.value() == m_selE) { target = it.key(); break; }
    }
    if (!target) return;

    m_scene->removeEdge(m_selE);
    m_edgeIds.remove(target);
    m_graph->DeleteE(target->v1(), target->v2());
    m_selE = -1;
    syncEdgeList();
    updateStats();
}

// ═══════════════════════════════════════════════════════════════════════════
// Algorithms (inline BFS/Prim with animation frames, using AppGraph data)
// ═══════════════════════════════════════════════════════════════════════════

using Adj = QMap<QString, QVector<QPair<QString, int>>>;

static Adj buildAdj(AppGraph* g, const QMap<AppED*, int>& eids, bool oriented)
{
    Adj adj;
    for (auto* v : g->vertices) adj[QString::fromStdString(v->GetName())] = {};
    auto& verts = g->vertices;
    for (AppED* e : g->storage->GetAllEdges()) {
        // For undirected, GetAllEdges returns both directions — skip duplicates
        if (!oriented) {
            int i1 = -1, i2 = -1;
            for (int i = 0; i < (int)verts.size(); ++i) {
                if (verts[i] == e->v1()) i1 = i;
                if (verts[i] == e->v2()) i2 = i;
            }
            if (i1 > i2) continue;
        }
        if (!eids.contains(e)) continue;
        int eid = eids[e];
        QString from = QString::fromStdString(e->v1()->GetName());
        QString to   = QString::fromStdString(e->v2()->GetName());
        adj[from].append({to, eid});
        if (!oriented) adj[to].append({from, eid});
    }
    return adj;
}

void MainWindow::onRunBFS()
{
    onRestart();
    if (!m_graph) {
        QMessageBox::warning(this, "Граф не создан", "Сначала создайте граф.");
        return;
    }
    if (m_graph->V() == 0) {
        m_resultEdit->setText("Граф пуст.");
        return;
    }
    const QString start = m_bfsStartEdit->text().trimmed().toUpper();
    const int d = m_bfsDSpin->value();
    if (start.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите стартовую вершину.");
        return;
    }
    if (!m_graph->FindVertex(start.toStdString())) {
        m_resultEdit->setText(QString("Вершина «%1» не найдена.").arg(start));
        return;
    }

    auto adj = buildAdj(m_graph, m_edgeIds, m_oriented);
    QMap<QString, int> dist;
    dist[start] = 0;
    QQueue<QString> queue;
    queue.enqueue(start);

    QVector<AlgoFrame> frames;
    { AlgoFrame f0; f0.nodes.insert(start); f0.dist = dist; frames.append(f0); }

    while (!queue.isEmpty()) {
        const QString cur = queue.dequeue();
        for (const auto& nb : adj[cur]) {
            const QString& to  = nb.first;
            const int      eid = nb.second;
            if (!dist.contains(to)) {
                dist[to] = dist[cur] + 1;
                queue.enqueue(to);
                AlgoFrame f = frames.last();
                f.nodes.insert(to);
                f.edges.insert(eid);
                f.dist = dist;
                frames.append(f);
            }
        }
    }

    m_finalDist = dist;

    AlgoFrame finalFrame;
    for (auto* v : m_graph->vertices) {
        QString name = QString::fromStdString(v->GetName());
        if (dist.value(name, -1) == d) finalFrame.nodes.insert(name);
    }
    auto& verts = m_graph->vertices;
    for (AppED* e : m_graph->storage->GetAllEdges()) {
        if (!m_edgeIds.contains(e)) continue;
        if (!m_oriented) {
            int i1=-1,i2=-1;
            for(int i=0;i<(int)verts.size();++i){if(verts[i]==e->v1())i1=i;if(verts[i]==e->v2())i2=i;}
            if(i1>i2)continue;
        }
        QString from = QString::fromStdString(e->v1()->GetName());
        QString to   = QString::fromStdString(e->v2()->GetName());
        int fd = dist.value(from, -1), td = dist.value(to, -1);
        if (fd >= 0 && td >= 0 && qAbs(fd - td) == 1 && (fd == d || td == d))
            finalFrame.edges.insert(m_edgeIds[e]);
    }
    finalFrame.dist = dist;
    frames.append(finalFrame);

    // Precompute result so the lambda doesn't touch m_graph after a possible rebuild
    QStringList res;
    for (auto* v : m_graph->vertices) {
        QString name = QString::fromStdString(v->GetName());
        if (dist.value(name, -1) == d) res << name;
    }
    const QString resultText = res.isEmpty()
        ? QString("Нет вершин на расстоянии %1").arg(d)
        : QString("Расстояние %1:  %2").arg(d).arg(res.join(", "));
    const QMap<QString, int> finalDistCopy = dist;

    startAnimation(frames, 300);

    const int delay = frames.size() * 300 + 200;
    QTimer::singleShot(delay, this, [this, resultText, finalDistCopy] {
        m_resultEdit->setText(resultText);
        m_scene->showBFSDist(finalDistCopy);
    });
}

void MainWindow::onRunPrim()
{
    onRestart();
    if (!m_graph) {
        QMessageBox::warning(this, "Граф не создан", "Сначала создайте граф.");
        return;
    }
    if (m_graph->V() == 0) {
        m_resultEdit->setText("Граф пуст.");
        return;
    }
    if (m_oriented) {
        QMessageBox::warning(this, "Ошибка",
            "Алгоритм Прима работает только с неориентированными графами.\n"
            "Отключите ориентацию и пересоздайте граф.");
        return;
    }
    const QString start = m_primStartEdit->text().trimmed().toUpper();
    if (start.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите стартовую вершину.");
        return;
    }
    if (!m_graph->FindVertex(start.toStdString())) {
        m_resultEdit->setText(QString("Вершина «%1» не найдена.").arg(start));
        return;
    }

    // Build weighted undirected adjacency: name → [(to, eid, weight)]
    using AdjW = QMap<QString, QVector<std::tuple<QString, int, int>>>;
    AdjW adj;
    for (auto* v : m_graph->vertices) adj[QString::fromStdString(v->GetName())] = {};
    auto& verts = m_graph->vertices;
    for (AppED* e : m_graph->storage->GetAllEdges()) {
        if (!m_edgeIds.contains(e)) continue;
        // de-dup for undirected
        int i1=-1,i2=-1;
        for(int i=0;i<(int)verts.size();++i){if(verts[i]==e->v1())i1=i;if(verts[i]==e->v2())i2=i;}
        if(i1>i2)continue;
        int eid = m_edgeIds[e];
        int w   = (int)e->GetW();
        QString from = QString::fromStdString(e->v1()->GetName());
        QString to   = QString::fromStdString(e->v2()->GetName());
        adj[from].append({to, eid, w});
        adj[to].append({from, eid, w});
    }

    QSet<QString> inMST;
    inMST.insert(start);
    QVector<int> mstEids;

    QVector<AlgoFrame> frames;
    { AlgoFrame f0; f0.nodes.insert(start); frames.append(f0); }

    while (inMST.size() < m_graph->V()) {
        int     bestW   = std::numeric_limits<int>::max();
        int     bestEid = -1;
        QString bestTo;

        for (const auto& v : inMST) {
            for (const auto& t : adj[v]) {
                const QString& to  = std::get<0>(t);
                const int      eid = std::get<1>(t);
                const int      w   = std::get<2>(t);
                if (!inMST.contains(to) && w < bestW) {
                    bestW = w; bestEid = eid; bestTo = to;
                }
            }
        }
        if (bestEid < 0) break;

        inMST.insert(bestTo);
        mstEids.append(bestEid);

        AlgoFrame f;
        f.nodes = QSet<QString>(inMST.begin(), inMST.end());
        f.edges = QSet<int>(mstEids.begin(), mstEids.end());
        frames.append(f);
    }

    // Precompute result before animation delay so it doesn't depend on m_graph later
    int totalW = 0;
    QStringList parts;
    for (int eid : mstEids) {
        for (auto it = m_edgeIds.cbegin(); it != m_edgeIds.cend(); ++it) {
            if (it.value() == eid) {
                AppED* e = it.key();
                int w = (int)e->GetW();
                parts << QString("%1–%2 (%3)")
                    .arg(QString::fromStdString(e->v1()->GetName()),
                         QString::fromStdString(e->v2()->GetName()))
                    .arg(w);
                totalW += w;
                break;
            }
        }
    }

    if (inMST.size() < (qsizetype)m_graph->V()) {
        parts.prepend("⚠ Граф несвязный, MST неполное");
    }

    const QString resultText = parts.isEmpty()
        ? "MST: пусто (нет рёбер)"
        : QString("MST:\n%1\n\nСуммарный вес: %2").arg(parts.join("\n")).arg(totalW);

    startAnimation(frames, 500);

    const int delay = frames.size() * 500 + 200;
    QTimer::singleShot(delay, this, [this, resultText] {
        m_resultEdit->setText(resultText);
    });
}

void MainWindow::onRestart()
{
    m_animTimer.stop();
    m_frames.clear();
    m_frameIdx = 0;
    m_finalDist.clear();
    m_scene->clearHighlights();
    if (m_resultEdit) m_resultEdit->clear();
}

// ═══════════════════════════════════════════════════════════════════════════
// Animation
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::startAnimation(const QVector<AlgoFrame>& frames, int ms)
{
    m_frames   = frames;
    m_frameIdx = 0;
    m_animTimer.start(ms);
}

void MainWindow::onAnimTick()
{
    if (m_frameIdx >= m_frames.size()) { m_animTimer.stop(); return; }
    const AlgoFrame& f = m_frames[m_frameIdx++];
    m_scene->highlightVertices(f.nodes);
    m_scene->highlightEdges(f.edges);
}

// ═══════════════════════════════════════════════════════════════════════════
// Scene callbacks
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::onVertexSelected(const QString& id)
{
    m_selV = id;
    m_selE = -1;
    for (int i = 0; i < m_vertList->count(); ++i)
        m_vertList->item(i)->setSelected(m_vertList->item(i)->text() == id);
    m_edgeListW->clearSelection();
}

void MainWindow::onEdgeSelected(int eid)
{
    m_selE = eid;
    m_selV.clear();
    m_vertList->clearSelection();
    syncEdgeList(); // re-sync to set selection
}

void MainWindow::onVertexMoved(const QString& id, qreal x, qreal y)
{
    m_pos[id] = {x, y};
}
