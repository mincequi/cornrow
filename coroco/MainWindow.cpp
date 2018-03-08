#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addButton_clicked()
{
    ui->filterComboBox->addItem(QString::number(ui->filterComboBox->count()+1));
    enableFilterWidgets(true);

    ui->filterComboBox->setCurrentIndex(ui->filterComboBox->count()-1);
}

void MainWindow::on_deleteButton_clicked()
{
    ui->filterComboBox->removeItem(ui->filterComboBox->count()-1);
    enableFilterWidgets(ui->filterComboBox->count() != 0);
}

void MainWindow::enableFilterWidgets(bool enable)
{
    ui->filterComboBox->setEnabled(enable);
    ui->typeComboBox->setEnabled(enable);
    ui->freqSpinBox->setEnabled(enable);
    ui->gainSpinBox->setEnabled(enable);
    ui->qSpinBox->setEnabled(enable);
}
