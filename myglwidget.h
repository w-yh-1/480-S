#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMenu>
#include <QList>
#include"include/guidemt.h"
#include "roi.h"
#define MaxValueLimit (1<<15)


class myGLwidget : public QOpenGLWidget , protected QOpenGLFunctions
{
Q_OBJECT

public:
myGLwidget(QWidget *parent = nullptr);
~myGLwidget();
void Save_Img(QString &fileName);
void Set_ImgPath(QString &path);
void Set_ImgColor(int index);
void Set_ROI_Type(ROI_type type);
void Clear_CurrentROI();
void Updata_Tdata();
void Update_Texdata();
short *m_paramsData;
float *m_paramsDataFloat;
float   m_TH;
float   m_TL;
bool    m_isoEnable;
float   m_Tdata[640*512];
guide_measure_external_param_t ext_param{};

protected:
void initializeGL() override;
void paintGL() override;
void resizeGL(int w, int h) override;
void mousePressEvent(QMouseEvent *ev)override ;
void mouseMoveEvent(QMouseEvent *ev)override ;
void mouseReleaseEvent(QMouseEvent *ev)override ;
void mouseDoubleClickEvent(QMouseEvent* event) override;
void keyPressEvent(QKeyEvent *ev)override ;
void contextMenuEvent(QContextMenuEvent *ev)override ;

private:
void Enhance_contrast(short *pic);
void Fresh_GammaTable(double k);

void GammaAdjust(short *data);
void SwitchAuto(){m_isoEnable=!m_isoEnable;Update_Texdata();};
QOpenGLShaderProgram program;
QOpenGLVertexArrayObject vao;
QOpenGLBuffer vbo;//QOpenGLBuffer构造函数默认创建VBO
QOpenGLBuffer pbo;//缓存转换后rgb图片
QImage  m_backgroundImg;
QOpenGLTexture m_texture0;
QString m_imgFilePath;

QMenu	*m_pOptMenu;
QAction *m_pDelAction;
QAction *m_pSaveAction;
ROI     *m_CurrentROI;					//绘制的ROI

QByteArray *m_pImgAllData;
QByteArray *m_pImgRawAndParamsData;
int     m_colorIndex;
short   *m_rawImgData;
short *m_amplifiedRawData;
//short   m_adjustedImgData[640*512];
short   m_GammaTable[MaxValueLimit];
float   m_GammaFactor;


QList<ROI> m_roiList;
int m_indexInList=-1;
ROI_type m_currentROIType = ROI_RECT;
};
#endif // MYGLWIDGET_H
