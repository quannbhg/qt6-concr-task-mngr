#include "LogModel.h"

#include <QDateTime>

LogModel::LogModel(QObject *parent) : QAbstractListModel(parent){
}

int LogModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()){
        return 0;
    }
    return m_logs.size();
}

QVariant LogModel::data(const QModelIndex &index, int role) const {
    if (index.isValid() == false){
            return QVariant();
        }

    if (index.row() < 0 || index.row() >= m_logs.size()){
        return QVariant();
    }

    const LogEntry &entry = m_logs[index.row()];

    switch(role){
        case TimeRole:
            return entry.time;
        case MessageRole:
            return entry.text;
        case IsErrorRole:
            return entry.isError;
    }

    return QVariant();
}

QHash<int, QByteArray> LogModel::roleNames() const {
    QHash<int, QByteArray> names;
    names[TimeRole] = "logTime";
    names[MessageRole] = "message";
    names[IsErrorRole] = "isError";
    return names;
}

void LogModel::addLog(const QString &text, bool IsError){
    /* Max 500 lines; when full, delete the oldest one */
    if (m_logs.size() >= 500){
        beginRemoveRows(QModelIndex(), 0, 0);
        m_logs.removeAt(0);
        endRemoveRows();
    }

    LogEntry entry;
    entry.time = QDateTime::currentDateTime().toString("hh:mm:ss");
    entry.text = text;
    entry.isError = IsError;

    int newRow = m_logs.size();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_logs.append(entry);
    endInsertRows();
}

void LogModel::clearLog(){
    beginResetModel();
    m_logs.clear();
    endResetModel();
}