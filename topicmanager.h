#ifndef TOPICMANAGER_H
#define TOPICMANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QJsonObject>

// Структура для хранения одной темы (можно и просто QJsonObject, но так удобнее)
struct TopicInfo {
    QString id;
    QString title;
};

class TopicManager : public QObject
{
    Q_OBJECT
public:
    static TopicManager* instance();

    int topicCount() const;
    TopicInfo topicAt(int index) const;               // доступ по индексу
    TopicInfo findById(const QString &id) const;      // доступ по id
    QVector<TopicInfo> allTopics() const;             // полная копия (или константная ссылка)

private:
    explicit TopicManager(QObject *parent = nullptr);
    void load();

    QVector<TopicInfo> m_topics;
};

#endif // TOPICMANAGER_H