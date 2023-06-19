#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mqtt/qmqtt.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow* ui;
    QMQTT::Client* client = Q_NULLPTR;
    void disConnectToHost();
    void appendLogEdit(QString str);
//    QString hostName;
//    quint16     port;

private slots:
    void on_btn_connect_clicked(void);      //连接事件槽
    void on_btn_clr_send_buf_click(void);
    void on_btn_clr_recv_buf_click(void);
    void on_btn_exp2file_click(void);
    void on_btn_showtime_click(void);
    void on_btn_showtopic_click(void);
    void on_btn_unsub_all_click(void);
    void on_btn_subtopic_click(void);
    void on_btn_unsubtopic_click(void);
    void on_btn_publish_click(void);
    void on_btn_ping_click(void);

    void onConnected();
    void onDisconnected(void);
    void onReceived(const QMQTT::Message& message);
    void onError(const QMQTT::ClientError error);
    void onSubscribed(const QString& topic, const quint8 qos = 0);
    void onUnsubscribed(const QString& topic);
    void onPublished(const QMQTT::Message& message, quint16 msgid = 0);
    void onPingresp();
};
#endif // MAINWINDOW_H
