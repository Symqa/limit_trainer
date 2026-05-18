#include "practicesessionwindow.h"
#include "practicemanager.h"
#include "topicmanager.h"
#include "progressmanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QFrame>
#include <QScreen>
#include <QGuiApplication>
#include <QIntValidator>
#include <QGraphicsDropShadowEffect>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QTimer>
#include <algorithm>
#include <random>
#include <QDebug>
#include <QScrollBar>

PracticeSessionWindow::PracticeSessionWindow(const QString &topicId, const QString &difficulty,
                                             QWidget *parent)
    : QDialog(parent), m_topicId(topicId), m_difficulty(difficulty)
{
    setWindowTitle("Практика – " + TopicManager::instance()->findById(topicId).title);
    setupUi();
    loadExercises(topicId, difficulty);
}

PracticeSessionWindow::~PracticeSessionWindow() {}

void PracticeSessionWindow::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #1e1e1e; }");
    mainLayout->addWidget(scrollArea);

    QWidget *content = new QWidget;
    content->setStyleSheet("background: #1e1e1e;");
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(20, 15, 20, 15);
    contentLayout->setSpacing(15);

    // Заголовок
    QString topicTitle = TopicManager::instance()->findById(m_topicId).title;
    QString diffText;
    if (m_difficulty == "all") diffText = "все сложности";
    else if (m_difficulty == "easy") diffText = "лёгкая";
    else if (m_difficulty == "medium") diffText = "средняя";
    else if (m_difficulty == "hard") diffText = "сложная";
    QLabel *titleLabel = new QLabel(QString("Практика: %1 (%2)").arg(topicTitle, diffText));
    titleLabel->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ffffff; background: transparent;");
    contentLayout->addWidget(titleLabel);

    // Карточки заданий
    QVBoxLayout *cardsLayout = new QVBoxLayout;
    cardsLayout->setSpacing(12);
    contentLayout->addLayout(cardsLayout);

    // Кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *backBtn = new QPushButton("← Назад к выбору");
    backBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #bbbbbb; border: 1px solid #555; border-radius: 6px; padding: 8px 16px; font-size: 12pt; }"
        "QPushButton:hover { background: #333; }"
        );
    connect(backBtn, &QPushButton::clicked, this, &QDialog::close);
    btnLayout->addWidget(backBtn);
    btnLayout->addStretch();

    m_checkAllBtn = new QPushButton("Проверить все ответы");
    m_checkAllBtn->setStyleSheet(
        "QPushButton { background: #1a73e8; color: white; border: none; border-radius: 6px; padding: 10px 24px; font-size: 12pt; font-weight: bold; }"
        "QPushButton:hover { background: #1557b0; }"
        "QPushButton:disabled { background: #555; color: #aaa; }"
        );
    m_checkAllBtn->setEnabled(false);
    connect(m_checkAllBtn, &QPushButton::clicked, this, &PracticeSessionWindow::onCheckAll);
    btnLayout->addWidget(m_checkAllBtn);

    contentLayout->addLayout(btnLayout);
    scrollArea->setWidget(content);
}

QString PracticeSessionWindow::generateHtml(const QString &questionText, const QStringList &options)
{
    QString numberedOptions;
    for (int i = 0; i < options.size(); ++i) {
        numberedOptions += QString("<p style='margin:4px 0;'>%1) %2</p>").arg(i+1).arg(options[i]);
    }

    QString html = R"(
<html><head>
<script>
MathJax = {
  tex: { inlineMath: [['$','$'], ['\\(','\\)']] }
};
</script>
<script src="qrc:/mathjax/data/tex-chtml.js"></script>
</head><body style="margin:0; padding:8px; font-family:sans-serif; background:transparent; color:#212121;">
<div style="font-size:12pt; margin-bottom:8px;">%1</div>
<div style="font-size:11pt;">%2</div>
</body></html>
)";
    html = html.arg(questionText).arg(numberedOptions);
    return html;
}

