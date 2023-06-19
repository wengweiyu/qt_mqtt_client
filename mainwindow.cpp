#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mywidgetitem.h"
#include <QMessageBox>
#include <QSslSocket>
#include <QHostAddress>
#include <QtDebug>
#include <QFileDialog>
#include <QMetaEnum>
#include <QProcess>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //1、SSL方式登录
    /*QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    // Add custom SSL options here (for example extra certificates)
    QMQTT::Client* client = new QMQTT::Client("mf1de116.ala.cn-hangzhou.emqxsl.cn", 8883, sslConfig);
    client->setClientId("clientId");
    client->setUsername("845325015@qq.com");
    client->setPassword("wwy920117");
    // Optionally, set ssl errors you want to ignore. Be careful, because this may weaken security.
    // See QSslSocket::ignoreSslErrors(const QList<QSslError> &) for more information.
    //client->ignoreSslErrors(<list of expected ssl errors>)
    client->connectToHost();
    // Here's another option to suppress SSL errors (again, be careful)
    QObject::connect(client, &QMQTT::Client::sslErrors, [&](const QList<QSslError>& errors)
    {
        // Investigate the errors here, if you find no serious problems, call ignoreSslErrors()
        // to continue connecting.
        client->ignoreSslErrors();
    });
    qDebug() << QSslSocket::sslLibraryBuildVersionString();
    connect(client, &QMQTT::Client::connected, [&]()
    {
        qDebug() << "MQTT connected";
    });*/

    //2、直接Tcp连接
    /*QMQTT::Client* client = new QMQTT::Client();
    QHostAddress host("10.5.8.14");
    quint16 port = 1883;
    client->setHost(host);
    client->setPort(port);
    client->setClientId("windows");
    client->setUsername("user");
    client->setPassword("password");
    client->connectToHost();
    */
    client = new QMQTT::Client();
    connect(this->ui->pb_connect, &QPushButton::clicked, this, &MainWindow::on_btn_connect_clicked);
    connect(this->ui->pb_clear_sendbuf, &QPushButton::clicked, this, &MainWindow::on_btn_clr_send_buf_click);
    connect(this->ui->pb_clear_recvbuf, &QPushButton::clicked, this, &MainWindow::on_btn_clr_recv_buf_click);
    connect(this->ui->pb_exp_to_file, &QPushButton::clicked, this, &MainWindow::on_btn_exp2file_click);
    connect(this->ui->pb_show_time, &QPushButton::clicked, this, &MainWindow::on_btn_showtime_click);
    connect(this->ui->pb_show_topic, &QPushButton::clicked, this, &MainWindow::on_btn_showtopic_click);
    connect(this->ui->pb_unsub_all, &QPushButton::clicked, this, &MainWindow::on_btn_unsub_all_click);
    connect(this->ui->pb_subcribe, &QPushButton::clicked, this, &MainWindow::on_btn_subtopic_click);
    connect(this->ui->pb_unsubscribe, &QPushButton::clicked, this, &MainWindow::on_btn_unsubtopic_click);
    connect(this->ui->pb_publish, &QPushButton::clicked, this, &MainWindow::on_btn_publish_click);
    connect(this->ui->pb_ping, &QPushButton::clicked, this, &MainWindow::on_btn_ping_click);

    connect(client, &QMQTT::Client::connected, this, &MainWindow::onConnected);
    connect(client, &QMQTT::Client::disconnected, this, &MainWindow::onDisconnected);
    connect(client, &QMQTT::Client::received, this, &MainWindow::onReceived);
    connect(client, &QMQTT::Client::error, this, &MainWindow::onError);
    connect(client, &QMQTT::Client::subscribed, this, &MainWindow::onSubscribed);
    connect(client, &QMQTT::Client::unsubscribed, this, &MainWindow::onUnsubscribed);
    connect(client, &QMQTT::Client::published, this, &MainWindow::onPublished);
    connect(client, &QMQTT::Client::pingresp, this, &MainWindow::onPingresp);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::disConnectToHost()
{
    this->ui->pb_connect->setText(tr("连接"));
    this->ui->le_server_addr->setEnabled(true);
    this->ui->le_server_ip->setEnabled(true);
    this->ui->le_server_port->setEnabled(true);
    this->ui->le_username->setEnabled(true);
    this->ui->le_passwd->setEnabled(true);
    client->disconnectFromHost();
}

void MainWindow::appendLogEdit(QString str)
{
    if(ui->pb_show_time->isChecked())
    {
        QString curTime = QDateTime::currentDateTime().toString("yyyy.mm.dd hh:mm:ss.zzz");
        ui->pet_recv->appendPlainText(curTime + ":" + str);
    }
    else
    {
        ui->pet_recv->appendPlainText(str);
    }
}

