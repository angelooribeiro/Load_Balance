#include "cdatabase.h"

#include <QMessageBox>
#include <QSqlError>

CDataBase* CDataBase::m_instance = nullptr;

CDataBase::CDataBase(QWidget *parent): QWidget(parent)
{
    /* Set TCP/IP parameters connection to database server
     * Using mysql server to create the TCP server
     */
        m_database = QSqlDatabase::addDatabase("QMYSQL");
        m_database.setHostName("localhost");
        m_database.setUserName("root");
        m_database.setPort(3306);
        m_database.setDatabaseName("db_p1");
        m_database.setPassword("");
}

CDataBase& CDataBase::getInstance()
{
    if (m_instance == nullptr) {
       m_instance = new CDataBase();
    }
    return *m_instance;
}

bool CDataBase::db_connect(void)
{
    // Open database connection and returns true or false

        if(!m_database.open()){
            QMessageBox::critical(this,"Error !", m_database.lastError().text());
            return false;
        }
        else{
            return true;
        }
}

CDataBase::~CDataBase()
{
}

