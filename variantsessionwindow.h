#ifndef VARIANTSESSIONWINDOW_H
#define VARIANTSESSIONWINDOW_H

#include <QDialog>
#include <QVector>
#include <QJsonObject>
#include <QLineEdit>
#include <QToolButton>
#include <QWebEngineView>
#include <QPushButton>
#include <QLabel>

class VariantSessionWindow : public QDialog
{
    Q_OBJECT
public:
    explicit VariantSessionWindow(const QStringList &topicIds, int taskCount,
                                  QWidget *parent = nullptr);
    ~VariantSessionWindow() = default;   // явно ничего не делаем

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

    void buildVariant(const QStringList &topicIds, int taskCount);
    void setupUi();
    QString generateHtml(const QString &questionText, const QStringList &options);
    void buildAnswerKey();

    QVector<ExerciseWidgets> m_exerciseWidgets;
    QPushButton *m_checkAllBtn;
    QVector<QJsonObject> m_exercises;

    QToolButton *m_answerKeyToggle;
    QWidget *m_answerKeyContainer;
    QLabel *m_answerKeyText;
};

#endif // VARIANTSESSIONWINDOW_H