#include "practiceselectdialog.h"
#include "topicmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

PracticeSelectDialog::PracticeSelectDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Выбор темы и сложности");
    setupUi();
}

void PracticeSelectDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel *titleLabel = new QLabel("Практика");
    titleLabel->setStyleSheet("font-size: 18pt; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(titleLabel);

    // Выбор темы
    QLabel *topicLabel = new QLabel("Тема:");
    topicLabel->setStyleSheet("color: #ffffff; font-size: 12pt;");
    mainLayout->addWidget(topicLabel);

    m_topicCombo = new QComboBox;
    m_topicCombo->setStyleSheet("QComboBox { background: #ffffff; color: #000000; }");
    TopicManager *tm = TopicManager::instance();
    for (int i = 0; i < tm->topicCount(); ++i) {
        TopicInfo info = tm->topicAt(i);
        m_topicCombo->addItem(info.title, info.id);
    }
    mainLayout->addWidget(m_topicCombo);

    // Выбор сложности
    QLabel *diffLabel = new QLabel("Сложность:");
    diffLabel->setStyleSheet("color: #ffffff; font-size: 12pt;");
    mainLayout->addWidget(diffLabel);

    m_difficultyCombo = new QComboBox;
    m_difficultyCombo->setStyleSheet("QComboBox { background: #ffffff; color: #000000; }");
    m_difficultyCombo->addItem("Все", "all");
    m_difficultyCombo->addItem("Лёгкая", "easy");
    m_difficultyCombo->addItem("Средняя", "medium");
    m_difficultyCombo->addItem("Сложная", "hard");
    mainLayout->addWidget(m_difficultyCombo);

    // Кнопка старта
    m_startBtn = new QPushButton("Начать практику");
    m_startBtn->setStyleSheet(
        "QPushButton { background: #1a73e8; color: white; border: none; border-radius: 6px; padding: 10px 24px; font-size: 12pt; font-weight: bold; }"
        "QPushButton:hover { background: #1557b0; }"
        );
    connect(m_startBtn, &QPushButton::clicked, this, &PracticeSelectDialog::onStartPractice);
    mainLayout->addWidget(m_startBtn);

    setStyleSheet("background-color: #1e1e1e;");
    resize(400, 250);
}

void PracticeSelectDialog::onStartPractice()
{
    if (m_topicCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Ошибка", "Выберите тему.");
        return;
    }
    accept();   // закрываем диалог с результатом QDialog::Accepted
}

QString PracticeSelectDialog::selectedTopicId() const
{
    return m_topicCombo->currentData().toString();
}

QString PracticeSelectDialog::selectedDifficulty() const
{
    return m_difficultyCombo->currentData().toString();
}