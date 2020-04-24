#include "todowindow.h"
#include "ui_todowindow.h"

#include <QFile>
#include <QTextCodec>
#include <QDateTime>
#include <QList>
#include <QDebug>

todoWindow::todoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::todoWindow)
{
    ui->setupUi(this);

    QString str = "MM-dd hh:mm:ss ddd";
    ui->dateLabel->setText(str);

    connect(ui->addButton,SIGNAL(clicked()),this,SLOT(addList()));
    connect(ui->doneButton,SIGNAL(clicked()),this,SLOT(deleteList()));
    connect(ui->clean,SIGNAL(clicked()),this,SLOT(cleanCookie()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(addButtonEnableFunc()));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(doneButtonEnableFunc()));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(dateShow()));
    initToDoList();
}

todoWindow::~todoWindow()
{
    delete ui;
}

void todoWindow::addList(){
    memoitem item;
    item.isDone=0;

    item.content = ui->lineEdit->text();

    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("MM-dd hh:mm:ss ddd");
    item.beginTime = str;
    item.row = -1;

    memoItem.append(item);

    ui->lineEdit->clear();

    showTodo();
}

void todoWindow::deleteList(){
    if (ui->listWidget->currentItem()==NULL){
        ui->doneButton->setEnabled(false);
        return ;
    }

    QString content = ui->listWidget->currentItem()->text();
//    qDebug() << "crnt row:" << ui->listWidget->currentRow() << std::endl;

    int index = findMemoItemIndex(content,ui->listWidget->currentRow());
    memoItem[index].isDone=1;
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss ddd");
    memoItem[index].endTime = str;

    //listWidget->takeItem(listWidget->currentRow());
    showTodo();
    showDone();
}

void todoWindow::addButtonEnableFunc(){
    ui->addButton->setEnabled(!(ui->lineEdit->text().isEmpty()));
}

void todoWindow::doneButtonEnableFunc(){
    ui->doneButton->setEnabled(ui->listWidget->currentItem()!=NULL);
}

void todoWindow::cleanCookie(){
    QList<memoitem>::iterator i;
    i=memoItem.begin();
    for (i=memoItem.begin(); i!=memoItem.end(); i++){
        if((*i).isDone==1){
            i=memoItem.erase(i);
            i--;
        }
    }
    qDebug()<<memoItem.count();
    ui->doneList->clear();
}

void todoWindow::initToDoList(){
    QFile file("List.data");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while( !in.atEnd()){
        memoitem item;

        QString line = in.readLine();
        (line=="0")? (item.isDone=0) : (item.isDone=1);

        line = in.readLine();
        item.content = line;
        line = in.readLine();
        item.beginTime = line;

        if (item.isDone==1){
            line = in.readLine();
            item.endTime = line;
        }

        memoItem.append(item);
    }
    in.flush();
    file.close();

    showTodo();
    showDone();
}

void todoWindow::saveBeforeExit(){

    QFile file("List.data");
    file.remove();
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream out(&file);
    for (int i=0; i<memoItem.count(); i++){
        out<<memoItem[i].isDone<<endl;
        out<<memoItem[i].content<<endl;
        out<<memoItem[i].beginTime<<endl;
        if(memoItem[i].isDone==1)
            out<<memoItem[i].endTime<<endl;
    }
    out.flush();
    file.close();
}

void todoWindow::dateShow(){
    if (!ui->listWidget->currentItem()){
        ui->dateLabel->setText("");
        return ;
    }
    int index = findMemoItemIndex(ui->listWidget->currentItem()->text(),ui->listWidget->currentRow());
    if (index>=0)
        ui->dateLabel->setText(memoItem[index].beginTime);
}

inline int todoWindow::findMemoItemIndex(QString s,int row){
    for(int i=0; i<memoItem.count(); i++){
        qDebug()<<memoItem[i].content<<"  "<<memoItem[i].row<<endl;
        if (memoItem[i].content == s && memoItem[i].row == row)
            return i;
    }
    return -1;
}

void todoWindow::showDone(){
    ui->doneList->clear();
    for(int i=0; i<memoItem.count(); i++){
        if (memoItem[i].isDone==1){
            QString temp = memoItem[i].content;
            temp += "   ";
            temp += memoItem[i].beginTime;
            temp += "--";
            temp += memoItem[i].endTime;
            ui->doneList->addItem(temp);
        }
    }
}

void todoWindow::showTodo(){
    ui->listWidget->clear();
    int count = 0;
    for(int i=0; i<memoItem.count(); i++){
        if (memoItem[i].isDone==0){
            ui->listWidget->addItem(memoItem[i].content);
            memoItem[i].row = count;
            count++;
        }
    }
}

void todoWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    reject();
}
