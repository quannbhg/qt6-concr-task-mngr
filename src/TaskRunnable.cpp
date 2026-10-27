#include "TaskRunnable.h"
#include <QThread>

TaskRunnable::TaskRunnable(int id, 
                        QString name, 
                        std::shared_ptr<std::atomic<bool>> cancelFlag,
                        std::shared_ptr<std::atomic<bool>> pauseFlag )
{
    m_id = id;
    m_name = name;
    m_cancelFlag = cancelFlag;
    m_pauseFlag = pauseFlag;
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

        /* If task currently paused, stop and wait here. */
        while (*m_pauseFlag == true){
            
            /* While waiting for the pause, you must still check for cancellation; */
            /* otherwise, the cancellation will not be possible */
            
            if (*m_cancelFlag == true){
                emit cancelled(m_id);
                return;
            }

            QThread::msleep(100);
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
