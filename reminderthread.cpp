#include "reminderthread.h"
#include "dbmanager.h"
#include <QDateTime>

ReminderThread::ReminderThread(QObject *parent) : QThread(parent), m_running(true) {}

void ReminderThread::stop() {
    m_running = false;
    wait();
}

void ReminderThread::run() {
    QList<int> remindedIds;

    while (m_running) {
        QList<Task> tasks = DbManager::instance().getAllTasks();
        QDateTime now = QDateTime::currentDateTime();

        for (const Task &t : tasks) {
            if (!t.isCompleted && !remindedIds.contains(t.id)) {
                if (now >= t.deadline.addSecs(-600)) {
                    emit taskDeadlineReached(t.name);
                    remindedIds.append(t.id);
                }
            }
        }

        for(int i=0; i<30; ++i) {
            if(!m_running) break;
            msleep(1000);
        }
    }
}

