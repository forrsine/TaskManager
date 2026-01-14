#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QList>
#include "taskdata.h"

class DbManager
{
public:
    static DbManager& instance();

    bool init(); // 初始化数据库和表
    bool addTask(const Task &task);
    bool updateTask(const Task &task);
    bool deleteTask(int id);
    QList<Task> getAllTasks(); // 获取所有数据

private:
    DbManager();
    ~DbManager();
    // 禁止拷贝
    DbManager(const DbManager&) = delete;
    DbManager& operator=(const DbManager&) = delete;

    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
