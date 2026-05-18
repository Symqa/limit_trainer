#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>
#include <Qmap>
#include <QString>

class ProgressManager : public QObject
{
    Q_OBJECT
public:
    static ProgressManager* instance();

    bool isCompleted(const QString &topicId) const;
    void setCompleted(const QString &topicId, bool completed = true);
    void resetAll();

private:
    explicit ProgressManager(QObject *parent = nullptr);

    void load();
    void save();

    QMap<QString, bool> m_progress;
    QString m_filePath;

signals:
    void topicStatusChanged(const QString &topicId, bool completed);
};

#endif // PROGRESSMANAGER_H
