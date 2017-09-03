#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>

#include "cdatabase.h"
#include "cpiddialog.h"
#include "cpidchart.h"

namespace Ui {
class MainWindow;
}

class CPidDialog;
class CDataBase;
class CPidChart;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pbCreateServer_clicked();
    void on_pbSubmitCmd_clicked();
    void on_pbPidTab_clicked();
    void on_pbPidChart_clicked();

public slots:
    void newConnection(); 
    void readyRead();
    void disconnected();

private:
    Ui::MainWindow *ui;
    QList<QString> listData;
    int i;
    CDataBase *mCDatabase;
    CPidDialog *mCPidDialog;
    CPidChart *mCPidChart;
    QTcpServer *m_server;
    QTcpSocket *m_socket;
};

#endif // MAINWINDOW_H
