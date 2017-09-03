#ifndef CPIDCHART_H
#define CPIDCHART_H

#include <QDialog>

#include "cdatabase.h"

namespace Ui {
class CPidChart;
}

class CDataBase;

class CPidChart : public QDialog
{
    Q_OBJECT

public:
    explicit CPidChart(QWidget *parent = 0);
    ~CPidChart();

private:
    Ui::CPidChart *ui;
    CDataBase *mDb;

};

#endif // CPIDCHART_H
