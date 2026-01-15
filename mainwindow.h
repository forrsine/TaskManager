#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QSet>
#include "taskmodel.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTask();
    void onEditTask();
    void onDeleteTask();
    void onToggleStatus();
    void onExportCsv();
    void onSearch(const QString &text);
    void checkReminders();
    void onShowStats();

private:
    void setupModel(); // 原来的 setupUi 的一部分逻辑移到这里
    void showTaskDialog(Task *task = nullptr);

    // 2. UI 指针
    Ui::MainWindow *ui;

    // 逻辑相关对象保持不变
    TaskModel *model;
    QSortFilterProxyModel *proxyModel;
    QTimer *timer;
    QSet<int> remindedIds;
};

#endif // MAINWINDOW_H

