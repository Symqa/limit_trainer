#include "theorycontentwindow.h"
#include "ui_theorycontentwindow.h"
#include "theorycontentmanager.h"
#include "practiceaftertheorywindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QScrollArea>
#include <QPixmap>
#include <QDebug>
#include <QScrollBar>
#include <QScreen>
#include <QGraphicsDropShadowEffect>

TheoryContentWindow::TheoryContentWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TheoryContentWindow)
{
    ui->setupUi(this);
}

TheoryContentWindow::~TheoryContentWindow()
{
    delete ui;
}

// Вспомогательная функция: создать блок "Определение" или "Свойство"
// Возвращает QFrame*, готовый для добавления в родительский layout.
QFrame* TheoryContentWindow::createContentBlock(const QJsonObject &item)
{
    QFrame *frame = new QFrame;
    frame->setFrameStyle(QFrame::StyledPanel);
    frame->setStyleSheet("QFrame { background: #ffffff; border: 1px solid #e0e0e0; border-radius: 8px; }");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(8);
    shadow->setOffset(0, 2);
    shadow->setColor(QColor(0, 0, 0, 30));
    frame->setGraphicsEffect(shadow);

    QVBoxLayout *blockLayout = new QVBoxLayout(frame);
    blockLayout->setContentsMargins(16, 12, 16, 12);
    blockLayout->setSpacing(8);

    QString value = item["value"].toString();
    if (!value.isEmpty()) {
        QLabel *textLabel = new QLabel(value);
        textLabel->setTextFormat(Qt::RichText);
        textLabel->setWordWrap(true);
        textLabel->setStyleSheet("font-size: 12pt; color: #212121; background: transparent; border: none;");
        blockLayout->addWidget(textLabel);
    }

    if (item.contains("image")) {
        QString imgPath = item["image"].toString();
        if (!imgPath.isEmpty()) {
            QPixmap pix(imgPath);
            if (!pix.isNull()) {
                QSize maxSize(300, 150);
                if (pix.width() > maxSize.width() || pix.height() > maxSize.height()) {
                    pix = pix.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
                QWidget *imgContainer = new QWidget;
                imgContainer->setStyleSheet("background: #f5f5f5; border-radius: 6px; padding: 4px;");
                QVBoxLayout *imgLayout = new QVBoxLayout(imgContainer);
                imgLayout->setContentsMargins(4, 4, 4, 4);
                imgLayout->setAlignment(Qt::AlignCenter);
                QLabel *imgLabel = new QLabel;
                imgLabel->setPixmap(pix);
                imgLabel->setAlignment(Qt::AlignCenter);
                imgLabel->setStyleSheet("background: transparent; border: none;");
                imgLayout->addWidget(imgLabel);
                blockLayout->addWidget(imgContainer);
            }
        }
    }
    return frame;
}

void TheoryContentWindow::loadTopic(const QString &topicId)
{
    // Получаем данные темы
    auto *tcm = TheoryContentManager::instance();
    QJsonObject topic = tcm->topicById(topicId);
    if (topic.isEmpty()) return;

    setWindowTitle(topic["title"].toString());

    // Очищаем предыдущее содержимое scrollArea
    QWidget *oldWidget = ui->scrollArea->takeWidget();
    if (oldWidget) {
        oldWidget->deleteLater();
    }

    // Создаём новый виджет для скролла
    QWidget *scrollWidget = new QWidget;
    scrollWidget->setStyleSheet("background: #f0f2f5;"); // общий фон страницы
    QVBoxLayout *mainLayout = new QVBoxLayout(scrollWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // === Заголовок темы ===
    QLabel *titleLabel = new QLabel(topic["title"].toString());
    titleLabel->setStyleSheet(
        "font-size: 22pt; font-weight: bold; color: #0d47a1; margin-bottom: 12px;"
        "background: transparent; border: none;"
        );
    mainLayout->addWidget(titleLabel);

    // === Секция "Определения" ===
    QJsonArray definitions = topic["definitions"].toArray();
    if (!definitions.isEmpty()) {
        QFrame *defSection = new QFrame;
        QVBoxLayout *defSectionLayout = new QVBoxLayout(defSection);
        QLabel *defHeader = new QLabel("Определения");
        defHeader->setStyleSheet("font-size: 16pt; font-weight: bold; color: #1a73e8; margin-bottom: 6px; background: transparent; border: none;");
        defSectionLayout->addWidget(defHeader);

        for (const QJsonValue &val : definitions) {
            QJsonObject obj = val.toObject();
            QString level = obj["level"].toString();

            if (level == "simplified") {
                // Кнопка "Пояснение" в общем стиле
                QToolButton *toggleBtn = new QToolButton;
                toggleBtn->setText("Пояснение ▼");
                toggleBtn->setCheckable(true);
                toggleBtn->setArrowType(Qt::RightArrow);
                toggleBtn->setStyleSheet(
                    "QToolButton {"
                    "  color: #1a73e8;"
                    "  font-weight: bold;"
                    "  font-size: 11pt;"
                    "  background: transparent;"
                    "  border: none;"
                    "  padding: 4px 0px;"
                    "}"
                    "QToolButton:hover { color: #1557b0; }"
                    "QToolButton:checked { color: #0d47a1; }"
                    );

                QWidget *simplifiedWidget = new QWidget;
                simplifiedWidget->setStyleSheet("background: #f5f5f5; border-radius: 6px; border: 1px solid #e0e0e0;");
                QVBoxLayout *simplifiedLayout = new QVBoxLayout(simplifiedWidget);
                simplifiedLayout->setContentsMargins(12, 8, 12, 8);
                QLabel *simplifiedText = new QLabel(obj["value"].toString());
                simplifiedText->setTextFormat(Qt::RichText);
                simplifiedText->setWordWrap(true);
                simplifiedText->setStyleSheet("font-size: 11pt; color: #424242; background: transparent; border: none;");
                simplifiedLayout->addWidget(simplifiedText);
                simplifiedWidget->setVisible(false);

                connect(toggleBtn, &QToolButton::toggled, this, [simplifiedWidget, toggleBtn](bool checked) {
                    simplifiedWidget->setVisible(checked);
                    toggleBtn->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
                });

                defSectionLayout->addWidget(toggleBtn);
                defSectionLayout->addWidget(simplifiedWidget);
            } else {
                QFrame *block = createContentBlock(obj);
                defSectionLayout->addWidget(block);
            }
        }
        mainLayout->addWidget(defSection);
    }

    // === Секция "Свойства" ===
    QJsonArray properties = topic["properties"].toArray();
    if (!properties.isEmpty()) {
        QFrame *propSection = new QFrame;
        QVBoxLayout *propSectionLayout = new QVBoxLayout(propSection);
        QLabel *propHeader = new QLabel("Свойства");
        propHeader->setStyleSheet("font-size: 16pt; font-weight: bold; color: #1a73e8; margin-bottom: 6px; background: transparent; border: none;");
        propSectionLayout->addWidget(propHeader);

        for (const QJsonValue &val : properties) {
            QJsonObject obj = val.toObject();
            QFrame *block = createContentBlock(obj);
            propSectionLayout->addWidget(block);
        }
        mainLayout->addWidget(propSection);
    }

    // === Секция "Примеры вычислений" ===
    QJsonArray examples = topic["worked_examples"].toArray();
    if (!examples.isEmpty()) {
        QFrame *exSection = new QFrame;
        QVBoxLayout *exSectionLayout = new QVBoxLayout(exSection);
        QLabel *exHeader = new QLabel("Примеры вычислений");
        exHeader->setStyleSheet("font-size: 16pt; font-weight: bold; color: #1a73e8; margin-bottom: 6px; background: transparent; border: none;");
        exSectionLayout->addWidget(exHeader);

        for (const QJsonValue &val : examples) {
            QJsonObject obj = val.toObject();
            QString type = obj["type"].toString();

            QFrame *block = new QFrame;
            block->setFrameStyle(QFrame::StyledPanel);
            block->setStyleSheet("QFrame { background: #ffffff; border: 1px solid #e0e0e0; border-radius: 8px; }");
            QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
            shadow->setBlurRadius(8);
            shadow->setOffset(0, 2);
            shadow->setColor(QColor(0, 0, 0, 30));
            block->setGraphicsEffect(shadow);

            QVBoxLayout *blockLayout = new QVBoxLayout(block);
            blockLayout->setContentsMargins(16, 12, 16, 12);
            blockLayout->setSpacing(8);

            if (type == "example_title") {
                QLabel *title = new QLabel("<b>" + obj["value"].toString() + "</b>");
                title->setTextFormat(Qt::RichText);
                title->setWordWrap(true);
                title->setStyleSheet("font-size: 13pt; font-weight: bold; color: #1a73e8; background: transparent; border: none;");
                blockLayout->addWidget(title);
            } else if (type == "step") {
                QLabel *stepLabel = new QLabel(obj["value"].toString());
                stepLabel->setTextFormat(Qt::RichText);
                stepLabel->setWordWrap(true);
                stepLabel->setStyleSheet("font-size: 12pt; color: #212121; background: transparent; border: none;");
                blockLayout->addWidget(stepLabel);
            } else if (type == "formula") {
                if (obj.contains("image")) {
                    QPixmap pix(obj["image"].toString());
                    if (!pix.isNull()) {
                        QSize maxSize(300, 150);
                        if (pix.width() > maxSize.width() || pix.height() > maxSize.height()) {
                            pix = pix.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                        }
                        QWidget *imgContainer = new QWidget;
                        imgContainer->setStyleSheet("background: #f5f5f5; border-radius: 6px; padding: 4px;");
                        QVBoxLayout *imgLayout = new QVBoxLayout(imgContainer);
                        imgLayout->setContentsMargins(4, 4, 4, 4);
                        imgLayout->setAlignment(Qt::AlignCenter);
                        QLabel *imgLabel = new QLabel;
                        imgLabel->setPixmap(pix);
                        imgLabel->setAlignment(Qt::AlignCenter);
                        imgLabel->setStyleSheet("background: transparent; border: none;");
                        imgLayout->addWidget(imgLabel);
                        blockLayout->addWidget(imgContainer);
                    }
                }
            }
            exSectionLayout->addWidget(block);
        }
        mainLayout->addWidget(exSection);
    }

    ui->button_to_practice->setProperty("topic_id", topicId);

    mainLayout->addStretch();
    scrollWidget->setLayout(mainLayout);
    ui->scrollArea->setWidget(scrollWidget);
    ui->scrollArea->setWidgetResizable(true);

    // === Автоматическая подгонка размера окна ===
    scrollWidget->adjustSize();
    QSize contentSize = scrollWidget->sizeHint();
    int frameWidth = ui->scrollArea->frameWidth() * 2;
    int scrollBarWidth = ui->scrollArea->verticalScrollBar()->isVisible()
                             ? ui->scrollArea->verticalScrollBar()->width() : 0;
    int neededWidth = contentSize.width() + frameWidth + scrollBarWidth + 20;
    int neededHeight = contentSize.height() + frameWidth + 20;

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int maxHeight = static_cast<int>(screenRect.height() * 0.85);
    if (neededHeight > maxHeight) {
        neededHeight = maxHeight;
    }

    // Минимальная ширина окна 800 пикселей
    int finalWidth = qMax(neededWidth * 2, 800);
    resize(finalWidth, neededHeight);
}


void TheoryContentWindow::on_button_to_practice_clicked()
{
    QString topicId(ui->button_to_practice->property("topic_id").toString());
    PracticeAfterTheoryWindow window(topicId);

    window.setModal(true);
    window.exec();



}

