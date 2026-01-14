#ifndef REMINDERTHREAD_H
#define REMINDERTHREAD_H

#include <QThread>
#include <QObject>

class ReminderThread : public QThread
{
    Q_OBJECT
public:
    explicit ReminderThread(QObject *parent = nullptr);
    void stop();

protected:
    void run() override;

signals:
    void taskDeadlineReached(QString taskName);

private:
    bool m_running;
};

#endif // REMINDERTHREAD_H
