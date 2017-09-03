#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pbSubmitCmd->hide();
    ui->comboBox->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbCreateServer_clicked()
{
      m_server = new QTcpServer(this);
      int port = 1234;

      if(!m_server->listen(QHostAddress::AnyIPv4, port))
      {
          qDebug() << "ERROR: Could not start server";
      }
      else
      {
          qDebug() << "Server started: listening to port "<<port<< "...";
          connect(m_server,SIGNAL(newConnection()),this,SLOT(newConnection()));
      }
}

void MainWindow::newConnection()
{   
   qDebug() << "Client connected";
   m_socket = m_server->nextPendingConnection();
   ui->pbSubmitCmd->show();
   ui->comboBox->show();
   connect(m_socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
   connect(m_socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
}

void MainWindow::readyRead()
{
    QByteArray Data;

    while(m_socket->bytesAvailable())
        Data.append(m_socket->readAll());
    
    qDebug() <<"Data received: " <<Data;
}

void MainWindow::on_pbSubmitCmd_clicked()
{
    int iCmd = ui->comboBox->currentIndex();
    QString cmd = ui->comboBox->currentText();

    if(iCmd==0)
        QMessageBox::information(this,"Error ","Select one command!");
    if (iCmd==1)
        m_socket->write("!ST");
    if (iCmd==2)
        m_socket->write("!SP");
    if (iCmd==3)
        m_socket->write("!CG");
    if (iCmd==4)
        m_socket->write("!CD");
}

void MainWindow::on_pbPidTab_clicked()
{
    mCPidDialog = new CPidDialog(this);
    mCPidDialog->show();
}

void MainWindow::on_pbPidChart_clicked()
{
    mCPidChart = new CPidChart(this);
    mCPidChart->show();
}

void MainWindow::disconnected()
{
    qDebug() << "Client disconnected";
    m_socket->deleteLater();
    exit(0);
}

