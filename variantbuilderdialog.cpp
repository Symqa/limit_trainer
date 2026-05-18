#include "variantbuilderdialog.h"
#include "topicmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>

VariantBuilderDialog::VariantBuilderDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Составитель варианта");
    setupUi();
}

void VariantBuilderDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    // Заголовок
    QLabel *titleLabel = new QLabel("Выберите темы для варианта:");
    titleLabel->setStyleSheet("font-size: 14pt; font-weight: bold; color: #ffffff;");
    mainLayout->addWidget(titleLabel);

    // Прокручиваемая область с чекбоксами тем
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; }");
    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *checksLayout = new QVBoxLayout(scrollWidget);
    checksLayout->setSpacing(4);

    TopicManager *tm = TopicManager::instance();
    for (int i = 0; i < tm->topicCount(); ++i) {
        TopicInfo info = tm->topicAt(i);
        QCheckBox *cb = new QCheckBox(info.title);
        cb->setStyleSheet("color: #ffffff; font-size: 11pt;");
        cb->setProperty("topicId", info.id);
        checksLayout->addWidget(cb);
        m_topicChecks.append(cb);
    }
    checksLayout->addStretch();
    scrollWidget->setLayout(checksLayout);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);

    // Количество заданий
    QHBoxLayout *countLayout = new QHBoxLayout;
    QLabel *countLabel = new QLabel("Заданий в варианте:");
    countLabel->setStyleSheet("color: #ffffff; font-size: 11pt;");
    countLayout->addWidget(countLabel);
    m_taskCountSpin = new QSpinBox;
    m_taskCountSpin->setMinimum(3);
    m_taskCountSpin->setMaximum(50);
    m_taskCountSpin->setValue(10);
    m_taskCountSpin->setStyleSheet("QSpinBox { background: #ffffff; color: #000000; padding: 4px; }");
    countLayout->addWidget(m_taskCountSpin);
    countLayout->addStretch();
    mainLayout->addLayout(countLayout);

    // Кнопка построить
    m_buildBtn = new QPushButton("Построить вариант");
    m_buildBtn->setStyleSheet(
        "QPushButton { background: #1a73e8; color: white; border: none; border-radius: 6px; padding: 10px 24px; font-size: 12pt; font-weight: bold; }"
        "QPushButton:hover { background: #1557b0; }"
        );
    connect(m_buildBtn, &QPushButton::clicked, this, &VariantBuilderDialog::onBuild);
    mainLayout->addWidget(m_buildBtn);

    setStyleSheet("background-color: #1e1e1e;");
    resize(400, 500);
}

void VariantBuilderDialog::onBuild()
{
    // Проверяем, что выбрана хотя бы одна тема
    QStringList ids;
    for (QCheckBox *cb : m_topicChecks) {
        if (cb->isChecked()) {
            ids << cb->property("topicId").toString();
        }
    }
    if (ids.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите хотя бы одну тему.");
        return;
    }
    accept();   // Закрываем диалог с результатом Accepted
}

QStringList VariantBuilderDialog::selectedTopicIds() const
{
    QStringList ids;
    for (QCheckBox *cb : m_topicChecks) {
        if (cb->isChecked()) {
            ids << cb->property("topicId").toString();
        }
    }
    return ids;
}

int VariantBuilderDialog::taskCount() const
{
    return m_taskCountSpin->value();
}