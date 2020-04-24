#ifndef TODOWINDOW_H
#define TODOWINDOW_H

#include <QDialog>
#include <QCloseEvent>
#include <QList>
#include <QString>

class memoitem{
public:
    bool isDone;
    QString content;
    QString beginTime;
    QString endTime;
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
    void addButtonEnableFunc();
    void doneButtonEnableFunc();
    void saveBeforeExit();
    void dateShow();
    void cleanCookie();
};

#endif // TODOWINDOW_H
