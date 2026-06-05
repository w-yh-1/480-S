#include <myglwidget.h>
#include <QFile>
#include <QPainter>
#include <QMouseEvent>
#include <QRegion>
#include <cmath>
#define CORE_VERSION  "#version 330 core\n"
#define GET_GL_SOURCE_STR(x) CORE_VERSION#x

#define W 640
#define H 512
myGLwidget::myGLwidget(QWidget *parent):
    QOpenGLWidget(parent),
    m_isoEnable(false),
    pbo(QOpenGLBuffer::PixelUnpackBuffer),
    m_backgroundImg(640,512,QImage::Format_RGB888),
    m_texture0(QOpenGLTexture::Target2D),
    m_imgFilePath(),
    m_CurrentROI(NULL),
    m_pImgAllData(nullptr),
    m_paramsData(nullptr),
    m_amplifiedRawData(nullptr),
    m_colorIndex(0),
    m_roiList()

{
    Fresh_GammaTable(0.45);
    this->setMouseTracking(true);
    this->setFocusPolicy(Qt::StrongFocus);
    m_pOptMenu = new QMenu(this);
    m_pDelAction = new QAction(QStringLiteral("删除"), this);
    connect(m_pDelAction, &QAction::triggered, this, [&]() {
        if(m_indexInList>=0){
            m_roiList.removeAt(m_indexInList);
            m_indexInList=-1;
        }
        else
            delete m_CurrentROI;
       m_CurrentROI=NULL; });

    m_pSaveAction = new QAction(QStringLiteral("保存"), this);
    connect(m_pSaveAction, &QAction::triggered, this, [&]() {
        if(m_indexInList<0)
            m_roiList.append(*m_CurrentROI);
        else
            m_indexInList=-1;
        m_CurrentROI=NULL;
    });
    m_pOptMenu->addAction(m_pSaveAction);
    m_pOptMenu->addAction(m_pDelAction);
}
myGLwidget::~myGLwidget()
{
    //多线程调用保护
    makeCurrent();
//    //对象释放
    vao.destroy();
    vbo.destroy();
    
    if(m_paramsData != nullptr) {
        delete[] m_paramsData;
        m_paramsData = nullptr;
    }
    
    if(m_amplifiedRawData != nullptr) {
        delete[] m_amplifiedRawData;
        m_amplifiedRawData = nullptr;
    }
//    //退出保护
//    doneCurrent();
}
void myGLwidget::initializeGL()
{
    float vertices[] = {
        //vertex.x,vertex.y,    texcoord.x,texcoord.y
        -1.f,  1.f,     0.f,  0.f,//左上
         1.f,  1.f,     1.f,  0.f,//右上
         1.f, -1.f,     1.f,  1.f,//右下
        -1.f, -1.f,     0.f,  1.f,//左下
    };
    initializeOpenGLFunctions();
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    //着色器程序链接
    program.addShaderFromSourceFile(QOpenGLShader::Vertex,":/Shaders/vertex_shader.vert");
    program.addShaderFromSourceFile(QOpenGLShader::Fragment,":/Shaders/fragment_shader.frag");
    program.link();
    //Texture0 init
    m_texture0.create();
    m_texture0.setMinMagFilters(QOpenGLTexture::Linear,QOpenGLTexture::Linear);
    m_texture0.setWrapMode(QOpenGLTexture::ClampToBorder);
    m_texture0.setBorderColor(QColor(1.0f,1.0f,1.0f,1.0f));
    m_texture0.setFormat(QOpenGLTexture::RGB8_UNorm);
    m_texture0.setSize(W,H);
    m_texture0.allocateStorage(QOpenGLTexture::RGB,QOpenGLTexture::UInt8);
    //PBO
    pbo.create();
    pbo.bind();
    pbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    pbo.allocate(W*H*sizeof (uchar)*3);

    //VAO
    vao.create();
    vao.bind();

    //VBO
    vbo.create();
    vbo.bind();              //绑定到当前的OpenGL上下文
    vbo.setUsagePattern(QOpenGLBuffer::StreamDraw);  //设置为一次修改，多次使用
    vbo.allocate(vertices, 16*sizeof(GLfloat));

    //设置顶点属性
    program.enableAttributeArray(0);
    program.setAttributeBuffer(0,GL_FLOAT,0,2,4*sizeof (float));
    program.enableAttributeArray(1);
    program.setAttributeBuffer(1,GL_FLOAT,2*sizeof (float),2,4*sizeof (float));

    //解除绑定
    vao.release();
    vbo.release();
    pbo.release();
    //program.release();
}

