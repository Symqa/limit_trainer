#ifndef PRACTICESELECTDIALOG_H
#define PRACTICESELECTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>

class PracticeSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PracticeSelectDialog(QWidget *parent = nullptr);

    QString selectedTopicId() const;
    QString selectedDifficulty() const;   // "easy", "medium", "hard" или "all"

private slots:
    void onStartPractice();

private:
    void setupUi();

    QComboBox *m_topicCombo;
    QComboBox *m_difficultyCombo;
    QPushButton *m_startBtn;
};

#endif // PRACTICESELECTDIALOG_H