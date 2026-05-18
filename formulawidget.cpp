#include "formulawidget.h"

FormulaWidget::FormulaWidget(const QString &latex, QWidget *parent)
    : QWebEngineView(parent)
{
    // Используем MathJax с CDN (нужен интернет)
    QString html = R"(
<html><head>
<script>
MathJax = {
  tex: { inlineMath: [['$','$'], ['\\(','\\)']] }
};
</script>
<script src="qrc:/mathjax/data/tex-chtml.js""></script>
</head><body style="margin:0; padding:0; background:transparent;">
<div style="font-size:1.2em;">$$%1$$</div>
</body></html>
)";
    html.replace("%1", latex);
    setHtml(html);
    setFixedHeight(60); // Подгоните высоту, если формулы большие
}