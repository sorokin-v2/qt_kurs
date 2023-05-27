#ifndef DATABASE_H
#define DATABASE_H

#include <QTableWidget>
#include <QObject>
#include <QSqlDataBase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>


#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

//Количество полей данных необходимых для подключения к БД
#define NUM_DATA_FOR_CONNECT_TO_DB 5


//Типы запросов
enum requestType{

    DepartFlights = 1,
    ArrivalFlights   = 2,
};



class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    void ConnectToDataBase(void);
    void DisconnectFromDataBase(const QString);
    void AddDataBase(const QString driver, const QString nameDB = "");
    void RequestAirportList();
    void RequestFlyList(const requestType, const QString, const QString);
    void RequestYearWorkload(const QString&, const int);
    void RequestMonthWorkload(const QString&, const int, const int);
    void ReadAnswerFromDB( int answerType );
    QSqlError GetLastError(void);


signals:

   void sig_SendDataFromDB(const QTableWidget *tableWg, int typeR);
   void sig_SendStatusConnection(bool);
   void sig_SendStatusAirportRequest(QSqlError, QSqlQuery*);
   void sig_SendStatusYearWorkloadRequest(QSqlError, QSqlQuery*);
   void sig_SendStatusMonthWorkloadRequest(QSqlError, QSqlQuery*);
   void sig_SendStatusFlightsRequest(QSqlError, QSqlQueryModel*);


private:

    QSqlDatabase* dataBase;
    QSqlQuery* query;
    QSqlQueryModel* queryModel;

};

#endif // DATABASE_H
