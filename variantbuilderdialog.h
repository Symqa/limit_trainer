#ifndef VARIANTBUILDERDIALOG_H
#define VARIANTBUILDERDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVector>

class VariantBuilderDialog : public QDialog
{
    Q_OBJECT
public:
    explicit VariantBuilderDialog(QWidget *parent = nullptr);

    QStringList selectedTopicIds() const;
    int taskCount() const;   // общее количество заданий в варианте (минимум 3)

private slots:
    void onBuild();

private:
    void setupUi();

    QVector<QCheckBox*> m_topicChecks;
    QSpinBox *m_taskCountSpin;
    QPushButton *m_buildBtn;
};

#endif // VARIANTBUILDERDIALOG_H