void myGLwidget::paintGL()
{
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT);
    program.bind();
    m_texture0.bind();
    vao.bind();
    glDrawArrays(GL_QUADS,0,4);
    m_texture0.release();
    vao.release();
    program.release();
    painter.endNativePainting();

//    foreach(const ROI &i,m_roiList)
//    {
//        i.Draw(painter);
//    }
    for(int i=m_roiList.size()-1;i>=0;--i)
    {
        m_roiList[i].Draw(painter);
        int idX = m_roiList[i].m_roiRect.x() - 25;
        int idY = m_roiList[i].m_roiRect.y() - 10;
        if(idX < 0) idX = 0;
        if(idY < 0) idY = 0;
        painter.drawText(idX, idY, "ID:"+QString::number(i+1));
    }

    if(m_CurrentROI!=NULL)
    {
        m_CurrentROI->Draw(painter,true);
    }
    //qDebug()<<rect();
}
void myGLwidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void myGLwidget::Set_ImgPath(QString &path)
{
    Save_CurrentROIList();
    
    int jpgTailPos;
    QFile rawImgFile(path);
    m_imgFilePath=path;
    rawImgFile.open(QIODevice::ReadOnly);
    if(m_pImgAllData!=nullptr&&!m_pImgAllData->isNull())
    {
        delete m_pImgAllData;
        delete m_pImgRawAndParamsData;
    }
    m_pImgAllData =new QByteArray(rawImgFile.readAll());
    jpgTailPos=m_pImgAllData->indexOf("\xFF\xD9");
    if(jpgTailPos==-1)
        jpgTailPos=0;
    else
        jpgTailPos+=2;
    m_pImgRawAndParamsData = new QByteArray(m_pImgAllData->mid(jpgTailPos));
    m_rawImgData = reinterpret_cast<short *>(m_pImgAllData->data()+jpgTailPos);
    m_paramsDataFloat = reinterpret_cast<float *>(m_pImgAllData->data()+jpgTailPos+640*512*sizeof(short));
    
    if(m_paramsData == nullptr) {
        m_paramsData = new short[640];
        memset(m_paramsData, 0, 640 * sizeof(short));
    }

    // 1. 读取温度数据
    for(int i = 0; i < W * H; i++) {
        m_Tdata[i] = m_rawImgData[i] / 100.0f;  // 除以100得到实际温度
    }
    auto result1 = std::minmax_element(m_Tdata, m_Tdata + W * H);
    m_TL = *result1.first;
    m_TH = *result1.second;
    if(m_amplifiedRawData != nullptr) {
        delete[] m_amplifiedRawData;
    }
    m_amplifiedRawData = new short[W * H];
    float tempRange = m_TH - m_TL;
    if(tempRange < 0.1f) tempRange = 1.0f;
    for(int i = 0; i < W * H; i++) {
        float normalized = (m_Tdata[i] - m_TL) / tempRange;
        m_amplifiedRawData[i] = (short)(normalized * 65535);
    }

    m_rawImgData = m_amplifiedRawData;


//    rawImgFile.read((char *)m_rawImgData,640*512*sizeof (short));
//    rawImgFile.read((char *)m_paramsData,640*sizeof (short));
    ext_param.relHum = m_paramsDataFloat[0];
    ext_param.atmosphericTemper = m_paramsDataFloat[1];
    ext_param.emiss = m_paramsDataFloat[2];
    ext_param.distance = m_paramsDataFloat[3];
    ext_param.reflectedTemper = m_paramsDataFloat[4];
    ext_param.modifyB=0;
    ext_param.modifyK=100;

    //m_TH=(float)m_paramsData[46]/10;
    //m_TL=(float)m_paramsData[49]/10;
    //Updata_Tdata();
    Update_Texdata();
    // auto result=std::minmax_element(m_Tdata, m_Tdata + sizeof(m_Tdata)/sizeof(m_Tdata[0]));
    // m_TL=*result.first;
    // m_TH=*result.second;
    
    Load_CurrentROIList();
    
    if(m_roiList.size()>-1)
    {
        for(int i=m_roiList.size()-1;i>=0;--i)
        {
            m_roiList[i].Analyst_Rect();
        }
    }
}
void myGLwidget::Set_ImgColor(int index)
{
    m_colorIndex=index;
    if(!m_imgFilePath.isEmpty())
        Update_Texdata();
}