void PracticeSessionWindow::loadExercises(const QString &topicId, const QString &difficulty)
{
    PracticeManager *pm = PracticeManager::instance();
    QJsonArray allEx;
    if (difficulty == "all") {
        allEx = pm->exercisesForTopic(topicId);
    } else {
        allEx = pm->exercisesForTopic(topicId, difficulty);
    }
    if (allEx.isEmpty()) {
        QLabel *emptyLabel = new QLabel("Нет заданий для выбранной сложности.");
        emptyLabel->setStyleSheet("color: #ffffff; font-size: 14pt;");
        // Добавляем в cardsLayout, но нужен доступ к cardsLayout... проще проверить до создания виджетов.
        return;
    }

    // Перемешиваем задания (чтобы порядок был случайным)
    QVector<QJsonObject> pool;
    for (const QJsonValue &v : allEx) pool.append(v.toObject());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(pool.begin(), pool.end(), gen);
    m_exercises = pool;

    // Получаем cardsLayout
    QScrollArea *scroll = findChild<QScrollArea*>();
    if (!scroll) return;
    QWidget *content = scroll->widget();
    QVBoxLayout *contentLayout = qobject_cast<QVBoxLayout*>(content->layout());
    if (!contentLayout) return;
    QVBoxLayout *cardsLayout = qobject_cast<QVBoxLayout*>(contentLayout->itemAt(1)->layout());
    if (!cardsLayout) return;

    // Очистка старых виджетов
    while (QLayoutItem *item = cardsLayout->takeAt(0)) {
        if (QWidget *w = item->widget()) w->deleteLater();
        delete item;
    }
    m_exerciseWidgets.clear();

    std::mt19937 localGen(rd());

    // Создаём карточки
    for (int i = 0; i < m_exercises.size(); ++i) {
        const QJsonObject &ex = m_exercises[i];
        QString questionText = ex["question_text"].toString();
        QJsonArray options = ex["options"].toArray();
        QStringList optionStrings;
        for (const QJsonValue &v : options) optionStrings << v.toString();
        QString correctText = optionStrings.first(); // правильный всегда первый

        // Перемешиваем варианты и запоминаем номер правильного
        std::shuffle(optionStrings.begin(), optionStrings.end(), localGen);
        int correctDisplayIndex = optionStrings.indexOf(correctText) + 1; // 1-based

        // Карточка
        QFrame *card = new QFrame;
        card->setFrameStyle(QFrame::StyledPanel);
        card->setStyleSheet("QFrame { background: #ffffff; border: 1px solid #e0e0e0; border-radius: 8px; }");
        // Тени здесь не используем, чтобы не конфликтовать с WebEngine

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 12, 16, 12);
        cardLayout->setSpacing(8);

        // WebEngineView для отображения вопроса и вариантов
        QWebEngineView *eqView = new QWebEngineView;
        eqView->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        eqView->setFocusPolicy(Qt::NoFocus);
        eqView->setStyleSheet("background: transparent;");
        QString html = generateHtml(questionText, optionStrings);
        eqView->setHtml(html, QUrl("qrc:/"));
        connect(eqView, &QWebEngineView::loadFinished, this, [eqView](bool ok) {
            if (!ok) return;
            eqView->page()->runJavaScript(
                "Math.max(document.body.scrollHeight, document.documentElement.scrollHeight)",
                [eqView](const QVariant &result) {
                    int height = result.toInt() + 10;
                    eqView->setFixedHeight(qMax(60, height));
                }
                );
        });
        cardLayout->addWidget(eqView);

        // Поле ввода ответа (чёрный текст)
        QLineEdit *lineEdit = new QLineEdit;
        lineEdit->setPlaceholderText("Введите номер ответа (1–4)");
        lineEdit->setValidator(new QIntValidator(1, 4, lineEdit));
        lineEdit->setStyleSheet(
            "QLineEdit { background: #f0f0f0; color: #000000; border: 1px solid #ccc; border-radius: 4px; padding: 6px; font-size: 12pt; }"
            "QLineEdit:focus { border-color: #1a73e8; }"
            );
        lineEdit->setMaxLength(1);
        connect(lineEdit, &QLineEdit::textChanged, this, &PracticeSessionWindow::validateInputs);
        cardLayout->addWidget(lineEdit);

        // Плашка результата (изначально скрыта)
        QWidget *resultPlaque = new QWidget;
        QHBoxLayout *plaqueLayout = new QHBoxLayout(resultPlaque);
        plaqueLayout->setContentsMargins(8, 4, 8, 4);
        QLabel *plaqueIcon = new QLabel;
        QLabel *plaqueText = new QLabel;
        plaqueIcon->setStyleSheet("font-size: 14pt; font-weight: bold; background: transparent;");
        plaqueText->setStyleSheet("font-size: 11pt; color: #ffffff; background: transparent;");
        plaqueLayout->addWidget(plaqueIcon);
        plaqueLayout->addWidget(plaqueText);
        plaqueLayout->addStretch();
        resultPlaque->setVisible(false);
        cardLayout->addWidget(resultPlaque);

        // Пояснение (скрыто)
        QToolButton *toggleBtn = new QToolButton;
        toggleBtn->setText("Пояснение ▼");
        toggleBtn->setCheckable(true);
        toggleBtn->setArrowType(Qt::RightArrow);
        toggleBtn->setStyleSheet(
            "QToolButton { color: #1a73e8; font-weight: bold; font-size: 11pt; background: transparent; border: none; }"
            "QToolButton:hover { color: #1557b0; }"
            );
        toggleBtn->setVisible(false);

        QWidget *explanationContainer = new QWidget;
        explanationContainer->setStyleSheet("background: #f5f5f5; border-radius: 6px; border: 1px solid #e0e0e0;");
        QVBoxLayout *explLayout = new QVBoxLayout(explanationContainer);
        QLabel *explText = new QLabel(ex["explanation"].toString());
        explText->setTextFormat(Qt::RichText);
        explText->setWordWrap(true);
        explText->setStyleSheet("font-size: 11pt; color: #424242; background: transparent; border: none;");
        explLayout->addWidget(explText);
        explanationContainer->setVisible(false);
        connect(toggleBtn, &QToolButton::toggled, [toggleBtn, explanationContainer](bool checked) {
            explanationContainer->setVisible(checked);
            toggleBtn->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
        });

        cardLayout->addWidget(toggleBtn);
        cardLayout->addWidget(explanationContainer);

        cardsLayout->addWidget(card);

        ExerciseWidgets ew;
        ew.frame = card;
        ew.equationView = eqView;
        ew.lineEdit = lineEdit;
        ew.toggleExplanation = toggleBtn;
        ew.explanationContainer = explanationContainer;
        ew.resultPlaque = resultPlaque;
        ew.plaqueIcon = plaqueIcon;
        ew.plaqueText = plaqueText;
        ew.correctDisplayIndex = correctDisplayIndex;
        m_exerciseWidgets.append(ew);
    }

    // Подгонка размеров окна
    content->adjustSize();
    QSize contentSize = content->sizeHint();
    int frameWidth = scroll->frameWidth() * 2;
    int scrollBarWidth = scroll->verticalScrollBar()->isVisible() ? scroll->verticalScrollBar()->width() : 0;
    int neededWidth = contentSize.width() + frameWidth + scrollBarWidth + 20;
    int neededHeight = contentSize.height() + frameWidth + 20;
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int maxHeight = screenRect.height() * 0.85;
    if (neededHeight > maxHeight) neededHeight = maxHeight;
    int finalWidth = qMin(qMax(neededWidth * 2, 600), screenRect.width() * 9 / 10);
    resize(finalWidth, neededHeight);
}

