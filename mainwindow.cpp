#include "mainwindow.h"
#include "ui_mainwindow.h" // 必须引入这个自动生成的头文件
#include "dbmanager.h"
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QTextStream>
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);


    if (!DbManager::instance().init()) {
        QMessageBox::critical(this, "错误", "无法连接数据库！");
    }


    setupModel();


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkReminders);
    timer->start(60000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupModel() {

    model = new TaskModel(this);

    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterKeyColumn(1);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableView->setModel(proxyModel);

    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddTask);
    connect(ui->btnEdit, &QPushButton::clicked, this, &MainWindow::onEditTask);
    connect(ui->btnDel, &QPushButton::clicked, this, &MainWindow::onDeleteTask);
    connect(ui->btnStatus, &QPushButton::clicked, this, &MainWindow::onToggleStatus);
    connect(ui->btnStats, &QPushButton::clicked, this, &MainWindow::onShowStats);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onExportCsv);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearch);
}

void MainWindow::onSearch(const QString &text) {
    proxyModel->setFilterFixedString(text);
}


void MainWindow::showTaskDialog(Task *task) {
    QDialog dlg(this);
    dlg.setWindowTitle(task ? "编辑任务" : "新建任务");

    QFormLayout *form = new QFormLayout(&dlg);
    QLineEdit *nameEdit = new QLineEdit(task ? task->name : "");
    QComboBox *catCombo = new QComboBox();
    catCombo->addItems({"工作", "学习", "生活", "其他"});
    if(task) catCombo->setCurrentText(task->category);

    QComboBox *prioCombo = new QComboBox();
    prioCombo->addItems({"高", "中", "低"});
    if(task) prioCombo->setCurrentText(task->priority);

    QDateTimeEdit *dateEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    if(task) dateEdit->setDateTime(task->deadline);

    form->addRow("任务名称:", nameEdit);
    form->addRow("分类:", catCombo);
    form->addRow("优先级:", prioCombo);
    form->addRow("截止时间:", dateEdit);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addWidget(box);

    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "警告", "任务名称不能为空！");
            return;
        }

        Task t;
        t.id = task ? task->id : -1;
        t.name = nameEdit->text();
        t.category = catCombo->currentText();
        t.priority = prioCombo->currentText();
        t.deadline = dateEdit->dateTime();
        t.isCompleted = task ? task->isCompleted : false;

        bool ok = false;
        if (task) ok = DbManager::instance().updateTask(t);
        else ok = DbManager::instance().addTask(t);

        if (ok) model->reloadData();
        else QMessageBox::warning(this, "错误", "数据库操作失败");
    }
}


void MainWindow::onAddTask() {
    showTaskDialog(nullptr);
}

void MainWindow::onEditTask() {
    QModelIndex cur = ui->tableView->currentIndex();
    if (!cur.isValid()) return;

    QModelIndex srcIdx = proxyModel->mapToSource(cur);
    Task t = model->getTask(srcIdx.row());
    showTaskDialog(&t);
}

void MainWindow::onDeleteTask() {
    QModelIndex cur = ui->tableView->currentIndex();
    if (!cur.isValid()) {
        QMessageBox::information(this, "提示", "请选择要删除的任务");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定删除该任务吗？") == QMessageBox::Yes) {
        QModelIndex srcIdx = proxyModel->mapToSource(cur);
        model->removeRow(srcIdx.row());
    }
}

void MainWindow::onToggleStatus() {
    QModelIndex cur = ui->tableView->currentIndex();
    if (!cur.isValid()) return;

    QModelIndex srcIdx = proxyModel->mapToSource(cur);
    Task t = model->getTask(srcIdx.row());

    t.isCompleted = !t.isCompleted;
    DbManager::instance().updateTask(t);
    model->reloadData();
}

void MainWindow::onExportCsv() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出任务", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << QString::fromLocal8Bit("\xEF\xBB\xBF");
        out << "ID,名称,分类,优先级,截止时间,状态\n";
        QList<Task> list = DbManager::instance().getAllTasks();
        for (const auto &t : list) {
            out << t.id << ","
                << t.name << ","
                << t.category << ","
                << t.priority << ","
                << t.deadline.toString("yyyy-MM-dd HH:mm") << ","
                << (t.isCompleted ? "已完成" : "未完成") << "\n";
        }
        file.close();
        QMessageBox::information(this, "成功", "文件导出成功！");
    } else {
        QMessageBox::warning(this, "错误", "无法写入文件，请检查权限。");
    }
}

void MainWindow::checkReminders() {
    QDateTime now = QDateTime::currentDateTime();
    QList<Task> list = DbManager::instance().getAllTasks();

    for (const Task &t : list) {
        if (t.isCompleted) continue;
        if (remindedIds.contains(t.id)) continue;
        if (now >= t.deadline) {
            remindedIds.insert(t.id);
            QMessageBox::information(this, "🔔 任务到期提醒",
                                     QString("任务：[%1] 截止时间已到！\n请尽快处理。").arg(t.name));
            return;
        }
    }
}

void MainWindow::onShowStats() {
    QList<Task> list = DbManager::instance().getAllTasks();
    int total = list.size();
    int completed = 0;
    int highPriority = 0;
    int overdue = 0;
    QDateTime now = QDateTime::currentDateTime();

    for (const Task &t : list) {
        if (t.isCompleted) completed++;
        else if (t.deadline < now) overdue++;
        if (t.priority == "高") highPriority++;
    }

    QDialog dlg(this);
    dlg.setWindowTitle("任务完成情况统计");
    dlg.resize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QLabel *title = new QLabel(QString("总体进度 (共 %1 个任务)").arg(total));
    title->setStyleSheet("font-weight: bold; font-size: 14px;");

    QProgressBar *bar = new QProgressBar();
    bar->setRange(0, total);
    bar->setValue(completed);
    bar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }");
    double rate = (total > 0) ? ((double)completed / total * 100.0) : 0.0;
    bar->setFormat(QString("完成率: %1%").arg(QString::number(rate, 'f', 1)));
    bar->setAlignment(Qt::AlignCenter);

    QString detailText = QString("✅ 已完成任务： %1\n⬜ 待办任务：   %2\n🔥 高优先级：   %3\n⏰ 已超时任务： %4")
                             .arg(completed).arg(total - completed).arg(highPriority).arg(overdue);
    QLabel *details = new QLabel(detailText);
    details->setStyleSheet("font-size: 14px; line-height: 24px; margin-top: 10px;");

    layout->addWidget(title);
    layout->addWidget(bar);
    layout->addWidget(details);
    layout->addStretch();
    QPushButton *btnClose = new QPushButton("关闭");
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(btnClose);
    dlg.exec();
}
