#ifndef TASK_H
#define TASK_H

#include <QString>
#include <memory>
#include <atomic>

/* Task status include: pending, running, completed, failed and a status marked cancel */
typedef enum {
    TASK_PENDING = 0,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_CANCELLED,
    TASK_PAUSED
} TaskStatus;

typedef struct {
    int id = 0;
    QString name;
    TaskStatus status = TASK_PENDING;
    int progress = 0;
    QString errorMsg;

    std::shared_ptr<std::atomic<bool>> cancelFlag;
    std::shared_ptr<std::atomic<bool>> pauseFlag;
} Task;

#endif /* TASK_H */