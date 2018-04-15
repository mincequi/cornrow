#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <iostream>

#include <QDebug>
#include <QPen>
#include <QThread>
#include <QTimer>

#include <kplotaxis.h>
#include <kplotobject.h>
#include <kplotwidget.h>

#include "common/Types.h"
#include "common/Util.h"

using namespace std::placeholders;

#define FUNC(code) std::string(1, static_cast<uint8_t>(code))

class MainWindow::Private
{
public:
    Private()
    {
        sumPlot = new KPlotObject(Qt::darkGreen, KPlotObject::Lines);
        sumPlot->setLinePen(QPen(Qt::darkGreen, 2.5));
    }

    ~Private()
    {
    }

    QList<KPlotObject*> plots;
    KPlotObject*        sumPlot;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    d(new Private()),
    m_model(new Model(twelfthOctaveBandsTable, qTable, this))
{
    ui->setupUi(this);

    ui->freqSpinBox->setTable(m_model->m_freqTable, 68);
    ui->qSpinBox->setTable(qTable, 17);

    ui->plotWidget->axis(KPlotWidget::TopAxis)->setVisible(false);
    ui->plotWidget->setLimits(0.0, m_model->m_freqTable.size()-1, -27.0, 9.0);
    ui->plotWidget->axis(KPlotWidget::LeftAxis)->setMajorTickMarks( {-24.0, -18.0, -12.0, -6.0, 0.0, 6.0} );
    ui->plotWidget->axis(KPlotWidget::LeftAxis)->setMinorTickMarks( {-21.0, -15.0, -9.0, -3.0, 3.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setMajorTickMarks( {-24.0, -18.0, -12.0, -6.0, 0.0, 6.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setMinorTickMarks( {-21.0, -15.0, -9.0, -3.0, 3.0} );
    ui->plotWidget->axis(KPlotWidget::RightAxis)->setTickLabelsShown(true);
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setMajorTickMarks( {12.0, 28.0, 44.0, 60.0, 76.0, 92.0, 108.0} );
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setMinorTickMarks({});
    ui->plotWidget->axis(KPlotWidget::BottomAxis)->setCustomTickLabels( { {12.0, QString::number(m_model->m_freqTable.at(12.0))},
                                                                          {28.0, QString::number(m_model->m_freqTable.at(28.0))},
                                                                          {44.0, QString::number(m_model->m_freqTable.at(44.0))},
                                                                          {60.0, QString::number(m_model->m_freqTable.at(60.0))},
                                                                          {76.0, QString::number(m_model->m_freqTable.at(76.0))},
                                                                          {92.0, QString::number(m_model->m_freqTable.at(92.0))},
                                                                          {108.0, QString::number(m_model->m_freqTable.at(108.0))},
                                                                        });
    ui->plotWidget->setBackgroundColor(palette().color(backgroundRole()));
    ui->plotWidget->setForegroundColor(palette().color(foregroundRole()));
    ui->plotWidget->setAxesFontSize(font().pointSize());
    ui->plotWidget->setAntialiasing(true);
    ui->plotWidget->addPlotObject(d->sumPlot);

    discover();

    connect(ui->addButton, &QToolButton::clicked, m_model, &Model::addFilter);
    connect(ui->deleteButton, &QToolButton::clicked, m_model, &Model::deleteFilter);
    connect(ui->filterComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_model, &Model::setCurrentFilter);
    connect(ui->typeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int i) { m_model->setType(i+1); });
    connect(ui->freqSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() { m_model->setFreqSlider((float)ui->freqSpinBox->index()/(float)(twelfthOctaveBandsTable.size()-1)); });
    connect(ui->gainSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), m_model, &Model::setGain);
    connect(ui->qSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() { m_model->setQSlider((float)ui->qSpinBox->index()/(float)(qTable.size()-1)); });

    connect(m_model, &Model::filterCountChanged, this, &MainWindow::onFilterCountChanged);
    connect(m_model, &Model::typeChanged, this, &MainWindow::onTypeChanged);
    connect(m_model, &Model::freqChanged, this, &MainWindow::onFreqChanged);
    connect(m_model, &Model::gainChanged, this, &MainWindow::onGainChanged);
    connect(m_model, &Model::qChanged, this, &MainWindow::onQChanged);
}

MainWindow::~MainWindow()
{
    delete ui;

    if (m_protocolAdapter) delete m_protocolAdapter;
    if (m_rpcClient) delete m_rpcClient;
}

void MainWindow::onFilterCountChanged()
{
    const int count = m_model->filterCount();

    if (ui->filterComboBox->count() < count) {
        d->plots.append(new KPlotObject(Qt::darkGreen, KPlotObject::Lines));
        ui->plotWidget->addPlotObject(d->plots.back());
        ui->filterComboBox->addItem(QString::number(count));
        ui->filterComboBox->setCurrentIndex(count-1);
    } else if (ui->filterComboBox->count() > count) {
        ui->plotWidget->removePlotObject(d->plots.at(ui->filterComboBox->currentIndex()));
        d->plots.removeAt(ui->filterComboBox->currentIndex());
        ui->filterComboBox->removeItem(ui->filterComboBox->count()-1);
    }

    // Update ui/plot
    updateUi();
    updateFilter();

    // Send
    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterCount(count);
}

void MainWindow::onTypeChanged()
{
    ui->typeComboBox->setCurrentIndex(static_cast<int>(m_model->m_curFilter->t)-1);

    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterType(m_model->m_curIndex, m_model->m_curFilter->t);
}

void MainWindow::onFreqChanged()
{
    ui->freqSpinBox->setIndex(m_model->m_curFilter->f);

    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterFreq(m_model->m_curIndex, m_model->m_curFilter->f);
}

void MainWindow::onGainChanged()
{
    ui->gainSpinBox->setValue(m_model->m_curFilter->g);

    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterGain(m_model->m_curIndex, m_model->m_curFilter->g);
}

void MainWindow::onQChanged()
{
    ui->qSpinBox->setIndex(m_model->m_curFilter->q);

    updateFilter();

    if (!m_protocolAdapter) return;
    m_protocolAdapter->setFilterQ(m_model->m_curIndex, m_model->m_curFilter->q);
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
    bool success = m_rpcClient->call(FUNC(v1::Code::Login), static_cast<uint8_t>(Version1), std::string()).as<bool>();

    if (success) {
        m_protocolAdapter = new v1::ClientAdapter(*m_rpcClient, [this](Error error, std::string errorString) {
                            if (error == Error::Timeout) {
                                QMetaObject::invokeMethod(this, "onProtocolTimeout", Qt::QueuedConnection);
                            }});
    }
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
    ui->deleteButton->setEnabled(m_model->filterCount() != 0);
    ui->filterComboBox->setEnabled(m_model->filterCount() != 0);
    ui->typeComboBox->setEnabled(m_model->filterCount() != 0);
    ui->freqSpinBox->setEnabled(m_model->filterCount() != 0);
    ui->gainSpinBox->setEnabled(m_model->filterCount() != 0);
    ui->qSpinBox->setEnabled(m_model->filterCount() != 0);

    ui->addButton->setEnabled(m_model->filterCount() < 12);
}

void MainWindow::updateFilter()
{
    if (!m_model->m_curFilter) {
        d->sumPlot->setShowLines(false);
        ui->plotWidget->update();
        return;
    } else {
        d->sumPlot->setShowLines(true);
    }

    // Change plot colors
    for (const auto& p : d->plots) {
        //f.plot->setLinePen(QPen(palette().color(QPalette::Light), 1.5));
        p->setLinePen(QPen(Qt::darkGray, 1.5));
    }

    // Compute filter response
    {
        Model::Filter& f = *(m_model->m_curFilter);
        computeResponse({f.t, m_model->m_freqTable.at(f.f), f.g, qTable.at(f.q)}, m_model->m_freqTable, &(f.mags), &(f.phases));
        auto p = d->plots.at(m_model->m_curIndex);
        p->clearPoints();
        for (size_t i = 0; i < f.mags.size(); ++i) {
            p->addPoint(i, f.mags.at(i));
            p->setLinePen(QPen(Qt::blue, 2.0));
        }

        BiQuad biquad;
        computeBiQuad(48000, {f.t, m_model->m_freqTable.at(f.f), f.g, qTable.at(f.q)}, &biquad);
        qDebug() << "b0: " << biquad.b0;
        qDebug() << "b1: " << biquad.b1;
        qDebug() << "b2: " << biquad.b2;
        qDebug() << "a1: " << biquad.a1;
        qDebug() << "a2: " << biquad.a2;

    }

    // Update sum plot
    std::vector<float> sums(m_model->m_freqTable.size());
    for (const auto& f1 : m_model->m_filters) {
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
