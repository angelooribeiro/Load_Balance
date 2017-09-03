#ifndef CDATABASE_H
#define CDATABASE_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class QSqlTableModel;
class QSqlQuery;

class CDataBase : public QWidget
{
    Q_OBJECT

public:
    explicit CDataBase(QWidget *parent = 0);
    static CDataBase& getInstance();
    bool db_connect(void);
    ~CDataBase();

private slots:

private:
  // Ui::CDataBase *ui;
    static CDataBase* m_instance;
    QSqlDatabase m_database;
    QSqlQuery m_query;
};

#endif // CDATABASE_H
