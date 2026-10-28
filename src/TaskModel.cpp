#include "TaskModel.h"
#include "TaskRunnable.h"

TaskModel::TaskModel(QObject *parent):QAbstractListModel(parent){
    /*QThreadPool quản lý việc chỉ cho chạy tối đa n = 2 task một lúc */
    /*QThreadPool manages the execution, ensuring no more than n = 2 tasks run simultaneously*/
    m_pool.setMaxThreadCount(2);
}

TaskModel::~TaskModel(){
    /* Safe shutdown */
    /* Step 1: Notify that all tasks should stop */
    for (int i = 0; i < m_tasks.size(); i++){
        *m_tasks[i].cancelFlag = true;
    }
    /* Step 2: Let all sub-threads finish running */
    m_pool.waitForDone();
}

int TaskModel::rowCount(const QModelIndex &parent) const{
    if (parent.isValid()){
        return 0;
    }
    return m_tasks.size();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() == false){
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= m_tasks.size()){
        return QVariant();
    }

    const Task &task = m_tasks[index.row()];

    switch (role) {
    case IdRole:
        return task.id;
    case NameRole:
        return task.name;
    case StatusRole:
        return task.status;
    case StatusTextRole:
        switch (task.status) {
        case TASK_PENDING:
            return "Pending";
        case TASK_RUNNING:
            return "Running";
        case TASK_COMPLETED:
            return "Completed";
        case TASK_FAILED:
            return "Failed";
        case TASK_CANCELLED:
            return "Cancelled";
        case TASK_PAUSED:
            return "Paused";
        }
        return "Unknown";
    case ProgressRole:
        return task.progress;
    case ErrorRole:
        return task.errorMsg;
    }

    return QVariant();
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    /* Mapping role names to be used in QML */
    QHash<int, QByteArray> names;
    names[IdRole] = "taskId";
    names[NameRole] = "name";
    names[StatusRole] = "status";
    names[StatusTextRole] = "statusText";
    names[ProgressRole] = "progress";
    names[ErrorRole] = "errorMsg";
    return names;
}

void TaskModel::addTask(const QString &name)
{
    /* Kiểm tra đầu vào, nếu tên Task rỗng thì không thêm */
    /* Validate input; if the task name is empty, do not add it */
    if (name.trimmed().isEmpty()) {
        return;
    }

    Task task;
    task.id = m_nextId;
    m_nextId++;
    task.name = name.trimmed();
    task.status = TASK_PENDING;
    task.progress = 0;
    task.cancelFlag = std::make_shared<std::atomic<bool>>(false);
    task.pauseFlag = std::make_shared<std::atomic<bool>>(false);

    /* Thêm Task vào cuối danh sách, báo cho QML biết để update bằng begin và end */
    /* Add a Task to the end of the list, and notify QML to perform an update using `begin` and `end` */
    int newRow = m_tasks.size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_tasks.append(task);
    endInsertRows();

    /* Tạo worker và nối signal của worker về slot của model */
    /* Create a worker and connect the worker's signal to the model's slot */
    TaskRunnable *runnable = new TaskRunnable(task.id, task.name, task.cancelFlag, task.pauseFlag);
    connect(runnable, &TaskRunnable::started, this, &TaskModel::onStarted);
    connect(runnable, &TaskRunnable::progressChanged, this, &TaskModel::onProgressChanged);
    connect(runnable, &TaskRunnable::completed, this, &TaskModel::onCompleted);
    connect(runnable, &TaskRunnable::failed, this, &TaskModel::onFailed);
    connect(runnable, &TaskRunnable::cancelled, this, &TaskModel::onCancelled);

    /* Chuyển runnable cho QThreadPool, QThreadPool sẽ tự xóa runnable sau khi chạy xong */
    /* Pass a runnable to QThreadPool; QThreadPool will automatically delete the runnable after it has finished executing */
    m_pool.start(runnable);
    /* Log message */
    emit logMessage("Task #" + QString::number(task.id) + " (" + task.name + ") added", false);
}

