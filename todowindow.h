#ifndef TODOWINDOW_H
#define TODOWINDOW_H

#include <QDialog>
#include <QCloseEvent>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QDebug>

class memoitem{
public:
    bool isDone;
    QString content;
    bool isPaused;
    QDateTime pauseTime;
    QDateTime beginTime;
    QDateTime endTime;
    int duration; // in second
    int row;
};

namespace Ui {
class todoWindow;
}

class todoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit todoWindow(QWidget *parent = 0);
    ~todoWindow();

    QString timeRecord;
    QList<memoitem> memoItem;
    void initToDoList();
    int findMemoItemIndex(QString s, int row);
    void showDone();
    void showTodo();

private:
    Ui::todoWindow *ui;
    void closeEvent(QCloseEvent *event);

private slots:
    void addList();
    void deleteList();
    void on_pauseButton_clicked();
    void addButtonEnableFunc();
    void doneButtonEnableFunc();
    void saveBeforeExit();
    void dateShow();
    void updateDuration(memoitem* item);
    int secTomin(int sec);
    void cleanCookie();
    void on_listWidget_clicked(const QModelIndex &index);
};

#endif // TODOWINDOW_H
