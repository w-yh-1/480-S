#ifndef ROI_H
#define ROI_H

#include"QRect"
#include"QPoint"
#include"QLine"
#include"QPainter"
#include "QPolygon"
#define CORPADDING       12       //四角可拉伸宽度
#define POINT_PADDING    15       //点的选中范围
#define MIN_WIDTH        6        //可拉伸的最小宽度
#define MIN_HEIGHT       6        //可拉伸的最小高度
#define EDGE_WIDTH       2        //边框的宽度
enum ROI_type{
    ROI_POINT=0,
    ROI_LINE,
    ROI_RECT
};

enum EmDirection
{
    DIR_LEFTTOP=0b0101,
    DIR_TOP ,
    DIR_RIGHTTOP,
    DIR_LEFT=0b1001,
    DIR_MIDDLE,
    DIR_RIGHT,
    DIR_LEFTBOTTOM=0b1101,
    DIR_BOTTOM,
    DIR_RIGHTBOTTOM,
    DIR_NONE
};

class ROI
{
public:
    ROI(QWidget *parent = nullptr,ROI_type type=ROI_RECT);
    ~ROI();
    void Draw(QPainter &painter,bool isCurrent=false) const;
    EmDirection region(const QPoint &point);   //根据鼠标位置设置鼠标形状
    bool contains(const QPoint &point)const;
    void ResponseMousePressEV(const QPoint &point);
    void ResponseMousePressAndMoveEV(const QPoint &point);
    void ResponseMouseMoveEV(const QPoint &point);
    void ResponseMouseReleaseEV(const QPoint &point);
    void Scale_Rect(const QPoint &mousePoint);  //缩放矩形
    void Create_Rect(const QPoint &mousePoint);  //绘制矩形
    void Move_Rect(const QPoint &mousePoint);   //移动矩形
    bool operator ==(const ROI &other);
    void Analyst_Rect();
    QRect	m_roiRect;					//绘制的ROI
    ROI_type m_type;

private:
    float(*data)[640]=NULL;
    QWidget *m_parent;
    QPoint  m_max;
    QPoint  m_min;
    QLine   m_line;
    float   m_maxVal;
    float   m_minVal;
    float   m_averVal;
    bool    m_bPainterPressed;          //是否正在绘制
    bool    m_bMovedPressed;            //是否正在拖动
    bool    m_bScalePressed;            //是否正在缩放大小
    QPoint	m_paintStartPoint;			//绘制的初始位置
    QPoint	m_moveStartPoint;			//拖动的初始位置
    EmDirection m_emCurDir;				//拖动的方向
    QPen pen0;
    QPen pen1;
    QPen pen2;
};

#endif // ROI_H
