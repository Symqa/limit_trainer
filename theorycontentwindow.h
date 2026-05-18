#ifndef THEORYCONTENTWINDOW_H
#define THEORYCONTENTWINDOW_H

#include <QDialog>
#include <QFrame>

namespace Ui {
class TheoryContentWindow;
}

class TheoryContentWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TheoryContentWindow(QWidget *parent = nullptr);
    ~TheoryContentWindow();
    void loadTopic(const QString &topicId);
    QFrame* createContentBlock(const QJsonObject &item);

private slots:
    void on_button_to_practice_clicked();

private:
    Ui::TheoryContentWindow *ui;
};

#endif // THEORYCONTENTWINDOW_H
