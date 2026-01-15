#include "dbmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>

DbManager::DbManager() {}

DbManager::~DbManager() {
    if (m_db.isOpen()) m_db.close();
}

DbManager& DbManager::instance() {
    static DbManager instance;
    return instance;
}

bool DbManager::init() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    QString dbPath = QCoreApplication::applicationDirPath() + "/tasks.db";
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Database Error:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    //创建表
    bool success = query.exec("CREATE TABLE IF NOT EXISTS tasks ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "name TEXT, "
                              "category TEXT, "
                              "priority TEXT, "
                              "deadline INTEGER, "
                              "is_completed INTEGER)");

    if(!success) qDebug() << "Create Table Error:" << query.lastError();
    return success;
}

bool DbManager::addTask(const Task &task) {
    QSqlQuery query;
    query.prepare("INSERT INTO tasks (name, category, priority, deadline, is_completed) "
                  "VALUES (:name, :cat, :prio, :dead, 0)");
    query.bindValue(":name", task.name);
    query.bindValue(":cat", task.category);
    query.bindValue(":prio", task.priority);
    query.bindValue(":dead", task.deadline.toSecsSinceEpoch());
    return query.exec();
}

bool DbManager::updateTask(const Task &task) {
    QSqlQuery query;
    query.prepare("UPDATE tasks SET name=:name, category=:cat, priority=:prio, "
                  "deadline=:dead, is_completed=:comp WHERE id=:id");
    query.bindValue(":name", task.name);
    query.bindValue(":cat", task.category);
    query.bindValue(":prio", task.priority);
    query.bindValue(":dead", task.deadline.toSecsSinceEpoch());
    query.bindValue(":comp", task.isCompleted ? 1 : 0);
    query.bindValue(":id", task.id);
    return query.exec();
}

bool DbManager::deleteTask(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM tasks WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QList<Task> DbManager::getAllTasks() {
    QList<Task> list;
    QSqlQuery query("SELECT * FROM tasks ORDER BY is_completed ASC, deadline ASC");
    while (query.next()) {
        Task t;
        t.id = query.value("id").toInt();
        t.name = query.value("name").toString();
        t.category = query.value("category").toString();
        t.priority = query.value("priority").toString();
        t.deadline = QDateTime::fromSecsSinceEpoch(query.value("deadline").toLongLong());
        t.isCompleted = query.value("is_completed").toInt() == 1;
        list.append(t);
    }
    return list;
}

