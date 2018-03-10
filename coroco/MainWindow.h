#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <rpc/client.h>

#include "ZeroconfWrapper.h"
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
    void on_freqSpinBox_valueChanged(double f);
    void on_gainSpinBox_valueChanged(double g);
    void on_qSpinBox_valueChanged(double q);

    void onServiceDiscovered(QString hostname, QString address, quint16 port);

private:
    void enableFilterWidgets(bool enable);
    void resizeFilters(int);

    Ui::MainWindow *ui;
    ZeroconfWrapper m_zeroconf;
    rpc::client*    m_rpcClient = nullptr;
    v1::ClientProtocolAdapter* m_protocolAdapter = nullptr;

    std::vector<int>    m_freqs;
    std::vector<double> m_gains;
    std::vector<int>    m_qs;
};

#endif // MAINWINDOW_H
