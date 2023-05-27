#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtConcurrent>
#include "database.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:


private slots:

    void ReceiveStatusConnection(const bool status);
    void ReceiveStatusAirportRequest(const QSqlError, QSqlQuery*);
    void ReceiveStatusFlightsRequest(const QSqlError, QSqlQueryModel*);



    void on_pb_getFlightList_clicked();

    void on_pb_showStat_clicked();

private:
    void ConnectToDB(void);
    Ui::MainWindow *ui;
    DataBase* dataBase {nullptr};
    bool connectionStatus {false};
};
#endif // MAINWINDOW_H
