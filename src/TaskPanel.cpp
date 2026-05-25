#include "TaskPanel.h"
#include "GraphCanvas.h"
#include "ControlPanel.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

static QLabel* heading(const QString& text) {
    auto* l = new QLabel(text);
    l->setStyleSheet("color:#e0e0e0; font-weight:bold; font-size:12px; padding: 4px 0 2px 0;");
    return l;
}

static QLabel* sub(const QString& text) {
    auto* l = new QLabel(text);
    l->setStyleSheet("color:#888; font-size:11px;");
    return l;
}

TaskPanel::TaskPanel(AppGraph** g, GraphCanvas* c, ControlPanel* ctrl, QWidget* parent)
    : QWidget(parent), graphPtr(g), canvas(c), control(ctrl) {
    setFixedWidth(230);
    setStyleSheet("background-color: #1a1a1a; border-left: 1px solid #2e2e2e;");

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(10, 10, 10, 10);
    lay->setSpacing(5);

    // BFS
    lay->addWidget(heading("Задача 2"));
    lay->addWidget(sub("Стартовая вершина:"));
    bfsStartInput = new QLineEdit;
    bfsStartInput->setPlaceholderText("Введите вершину...");
    lay->addWidget(bfsStartInput);
    lay->addWidget(sub("Расстояние d:"));
    bfsDSpin = new QSpinBox;
    bfsDSpin->setRange(1, 19);
    lay->addWidget(bfsDSpin);
    auto* btnBFS = new QPushButton("Запустить BFS");
    btnBFS->setObjectName("btnBFS");
    btnBFS->setFixedHeight(30);
    lay->addWidget(btnBFS);

    auto* line1 = new QFrame; line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet("color:#333; margin: 4px 0;");
    lay->addWidget(line1);

    // Prim
    lay->addWidget(heading("Задача 3"));
    lay->addWidget(sub("Стартовая вершина:"));
    primStartInput = new QLineEdit;
    primStartInput->setPlaceholderText("Введите вершину...");
    lay->addWidget(primStartInput);
    auto* btnPrim = new QPushButton("Запустить Prim");
    btnPrim->setObjectName("btnPrim");
    btnPrim->setFixedHeight(30);
    lay->addWidget(btnPrim);

    auto* line2 = new QFrame; line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet("color:#333; margin: 4px 0;");
    lay->addWidget(line2);

    auto* btnRestart = new QPushButton("Restart");
    btnRestart->setObjectName("btnRestart");
    lay->addWidget(btnRestart);

    auto* line3 = new QFrame; line3->setFrameShape(QFrame::HLine);
    line3->setStyleSheet("color:#333; margin: 4px 0;");
    lay->addWidget(line3);

    lay->addWidget(sub("Результат:"));
    resultOutput = new QTextEdit;
    resultOutput->setReadOnly(true);
    resultOutput->setPlaceholderText("Здесь появится результат...");
    lay->addWidget(resultOutput, 1);

    connect(btnBFS,     &QPushButton::clicked, this, &TaskPanel::onRunBFS);
    connect(btnPrim,    &QPushButton::clicked, this, &TaskPanel::onRunPrim);
    connect(btnRestart, &QPushButton::clicked, this, &TaskPanel::onRestart);
}

TaskPanel::~TaskPanel() {
    delete bfsTask;
    delete primTask;
}

void TaskPanel::onRunBFS() {
    if (!*graphPtr) { resultOutput->setText("Граф не создан."); return; }
    AppVD* start = (*graphPtr)->FindVertex(bfsStartInput->text().toStdString());
    if (!start) { resultOutput->setText("Вершина не найдена."); return; }
    int d = bfsDSpin->value();

    delete bfsTask;
    bfsTask = new AppBFS(*graphPtr, start, d);
    lastTask = LastTask::BFS;

    auto res = bfsTask->Result();
    QString out = QString("Вершины на расст. %1 от %2:\n").arg(d).arg(bfsStartInput->text());
    for (AppVD* v : res) out += QString::fromStdString(v->GetName()) + "  ";
    if (res.empty()) out += "(нет)";
    resultOutput->setText(out);
    canvas->setBFSResult(res);
}

void TaskPanel::onRunPrim() {
    if (!*graphPtr) { resultOutput->setText("Граф не создан."); return; }
    AppVD* start = (*graphPtr)->FindVertex(primStartInput->text().toStdString());
    if (!start) { resultOutput->setText("Вершина не найдена."); return; }

    delete primTask;
    primTask = new AppPrim(*graphPtr, start);
    lastTask = LastTask::Prim;

    auto res = primTask->Result();
    QString out = "MST рёбра:\n";
    for (AppED* e : res.edges)
        out += QString::fromStdString(e->v1()->GetName()) + "−"
             + QString::fromStdString(e->v2()->GetName())
             + QString(" (w=%1)\n").arg((int)e->GetW());
    out += QString("Итого: %1").arg(res.totalWeight);
    resultOutput->setText(out);
    canvas->setMSTResult(res.edges);
}

void TaskPanel::onRestart() {
    if (lastTask == LastTask::BFS && bfsTask) {
        bfsTask->Set(*graphPtr);
        auto res = bfsTask->Result();
        QString out = "BFS Restart:\n";
        for (AppVD* v : res) out += QString::fromStdString(v->GetName()) + "  ";
        resultOutput->setText(out);
        canvas->setBFSResult(res);
    } else if (lastTask == LastTask::Prim && primTask) {
        primTask->Set(*graphPtr);
        auto res = primTask->Result();
        QString out = "Prim Restart:\n";
        for (AppED* e : res.edges)
            out += QString::fromStdString(e->v1()->GetName()) + "−"
                 + QString::fromStdString(e->v2()->GetName())
                 + QString(" (w=%1)\n").arg((int)e->GetW());
        out += QString("Итого: %1").arg(res.totalWeight);
        resultOutput->setText(out);
        canvas->setMSTResult(res.edges);
    } else {
        resultOutput->setText("Нет сохранённой задачи.");
    }
}
