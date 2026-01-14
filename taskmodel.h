#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractTableModel>
#include "taskdata.h"

class TaskModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TaskModel(QObject *parent = nullptr);

    // 必须重写的四个虚函数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // 自定义接口
    void reloadData();       // 刷新数据
    Task getTask(int row);   // 获取某一行的数据
    void removeRow(int row); // 删除某一行

private:
    QList<Task> m_tasks;
    QStringList m_headers;
};

#endif // TASKMODEL_H
