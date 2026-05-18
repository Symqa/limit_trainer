#include "theorycontentmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

TheoryContentManager* TheoryContentManager::instance()
{
    static TheoryContentManager manager;
    return &manager;
}

TheoryContentManager::TheoryContentManager(QObject *parent)
    : QObject(parent)
{
    load();
}

void TheoryContentManager::load()
{
    // Путь к вашему JSON-файлу в ресурсах (убедитесь, что он прописан в .qrc)
    QFile file(":topics/data/theory.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "TheoryContentManager: Cannot open theory_content.json";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        qWarning() << "TheoryContentManager: JSON root is not an array";
        return;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        QJsonObject obj = val.toObject();
        QString id = obj["id"].toString();
        if (!id.isEmpty()) {
            m_topics[id] = obj;
        }
    }

    qDebug() << "TheoryContentManager loaded" << m_topics.size() << "topics";
}

QJsonObject TheoryContentManager::topicById(const QString &id) const
{
    return m_topics.value(id);
}

QJsonArray TheoryContentManager::definitionsForTopic(const QString &id) const
{
    return m_topics.value(id).value("definitions").toArray();
}

QJsonArray TheoryContentManager::propertiesForTopic(const QString &id) const
{
    return m_topics.value(id).value("properties").toArray();
}

QJsonArray TheoryContentManager::workedExamplesForTopic(const QString &id) const
{
    return m_topics.value(id).value("worked_examples").toArray();
}