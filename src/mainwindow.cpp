#include "websocket.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    auto const address = boost::asio::ip::make_address("127.0.0.1");
    auto const port = static_cast<unsigned short>(std::atoi("68888"));
    auto const threads = 1;

    //the io_context is erquired for all I/O
    boost::asio::io_context ioc{threads};

    //create and launch a listening port
    std::make_shared<Listener>(ioc, tcp::endpoint{address, port})->run();

    //run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    for (int i = threads - 1 ; i > 0 ; --i) {
        v.emplace_back(
                    [&ioc]{ioc.run();});
    }
    ioc.run();
}

MainWindow::~MainWindow() {
    delete ui;
}
