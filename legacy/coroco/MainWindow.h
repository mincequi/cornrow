#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <rpc/client.h>

#include "Model.h"
#include "Types.h"
#include "ZeroconfBonjour.h"
#include "protocol/v2/ClientAdapter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onFilterCountChanged();
    void onTypeChanged();
    void onFreqChanged();
    void onGainChanged();
    void onQChanged();

    void onServiceDiscovered(QString hostname, QString address, quint16 port);
    void onProtocolTimeout();

private:
    void discover();

    void updateUi();
    void updateFilter();

    Ui::MainWindow *ui;

    ZeroconfBonjour m_zeroconfBonjour;
    rpc::client*    m_rpcClient = nullptr;
    v1::ClientAdapter* m_protocolAdapter = nullptr;

    class Private;
    Private *const d;

    Model* m_model;
};

#endif // MAINWINDOW_H
