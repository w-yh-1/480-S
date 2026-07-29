#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qtkeyboard.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // // 创建一个输入框
    // QLineEdit *inputField = new QLineEdit(this);

    // 创建虚拟键盘
    QKeyBoard *keyboard = QKeyBoard::getInstance();

    // // 设置输入框
    // inputField->setPlaceholderText("请输入文本");

    // // 设置布局
    // QVBoxLayout *layout = new QVBoxLayout;
    // layout->addWidget(inputField);
    // layout->addWidget(keyboard);  // 将虚拟键盘添加到布局中

    // QWidget *centralWidget = new QWidget(this);
    // centralWidget->setLayout(layout);
    // setCentralWidget(centralWidget);


    // // 使用 QWidget::focusIn 信号
    // connect(inputField, &QWidget::focusIn, this, [&]() {
    //     keyboard->showPanel(); // 焦点获取时的处理代码
    // });
    // connect(inputField, &QLineEdit::focusOut, this, [&]() {
    //     keyboard->hidePanel();  // 输入框失去焦点时隐藏虚拟键盘
    // });




}

MainWindow::~MainWindow()
{
    delete ui;
}
