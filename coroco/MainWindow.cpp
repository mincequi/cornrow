#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <iostream>

#include <QThread>

#include "common/Types.h"

using namespace std::placeholders;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->freqSpinBox->setTable(twelfthOctaveBandsTable, 68);
    ui->qSpinBox->setTable(qTable, 17);

    m_zeroconf.listen([this](std::string hostname, uint32_t address, uint16_t port) {
        QMetaObject::invokeMethod(this, "onServiceDiscovered", Qt::QueuedConnection,
                                  Q_ARG(QString, QString::fromStdString(hostname)),
                                  Q_ARG(quint32, address),
                                  Q_ARG(quint16, port));
    });
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_protocolAdapter) delete m_protocolAdapter;
    if (m_rpcClient) delete m_rpcClient;
}

void MainWindow::on_addButton_clicked()
{
    ui->filterComboBox->addItem(QString::number(ui->filterComboBox->count()+1));
    enableFilterWidgets(true);
    ui->addButton->setEnabled(ui->filterComboBox->count() < 12);

    ui->filterComboBox->setCurrentIndex(ui->filterComboBox->count()-1);

    resizeFilters(ui->filterComboBox->count());
}

void MainWindow::on_deleteButton_clicked()
{
    ui->filterComboBox->removeItem(ui->filterComboBox->count()-1);
    enableFilterWidgets(ui->filterComboBox->count() != 0);
    ui->addButton->setEnabled(true);

    resizeFilters(ui->filterComboBox->count());
}

void MainWindow::on_filterComboBox_currentIndexChanged(int i)
{

}

void MainWindow::on_freqSpinBox_valueChanged(int d)
{
    if (!m_protocolAdapter) return;

    m_protocolAdapter->setFilterFreq(ui->filterComboBox->currentIndex(), d);
}

void MainWindow::on_gainSpinBox_valueChanged(double g)
{
    if (!m_protocolAdapter) return;

    m_protocolAdapter->setFilterGain(ui->filterComboBox->currentIndex(), g);
}

void MainWindow::on_qSpinBox_valueChanged(int q)
{
    if (!m_protocolAdapter) return;

    m_protocolAdapter->setFilterQ(ui->filterComboBox->currentIndex(), q);
}

void MainWindow::onServiceDiscovered(QString hostname, quint32 address, quint16 port)
{
    std::cerr << "QT thread: " << thread()->currentThreadId() << ", hostname: " << hostname.toStdString() << ", address: " << address << ", port: " << port << std::endl;

    m_rpcClient = new rpc::client("192.168.26.139", port);
    m_rpcClient->set_timeout(500);
    m_protocolAdapter = new v1::ClientProtocolAdapter(*m_rpcClient);
}

void MainWindow::enableFilterWidgets(bool enable)
{
    ui->deleteButton->setEnabled(enable);
    ui->filterComboBox->setEnabled(enable);
    ui->typeComboBox->setEnabled(enable);
    ui->freqSpinBox->setEnabled(enable);
    ui->gainSpinBox->setEnabled(enable);
    ui->qSpinBox->setEnabled(enable);
}

void MainWindow::resizeFilters(int count)
{
    m_freqs.resize(count);
    m_gains.resize(count);
    m_qs.resize(count);
}


