#ifndef FORMULAWIDGET_H
#define FORMULAWIDGET_H

#include <QWebEngineView>

class FormulaWidget : public QWebEngineView
{
    Q_OBJECT
public:
    explicit FormulaWidget(const QString &latex, QWidget *parent = nullptr);
};

#endif