void MainWindow::on_btn_connect_clicked()
{
    client->setUsername(ui->le_username->text());
    client->setPassword(ui->le_passwd->text().toLatin1());
    if(this->ui->le_server_port->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("端口号不能为空！"));
        return;
    }
    client->setPort(ui->le_server_port->text().toInt());
    if(this->ui->le_server_ip->text().isEmpty() && ui->le_server_addr->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("服务器地址、服务器IP不能都为空！"));
        return;
    }
    if(!ui->le_server_addr->text().isEmpty())
    {
        client->setHostName(ui->le_server_addr->text());
    }
    if(!ui->le_server_ip->text().isEmpty())
    {
        client->setHost(QHostAddress(ui->le_server_ip->text()));
    }
//    client->setAutoReconnectInterval(5000);
//    client->setAutoReconnect(true);
    qDebug() << "on_btn_connect_clicked";
    if(client->connectionState() != QMQTT::STATE_CONNECTED)
    {
        this->ui->pb_connect->setText(tr("断开连接"));
        this->ui->le_server_addr->setEnabled(false);
        this->ui->le_server_ip->setEnabled(false);
        this->ui->le_server_port->setEnabled(false);
        this->ui->le_username->setEnabled(false);
        this->ui->le_passwd->setEnabled(false);
        client->connectToHost();
    }
    else
    {
        disConnectToHost();
    }
}

void MainWindow::on_btn_clr_send_buf_click()
{
    qDebug() << "on_btn_clr_send_buf_click";
    ui->pet_publish_payload->clear();
}

void MainWindow::on_btn_clr_recv_buf_click()
{
    qDebug() << "on_btn_clr_recv_buf_click";
    this->ui->pet_recv->clear();
}

void MainWindow::on_btn_exp2file_click()
{
    qDebug() << "on_btn_exp2file_click";
    QString path = QFileDialog::getSaveFileName(this, tr("保存路径"), ".", tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, tr("Error"), tr("Save Failed!"));
            return;
        }
        QTextStream out(&file);
        out << ui->pet_recv->toPlainText();
        file.close();
    }
    else
    {
        QMessageBox::information(this, tr("提示"), tr("未选择路径！"));
    }
}

void MainWindow::on_btn_showtime_click()
{
    qDebug() << "on_btn_showtime_click";
}

void MainWindow::on_btn_showtopic_click()
{
    qDebug() << "on_btn_showtopic_click";
}

void MainWindow::on_btn_unsub_all_click()
{
    qDebug() << "on_btn_unsub_all_click";
    while(ui->lw_topics->count())
    {
        QListWidgetItem* item_sel = ui->lw_topics->item(0);
        MyWidgetItem* cur_item = dynamic_cast<MyWidgetItem*>(ui->lw_topics->itemWidget(item_sel));
        client->unsubscribe(cur_item->text->text());
        ui->lw_topics->takeItem(0);
    }
    QMessageBox::information(this, tr("提示"), tr("取消订阅成功"));
}

void MainWindow::on_btn_subtopic_click()
{
    qDebug() << "on_btn_subtopic_click";
    QString topic = ui->le_sub_topic->text();
    for(int i = 0; i < ui->lw_topics->count(); i++)
    {
        QListWidgetItem* item_sel = ui->lw_topics->item(i);
        MyWidgetItem* cur_item = dynamic_cast<MyWidgetItem*>(ui->lw_topics->itemWidget(item_sel));
        if(cur_item->text->text() == topic )
        {
            QMessageBox::critical(this, tr("Error"), tr("主题：%1已经订阅").arg(topic));
            return;
        }
    }
    client->subscribe(topic);

    MyWidgetItem* myitem = new MyWidgetItem;
    myitem->text->setText(ui->le_sub_topic->text());


    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(QSize(0, 30));
    ui->lw_topics->addItem(item);
    ui->lw_topics->setItemWidget(item, myitem);

    connect(myitem->btn_del, &QPushButton::clicked, [this]()
    {

        int item_n = ui->lw_topics->currentRow();
        if(item_n < 0) return;
        QListWidgetItem* item_sel = ui->lw_topics->item(item_n);
        MyWidgetItem* cur_item = dynamic_cast<MyWidgetItem*>(ui->lw_topics->itemWidget(item_sel));
        client->unsubscribe(cur_item->text->text());
        ui->lw_topics->takeItem(item_n);
    });
}

