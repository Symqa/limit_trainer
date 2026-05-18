#ifndef PRACTICEAFTERTHEORYWINDOW_H
#define PRACTICEAFTERTHEORYWINDOW_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector>
#include <QLineEdit>
#include <QToolButton>
#include <QWebEngineView>

class QFrame;
class QPushButton;

class PracticeAfterTheoryWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PracticeAfterTheoryWindow(const QString &topicId, QWidget *parent = nullptr);
    ~PracticeAfterTheoryWindow();

private slots:
    void onCheckAnswers();
    void onFinishTopic();
    void validateInputs();

private:
    struct ExerciseWidgets {
        QFrame *frame;
        QWebEngineView *equationView;   // отображение вопроса + вариантов
        QLineEdit *lineEdit;
        QToolButton *toggleExplanation;
        QWidget *explanationContainer;
        QWidget *resultPlaque;          // плашка с иконкой и текстом
        QLabel *plaqueIcon;
        QLabel *plaqueText;
        int correctDisplayIndex;        // правильный номер (1..4) в отображаемом списке
        QStringList shuffledOptions;    // перемешанные варианты (тексты)
    };

    void loadExercises(const QString &topicId);
    void setupUi();
    QWidget* createResultPlaque(ExerciseWidgets &ew);
    QString generateHtml(const QString &questionText, const QStringList &options);

    QString m_topicId;
    QVector<ExerciseWidgets> m_exerciseWidgets;
    QPushButton *m_checkBtn;
    QPushButton *m_finishBtn;
    QVector<QJsonObject> m_exercises;
};

#endif // PRACTICEAFTERTHEORYWINDOW_H