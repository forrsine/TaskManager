#ifndef TASKDATA_H
#define TASKDATA_H

#include <QString>
#include <QDateTime>

struct Task {
    int id;
    QString name;
    QString category; // 工作、学习、生活
    QString priority; // 高、中、低
    QDateTime deadline;
    bool isCompleted; // true:已完成, false:未完成

    // C++17 特性：可以在结构体里写简单的辅助函数
    QString statusStr() const {
        return isCompleted ? "已完成" : "进行中";
    }
};

#endif // TASKDATA_H
