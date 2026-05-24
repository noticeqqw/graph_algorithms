#include "ControlPanel.h"
#include "GraphCanvas.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>

static QLabel* heading(const QString& text) {
    auto* l = new QLabel(text);
    l->setProperty("heading", true);
    l->setStyleSheet("color:#e0e0e0; font-weight:bold; font-size:12px; padding: 4px 0 2px 0;");
    return l;
}

ControlPanel::ControlPanel(AppGraph** g, GraphCanvas* c, QWidget* parent)
    : QWidget(parent), graphPtr(g), canvas(c) {
    setFixedWidth(210);
    setStyleSheet("background-color: #1a1a1a; border-right: 1px solid #2e2e2e;");

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(4);

    lay->addWidget(heading("Вершины"));
    vertexList = new QListWidget;
    vertexList->setAlternatingRowColors(true);
    vertexList->setMaximumHeight(120);
    lay->addWidget(vertexList);

    nameInput = new QLineEdit;
    nameInput->setPlaceholderText("Имя вершины");
    lay->addWidget(nameInput);

    auto* btnAddV = new QPushButton("+ Добавить вершину");
    auto* btnDelV = new QPushButton("− Удалить выбранную");
    btnDelV->setStyleSheet("color: #e57373; border-color: #c62828;");
    lay->addWidget(btnAddV);
    lay->addWidget(btnDelV);

    auto* line1 = new QFrame; line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("color: #333;");
    lay->addWidget(line1);

    lay->addWidget(heading("Рёбра"));
    edgeList = new QListWidget;
    edgeList->setAlternatingRowColors(true);
    edgeList->setMaximumHeight(140);
    lay->addWidget(edgeList);

    v1Input = new QLineEdit; v1Input->setPlaceholderText("Вершина 1");
    v2Input = new QLineEdit; v2Input->setPlaceholderText("Вершина 2");
    weightInput = new QLineEdit; weightInput->setPlaceholderText("Вес");
    weightInput->setText("1");
    lay->addWidget(v1Input);
    lay->addWidget(v2Input);
    lay->addWidget(weightInput);

    auto* btnAddE = new QPushButton("+ Добавить ребро");
    auto* btnDelE = new QPushButton("− Удалить ребро");
    btnDelE->setStyleSheet("color: #e57373; border-color: #c62828;");
    lay->addWidget(btnAddE);
    lay->addWidget(btnDelE);

    lay->addStretch(1);

    connect(btnAddV, &QPushButton::clicked, this, &ControlPanel::onAddVertex);
    connect(btnDelV, &QPushButton::clicked, this, &ControlPanel::onDelVertex);
    connect(btnAddE, &QPushButton::clicked, this, &ControlPanel::onAddEdge);
    connect(btnDelE, &QPushButton::clicked, this, &ControlPanel::onDelEdge);
}

void ControlPanel::refreshLists() {
    vertexList->clear();
    edgeList->clear();
    if (!*graphPtr) return;
    AppGraph* g = *graphPtr;
    for (AppVD* v : g->vertices)
        vertexList->addItem(QString::fromStdString(v->GetName()));
    QString arrow = g->Directed() ? " → " : " — ";
    auto& verts = g->vertices;
    for (AppED* e : g->storage->GetAllEdges()) {
        if (!g->Directed()) {
            int i1 = -1, i2 = -1;
            for (int i = 0; i < (int)verts.size(); ++i) {
                if (verts[i] == e->v1()) i1 = i;
                if (verts[i] == e->v2()) i2 = i;
            }
            if (i1 > i2) continue;
        }
        QString s = QString::fromStdString(e->v1()->GetName()) + arrow
                  + QString::fromStdString(e->v2()->GetName())
                  + QString(" (w=%1)").arg((int)e->GetW());
        edgeList->addItem(s);
    }
}

void ControlPanel::onAddVertex() {
    if (!*graphPtr) return;
    std::string name = nameInput->text().toStdString();
    if (name.empty()) return;
    (*graphPtr)->InsertV(name);
    nameInput->clear();
    refreshLists();
    canvas->setGraph(*graphPtr);
}

void ControlPanel::onDelVertex() {
    if (!*graphPtr) return;
    auto* item = vertexList->currentItem();
    if (!item) return;
    AppVD* v = (*graphPtr)->FindVertex(item->text().toStdString());
    if (v) (*graphPtr)->DeleteV(v);
    refreshLists();
    canvas->setGraph(*graphPtr);
}

void ControlPanel::onAddEdge() {
    if (!*graphPtr) return;
    AppVD* v1 = (*graphPtr)->FindVertex(v1Input->text().toStdString());
    AppVD* v2 = (*graphPtr)->FindVertex(v2Input->text().toStdString());
    if (!v1 || !v2) { QMessageBox::warning(this, "Ошибка", "Вершина не найдена"); return; }
    bool ok; double w = weightInput->text().toDouble(&ok);
    if (!ok) w = 1.0;
    (*graphPtr)->InsertE(v1, v2, w);
    refreshLists();
    canvas->update();
}

void ControlPanel::onDelEdge() {
    if (!*graphPtr) return;
    AppVD* v1 = (*graphPtr)->FindVertex(v1Input->text().toStdString());
    AppVD* v2 = (*graphPtr)->FindVertex(v2Input->text().toStdString());
    if (!v1 || !v2) return;
    (*graphPtr)->DeleteE(v1, v2);
    refreshLists();
    canvas->update();
}