void myGLwidget::Set_ROI_Type(ROI_type type)
{
    m_currentROIType = type;
}

void myGLwidget::Clear_CurrentROI()
{
    if(m_CurrentROI != NULL)
    {
        delete m_CurrentROI;
        m_CurrentROI = NULL;
        m_indexInList = -1;
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void myGLwidget::Save_CurrentROIList()
{
    if(!m_imgFilePath.isEmpty())
    {
        m_allRoiLists[m_imgFilePath] = m_roiList;
    }
}

void myGLwidget::Load_CurrentROIList()
{
    if(m_allRoiLists.contains(m_imgFilePath))
    {
        m_roiList = m_allRoiLists[m_imgFilePath];
    }
    else
    {
        m_roiList.clear();
    }
    m_indexInList = -1;
    m_CurrentROI = NULL;
    update();
}

void myGLwidget::Update_Texdata()
{
    uchar *ptr=NULL;
    m_texture0.bind();
    pbo.bind();
    ptr=(uchar *)pbo.map(QOpenGLBuffer::ReadWrite);
    QByteArray yuvOut;
    yuvOut.resize(m_pImgAllData->size());

    short* yuvdst = new short[W * H * 3];
    if(m_isoEnable)
        guide_isotherm(2,1,m_TL,m_TH,m_rawImgData,m_rawImgData,yuvdst,(uchar *)m_paramsData,W,H,&ext_param,ISOTHERM_MODE_RANGE_MIDDLE,m_colorIndex);
    else
        guide_temp_to_rgb24(m_Tdata,ptr,W,H,m_TL,m_TH,m_colorIndex);
    //GammaAdjust(ptr);
    //Enhance_contrast(ptr);
    pbo.unmap();
    glTexSubImage2D(GL_TEXTURE_2D,
    0,
    0,0,
    W,H,GL_RGB,GL_UNSIGNED_BYTE,0);
    delete[] yuvdst;
    pbo.release();
    m_texture0.release();
    this->update();
}

void myGLwidget::Updata_Tdata()
{
    guide_measure_convertgray2temper(2,1,m_rawImgData,(uchar *)m_paramsData,W*H,&ext_param,m_Tdata);
}

void myGLwidget::GammaAdjust(short *data)
{
    for(int i=0;i<W*H;i++)
    {
        data[i]=m_GammaTable[data[i]];
    }
}

void myGLwidget::Fresh_GammaTable(double k)
{
    m_GammaFactor = k;
    double f=0.;
    for (int value=0;value<(MaxValueLimit);value++) {
        f = (double)value/MaxValueLimit;
        f = pow(f,m_GammaFactor);
        m_GammaTable[value]=f*MaxValueLimit;
    }

}
void myGLwidget::Enhance_contrast(short *pic)
{
    int gray[MaxValueLimit] = { 0 };  //记录每个灰度级别下的像素个数
    double gray_prob[MaxValueLimit] = { 0 };  //记录灰度分布密度
    double gray_distribution[MaxValueLimit] = { 0 };  //记录累计密度
    int gray_equal[MaxValueLimit] = { 0 };  //均衡化后的灰度值

    int gray_sum = W * H;  //像素总数

    //统计每个灰度下的像素个数
    for (int i = 0; i < gray_sum; i++)
    {
        short vaule = pic[i];
        gray[vaule]++;
    }


    //统计灰度频率
    for (int i = 0; i < MaxValueLimit; i++)
    {
        gray_prob[i] = ((double)gray[i] / gray_sum);
    }

    //计算累计密度
    gray_distribution[0] = gray_prob[0];
    for (int i = 1; i < MaxValueLimit; i++)
    {
        gray_distribution[i] = gray_distribution[i-1] +gray_prob[i];
    }

    //重新计算均衡化后的灰度值，四舍五入。参考公式：(N-1)*T+0.5
    for (int i = 0; i < MaxValueLimit; i++)
    {
        gray_equal[i] = (short)(MaxValueLimit * gray_distribution[i] + 0.5);
    }


    //直方图均衡化,更新原图每个点的像素值

    for (int j = 0; j < gray_sum; j++)
    {
        pic[j] = gray_equal[pic[j]];
    }
}

/**
 * @brief				鼠标按下事件 用于拖动绘制缩放起始
 * @param ev	        鼠标事件
 *
 * @return				void
 */
void myGLwidget::mousePressEvent(QMouseEvent * ev)
{
    if(m_imgFilePath==NULL)
        return;
    if (ev->buttons() & Qt::LeftButton)
    {
        QPoint pos = ev->pos();
        
        // 如果已选中某个ROI，检查点击是否在ROI外
        if(m_indexInList >= 0 && m_CurrentROI != NULL)
        {
            if(!m_CurrentROI->contains(pos))
            {
                // 点击在ROI外，取消选中
                m_indexInList = -1;
                m_CurrentROI = NULL;
                setCursor(Qt::ArrowCursor);
                update();
                return;
            }
        }
        
        // 检查是否点击在已存在的ROI范围内
        for(int i = m_roiList.size() - 1; i >= 0; --i)
        {
            if(m_roiList.at(i).contains(pos))
            {
                // 点击在已存在的ROI范围内，选中它而不是创建新的
                m_CurrentROI = &m_roiList[i];
                m_indexInList = i;
                m_CurrentROI->ResponseMousePressEV(pos);
                update();
                return;
            }
        }
        
        if(m_CurrentROI==NULL)
            m_CurrentROI = new ROI(this, m_currentROIType);
        m_CurrentROI->ResponseMousePressEV(pos);
    }
}
/**
 * @brief				鼠标移动事件
 * @param ev         	鼠标事件
 *
 * @return				void
 */
void myGLwidget::mouseMoveEvent(QMouseEvent * ev)
{
    if(m_CurrentROI==NULL)
        return;
    if (ev->buttons() & Qt::LeftButton)
    {
        m_CurrentROI->ResponseMousePressAndMoveEV(ev->pos());
        return;
    }
    m_CurrentROI->ResponseMouseMoveEV(ev->pos());
}
/**
 * @brief				鼠标松开事件
 * @param ev	        鼠标事件
 *
 * @return				void
 */
void myGLwidget::mouseReleaseEvent(QMouseEvent * ev)
{
    if(m_CurrentROI==NULL)
        return;

    //qDebug()<<ev->buttons()<<endl;
    m_CurrentROI->ResponseMouseReleaseEV(ev->pos());
    
    // 自动保存ROI
    if(m_indexInList < 0)
    {
        bool shouldSave = false;
        if(m_CurrentROI->m_type == ROI_RECT)
        {
            // 矩形需要有有效的尺寸
            shouldSave = (m_CurrentROI->m_roiRect.width() > 5 && m_CurrentROI->m_roiRect.height() > 5);
        }
        else if(m_CurrentROI->m_type == ROI_POINT)
        {
            // 点只要有位置就保存
            shouldSave = true;
        }
        
        if(shouldSave)
        {
            m_roiList.append(*m_CurrentROI);
            delete m_CurrentROI;
            m_CurrentROI = NULL;
            setCursor(Qt::ArrowCursor);
            update();
        }
    }
}
/**
 * @brief				鼠标双击事件
 * @param event	        鼠标事件
 *
 * @return				void
 */
void myGLwidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    static bool dir=0;
    QPoint pos=event->pos();
    if (event->buttons() & Qt::RightButton)
    {
        event->ignore();
        return;
    }

    if(m_indexInList<0&&m_CurrentROI!=NULL)
    {
        if(!m_CurrentROI->contains(pos))
        {
            delete m_CurrentROI;
            m_CurrentROI =NULL;
            setCursor(Qt::ArrowCursor);
            update();
        }
    }

    if(dir)
    {
        for(int i=0;i<m_roiList.size();++i)
        {
            if(m_roiList.at(i).contains(pos)){
                m_CurrentROI= &m_roiList[i];
                m_indexInList=i;
                update();
                break;
            }
        }
    }
    else
    {
        for(int i=m_roiList.size()-1;i>=0;--i)
        {
            if(m_roiList.at(i).contains(pos)){
                m_CurrentROI= &m_roiList[i];
                m_indexInList=i;
                update();
                break;
            }
        }
    }
    dir=!dir;
}

/**
 * @brief				键盘按下事件
 * @param ev	        键盘事件
 *
 * @return				voi
 *
 * d
 */
void myGLwidget::keyPressEvent(QKeyEvent * ev)
{

    if (ev->key() == Qt::Key_Delete&&m_CurrentROI!=NULL)
    {
        emit m_pDelAction->triggered();
        setCursor(Qt::ArrowCursor);
        update();
        return;
    }
    if (ev->key() == Qt::Key_Return&&m_CurrentROI!=NULL)
    {
        emit m_pSaveAction->triggered();
        setCursor(Qt::ArrowCursor);
        update();
    }
}

/**
 * @brief				右键菜单
 * @param ev	        菜单事件
 *
 * @return				void
 */
void myGLwidget::contextMenuEvent(QContextMenuEvent * ev)
{
    QPoint mousePos = ev->pos();

    if (m_CurrentROI!=NULL && m_CurrentROI->contains(mousePos))
    {
        m_pOptMenu->exec(QCursor::pos());
    }

    ev->accept();
}



//void myGLwidget::InitROIsettins()
//{
//    m_bPainterPressed = false;
//    m_bMovedPressed = false;
//    m_bScalePressed = false;
//    m_roiRect = QRect(0, 0, 0, 0);
//    m_emCurDir = EmDirection::DIR_NONE;
//    this->setMouseTracking(true);
//    this->setFocusPolicy(Qt::StrongFocus);
//    m_pOptMenu = new QMenu(this);
//    m_pDelAction = new QAction(QStringLiteral("删除"), this);
//    connect(m_pDelAction, &QAction::triggered, this, [&]() { m_roiRect = QRect(0, 0, 0, 0); });
//    m_pSaveAction = new QAction(QStringLiteral("保存"), this);
//    connect(m_pSaveAction, &QAction::triggered, this, &MyLabel::saveROIImage);
//    m_pOptMenu->addAction(m_pDelAction);
//    m_pOptMenu->addAction(m_pSaveAction);
//}
void myGLwidget::Save_Img(QString &fileName)
{
    QFile fileToSave(fileName);
    fileToSave.open(QIODevice::WriteOnly);
    QPixmap pixmap = grab();
    QPainter painter(&pixmap);
    for(int i=m_roiList.size()-1;i>=0;--i)
    {
        m_roiList[i].Draw(painter);
        painter.drawText(m_roiList[i].m_roiRect.x(),m_roiList[i].m_roiRect.y()-3,"ID:"+QString::number(i));
    }
    pixmap.save(&fileToSave,"JPG");
    fileToSave.write(*m_pImgRawAndParamsData);
}
