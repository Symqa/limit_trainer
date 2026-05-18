#ifndef PRACTICESESSIONWINDOW_H
#define PRACTICESESSIONWINDOW_H

#include <QDialog>
#include <QVector>
#include <QJsonObject>
#include <QLineEdit>
#include <QToolButton>
#include <QWebEngineView>
#include <QPushButton>

class PracticeSessionWindow : public QDialog
{
    Q_OBJECT
public:
    explicit PracticeSessionWindow(const QString &topicId, const QString &difficulty,
                                   QWidget *parent = nullptr);
    ~PracticeSessionWindow();

private slots:
    void onCheckAll();
    void validateInputs();

private:
    struct ExerciseWidgets {
        QFrame *frame;
        QWebEngineView *equationView;
        QLineEdit *lineEdit;
        QToolButton *toggleExplanation;
        QWidget *explanationContainer;
        QWidget *resultPlaque;
        QLabel *plaqueIcon;
        QLabel *plaqueText;
        int correctDisplayIndex;
    };

    void loadExercises(const QString &topicId, const QString &difficulty);
    void setupUi();
    QString generateHtml(const QString &questionText, const QStringList &options);

    QString m_topicId;
    QString m_difficulty;
    QVector<ExerciseWidgets> m_exerciseWidgets;
    QPushButton *m_checkAllBtn;
    QPushButton *m_finishBtn;   // необязательно, можно оставить для совместимости
    QVector<QJsonObject> m_exercises;
};

#endif // PRACTICESESSIONWINDOW_H