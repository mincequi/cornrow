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
    void on_freqSpinBox_valueChanged(int d);
    void on_gainSpinBox_valueChanged(double d);
    void on_qSpinBox_valueChanged(int d);

    void onServiceDiscovered(QString hostname, quint32 address, quint16 port);

private:
    void enableFilterWidgets(bool enable);

    Ui::MainWindow *ui;
    ZeroconfWrapper m_zeroconf;
    rpc::client*    m_rpcClient;
    v1::ClientProtocolAdapter* m_protocolAdapter;
};

#endif // MAINWINDOW_H