void TaskModel::cancelTask(int id)
{
    int row = findRow(id);
    if (row == -1) {
        return;
    }

    if (m_tasks[row].status == TASK_PENDING){
        *m_tasks[row].cancelFlag = true;
        m_tasks[row].status = TASK_CANCELLED;
        updateRow(row, StatusRole);
        updateRow(row, StatusTextRole);
        /* Log message */
        emit logMessage(taskLabel(row) + " cancelled", false);
    } else if (m_tasks[row].status == TASK_RUNNING || m_tasks[row].status == TASK_PAUSED){
        *m_tasks[row].cancelFlag = true;
        /* Log message */
        emit logMessage(taskLabel(row) + " cancel requested", false);
    }

}

int TaskModel::findRow(int id){
    for (int i = 0; i < m_tasks.size(); i++){
        if (m_tasks[i].id == id){
            return i;
        }
    }
    return -1;
}

void TaskModel::updateRow(int row, int role){
    QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {role});
}

void TaskModel::onStarted(int id){
    int row = findRow(id);
    if (row == -1){
        return;
    }
    if (m_tasks[row].status == TASK_CANCELLED){
        return;
    }

    m_tasks[row].status = TASK_RUNNING;
    updateRow(row, StatusRole);
    updateRow(row, StatusTextRole);
    emit logMessage(taskLabel(row) + " started", false);
}

void TaskModel::onProgressChanged(int id, int progress)
{
    int row = findRow(id);
    if (row == -1) {
        return;
    }
    m_tasks[row].progress = progress;
    updateRow(row, ProgressRole);
}

void TaskModel::onCompleted(int id)
{
    int row = findRow(id);
    if (row == -1) {
        return;
    }
    m_tasks[row].status = TASK_COMPLETED;
    updateRow(row, StatusRole);
    updateRow(row, StatusTextRole);
    emit logMessage(taskLabel(row) + " completed", false);
}

void TaskModel::onFailed(int id, QString message)
{
    int row = findRow(id);
    if (row == -1) {
        return;
    }
    m_tasks[row].status = TASK_FAILED;
    m_tasks[row].errorMsg = message;
    updateRow(row, StatusRole);
    updateRow(row, StatusTextRole);
    updateRow(row, ErrorRole);
    /* Log message */
    emit logMessage(taskLabel(row) + " failed: " + message, true);
}

void TaskModel::onCancelled(int id){
    int row = findRow(id);
    if (row == -1){
        return;
    }
    if (m_tasks[row].status == TASK_CANCELLED){
        return;
    }

    m_tasks[row].status = TASK_CANCELLED;
    updateRow(row, StatusRole);
    updateRow(row, StatusTextRole);
    /* Log message */
    emit logMessage(taskLabel(row) + " cancelled", false);
}

void TaskModel::clearFinished()
{
    /* Clear các task finished duyệt từ cuối lên đầu để tránh bỏ sót dòng */
    /* Clear finished tasks by iterating from the end to the beginning to avoid skipping any rows */
    int removed = 0;
    for (int i = m_tasks.size() - 1; i >= 0; i--){
        TaskStatus status = m_tasks[i].status;

        if (status == TASK_COMPLETED ||
            status == TASK_FAILED ||
            status == TASK_CANCELLED){
                beginRemoveRows(QModelIndex(), i, i);
                m_tasks.removeAt(i);
                endRemoveRows();
                removed++;
            }
    }

    if (removed > 0){
        emit logMessage("Cleared " + QString::number(removed) + " finished task(s)", false);
    }
}

void TaskModel::pauseTask(int id){
    int row = findRow(id);
    if (row == -1){
        return;
    }

    if (m_tasks[row].status == TASK_RUNNING){
        *m_tasks[row].pauseFlag = true;
        m_tasks[row].status = TASK_PAUSED;
        updateRow(row, StatusRole);
        updateRow(row, StatusTextRole);
        /* Log message */
        emit logMessage(taskLabel(row) + " paused", false);
    }
}

void TaskModel::resumeTask(int id){
    int row = findRow(id);
    if (row == -1){
        return;
    }

    if (m_tasks[row].status == TASK_PAUSED){
        *m_tasks[row].pauseFlag = false;
        m_tasks[row].status = TASK_RUNNING;
        updateRow(row, StatusRole);
        updateRow(row, StatusTextRole);
        /* Log message */
        emit logMessage(taskLabel(row) + " resumed", false);
    }
}

QString TaskModel::taskLabel(int row){
    return "Task #" + QString::number(m_tasks[row].id) + " (" + m_tasks[row].name + ")";
}