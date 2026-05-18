#include "practiceaftertheorywindow.h"
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
#include <QRandomGenerator>
#include <QIntValidator>
#include <QGraphicsDropShadowEffect>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebChannel>
#include <algorithm>
#include <random>
#include <QDebug>
#include <QScrollBar>
#include <QTimer>

// ------------------------------------------------------------
//  Конструктор
// ------------------------------------------------------------
PracticeAfterTheoryWindow::PracticeAfterTheoryWindow(const QString &topicId, QWidget *parent)
    : QDialog(parent), m_topicId(topicId)
{
    setWindowTitle("Практика");
    setupUi();
    loadExercises(topicId);
}

PracticeAfterTheoryWindow::~PracticeAfterTheoryWindow() {
    for (auto &ew : m_exerciseWidgets) {
        ew.equationView->setHtml(""); // очищаем страницу
    }
}

// ------------------------------------------------------------
//  Создание интерфейса (кнопки, скролл)
// ------------------------------------------------------------
void PracticeAfterTheoryWindow::setupUi()
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
    QLabel *titleLabel = new QLabel("Практика: " + topicTitle);
    titleLabel->setStyleSheet("font-size: 20pt; font-weight: bold; color: #ffffff; background: transparent; border: none;");
    contentLayout->addWidget(titleLabel);

    // Карточки заданий
    QVBoxLayout *cardsLayout = new QVBoxLayout;
    cardsLayout->setSpacing(12);
    contentLayout->addLayout(cardsLayout);

    // Кнопки
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *backBtn = new QPushButton("← Назад к теории");
    backBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #bbbbbb; border: 1px solid #555; border-radius: 6px; padding: 8px 16px; font-size: 12pt; }"
        "QPushButton:hover { background: #333; }"
        );
    connect(backBtn, &QPushButton::clicked, this, &QDialog::close);
    btnLayout->addWidget(backBtn);
    btnLayout->addStretch();

    m_checkBtn = new QPushButton("Проверить");
    m_checkBtn->setStyleSheet(
        "QPushButton { background: #1a73e8; color: white; border: none; border-radius: 6px; padding: 10px 24px; font-size: 12pt; font-weight: bold; }"
        "QPushButton:hover { background: #1557b0; }"
        "QPushButton:disabled { background: #555; color: #aaa; }"
        );
    m_checkBtn->setEnabled(false);
    connect(m_checkBtn, &QPushButton::clicked, this, &PracticeAfterTheoryWindow::onCheckAnswers);
    btnLayout->addWidget(m_checkBtn);

    m_finishBtn = new QPushButton("✓ Завершить тему");
    m_finishBtn->setStyleSheet(
        "QPushButton { background: #2e7d32; color: white; border: none; border-radius: 6px; padding: 10px 24px; font-size: 12pt; font-weight: bold; }"
        "QPushButton:hover { background: #1b5e20; }"
        "QPushButton:disabled { background: #555; color: #aaa; }"
        );
    m_finishBtn->setEnabled(false);
    connect(m_finishBtn, &QPushButton::clicked, this, &PracticeAfterTheoryWindow::onFinishTopic);
    btnLayout->addWidget(m_finishBtn);

    contentLayout->addLayout(btnLayout);
    scrollArea->setWidget(content);
}

// ------------------------------------------------------------
//  Генерация HTML-страницы с MathJax для вопроса и вариантов
// ------------------------------------------------------------
QString PracticeAfterTheoryWindow::generateHtml(const QString &questionText, const QStringList &options)
{
    // Нумерованный список вариантов
    QString numberedOptions;
    for (int i = 0; i < options.size(); ++i) {
        numberedOptions += QString("<p style='margin:4px 0;'>%1) %2</p>").arg(i+1).arg(options[i]);
    }

    // Используем локальный MathJax (если он есть в ресурсах) или CDN
    QString html = R"(
<html>
<head>
<script>
MathJax = {
  tex: { inlineMath: [['$','$'], ['\\(','\\)']] }
};
</script>
<script src="qrc:/mathjax/data/tex-chtml.js"></script>
</head>
<body style="margin:0; padding:8px; font-family:sans-serif; background:transparent; color:#212121;">
<div style="font-size:12pt; margin-bottom:8px;">%1</div>
<div style="font-size:11pt;">%2</div>
</body>
</html>
)";
    html = html.arg(questionText).arg(numberedOptions);
    return html;
}

// ------------------------------------------------------------
//  Создание плашки результата (зелёная/красная)
// ------------------------------------------------------------
QWidget* PracticeAfterTheoryWindow::createResultPlaque(ExerciseWidgets &ew)
{
    QWidget *plaque = new QWidget;
    QHBoxLayout *plaqueLayout = new QHBoxLayout(plaque);
    plaqueLayout->setContentsMargins(8, 4, 8, 4);

    QLabel *icon = new QLabel;
    QLabel *text = new QLabel;
    icon->setStyleSheet("font-size: 14pt; font-weight: bold; background: transparent;");
    text->setStyleSheet("font-size: 11pt; color: #ffffff; background: transparent;");

    plaqueLayout->addWidget(icon);
    plaqueLayout->addWidget(text);
    plaqueLayout->addStretch();
    plaque->setVisible(false);

    // Сохраняем указатели в структуре
    ew.plaqueIcon = icon;
    ew.plaqueText = text;
    return plaque;
}

