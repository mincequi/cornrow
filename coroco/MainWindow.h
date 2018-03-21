#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <rpc/client.h>

#include "Types.h"
#include "ZeroconfBonjour.h"
#include "protocol/ProtocolAdaptersV1.h"

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
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_filterComboBox_currentIndexChanged(int i);
    void on_freqSpinBox_valueChanged(double);
    void on_gainSpinBox_valueChanged(double g);
    void on_qSpinBox_valueChanged(double);

    void onServiceDiscovered(QString hostname, QString address, quint16 port);
    void onProtocolTimeout();

private:
    void discover();

    void updateUi();
    void updateFilter();

    Ui::MainWindow *ui;

    const std::vector<float> m_freqTable;

    ZeroconfBonjour m_zeroconfBonjour;
    rpc::client*    m_rpcClient = nullptr;
    v1::ClientProtocolAdapter* m_protocolAdapter = nullptr;

    class Private;
    Private *const d;
};

#endif // MAINWINDOW_H
