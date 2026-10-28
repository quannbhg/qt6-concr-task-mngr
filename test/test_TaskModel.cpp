#include <QtTest>
#include <QSignalSpy>
#include <QElapsedTimer>

#include "TaskModel.h"

class TestTaskModel : public QObject {
    Q_OBJECT

    private:
        /* Support function: Get data of one line in model */
        QString statusOf(TaskModel &model, int row){
            return model.data(model.index(row, 0), TaskModel::StatusTextRole).toString();
        }

        int progressOf(TaskModel &model, int row){
            return model.data(model.index(row, 0), TaskModel::ProgressRole).toInt();
        }

        /* Count logs which have correct content by text */
        int countLog(QSignalSpy &spy, const QString &text){
            int count = 0;
            for (int i = 0; i < spy.size(); i++){
                if (spy.at(i).at(0).toString() == text){
                    count ++;
                }
            }
            return count;
        }

        /* Check tasks status finished */
        bool allFinished(TaskModel &model)
        {
            for (int i = 0; i < model.rowCount(); i++) {
                QString s = statusOf(model, i);
                if (s != "Completed" && s != "Failed" && s != "Cancelled") {
                    return false;
                }
            }
            return true;
        }

    private slots:
        /* ---------- Input ---------- */
        void addTask_emptyName_isRejected();
        void addTask_validName_isTrimmedAndAdded();
        void invalidId_doesNotCrash();

        /* ---------- Task Status ---------- */
        void task_runsToCompleted();
        void failTask_endsFailedWithMessage();
        void cancelRunning_endsCancelled();
        void pauseAndResume();

        /* ---------- Cancel Pending Task ---------- */
        void cancelPending_changesStatusImmediately();
        void cancelPending_logsOnlyOnce();

        /* ---------- Clear ---------- */
        void clearFinished_removesAllFinishedRows();

        /* ---------- Confidence Score ---------- */
        void cancelPausedTask_doesNotHang();
        void shutdown_withBusyTasks_isFast();
        void stress_manyTasks();
    
};

/* ===================== Input ===================== */

void TestTaskModel::addTask_emptyName_isRejected(){
    TaskModel model;

    model.addTask("");
    model.addTask("     ");

    QCOMPARE(model.rowCount(), 0);
}

void TestTaskModel::addTask_validName_isTrimmedAndAdded(){
    TaskModel model;
    model.setStepDelay(50);

    model.addTask("  hello  ");

    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0), TaskModel::NameRole).toString(), QString("hello"));

}

void TestTaskModel::invalidId_doesNotCrash()
{
    TaskModel model;

    /* Id not exist: no crash, do nothing */
    model.cancelTask(999);
    model.pauseTask(-5);
    model.resumeTask(0);

    QCOMPARE(model.rowCount(), 0);
}

/* ===================== Task Status ===================== */
void TestTaskModel::task_runsToCompleted(){
    TaskModel model;
    model.setStepDelay(0);

    model.addTask("a");

    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Completed"), 5000);
    QCOMPARE(progressOf(model, 0), 100);
}

void TestTaskModel::failTask_endsFailedWithMessage(){
    TaskModel model;
    model.setStepDelay(0);

    model.addTask("fail");

    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Failed"), 5000);
    QString msg = model.data(model.index(0, 0), TaskModel::ErrorRole).toString();
    QVERIFY(msg.isEmpty() == false);
}

void TestTaskModel::cancelRunning_endsCancelled()
{
    TaskModel model;
    model.setStepDelay(20);

    model.addTask("a");
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Running"), 2000);

    model.cancelTask(1);

    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Cancelled"), 2000);
}

void TestTaskModel::pauseAndResume()
{
    TaskModel model;
    model.setStepDelay(20);

    model.addTask("a");
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Running"), 2000);

    model.pauseTask(1);
    QCOMPARE(statusOf(model, 0), QString("Paused"));

    /* Wait a moment for the final signal to arrive, then record the process */
    QTest::qWait(200);
    int progressWhenPaused = progressOf(model, 0);

    /* While in a paused state, the process does not advance further */
    QTest::qWait(300);
    QCOMPARE(progressOf(model, 0), progressWhenPaused);

    model.resumeTask(1);
    QCOMPARE(statusOf(model, 0), QString("Running"));

    /* If it continues to run, the process count increases */
    QTRY_VERIFY_WITH_TIMEOUT(progressOf(model, 0) > progressWhenPaused, 3000);
}

