#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <iostream>

#include <QPen>
#include <QThread>
#include <QTimer>

#include <kplotaxis.h>
#include <kplotobject.h>
#include <kplotwidget.h>

#include "common/Types.h"
#include "common/Util.h"

using namespace std::placeholders;

class MainWindow::Private
{
public:
    Private(MainWindow* q_)
        : q(q_)
    {
        sumPlot = new KPlotObject(Qt::darkGreen, KPlotObject::Lines);
        sumPlot->setLinePen(QPen(Qt::darkGreen, 2.5));
    }

    ~Private()
    {
    }

    MainWindow* q;

    const std::vector<float>  freqTable = twelfthOctaveBandsTable;
    QList<Filter>   filters;
    Filter*         curFilter = nullptr;
    int             curIndex = -1;
    KPlotObject*    sumPlot;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_freqTable(twelfthOctaveBandsTable),
    d(new Private(this))
{
    ui->setupUi(this);

    ui->freqSpinBox->setTable(m_freqTable, 68);
    ui->qSpinBox->setTable(qTable, 17);

    ui->plotWidget->axis(KPlotWidget::TopAxis)->setVisible(false);
    ui->plotWidget->setLimits(0.0, m_freqTable.size()-1, -27.0, 9.0);
    ui->plotWidget->axis(KPlotWidget::LeftAxis)->setMajorTickMarks( {-24.0, -18.0, -12.0, -6.0, 0.0, 6.0} );
    ui->plotWidget->axis(KPlotWidget::LeftAxis)->setMinorTickMarks( {-21.0, -15.0, -9.0, -3.0, 3.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setMajorTickMarks( {-24.0, -18.0, -12.0, -6.0, 0.0, 6.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setMinorTickMarks( {-21.0, -15.0, -9.0, -3.0, 3.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setTickLabelsShown(true);
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setMajorTickMarks( {12.0, 28.0, 44.0, 60.0, 76.0, 92.0, 108.0} );
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setMinorTickMarks({});
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setCustomTickLabels( { {12.0, QString::number(d->freqTable.at(12.0))},
                                                                          {28.0, QString::number(d->freqTable.at(28.0))},
                                                                          {44.0, QString::number(d->freqTable.at(44.0))},
                                                                          {60.0, QString::number(d->freqTable.at(60.0))},
                                                                          {76.0, QString::number(d->freqTable.at(76.0))},
                                                                          {92.0, QString::number(d->freqTable.at(92.0))},
                                                                          {108.0, QString::number(d->freqTable.at(108.0))},
                                                                        });
    ui->plotWidget->setBackgroundColor(palette().color(backgroundRole()));
    ui->plotWidget->setForegroundColor(palette().color(foregroundRole()));
    ui->plotWidget->setAxesFontSize(font().pointSize());
    ui->plotWidget->setAntialiasing(true);
    ui->plotWidget->addPlotObject(d->sumPlot);

    discover();
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_protocolAdapter) delete m_protocolAdapter;
    if (m_rpcClient) delete m_rpcClient;
}

void MainWindow::on_addButton_clicked()
{
    // Update model (pimpl)
    d->filters.push_back({});
    d->curFilter = &(d->filters.back());
    d->curIndex = d->filters.size()-1;

    // Setup ui/plot
    ui->filterComboBox->addItem(QString::number(d->filters.size()));
    ui->filterComboBox->setCurrentIndex(d->filters.size()-1);
    ui->plotWidget->addPlotObject(d->curFilter->plot);

    // Update ui/plot
    updateUi();
    updateFilter();

    // Send
    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterCount(d->filters.size());
}

void MainWindow::on_deleteButton_clicked()
{
    // Save old values
    auto oldPlot = d->curFilter->plot;

    // Update model (pimpl)
    d->filters.removeAt(d->curIndex);
    if (d->filters.empty()) {
        d->curFilter = nullptr;
        d->curIndex = -1;
    } else {
        if (d->curIndex >= d->filters.size()) {
            d->curIndex = d->filters.size()-1;
        }
        d->curFilter = &(d->filters[d->curIndex]);
    }

    // Setup ui/plot
    ui->filterComboBox->removeItem(ui->filterComboBox->currentIndex());
    for (int i = 0; i < d->filters.size(); ++i) {
        ui->filterComboBox->setItemText(i, QString::number(i+1));
    }
    ui->plotWidget->removePlotObject(oldPlot);

    // Update ui/plot
    updateUi();
    updateFilter();

    // Send
    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterCount(ui->filterComboBox->count());
}

void MainWindow::on_filterComboBox_currentIndexChanged(int i)
{
    // Update model (pimpl)
    if (d->filters.empty()) {
        d->curFilter = nullptr;
        d->curIndex = -1;
    } else {
        d->curIndex = i;
        d->curFilter = &(d->filters[i]);
    }

    if (d->curFilter) {
        ui->freqSpinBox->setIndex(d->curFilter->f);
        ui->gainSpinBox->setValue(d->curFilter->g);
        ui->qSpinBox->setIndex(d->curFilter->q);
    }

    updateFilter();
}

void MainWindow::on_typeComboBox_currentIndexChanged(int i)
{
    d->curFilter->t = static_cast<Type>(i+1);
    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterType(ui->filterComboBox->currentIndex(), d->curFilter->t);
}

void MainWindow::on_freqSpinBox_valueChanged(double)
{
    d->curFilter->f = ui->freqSpinBox->index();
    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterFreq(ui->filterComboBox->currentIndex(), ui->freqSpinBox->index());
}

void MainWindow::on_gainSpinBox_valueChanged(double g)
{
    d->curFilter->g = g;
    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterGain(ui->filterComboBox->currentIndex(), g);
}

void MainWindow::on_qSpinBox_valueChanged(double)
{
    d->curFilter->q = ui->qSpinBox->index();
    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterQ(ui->filterComboBox->currentIndex(), ui->qSpinBox->index());
}

void MainWindow::onServiceDiscovered(QString hostname, QString address, quint16 port)
{
    QString status("Discovered cornrow: ");
    status += address + ":" + QString::number(port);
    ui->statusBar->showMessage(status, 5000);

    if (m_protocolAdapter) delete m_protocolAdapter;
    if (m_rpcClient) delete m_rpcClient;
    m_rpcClient = new rpc::client(address.toStdString(), port);
    m_rpcClient->set_timeout(500);
    m_protocolAdapter = new v1::ClientProtocolAdapter(*m_rpcClient, [this](Error error, std::string errorString) {
                        if (error == Error::Timeout) {
                            QMetaObject::invokeMethod(this, "onProtocolTimeout", Qt::QueuedConnection);
                        }});
}

void MainWindow::onProtocolTimeout()
{
    if (m_protocolAdapter) {
        delete m_protocolAdapter;
        m_protocolAdapter = nullptr;
    }
    if (m_rpcClient) {
        delete m_rpcClient;
        m_rpcClient = nullptr;
    }

    m_zeroconfBonjour.stop();
    discover();
}

void MainWindow::discover()
{
    ui->statusBar->showMessage("Discovering");
    if (!m_zeroconfBonjour.discover("_cornrow._tcp", [this](ZeroconfBonjour::Service service) {
        QMetaObject::invokeMethod(this, "onServiceDiscovered", Qt::QueuedConnection,
                                  Q_ARG(QString, QString::fromStdString(service.hostname)),
                                  Q_ARG(QString, QString::fromStdString(service.address)),
                                  Q_ARG(quint16, service.port));
    })) {
        ui->statusBar->showMessage("Discovery error");
    }
}

void MainWindow::updateUi()
{
    ui->deleteButton->setEnabled(!d->filters.empty());
    ui->filterComboBox->setEnabled(!d->filters.empty());
    ui->typeComboBox->setEnabled(!d->filters.empty());
    ui->freqSpinBox->setEnabled(!d->filters.empty());
    ui->gainSpinBox->setEnabled(!d->filters.empty());
    ui->qSpinBox->setEnabled(!d->filters.empty());

    ui->addButton->setEnabled(d->filters.size() < 12);
}

void MainWindow::updateFilter()
{
    if (!d->curFilter) {
        d->sumPlot->setShowLines(false);
        ui->plotWidget->update();
        return;
    } else {
        d->sumPlot->setShowLines(true);
    }

    // Change plot colors
    for (const auto& f : d->filters) {
        //f.plot->setLinePen(QPen(palette().color(QPalette::Light), 1.5));
        f.plot->setLinePen(QPen(Qt::darkGray, 1.5));
    }

    // Compute filter response
    {
        Filter& f = *(d->curFilter);
        computeResponse(f.t, d->freqTable.at(f.f), f.g, qTable.at(f.q), d->freqTable, &(f.mags), &(f.phases));
        f.plot->clearPoints();
        for (size_t i = 0; i < f.mags.size(); ++i) {
            f.plot->addPoint(i, f.mags.at(i));
            f.plot->setLinePen(QPen(Qt::blue, 2.0));
        }
    }

    // Update sum plot
    std::vector<float> sums(d->freqTable.size());
    for (const auto& f1 : d->filters) {
        for (size_t i = 0; i < f1.mags.size(); ++i) {
            sums[i] += f1.mags[i];
        }
    }
    d->sumPlot->clearPoints();
    for (size_t i = 0; i < sums.size(); ++i) {
        d->sumPlot->addPoint(i, sums.at(i));
    }

    ui->plotWidget->update();
}
