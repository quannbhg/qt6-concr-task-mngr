#include "TaskRunnable.h"
#include <QThread>

TaskRunnable::TaskRunnable(int id, QString name, std::shared_ptr<std::atomic<bool>> cancelFlag){
    m_id = id;
    m_name = name;
    m_cancelFlag = cancelFlag;
}

void TaskRunnable::run(){
    if (*m_cancelFlag == true){
        emit cancelled(m_id);
        return;
    }

    emit started(m_id);

    for (int i = 1; i <= 100; i++){

        if (*m_cancelFlag == true){
            emit cancelled(m_id);
            return;
        }

        if (m_name == "fail" && i == 50){
            emit failed(m_id, "Simulation error at 50%");
            return;
        }

        QThread::msleep(50);
        emit progressChanged(m_id, i);
    }

    emit completed(m_id);
}