/* ===================== Cancel Pending Task ===================== */

void TestTaskModel::cancelPending_changesStatusImmediately()
{
    TaskModel model;
    model.setStepDelay(50);

    /* Pool run 2 tasks at the same time, task c pending */
    model.addTask("a");
    model.addTask("b");
    model.addTask("c");
    QCOMPARE(statusOf(model, 2), QString("Pending"));

    model.cancelTask(3);

    /* Not wait worker, status change immediately */
    QCOMPARE(statusOf(model, 2), QString("Cancelled"));
}

void TestTaskModel::cancelPending_logsOnlyOnce()
{
    TaskModel model;
    model.setStepDelay(50);
    QSignalSpy logSpy(&model, &TaskModel::logMessage);

    model.addTask("a");
    model.addTask("b");
    model.addTask("c");

    model.cancelTask(3);   /* c pending -> cancel immediately */
    model.cancelTask(1);   /* a, b running -> cancel */
    model.cancelTask(2);

    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Cancelled"), 3000);
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 1), QString("Cancelled"), 3000);

    /* At this point, c's worker finally runs, and it also emits "cancelled". 
    Wait a moment for that signal to arrive */
    QTest::qWait(300);

    QCOMPARE(countLog(logSpy, "Task #3 (c) cancelled"), 1);
}

/* ===================== Clear ===================== */

void TestTaskModel::clearFinished_removesAllFinishedRows()
{
    TaskModel model;
    model.setStepDelay(50);

    model.addTask("a");
    model.addTask("b");
    model.addTask("c");
    model.addTask("d");   /* d running when a, b cancelled */

    model.cancelTask(3);
    model.cancelTask(1);
    model.cancelTask(2);

    /* the first 3 tasks were all cancelled; they appear consecutively in the list */
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Cancelled"), 3000);
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 1), QString("Cancelled"), 3000);
    QCOMPARE(statusOf(model, 2), QString("Cancelled"));

    model.clearFinished();

    /* Only d is still running; the 3 cancelled tasks must all be removed */
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.data(model.index(0, 0), TaskModel::NameRole).toString(), QString("d"));
}

/* ===================== Confidence Score ===================== */

void TestTaskModel::cancelPausedTask_doesNotHang()
{
    TaskModel model;
    model.setStepDelay(20);

    model.addTask("a");
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Running"), 2000);

    model.pauseTask(1);
    QCOMPARE(statusOf(model, 0), QString("Paused"));

    model.cancelTask(1);

    QTRY_COMPARE_WITH_TIMEOUT(statusOf(model, 0), QString("Cancelled"), 2000);
}

void TestTaskModel::shutdown_withBusyTasks_isFast()
{
    TaskModel *model = new TaskModel;
    model->setStepDelay(50);

    /* 6 task: 2 running, 4 pending. Pause a task running. */
    for (int i = 0; i < 6; i++) {
        model->addTask("t" + QString::number(i));
    }
    QTRY_COMPARE_WITH_TIMEOUT(statusOf(*model, 0), QString("Running"), 2000);
    model->pauseTask(1);

    QElapsedTimer timer;
    timer.start();

    delete model;   /* the destructor must stop all threads */

    QVERIFY2(timer.elapsed() < 2000,
             qPrintable("Shutdown qua lau: " + QString::number(timer.elapsed()) + " ms"));
}

void TestTaskModel::stress_manyTasks()
{
    TaskModel model;
    model.setStepDelay(0);

    /* add 50 consecutive tasks */
    for (int i = 0; i < 50; i++) {
        model.addTask("t" + QString::number(i));
    }

    /* Pause and then resume the even-numbered tasks, 
    and cancel the odd-numbered tasks while they are running */
    for (int id = 1; id <= 50; id++) {
        if (id % 2 == 0) {
            model.pauseTask(id);
            model.resumeTask(id);
        } else {
            model.cancelTask(id);
        }
    }

    /* Finally, all task must be finished */
    QTRY_VERIFY_WITH_TIMEOUT(allFinished(model), 15000);
    QCOMPARE(model.rowCount(), 50);
}

QTEST_GUILESS_MAIN(TestTaskModel)
#include "test_TaskModel.moc"