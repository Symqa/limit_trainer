#include "practicemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRandomGenerator>
#include <algorithm>
#include <random>
#include <QDebug>

PracticeManager* PracticeManager::instance() {
    static PracticeManager mgr;
    return &mgr;
}

PracticeManager::PracticeManager(QObject *parent) : QObject(parent) {
    load();
}

void PracticeManager::load() {
    QFile file(":/topics/data/practice.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "PracticeManager: cannot open practice.json";
        return;
    }
    QJsonArray arr = QJsonDocument::fromJson(file.readAll()).array();
    file.close();
    for (const QJsonValue &v : arr) {
        QJsonObject obj = v.toObject();
        QString id = obj["id"].toString();
        if (!id.isEmpty()) {
            m_exercises[id] = obj["exercises"].toArray();
        }
    }
    qDebug() << "PracticeManager loaded" << m_exercises.size() << "topics";
}

QJsonArray PracticeManager::exercisesForTopic(const QString &topicId) const {
    return m_exercises.value(topicId);
}

QJsonArray PracticeManager::exercisesForTopic(const QString &topicId, const QString &difficulty) const {
    QJsonArray all = m_exercises.value(topicId);
    QJsonArray filtered;
    for (const QJsonValue &v : all) {
        QJsonObject obj = v.toObject();
        if (obj["difficulty"].toString() == difficulty) {
            filtered.append(obj);
        }
    }
    return filtered;
}

QJsonArray PracticeManager::generateVariant(const QStringList &topicIds, int countPerDifficulty) const {
    QJsonArray variant;
    QStringList difficulties = {"easy", "medium", "hard"};
    std::random_device rd;
    std::mt19937 gen(rd());

    for (const QString &topicId : topicIds) {
        for (const QString &diff : difficulties) {
            QJsonArray pool = exercisesForTopic(topicId, diff);
            if (pool.isEmpty()) continue;
            QVector<QJsonValue> shuffled;
            for (const QJsonValue &v : pool) shuffled.append(v);
            std::shuffle(shuffled.begin(), shuffled.end(), gen);
            int n = std::min(countPerDifficulty, static_cast<int>(shuffled.size()));
            for (int i = 0; i < n; ++i) {
                variant.append(shuffled[i]);
            }
        }
    }

    // Перемешиваем готовый вариант, чтобы задания разной сложности шли вразнобой
    QVector<QJsonValue> vArr;
    for (const QJsonValue &v : variant) vArr.append(v);
    std::shuffle(vArr.begin(), vArr.end(), gen);
    QJsonArray result;
    for (const QJsonValue &v : vArr) result.append(v);
    return result;
}

int PracticeManager::exerciseCount(const QString &topicId) const {
    return m_exercises.value(topicId).size();
}