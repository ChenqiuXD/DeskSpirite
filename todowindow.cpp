#include "todowindow.h"
#include "ui_todowindow.h"

#include <QFile>
#include <QTextCodec>
#include <iostream>

todoWindow::todoWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::todoWindow)
{
    ui->setupUi(this);

    QString str = "MM-dd hh:mm:ss";
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

    if(ui->lineEdit->text()==""){
        return;
    }
    item.content = ui->lineEdit->text();
    item.isPaused = false;
    item.duration = 0;
    item.pauseTime = QDateTime::currentDateTime();

    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("MM-dd hh:mm:ss");
    item.beginTime = time;
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
    QString str = time.toString("MM-dd hh:mm:ss");
    memoItem[index].endTime = time;

    //listWidget->takeItem(listWidget->currentRow());
    showTodo();
    showDone();
}

void todoWindow::on_pauseButton_clicked()
{
    if(ui->listWidget->currentItem()==NULL){
        return;
    }

    QString content = ui->listWidget->currentItem()->text();
    int index = findMemoItemIndex(content,ui->listWidget->currentRow());
    if(memoItem[index].isPaused==false){
        updateDuration(&memoItem[index]);
        memoItem[index].isPaused = true;
        QDateTime time = QDateTime::currentDateTime();
        memoItem[index].pauseTime = time;
        ui->pauseButton->setText("continue");
    }else{
        memoItem[index].isPaused = false;
        ui->pauseButton->setText("pause");
    }
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
        QDateTime time = QDateTime::fromString(line, "MM-dd hh:mm:ss");
        item.beginTime = time;

        if (item.isDone==1){
            line = in.readLine();
            time = QDateTime::fromString(line, "MM-dd hh:mm:ss");
            item.endTime = time;
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
        QString str;
        str = memoItem[i].beginTime.toString("MM-dd hh:mm:ss");
        out<<str<<endl;
        if(memoItem[i].isDone==1)
            str = memoItem[i].endTime.toString("MM-dd hh:mm:ss");
            out<<str<<endl;
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
    if (index>=0){
        ui->dateLabel->setText(memoItem[index].beginTime.toString("MM-dd hh:mm:ss"));
        updateDuration(&memoItem[index]);
        QString durStr(memoItem[index].duration);
    }

}

void todoWindow::updateDuration(memoitem* item){
    if(item->isPaused==true){return;}
    else{
        QDateTime time = QDateTime::currentDateTime();
        item->duration += time.secsTo(item->pauseTime);
    }
}

int todoWindow::secTomin(int sec){
    int min;
    min = sec/60;
    return min;
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
            temp += memoItem[i].beginTime.toString("MM-dd hh:mm:ss");
            temp += "--";
            temp += memoItem[i].endTime.toString("MM-dd hh:mm:ss");
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
    saveBeforeExit();
    event->ignore();
    reject();
}

void todoWindow::on_listWidget_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    QString content = ui->listWidget->currentItem()->text();
    int itemIndex = findMemoItemIndex(content,ui->listWidget->currentRow());
    if(memoItem[itemIndex].isPaused){
        ui->pauseButton->setText("continue");
    }else{
        ui->pauseButton->setText("pause");
    }
}
