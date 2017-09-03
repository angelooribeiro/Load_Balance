#ifndef CPIDDIALOG_H
#define CPIDDIALOG_H

#include <QDialog>
#include <QSqlTableModel>
#include <QDebug>

#include "cdatabase.h"

namespace Ui {
class CPidDialog;
}

class CDataBase;

class CPidDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CPidDialog(QWidget *parent = 0);
    ~CPidDialog();

private slots:
    void on_pbRefresh_clicked();
    void on_pbClose_clicked();

private:
    Ui::CPidDialog *ui;

    CDataBase *mDb;
    QSqlTableModel *m_model;
};

#endif // CPIDDIALOG_H
