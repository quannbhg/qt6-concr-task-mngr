#ifndef TASKRUNNABLE_H
#define TASKRUNNABLE_H

#include <QObject>
#include <QRunnable>
#include <QString>
#include <memory>
#include <atomic>

class TaskRunnable : public QObject, public QRunnable 
{
    Q_OBJECT

    public:
        TaskRunnable(int id, 
            QString name, 
            std::shared_ptr<std::atomic<bool>> cancelFlag;
        );
        void run() override;

    signals:
        void started(int id);
        void progressChanged(int id, int progress);
        void completed(int id);
        void failed(int id, QString msg);
        void cancelled(int id);

    private:
        int m_id;
        QString m_name;
        std::shared_ptr<std::atomic<bool>> m_cancelFlag;
};

#endif /* TASKRUNNABLE_H */