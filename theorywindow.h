#ifndef THEORYWINDOW_H
#define THEORYWINDOW_H

#include <QDialog>

namespace Ui {
class TheoryWindow;
}

class TheoryWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TheoryWindow(QWidget *parent = nullptr);
    ~TheoryWindow();

private slots:
    void on_button_to_topic_clicked();

private:
    void setupTopics();
    void updateContinueButton();
    void checkAllCompleted();

    Ui::TheoryWindow *ui;
};

#endif // THEORYWINDOW_H