void PracticeSessionWindow::validateInputs()
{
    bool allFilled = true;
    for (const ExerciseWidgets &ew : m_exerciseWidgets) {
        if (ew.lineEdit->text().trimmed().isEmpty()) {
            allFilled = false;
            break;
        }
    }
    m_checkAllBtn->setEnabled(allFilled);
}

void PracticeSessionWindow::onCheckAll()
{
    int correctCount = 0;
    for (int i = 0; i < m_exerciseWidgets.size(); ++i) {
        ExerciseWidgets &ew = m_exerciseWidgets[i];
        bool ok = false;
        int userAnswer = ew.lineEdit->text().trimmed().toInt(&ok);
        bool isCorrect = ok && (userAnswer == ew.correctDisplayIndex);
        if (isCorrect) ++correctCount;

        // Подсветка рамки
        QString borderColor = isCorrect ? "#c8e6c9" : "#ffcdd2";
        ew.frame->setStyleSheet(
            QString("QFrame { background: #ffffff; border: 1px solid %1; border-radius: 8px; }").arg(borderColor)
            );

        // Обновление плашки
        if (isCorrect) {
            ew.resultPlaque->setStyleSheet("background-color: #4caf50; border-radius: 4px; padding: 4px;");
            ew.plaqueIcon->setText("✓");
            ew.plaqueText->setText("Правильно");
        } else {
            ew.resultPlaque->setStyleSheet("background-color: #f44336; border-radius: 4px; padding: 4px;");
            ew.plaqueIcon->setText("✗");
            ew.plaqueText->setText("Неправильно");
        }
        ew.resultPlaque->setVisible(true);

        // Показываем кнопку пояснения
        ew.toggleExplanation->setVisible(true);
    }

    // Можно добавить завершение темы, если все правильные, но необязательно.
    if (correctCount == m_exerciseWidgets.size()) {
        // опционально: отметить тему пройденной?
        // ProgressManager::instance()->setCompleted(m_topicId, true);
    }
}