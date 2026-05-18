#ifndef TOPICLEARNINGMANAGER_H
#define TOPICLEARNINGMANAGER_H

#include <QObject>

struct TopicLearningInfo {

};

class TopicLaerningManager : public QObject
{
    Q_OBJECT
public:
    static TopicLearningManager* instance();

private:
     explicit TopicLearningManager(QObject *parent = nullptr);
};

#endif // TOPICLEARNINGMANAGER_H

