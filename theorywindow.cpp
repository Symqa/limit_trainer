#include "theorywindow.h"
#include "ui_theorywindow.h"

#include "theorycontentwindow.h"
#include "topicmanager.h"
#include "progressmanager.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QMessageBox>

#include <QLabel>
#include <QCheckBox>
#include <QDebug>

TheoryWindow::TheoryWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TheoryWindow)
{
    ui->setupUi(this);
    setWindowTitle("Обучение");
    setupTopics();
    // Никаких динамических соединений с ProgressManager
}

TheoryWindow::~TheoryWindow()
{
    delete ui;
}

void TheoryWindow::setupTopics()
{
    // Удаляем старый центральный виджет (если есть)
    QWidget *oldWidget = ui->scroll_topics->takeWidget();
    if (oldWidget) {
        delete oldWidget;  // немедленное удаление, без deleteLater
        oldWidget = nullptr;
    }

    QWidget *scroll_widget = new QWidget;
    scroll_widget->setStyleSheet("background-color: #3c3c3c;");
    QVBoxLayout *layout = new QVBoxLayout(scroll_widget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(6);

    TopicManager *tm = TopicManager::instance();
    ProgressManager *pm = ProgressManager::instance();

    for (int i = 0; i < tm->topicCount(); ++i) {
        TopicInfo topic = tm->topicAt(i);
        bool completed = pm->isCompleted(topic.id);

        // Название темы
        QLabel *label = new QLabel(QString("%1. %2").arg(i + 1).arg(topic.title));
        label->setWordWrap(false);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        label->setStyleSheet("font-size: 12pt; color: #ffffff;");

        // Индикатор статуса (чекбокс)
        QCheckBox *checkBox = new QCheckBox;
        checkBox->setChecked(completed);
        checkBox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        checkBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        if (completed) {
            checkBox->setText("Пройдено");
            checkBox->setStyleSheet("font-size: 11pt; color: #4CAF50; font-weight: bold;");
        } else {
            checkBox->setText("Не изучено");
            checkBox->setStyleSheet("font-size: 11pt; color: #ecf0f1;");
        }

        QHBoxLayout *rowLayout = new QHBoxLayout;
        rowLayout->addWidget(label);
        rowLayout->addWidget(checkBox);
        rowLayout->addStretch();
        rowLayout->setAlignment(Qt::AlignVCenter);
        layout->addLayout(rowLayout);
    }

    ui->scroll_topics->setWidget(scroll_widget);
    ui->scroll_topics->setWidgetResizable(true);

    // Размеры окна
    scroll_widget->layout()->activate();
    QSize contentSize = scroll_widget->sizeHint();
    int frameWidth = ui->scroll_topics->frameWidth() * 2;
    int scrollBarWidth = ui->scroll_topics->verticalScrollBar()->isVisible()
                             ? ui->scroll_topics->verticalScrollBar()->width() : 0;
    int neededWidth = contentSize.width() + frameWidth + scrollBarWidth + 20;
    int neededHeight = contentSize.height() + frameWidth + 40;

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int desiredHeight = qMin(static_cast<int>(neededHeight * 1.5),
                             static_cast<int>(screenRect.height() * 0.85));
    int finalHeight = qMax(desiredHeight, neededHeight);
    int finalWidth = qMin(qMax(neededWidth * 2, 600), screenRect.width() * 9 / 10);
    resize(finalWidth, finalHeight);

    // Обновляем кнопку «Продолжить» и проверяем завершение курса
    updateContinueButton();
    checkAllCompleted();
}

void TheoryWindow::updateContinueButton()
{
    TopicManager *tm = TopicManager::instance();
    ProgressManager *pm = ProgressManager::instance();

    int lastLearned = -1;
    for (int i = 0; i < tm->topicCount(); ++i) {
        if (pm->isCompleted(tm->topicAt(i).id)) {
            lastLearned = i;
        }
    }

    if (lastLearned >= 0) {
        int nextIndex = (lastLearned + 1 < tm->topicCount()) ? lastLearned + 1 : lastLearned;
        TopicInfo nextTopic = tm->topicAt(nextIndex);
        ui->button_to_topic->setText(
            QString("Продолжить: Тема %1. %2")
                .arg(nextIndex + 1)
                .arg(nextTopic.title)
            );
        ui->button_to_topic->setProperty("topic_id", nextTopic.id);
    } else {
        ui->button_to_topic->setText("Начать обучение");
        ui->button_to_topic->setProperty("topic_id", tm->topicAt(0).id);
    }
}

void TheoryWindow::checkAllCompleted()
{
    TopicManager *tm = TopicManager::instance();
    ProgressManager *pm = ProgressManager::instance();

    bool allCompleted = true;
    for (int i = 0; i < tm->topicCount(); ++i) {
        if (!pm->isCompleted(tm->topicAt(i).id)) {
            allCompleted = false;
            break;
        }
    }

    if (allCompleted) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Курс завершён");
        msgBox.setText("🎉 Все темы изучены!\nХотите сбросить прогресс и пройти курс заново?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setStyleSheet("QLabel { color: white; font-size: 12pt; } QPushButton { min-width: 80px; }");

        if (msgBox.exec() == QMessageBox::Yes) {
            pm->resetAll();
            setupTopics();   // перерисовываем список (статусы сброшены)
        }
    }
}

void TheoryWindow::on_button_to_topic_clicked()
{
    QString topicId = ui->button_to_topic->property("topic_id").toString();
    if (topicId.isEmpty())
        return;

    TheoryContentWindow *window = new TheoryContentWindow(this);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->loadTopic(topicId);
    window->setModal(true);
    window->exec();

    // После закрытия окна теории (а может быть и практики) обновляем список тем
    setupTopics();
}