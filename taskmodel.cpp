#include "taskmodel.h"
#include "dbmanager.h"
#include <QColor>
#include <QBrush>

TaskModel::TaskModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "ID" << "任务名称" << "分类" << "优先级" << "截止时间" << "状态";
    reloadData();
}

void TaskModel::reloadData() {
    beginResetModel();
    m_tasks = DbManager::instance().getAllTasks();
    endResetModel();
}

int TaskModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_tasks.size();
}

int TaskModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_tasks.size())
        return QVariant();

    const Task &t = m_tasks.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return t.id;
        case 1: return t.name;
        case 2: return t.category;
        case 3: return t.priority;
        case 4: return t.deadline.toString("yyyy-MM-dd HH:mm");
        case 5: return t.statusStr();
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 1) // 任务名左对齐
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        return QVariant(Qt::AlignCenter);
    }
    else if (role == Qt::ForegroundRole) {
        if (t.isCompleted) return QBrush(Qt::gray);
        if (t.priority == "高") return QBrush(Qt::red);
    }
    else if (role == Qt::BackgroundRole) {
        if (t.isCompleted) return QBrush(QColor(245, 245, 245));
    }

    return QVariant();
}

QVariant TaskModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_headers.value(section);
    }
    return QVariant();
}

Task TaskModel::getTask(int row) {
    if (row >= 0 && row < m_tasks.size())
        return m_tasks.at(row);
    return Task();
}

void TaskModel::removeRow(int row) {
    if (row >= 0 && row < m_tasks.size()) {
        int taskId = m_tasks.at(row).id;
        if (DbManager::instance().deleteTask(taskId)) {
            reloadData();
        }
    }
}

