#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QThreadPool>
#include <QtQml/qqmlregistration.h>

#include "Task.h"

/* List of tasks for QML display */
/* Runs on the MAIN thread. The worker only sends signals back here */
class TaskModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT /* QML: TaskModel { } */

    public:
        typedef enum {
            IdRole = Qt::UserRole + 1,
            NameRole, 
            StatusRole, 
            StatusTextRole,
            ProgressRole,
            ErrorRole
        } Roles;

        explicit TaskModel(QObject *parent = nullptr);
        ~TaskModel();

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE void addTask(const QString &name);
        Q_INVOKABLE void cancelTask(int id);
        Q_INVOKABLE void clearFinished();
        Q_INVOKABLE void pauseTask(int id);
        Q_INVOKABLE void resumeTask(int id);
        /* Used by tests: change the worker's time per step (default 50ms) */
        void setStepDelay(int ms);
    
    signals:
        void logMessage(const QString &text, bool isError);

    private slots:
        /* Receive signal from workers */
        void onStarted(int id);
        void onProgressChanged(int id, int progress);
        void onCompleted(int id);
        void onFailed(int id, QString message);
        void onCancelled(int id);
    
    
    private:
        int findRow(int id);
        QString taskLabel(int row);
        void updateRow(int row, int role);

        QList<Task> m_tasks;
        int m_nextId = 1;
        int m_stepDelayMs = 50;
        QThreadPool m_pool;

};

#endif /* TASKMODEL_H */