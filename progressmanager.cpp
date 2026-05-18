#include "progressmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

ProgressManager* ProgressManager::instance()
{
    static ProgressManager manager;
    return &manager;
}

ProgressManager::ProgressManager(QObject *parent)
    : QObject(parent)
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    m_filePath = dir + "/progress.json";
    load(); // загружаем или создаём при первом запуске
}

void ProgressManager::load()
{
    QFile file(m_filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonObject obj = QJsonDocument::fromJson(file.readAll()).object();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            m_progress[it.key()] = it.value().toBool();
        }
        file.close();
        qDebug() << "Прогресс загружен из" << m_filePath;
    } else {
        // Файла нет — создадим позже при первом сохранении или сейчас пустой
        qDebug() << "Файл прогресса не найден, будет создан при первом изменении.";
    }
}

void ProgressManager::save()
{
    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        for (auto it = m_progress.begin(); it != m_progress.end(); ++it) {
            obj[it.key()] = it.value();
        }
        file.write(QJsonDocument(obj).toJson());
        file.close();
        qDebug() << "Прогресс сохранён.";
    } else {
        qWarning() << "Ошибка сохранения прогресса в" << m_filePath;
    }
}

bool ProgressManager::isCompleted(const QString &topicId) const
{
    return m_progress.value(topicId, false); // по умолчанию false
}

void ProgressManager::setCompleted(const QString &topicId, bool completed)
{
    if (m_progress.value(topicId, false) != completed) {
        m_progress[topicId] = completed;
        save(); // можно сохранять не сразу, а по таймеру для оптимизации
        emit topicStatusChanged(topicId, completed);
    }
}

void ProgressManager::resetAll()
{
    m_progress.clear();
    save();
    // не шлём сигналы для каждой темы, лучше сделать отдельный сигнал resetProgress()
}