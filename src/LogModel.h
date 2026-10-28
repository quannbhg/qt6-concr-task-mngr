#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QtQml/qqmlregistration.h>


/* One log line */
typedef struct {
    QString time;
    QString text;
    bool isError = false;
} LogEntry;

/* List of logs for QML view (run in MAIN thread) */
class LogModel : public QAbstractListModel {
    Q_OBJECT
    QML_ELEMENT /* QML: LogModel { }*/

    public:
        typedef enum {
            TimeRole = Qt::UserRole + 1,
            MessageRole,
            IsErrorRole
        } Roles;

        explicit LogModel(QObject *parent = nullptr);

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role) const override;
        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE void addLog(const QString &text, bool isError);
        Q_INVOKABLE void clearLog();
    
    private:
        QList<LogEntry> m_logs;

};

#endif