#include "mainwindow.h"
#include "ui_mainwindow.h.bak"
#include "QFileDialog"
#include "QComboBox"
#include "QxtSpanSlider.h"
#include <QStandardPaths>
#include "roi.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_isFileOpen(false)
    , m_model(this)
{

    ui->setupUi(this);
    ui->comboBox->setCurrentIndex(2);
    ui->openGLwidget->Set_ImgColor(2);
    setFixedSize(this->width(),this->height());//固定窗口大小
//    setWindowFlags(Qt::FramelessWindowHint);//无边框
    setWindowFlags(Qt::WindowCloseButtonHint);//只保留关闭,去除最大化和最小化.
    this->setWindowIcon(QIcon(":/title.ico"));//生成窗口图标
    QActionGroup *AG=new QActionGroup(this);
    AG->addAction(ui->actionRect);
    AG->addAction(ui->actionPoint);
    connect(ui->actionRect, &QAction::triggered, this, [=](){ ui->openGLwidget->Clear_CurrentROI(); ui->openGLwidget->Set_ROI_Type(ROI_RECT); });
    connect(ui->actionPoint, &QAction::triggered, this, [=](){ ui->openGLwidget->Clear_CurrentROI(); ui->openGLwidget->Set_ROI_Type(ROI_POINT); });
    connect(ui->lineEdit_relHum,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_distance,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_emiss,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_reflectedT,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_atmospherT,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_B,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_K,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_TH,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->lineEdit_TL,&QLineEdit::editingFinished,this,&MainWindow::OnLineEditEditingFinished);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxChanged(int)));
    connect(ui->action_open,&QAction::triggered,            this,&MainWindow::OnActionOpenTriggerd);
    connect(ui->toolButton,&QToolButton::released,this,[&](){ui->openGLwidget->m_isoEnable=!ui->toolButton->isChecked();ui->openGLwidget->Update_Texdata();});
    connect(ui->action_save,&QAction::triggered,            this,&MainWindow::OnActionSaveTriggerd);
    connect(ui->verticalSlider,&QxtSpanSlider::spanChanged,this,&MainWindow::Sync_slideAndEdit);
    connect(ui->listView,&QListView::doubleClicked,this,&MainWindow::Select_imgFile);}

MainWindow::~MainWindow()
{
    delete ui;
}
//switch colorbar
void MainWindow::OnComboBoxChanged(int index)
{
    ui->verticalSlider->setStyleSheet(QString("QSlider::groove:vertical{width: 20px;background: url(:/colorBar/index%1.png)}QSlider::handle:vertical{width: 20px;height: 8px;background:rgba(32,178,170,0.8);margin: -5px 0px -5px 0px;border-radius: 2px;}").arg(index));
    ui->openGLwidget->Set_ImgColor(index);
}
//select the .raw files which is to be analysed and attach the file to the list view
void MainWindow::OnActionOpenTriggerd()
{
    QStringList output_name;
    m_pathList.clear();
    m_pathList = QFileDialog::getOpenFileNames(this,
                                              tr("Open File"),
                                              QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                              tr("(*.raw *.jpg)"));
    if(m_pathList.size()==0)
        return;
    for (int i = 0; i < m_pathList.size(); i++){
        //获得文件名
        output_name.append(QFileInfo(m_pathList[i]).fileName());
    }
    m_model.setStringList(output_name);
    ui->listView->setModel(&m_model);
    m_isFileOpen=true;
    //Update the external parameter display box at the top of the window
    initValues();
}
//save the current analysed result
void MainWindow::OnActionSaveTriggerd()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save image"),
            "",
            tr("image Files (*.jpg)"));

    if (!fileName.isNull())
        ui->openGLwidget->Save_Img(fileName);
}
//更改外参
void MainWindow::OnLineEditEditingFinished()
{
    QLineEdit *sender = qobject_cast<QLineEdit *>(QObject::sender());
    guide_measure_external_param_t *ext_param=&ui->openGLwidget->ext_param;
    short value = sender->text().toInt();
    float valuef = sender->text().toFloat();

    if(sender==ui->lineEdit_TH)
    {
        ui->openGLwidget->m_TH=valuef;
        ui->verticalSlider->setUpperValue(valuef*10);
        ui->openGLwidget->Update_Texdata();
        return;
    }
    else if(sender==ui->lineEdit_TL)
    {
        ui->openGLwidget->m_TL=valuef;
        ui->verticalSlider->setLowerValue(valuef*10);
        ui->openGLwidget->Update_Texdata();
        return;
    }
    else if(sender == ui->lineEdit_relHum)
        ext_param->relHum=value;
    else if(sender == ui->lineEdit_distance)
        ext_param->distance=value;
    else if(sender == ui->lineEdit_emiss)
        ext_param->emiss=value;
    else if(sender == ui->lineEdit_reflectedT)
        ext_param->reflectedTemper=value;
    else if(sender == ui->lineEdit_atmospherT)
        ext_param->atmosphericTemper=value;
    else if(sender == ui->lineEdit_B)
        ext_param->modifyB=value;
    else if(sender == ui->lineEdit_K)
        ext_param->modifyK=value;
    else
        goto out;
    ui->openGLwidget->Updata_Tdata();

out:
    return;
}
void MainWindow::Sync_slideAndEdit(int min, int max)
{
    ui->openGLwidget->m_TL=(float)min/10;
    ui->openGLwidget->m_TH=(float)max/10;
    ui->openGLwidget->Update_Texdata();
    ui->lineEdit_TL->setText(QString::number((float)min/10));
    ui->lineEdit_TH->setText(QString::number((float)max/10));
}
//initial external params
void MainWindow::initValues(int fileIndex)
{
    if(m_pathList[fileIndex].isEmpty())
        return;
    //Set the path of the file to be parsed and render it in the openGLwidget
    ui->openGLwidget->Set_ImgPath(m_pathList[fileIndex]);
    guide_measure_external_param_t *ext_param=&ui->openGLwidget->ext_param;

    ui->lineEdit_relHum->setText(QString::number(ext_param->relHum));
    ui->lineEdit_distance->setText(QString::number(ext_param->distance));
    ui->lineEdit_emiss->setText(QString::number(ext_param->emiss));
    ui->lineEdit_reflectedT->setText(QString::number(ext_param->reflectedTemper));
    ui->lineEdit_atmospherT->setText(QString::number(ext_param->reflectedTemper));
    ui->lineEdit_B->setText(QString::number(ext_param->modifyB));
    ui->lineEdit_K->setText(QString::number(ext_param->modifyK));

    ui->verticalSlider->blockSignals(true);
    ui->verticalSlider->setMaximum(ui->openGLwidget->m_TH*10);
    ui->verticalSlider->setMinimum(ui->openGLwidget->m_TL*10);
    ui->verticalSlider->blockSignals(false);
//    ui->lineEdit_TL->setText(QString::number(ui->openGLwidget->m_TL));
//    ui->lineEdit_TH->setText(QString::number(ui->openGLwidget->m_TH));

    ui->verticalSlider->setSpan(ui->openGLwidget->m_TL*10,ui->openGLwidget->m_TH*10);
    ui->verticalSlider->blockSignals(false);
}
void MainWindow::Select_imgFile(QModelIndex index)
{
     initValues(index.row());
}
