#include "cpiddialog.h"
#include "ui_cpiddialog.h"

CPidDialog::CPidDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPidDialog)
{
    ui->setupUi(this);
}

CPidDialog::~CPidDialog()
{
    delete ui;
}

void CPidDialog::on_pbRefresh_clicked()
{
    m_model = new QSqlTableModel(this);
    m_model->setTable("tab_read");
    m_model->select();
    ui->tabPid->setModel(m_model);
    // block edit on table view
    ui->tabPid->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void CPidDialog::on_pbClose_clicked()
{
    this->close();
}

