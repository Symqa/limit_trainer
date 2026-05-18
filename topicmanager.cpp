#include "topicmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

TopicManager* TopicManager::instance()
{
    static TopicManager manager;
    return &manager;
}

TopicManager::TopicManager(QObject *parent)
    : QObject(parent)
{
    load(); // сразу при создании (которое происходит в instance())
}

void TopicManager::load()
{
    QFile file(":/topics/data/topics.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть topics.json из ресурсов";
        return;
    }

    QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    m_topics.clear();
    m_topics.reserve(array.size());

    for (const QJsonValue &val : array) {
        QJsonObject obj = val.toObject();
        TopicInfo info;
        info.id = obj["id"].toString();
        info.title = obj["title"].toString();
        // никакого order больше нет
        m_topics.append(info);
    }
    qDebug() << "Загружено тем:" << m_topics.size();
}

int TopicManager::topicCount() const
{
    return m_topics.size();
}

TopicInfo TopicManager::topicAt(int index) const
{
    if (index >= 0 && index < m_topics.size())
        return m_topics[index];
    return TopicInfo(); // пустая структура в случае ошибки
}

TopicInfo TopicManager::findById(const QString &id) const
{
    for (const auto &topic : m_topics) {
        if (topic.id == id)
            return topic;
    }
    return TopicInfo(); // не найдено
}

QVector<TopicInfo> TopicManager::allTopics() const
{
    return m_topics;
}