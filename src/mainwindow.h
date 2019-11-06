#pragma once

#include <QMainWindow>

#include "websocket.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

    boost::asio::ip::address const address = boost::asio::ip::make_address("127.0.0.1");
    unsigned short const port = static_cast<unsigned short>(std::atoi("68888"));
    int const threads = 1;
    std::thread thread_;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void serverStart();

private:
    Ui::MainWindow *ui;
};

