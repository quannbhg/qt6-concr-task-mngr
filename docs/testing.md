# Testing

Framework: Qt Test, run with CTest. The tests use `TaskModel` directly
(no QML) and `setStepDelay()` to make tasks run fast.

## Run

Build (if needed) and run the tests for Debug and Release:

```powershell
.\t.bat
```

Or run CTest yourself:

```powershell
ctest --test-dir build-release --output-on-failure
```

To see every test case, build Debug and run the test program directly:

```powershell
.\.build.test.cmd
```

## What is tested

| Area | Tests |
|---|---|
| Input | empty name rejected, name trimmed, invalid ids do not crash |
| States | completed, failed with message, cancel running, pause and resume |
| Pending cancel | status changes immediately, log written only once |
| Clear | all finished rows removed, running task kept |
| Reliability | cancel a paused task, shutdown with busy tasks, stress with 50 tasks |

## Results

Windows 11, Qt 6.11.2, MinGW 13.1, Release build:

```
********* Start testing of TestTaskModel *********
Config: Using QtTest library 6.11.2, Qt 6.11.2 (x86_64-little_endian-llp64 shared (dynamic) release build; by GCC 13.1.0), windows 11
PASS   : TestTaskModel::initTestCase()
PASS   : TestTaskModel::addTask_emptyName_isRejected()
PASS   : TestTaskModel::addTask_validName_isTrimmedAndAdded()
PASS   : TestTaskModel::invalidId_doesNotCrash()
PASS   : TestTaskModel::task_runsToCompleted()
PASS   : TestTaskModel::failTask_endsFailedWithMessage()
PASS   : TestTaskModel::cancelRunning_endsCancelled()
PASS   : TestTaskModel::pauseAndResume()
PASS   : TestTaskModel::cancelPending_changesStatusImmediately()
PASS   : TestTaskModel::cancelPending_logsOnlyOnce()
PASS   : TestTaskModel::clearFinished_removesAllFinishedRows()
PASS   : TestTaskModel::cancelPausedTask_doesNotHang()
PASS   : TestTaskModel::shutdown_withBusyTasks_isFast()
PASS   : TestTaskModel::stress_manyTasks()
PASS   : TestTaskModel::cleanupTestCase()
Totals: 15 passed, 0 failed, 0 skipped, 0 blacklisted, 1530ms
********* Finished testing of TestTaskModel *********
```