// ------------------------------------------------------------
//  Загрузка трёх случайных заданий
// ------------------------------------------------------------
void PracticeAfterTheoryWindow::loadExercises(const QString &topicId)
{
    PracticeManager *pm = PracticeManager::instance();
    QJsonArray easy = pm->exercisesForTopic(topicId, "easy");
    QJsonArray medium = pm->exercisesForTopic(topicId, "medium");
    QJsonArray hard = pm->exercisesForTopic(topicId, "hard");

    auto pickRandom = [](const QJsonArray &arr) -> QJsonObject {
        if (arr.isEmpty()) return QJsonObject();
        int idx = QRandomGenerator::global()->bounded(arr.size());
        return arr[idx].toObject();
    };

    QJsonObject easyEx = pickRandom(easy);
    QJsonObject medEx = pickRandom(medium);
    QJsonObject hardEx = pickRandom(hard);

    QVector<QJsonObject> selected;
    if (!easyEx.isEmpty()) selected.append(easyEx);
    if (!medEx.isEmpty()) selected.append(medEx);
    if (!hardEx.isEmpty()) selected.append(hardEx);

    // Добираем случайные задания, если какого-то уровня нет
    QJsonArray all = pm->exercisesForTopic(topicId);
    QVector<QJsonObject> pool;
    for (const QJsonValue &v : all) pool.append(v.toObject());
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(pool.begin(), pool.end(), gen);
    for (const QJsonObject &ex : pool) {
        if (selected.size() >= 3) break;
        if (!selected.contains(ex)) selected.append(ex);
    }
    m_exercises = selected;

    // Получаем layout карточек
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

        // Перемешиваем варианты
        std::shuffle(optionStrings.begin(), optionStrings.end(), localGen);
        int correctDisplayIndex = optionStrings.indexOf(correctText) + 1; // 1-based

        // Карточка
        QFrame *card = new QFrame;
        card->setFrameStyle(QFrame::StyledPanel);
        card->setStyleSheet("QFrame { background: #ffffff; border: 1px solid #e0e0e0; border-radius: 8px; }");
        // QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
        // shadow->setBlurRadius(8);
        // shadow->setOffset(0, 2);
        // shadow->setColor(QColor(0, 0, 0, 30));
        // card->setGraphicsEffect(shadow);

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 12, 16, 12);
        cardLayout->setSpacing(8);

        // WebEngineView для отображения вопроса и вариантов
        QWebEngineView *eqView = new QWebEngineView;
        eqView->setStyleSheet("background: transparent;");
        eqView->setAttribute(Qt::WA_TransparentForMouseEvents, true); // теперь клики проходят сквозь
        eqView->setFocusPolicy(Qt::NoFocus);
        eqView->setMinimumHeight(60);
        // Загружаем HTML с MathJax
        QString html = generateHtml(questionText, optionStrings);
        eqView->setHtml(html, QUrl("qrc:/"));  // база для CDN
        // Динамически подгоняем высоту после загрузки страницы
        connect(eqView, &QWebEngineView::loadFinished, this, [eqView](bool ok) {
            if (!ok) return;
            eqView->page()->runJavaScript(
                "Math.max(document.body.scrollHeight, document.documentElement.scrollHeight)",
                [eqView](const QVariant &result) {
                    int height = result.toInt() + 10; // небольшой запас
                    eqView->setFixedHeight(qMax(60, height));
                }
                );
        });
        cardLayout->addWidget(eqView);

        // Поле ввода (чёрный текст)
        QLineEdit *lineEdit = new QLineEdit;
        lineEdit->setPlaceholderText("Введите номер ответа (1–4)");
        lineEdit->setValidator(new QIntValidator(1, 4, lineEdit));
        lineEdit->setStyleSheet(
            "QLineEdit { background: #f0f0f0; color: #000000; border: 1px solid #ccc; border-radius: 4px; padding: 6px; font-size: 12pt; }"
            "QLineEdit:focus { border-color: #1a73e8; }"
            );
        lineEdit->setMaxLength(1);
        connect(lineEdit, &QLineEdit::textChanged, this, &PracticeAfterTheoryWindow::validateInputs);
        cardLayout->addWidget(lineEdit);

        // Плашка результата (изначально скрыта)
        ExerciseWidgets ew;   // временная структура для передачи
        ew.plaqueIcon = nullptr;
        ew.plaqueText = nullptr;
        QWidget *resultPlaque = createResultPlaque(ew);
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

        // Заполняем структуру
        ExerciseWidgets ewFinal;
        ewFinal.frame = card;
        ewFinal.equationView = eqView;
        ewFinal.lineEdit = lineEdit;
        ewFinal.toggleExplanation = toggleBtn;
        ewFinal.explanationContainer = explanationContainer;
        ewFinal.resultPlaque = resultPlaque;
        ewFinal.plaqueIcon = ew.plaqueIcon;   // из createResultPlaque
        ewFinal.plaqueText = ew.plaqueText;
        ewFinal.correctDisplayIndex = correctDisplayIndex;
        ewFinal.shuffledOptions = optionStrings;
        m_exerciseWidgets.append(ewFinal);
    }

    // Подгонка размера окна (как в теории)
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

// ------------------------------------------------------------
//  Валидация: все поля заполнены → кнопка активна
// ------------------------------------------------------------
void PracticeAfterTheoryWindow::validateInputs()
{
    bool allFilled = true;
    for (const ExerciseWidgets &ew : m_exerciseWidgets) {
        if (ew.lineEdit->text().trimmed().isEmpty()) {
            allFilled = false;
            break;
        }
    }
    m_checkBtn->setEnabled(allFilled);
}

// ------------------------------------------------------------
//  Проверка ответов
// ------------------------------------------------------------
void PracticeAfterTheoryWindow::onCheckAnswers()
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
    m_finishBtn->setEnabled(correctCount == m_exerciseWidgets.size());
}

// ------------------------------------------------------------
//  Завершение темы (отмечает как пройденную)
// ------------------------------------------------------------
void PracticeAfterTheoryWindow::onFinishTopic()
{
    ProgressManager::instance()->setCompleted(m_topicId, true);
    accept();
}