#include "stat.h"
#include "ui_stat.h"
#include <QMessageBox>
#include <QMainWindow>

Stat::Stat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Stat)
{
    ui->setupUi(this);

    monthChart = new QChart();
    monthChart->legend()->setVisible(false);
    monthChartView = new QChartView(monthChart);
    lineSeries = new QLineSeries();

    yearLayout = new QGridLayout;
    monthLayout = new QGridLayout;

}

Stat::~Stat()
{
    delete yearChart;
    delete yearChartView;

    delete monthChart;
    delete monthChartView;

    delete yearLayout;
    delete monthLayout;

    delete ui;
}

void Stat::SetData(DataBase * db, const QString &name, const QString& code, const QDate date)
{
    if(airportName.isEmpty()){      //Запускаем только 1 раз
        dataBase = db;
        airportName = name;
        airportCode = code;
        statDate = date;
        on_tw_statTab_currentChanged(ui->tw_statTab->currentIndex());
        QStringList ml  = {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
        ui->cb_monthList->addItems(ml);

        connect(dataBase, &DataBase::sig_SendStatusYearWorkloadRequest, this, &Stat::ReceiveStatusYearWorkloadRequest);
        connect(dataBase, &DataBase::sig_SendStatusMonthWorkloadRequest, this, &Stat::ReceiveStatusMonthWorkloadRequest);


        auto req = [&]{dataBase->RequestYearWorkload(code, date.year());};
        QtConcurrent::run(req);
    }

}

void Stat::ReceiveStatusYearWorkloadRequest(const QSqlError error, QSqlQuery * query)
{
    //qDebug() << "YearWorkload ok";
    if(error.type() == QSqlError::NoError){
    QBarSet *barSet = new QBarSet("Загрузка по месяцам");

    QBarSeries *series = new QBarSeries();
    series->append(barSet);
    yearChart = new QChart();
    yearChart->legend()->setVisible(false);
    yearChart->addSeries(series);
    yearChartView = new QChartView(yearChart);

    QStringList categories;

    qreal maxCount = 0;
    while(query->next()){
        *barSet << query->value(0).toReal();
        categories << query->value(1).toString();
        if(maxCount < query->value(0).toReal()) maxCount = query->value(0).toReal();
    }
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    yearChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxCount);
    yearChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);


    ui->wid_yearWorkload->setLayout(yearLayout);
    yearLayout->addWidget(yearChartView);
    yearChartView->show( );

    }
    else{
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("Ошибка выполнения запроса");
        mb.setText(error.text());
        mb.exec();
    }
}

void Stat::ReceiveStatusMonthWorkloadRequest(const QSqlError error, QSqlQuery * query)
{
    if(error.type() == QSqlError::NoError){
        //qDebug() << "MonthWorkload ok";
        lineSeries->clear();
        if(monthChart->series().count() > 0){
            monthChart->removeSeries(lineSeries);
        }
        while(query->next()){
            //qDebug() << query->value(1).toInt() << ", " << query->value(0).toReal();
            lineSeries->append(query->value(1).toInt(), query->value(0).toReal());
        }
        monthChart->addSeries(lineSeries);
        monthChart->createDefaultAxes();

        ui->wid_monthWorkload->setLayout(monthLayout);
        monthLayout->addWidget(monthChartView);

        monthChartView->show();
    }
    else{
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setWindowTitle("Ошибка выполнения запроса");
        mb.setText(error.text());
        mb.exec();
    }
}


void Stat::on_tw_statTab_currentChanged(int index)
{
    if(index == 0){
        setWindowTitle("Загруженность аэропорта " + airportName + " за " + statDate.toString("yyyy") + " год.");
    }
    else{
        setWindowTitle("Загруженность аэропорта " + airportName + " за месяц.");
        RequestMonthWorkload();
    }
}

void Stat::RequestMonthWorkload()
{

    auto req = [&]{dataBase->RequestMonthWorkload(airportCode, ui->cb_monthList->currentIndex() + 1, statDate.year());};
    QtConcurrent::run(req);
}

void Stat::on_cb_monthList_currentIndexChanged(int index)
{
    RequestMonthWorkload();
}

