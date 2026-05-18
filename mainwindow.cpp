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

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDialog>
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
    setWindowTitle("Тренажёр пределов");

    connect(ui->practiceBtn, &QPushButton::clicked, this, [this]() {
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
        // Создаём новое окно (в куче, атрибут DeleteOnClose, чтобы само удалилось)
        VariantSessionWindow *session = new VariantSessionWindow(ids, count);
        session->setAttribute(Qt::WA_DeleteOnClose);
        session->exec();   // или show()
    }
}

