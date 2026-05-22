#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "theorywindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QGuiApplication>

#include "formulawidget.h"
#include "practicesessionwindow.h"
#include "practiceselectdialog.h"
#include "variantbuilderdialog.h"
#include "variantsessionwindow.h"

#include <QWebEngineView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Заголовок окна
    setWindowTitle("Тренажёр по пределам");
    setMinimumSize(500, 600);

    // --- Градиентный фон главного окна ---
    setStyleSheet(R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #1a1a2e, stop:1 #16213e);
        }
    )");

    // Центральный виджет с вертикальным layout
    QWidget *central = new QWidget;
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(25);

    // --- Заголовок и подзаголовок ---
    QLabel *title = new QLabel("Пределы функций");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:32pt; font-weight:bold; color:#e0e0e0; background:transparent;");

    QLabel *subtitle = new QLabel("Изучайте теорию, решайте задачи, составляйте варианты");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("font-size:14pt; color:#a0a0a0; background:transparent;");

    mainLayout->addStretch(1);
    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);
    mainLayout->addSpacing(30);

    // --- Общий стиль кнопок ---
    QString btnStyle = R"(
        QPushButton {
            background: #0f3460;
            color: white;
            border: none;
            border-radius: 12px;
            padding: 20px 40px;
            font-size: 16pt;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #1a1a40;
            border: 2px solid #e94560;
        }
        QPushButton:pressed {
            background: #16213e;
        }
    )";

    // Функция для создания тени
    auto addShadow = [](QWidget *w) {
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        shadow->setBlurRadius(15);
        shadow->setOffset(0, 4);
        shadow->setColor(QColor(0, 0, 0, 80));
        w->setGraphicsEffect(shadow);
    };

    // --- Кнопка "Обучение" ---
    QPushButton *learnBtn = ui->button_learn;
    learnBtn->setStyleSheet(btnStyle);
    addShadow(learnBtn);
    mainLayout->addWidget(learnBtn, 0, Qt::AlignCenter);

    // --- Кнопка "Практика" ---
    QPushButton *practiceBtn = ui->practiceBtn;
    practiceBtn->setStyleSheet(btnStyle);
    addShadow(practiceBtn);
    mainLayout->addWidget(practiceBtn, 0, Qt::AlignCenter);

    // --- Кнопка "Составитель вариантов" ---
    QPushButton *variantBtn = ui->button_variant_creator;
    variantBtn->setStyleSheet(btnStyle);
    addShadow(variantBtn);
    mainLayout->addWidget(variantBtn, 0, Qt::AlignCenter);

    mainLayout->addStretch(2);

    // Подключение слотов
    connect(practiceBtn, &QPushButton::clicked, this, [this]() {
        PracticeSelectDialog dlg;
        if (dlg.exec() == QDialog::Accepted) {
            QString topicId = dlg.selectedTopicId();
            QString difficulty = dlg.selectedDifficulty();
            PracticeSessionWindow *session = new PracticeSessionWindow(topicId, difficulty, this);
            session->setAttribute(Qt::WA_DeleteOnClose);
            session->exec();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_button_learn_clicked()
{
    TheoryWindow window;
    window.setModal(true);
    window.exec();
}

void MainWindow::on_button_variant_creator_clicked()
{
    VariantBuilderDialog builder;
    if (builder.exec() == QDialog::Accepted) {
        QStringList ids = builder.selectedTopicIds();
        int count = builder.taskCount();
        VariantSessionWindow *session = new VariantSessionWindow(ids, count);
        session->setAttribute(Qt::WA_DeleteOnClose);
        session->exec();
    }
}