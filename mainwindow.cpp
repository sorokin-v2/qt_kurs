#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "stat.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->showMessage("Подключение к базе данных...");
    dataBase = new DataBase(this);
    ui->tw_flightList->setModel(nullptr);
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnection);
    connect(dataBase, &DataBase::sig_SendStatusAirportRequest, this, &MainWindow::ReceiveStatusAirportRequest);
    connect(dataBase, &DataBase::sig_SendStatusFlightsRequest, this, &MainWindow::ReceiveStatusFlightsRequest);

    auto conn = [&]{dataBase->ConnectToDataBase();};
    QtConcurrent::run(conn);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ReceiveStatusConnection(const bool status)
{
    if(status){
        ui->statusbar->showMessage("Подключено");
        ui->cb_Airport->setEnabled(true);
        ui->cw_date->setEnabled(true);
        ui->pb_getFlightList->setEnabled(true);
        ui->rb_departure->setEnabled(true);
        ui->rb_arrival->setEnabled(true);
        ui->pb_showStat->setEnabled(true);

        auto req = [&]{dataBase->RequestAirportList();};
        QtConcurrent::run(req);
    }
    else{
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("Ошибка");
        mb.setText("Ошибка подключения к БД\n" + dataBase->GetLastError().text());
        mb.exec();

        ui->statusbar->showMessage("Повторное подключение к базе данных...");
    }
}

void MainWindow::ReceiveStatusAirportRequest(const QSqlError error, QSqlQuery* query)
{
    if(error.type() != QSqlError::NoError){
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("Ошибка выполнения запроса");
        mb.setText(error.text());
        mb.exec();
    }
    else{
        while(query->next()){
            ui->cb_Airport->addItem(query->value(0).toString(), query->value(1).toString());
        }
    }
}

void MainWindow::ReceiveStatusFlightsRequest(const QSqlError error, QSqlQueryModel * queryModel)
{
    if(error.type() != QSqlError::NoError){
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("Ошибка выполнения запроса");
        mb.setText(error.text());
        mb.exec();
    }
    else{
        ui->tw_flightList->setModel(queryModel);
        ui->tw_flightList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tw_flightList->horizontalHeader()->setStretchLastSection(true);

        ui->tw_flightList->show();
    }
}


void MainWindow::on_pb_getFlightList_clicked()
{
    ui->lb_flightListLabel->setText("Список рейсов по аэропорту " + ui->cb_Airport->currentText() + " на " + ui->cw_date->selectedDate().toString("dd.MM.yyyy"));
    if(ui->rb_departure->isChecked()) {
        auto req = [&]{dataBase->RequestFlyList(DepartFlights, ui->cb_Airport->currentData().toString(), ui->cw_date->selectedDate().toString("yyyy-MM-dd"));};
        QtConcurrent::run(req);
    }
    else{
        auto req = [&]{dataBase->RequestFlyList(ArrivalFlights, ui->cb_Airport->currentData().toString(), ui->cw_date->selectedDate().toString("yyyy-MM-dd"));};
        QtConcurrent::run(req);
    }
}

void MainWindow::on_pb_showStat_clicked()
{
    Stat statForm;
    statForm.SetData(dataBase, ui->cb_Airport->currentText(), ui->cb_Airport->currentData().toString(), ui->cw_date->selectedDate());
    statForm.exec();
}

