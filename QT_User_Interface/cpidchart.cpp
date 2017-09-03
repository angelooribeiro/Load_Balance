#include "cpidchart.h"
#include "ui_cpidchart.h"

CPidChart::CPidChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPidChart)
{
    ui->setupUi(this);
}

CPidChart::~CPidChart()
{
    delete ui;
}