void MainWindow::on_btn_unsubtopic_click()
{
    qDebug() << "on_btn_unsubtopic_click";
    QString topic = ui->le_sub_topic->text();
    if(!topic.isEmpty())
    {
        client->unsubscribe(topic);
        for(int i = 0; i < ui->lw_topics->count(); i++)
        {
            QListWidgetItem* item_sel = ui->lw_topics->item(i);
            MyWidgetItem* cur_item = dynamic_cast<MyWidgetItem*>(ui->lw_topics->itemWidget(item_sel));
            if(cur_item->text->text() == topic )
            {
                ui->lw_topics->takeItem(i);
            }
        }
    }
}

void MainWindow::on_btn_publish_click()
{
    qDebug() << "on_btn_publish_click";

    QString topic = this->ui->le_topic->text();
    QString payload = this->ui->pet_publish_payload->toPlainText();
    int Qos = this->ui->sb_Qos->value();

    QMQTT::Message message;
    message.setQos(Qos);
    message.setTopic(topic);
    message.setPayload(payload.toUtf8());
    client->publish(message);
    //this->ui->pet_recv->appendPlainText(payload);
}

void MainWindow::on_btn_ping_click()
{
    qDebug() << "on_btn_ping_click";
    /*QMetaEnum meta = QMetaEnum::fromType<QMQTT::ClientError>();
    qDebug() << meta.name(); //枚举的类型名称
    qDebug() << meta.keyCount(); //键个数
    qDebug() << meta.isValid() << meta.scope(); //是否有效 声明自哪个类
    qDebug() << meta.keyToValue("SocketRemoteHostClosedError"); //字符串转枚举
    qDebug() << QString(meta.valueToKey(QMQTT::UnknownError)); //枚举转字符串
    qDebug() << QString(meta.valueToKey(QMQTT::SocketConnectionRefusedError));
    for(int i = 0; i < meta.keyCount(); i++) //遍历枚举
    {
        qDebug() << QString(meta.key(i)) << meta.value(i);
    }*/
    if(!(ui->le_server_addr->text().isEmpty() && ui->le_server_ip->text().isEmpty()))
    {
        this->setEnabled(false);
        QString remote;
        if(!ui->le_server_addr->text().isEmpty())
        {
            remote = ui->le_server_addr->text();
        }
        if(!ui->le_server_ip->text().isEmpty())
        {
            remote = ui->le_server_ip->text();
        }
        QProcess cmd(this);

        cmd.start("ping " + remote);
        while (cmd.waitForFinished(20) == false)
        {
            QByteArray out = cmd.readAllStandardOutput();
            if (!out.isEmpty())
            {
                ui->pet_recv->appendPlainText(QString::fromLocal8Bit(out));
                repaint();
            }
        }
        QByteArray out = cmd.readAllStandardOutput();
        ui->pet_recv->appendPlainText(QString::fromLocal8Bit(out));

        this->setEnabled(true);
    }
    else
    {
        appendLogEdit("请检查参数后重新操作...");
    }
}

void MainWindow::onConnected()
{
    qDebug() << "服务器已连接";
    appendLogEdit("服务器已连接");
}

void MainWindow::onDisconnected()
{
    qDebug() << "服务器断开";
    appendLogEdit("服务器断开");
    disConnectToHost();
}

void MainWindow::onReceived(const QMQTT::Message& message)
{
    QString content;
    if(ui->pb_show_topic->isChecked())
    {
        content += QLatin1String(" Received Topic: ") + message.topic();
    }

    content += QLatin1String(" Message: ") + message.payload() + QLatin1Char('\n');
    appendLogEdit(content);
}

void MainWindow::onError(const QMQTT::ClientError error)
{
    qDebug() << "发生错误，错误类型:" << error;
    QMetaEnum meta = QMetaEnum::fromType<QMQTT::ClientError>();
    appendLogEdit("连接异常，请检查参数后重新操作,错误原因:" + QString(meta.valueToKey(error)));
    disConnectToHost();
}

void MainWindow::onSubscribed(const QString& topic, const quint8 qos)
{
    QString sqos;
    sqos.sprintf("%d", qos);

    appendLogEdit("主题订阅成功 [ " + topic + "_" + sqos + " ] ...");
}

void MainWindow::onUnsubscribed(const QString& topic)
{
    appendLogEdit("主题取消 [ " + topic + " ] ...");
}

void MainWindow::onPublished(const QMQTT::Message& message, quint16 msgid)
{
    qDebug() << "发送成功" << "msgid=" << msgid << "topic:" << message.topic() << "payload:" << message.payload();
}

void MainWindow::onPingresp()
{
    qDebug() << "onPingresp";
}



