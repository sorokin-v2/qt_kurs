#include <QTest>
#include "database.h"
#include <QMessageBox>

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{

    dataBase = new QSqlDatabase();
    query = new QSqlQuery();
    queryModel = new QSqlQueryModel(this);
}

DataBase::~DataBase()
{
    if(dataBase->isOpen()){
        dataBase->close();
    }
    delete query;
    delete dataBase;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);
}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(void)
{
    dataBase->setHostName("981757-ca08998.tmweb.ru");
    dataBase->setDatabaseName("demo");
    dataBase->setUserName("netology_usr_cpp");
    dataBase->setPassword("CppNeto3");
    dataBase->setPort(5432);
    bool statusConnection = dataBase->open();
    while(!statusConnection){
        QTest::qWait(5000);
        emit sig_SendStatusConnection(statusConnection);
        statusConnection = dataBase->open();
    }
    emit sig_SendStatusConnection(statusConnection);
}
/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(const QString nameDb)
{
    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();
}
/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */

void DataBase::RequestAirportList(void)
{
    *query = QSqlQuery(*dataBase);
    QSqlError err;
    if(query->exec("SELECT airport_name->>'ru' as \"airportName\", airport_code FROM bookings.airports_data  order by 1") == false){
        err = query->lastError();
    }
    emit sig_SendStatusAirportRequest(err, query);
}

void DataBase::RequestFlyList(const requestType requestType, const QString airportCode, const QString targetDate)
{
    *query = QSqlQuery(*dataBase);
    switch (requestType) {
        case DepartFlights:{
            queryModel->setQuery(QString("select flight_no as \"Номер рейса\", to_char(scheduled_departure, 'HH24:MI') as \"Время вылета\", "
                                         "ad.airport_name->>'ru' as \"Аэропорт назначения\" from bookings.flights f left join bookings.airports_data ad "
                                         "on ad.airport_code = f.arrival_airport "
                                         "where f.departure_airport = '%1'and scheduled_departure::date = date('%2') "
                                         "order by 2").arg(airportCode).arg(targetDate), *dataBase);

            emit sig_SendStatusFlightsRequest(queryModel->lastError(), queryModel);
            break;

        }
        case ArrivalFlights:{
            queryModel->setQuery(QString("select flight_no as \"Номер рейса\", to_char(scheduled_departure, 'HH24:MI') as \"Время вылета\", "
                                         "ad.airport_name->>'ru' as \"Аэропорт отправления\" from bookings.flights f left join bookings.airports_data ad "
                                         "on ad.airport_code = f.departure_airport "
                                         "where f.arrival_airport = '%1'and scheduled_departure::date = date('%2') "
                                         "order by 2").arg(airportCode).arg(targetDate), *dataBase);

            emit sig_SendStatusFlightsRequest(queryModel->lastError(), queryModel);
            break;
        }
        default:{
            QMessageBox mb;
            mb.setIcon(QMessageBox::Warning);
            mb.setWindowTitle("Внимание!!!");
            mb.setText("Неизвестный тип запроса");
            mb.exec();
        }
    }
}

void DataBase::RequestYearWorkload(const QString & airportCode, const int year)
{
    if(!dataBase->isOpen()){        //Почему-то иногда выскакивает сообщение QSqlQuery::exec: database not open
        dataBase->open();
    }
    *query = QSqlQuery(*dataBase);
    QSqlError err;

    if(query->exec(QString("SELECT count(flight_no), to_char(scheduled_departure, 'Mon') as \"Month\", "
                           "to_char(scheduled_departure, 'MM') as \"Monthnumber\" "
                           "from bookings.flights f "
                           "where ((scheduled_departure::date >= date('%1-01-01') "
                           "and scheduled_departure::date <= date('%1-12-31')) or "
                           "(scheduled_arrival::date >= date('%1-01-01') "
                           "and scheduled_arrival::date <= date('%1-12-31'))) and "
                           "(departure_airport = '%2' or arrival_airport = '%2') "
                           "group by \"Month\", \"Monthnumber\" order by 3").arg(year).arg(airportCode)) == false){
        err = query->lastError();
    }
    emit sig_SendStatusYearWorkloadRequest(err, query);

}

void DataBase::RequestMonthWorkload(const QString& airportCode, const int month, const int year)
{
    if(!dataBase->isOpen()){        //Почему-то иногда выскакивает сообщение QSqlQuery::exec: database not open
        dataBase->open();
    }

    *query = QSqlQuery(*dataBase);
    QSqlError err;

    if(query->exec(QString("SELECT count(flight_no), to_char(scheduled_departure, 'dd') as \"Day\" "
                           "from bookings.flights f "
                           "where ((to_char(scheduled_departure, 'YYYY')::integer = %1 and to_char(scheduled_departure, 'MM')::integer = %2) or "
                           "(to_char(scheduled_arrival, 'YYYY')::integer = %1 and to_char(scheduled_arrival, 'MM')::integer = %2)) and "
                           "(departure_airport = '%3' or arrival_airport = '%3') "
                           "group by \"Day\" order by 2").arg(year).arg(month).arg(airportCode)) == false){
        err = query->lastError();
    }

    emit sig_SendStatusMonthWorkloadRequest(err, query);
}


/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}
