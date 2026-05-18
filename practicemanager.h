#ifndef PRACTICEMANAGER_H
#define PRACTICEMANAGER_H

#include <QObject>
#include <QJsonArray>
#include <QMap>
#include <QString>

class PracticeManager : public QObject {
    Q_OBJECT
public:
    static PracticeManager* instance();

    QJsonArray exercisesForTopic(const QString &topicId) const;
    QJsonArray exercisesForTopic(const QString &topicId, const QString &difficulty) const;
    QJsonArray generateVariant(const QStringList &topicIds, int countPerDifficulty = 2) const;
    int exerciseCount(const QString &topicId) const;

private:
    explicit PracticeManager(QObject *parent = nullptr);
    void load();
    QMap<QString, QJsonArray> m_exercises; // topicId -> массив заданий
};

#endif // PRACTICEMANAGER_H