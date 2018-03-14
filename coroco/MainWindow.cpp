#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <iostream>

#include <QGraphicsItemGroup>
#include <QThread>

#include <KPlotAxis>
#include <KPlotObject>
#include <KPlotWidget>

#include "common/Types.h"
#include "common/Util.h"

using namespace std::placeholders;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->freqSpinBox->setTable(twelfthOctaveBandsTable, 68);
    ui->qSpinBox->setTable(qTable, 17);

    ui->plotWidget->axis(KPlotWidget::TopAxis)->setVisible(false);
    ui->plotWidget->setLimits(0.0, 120.0, -27.0, 9.0);
    ui->plotWidget->axis(KPlotWidget::LeftAxis)->setMajorTickMarks( {-24.0, -18.0, -12.0, -6.0, 0.0, 6.0} );
    ui->plotWidget->axis(KPlotWidget::LeftAxis)->setMinorTickMarks( {-21.0, -15.0, -9.0, -3.0, 3.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setMajorTickMarks( {-24.0, -18.0, -12.0, -6.0, 0.0, 6.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setMinorTickMarks( {-21.0, -15.0, -9.0, -3.0, 3.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setTickLabelsShown(true);
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setMajorTickMarks( {12.0, 28.0, 44.0, 60.0, 76.0, 92.0, 108.0} );
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setMinorTickMarks({});
    ui->plotWidget->setBackgroundColor(palette().color(backgroundRole()));
    ui->plotWidget->setForegroundColor(palette().color(foregroundRole()));
    ui->plotWidget->setAxesFontSize(font().pointSize());
    ui->plotWidget->setAntialiasing(true);

    m_zeroconf.listen([this](std::string hostname, std::string address, uint16_t port) {
        QMetaObject::invokeMethod(this, "onServiceDiscovered", Qt::QueuedConnection,
                                  Q_ARG(QString, QString::fromStdString(hostname)),
                                  Q_ARG(QString, QString::fromStdString(address)),
                                  Q_ARG(quint16, port));
    });
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_protocolAdapter) delete m_protocolAdapter;
    if (m_rpcClient) delete m_rpcClient;
}

void MainWindow::on_portBox_valueChanged(int i)
{
    onServiceDiscovered("Bla", "127.0.0.1", i);
}

void MainWindow::on_addButton_clicked()
{
    m_filters.push_back({});
    ui->plotWidget->addPlotObject(m_filters.back().plot);

    ui->filterComboBox->addItem(QString::number(ui->filterComboBox->count()+1));
    enableFilterWidgets(true);
    ui->addButton->setEnabled(ui->filterComboBox->count() < 12);

    ui->filterComboBox->setCurrentIndex(ui->filterComboBox->count()-1);

    updateCurrentFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterCount(ui->filterComboBox->count());
}

void MainWindow::on_deleteButton_clicked()
{
    ui->plotWidget->removePlotObject(m_filters.back().plot);
    m_filters.pop_back();

    ui->filterComboBox->removeItem(ui->filterComboBox->count()-1);
    enableFilterWidgets(ui->filterComboBox->count() != 0);
    ui->addButton->setEnabled(true);

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterCount(ui->filterComboBox->count());
}

void MainWindow::on_filterComboBox_currentIndexChanged(int i)
{
    if (i >= m_filters.size()) return;

    ui->freqSpinBox->setIndex(m_filters.at(i).f);
    ui->gainSpinBox->setValue(m_filters.at(i).g);
    ui->qSpinBox->setIndex(m_filters.at(i).q);
}

void MainWindow::on_freqSpinBox_valueChanged(double)
{
    m_filters.at(ui->filterComboBox->currentIndex()).f = ui->freqSpinBox->index();
    updateCurrentFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterFreq(ui->filterComboBox->currentIndex(), ui->freqSpinBox->index());
}

void MainWindow::on_gainSpinBox_valueChanged(double g)
{
    m_filters.at(ui->filterComboBox->currentIndex()).g = g;
    updateCurrentFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterGain(ui->filterComboBox->currentIndex(), g);
}

void MainWindow::on_qSpinBox_valueChanged(double)
{
    m_filters.at(ui->filterComboBox->currentIndex()).q = ui->qSpinBox->index();
    updateCurrentFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterQ(ui->filterComboBox->currentIndex(), ui->qSpinBox->index());
}

void MainWindow::onServiceDiscovered(QString hostname, QString address, quint16 port)
{
    std::cerr << "QT thread: " << thread()->currentThreadId() << ", hostname: " << hostname.toStdString() << ", address: " << address.toStdString() << ", port: " << port << std::endl;

    if (m_protocolAdapter) delete m_protocolAdapter;
    if (m_rpcClient) delete m_rpcClient;
    m_rpcClient = new rpc::client(address.toStdString(), port);
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

void MainWindow::updateCurrentFilter()
{
    Filter& f = m_filters.at(ui->filterComboBox->currentIndex());

    computeResponse(f.t, twelfthOctaveBandsTable.at(f.f), f.g, qTable.at(f.q), twelfthOctaveBandsTable, &(f.mags), &(f.phases));
    f.plot->clearPoints();
    for (size_t i = 0; i < f.mags.size(); ++i) {
        f.plot->addPoint(i, f.mags.at(i));
    }
    ui->plotWidget->update();
}
