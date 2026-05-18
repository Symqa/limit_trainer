#include "mainwindow.h"
#include "progressmanager.h"
#include "topicmanager.h"
#include "theorycontentmanager.h"
#include "practicemanager.h"

#include <QApplication>
#include <Qlabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("Limit");
    TopicManager::instance();   // загрузит список тем из ресурсов
    ProgressManager::instance(); // загрузит прогресс (или создаст пустой)
    TheoryContentManager::instance(); // загрузит список теории по темам
    PracticeManager::instance(); // загрузит список практики по темам

    MainWindow w;
    w.show();

    return QApplication::exec();
}
