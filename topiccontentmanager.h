#ifndef THEORYCONTENTMANAGER_H
#define THEORYCONTENTMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QString>

class TheoryContentManager : public QObject
{
    Q_OBJECT
public:
    static TheoryContentManager* instance();

    QJsonObject topicById(const QString &id) const;
    QJsonArray definitionsForTopic(const QString &id) const;
    QJsonArray propertiesForTopic(const QString &id) const;
    QJsonArray workedExamplesForTopic(const QString &id) const;

private:
    explicit TheoryContentManager(QObject *parent = nullptr);
    void load();
    QMap<QString, QJsonObject> m_topics;
};

#endif