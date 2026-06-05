#include "roi.h"
#include "QWidget"
#include "QDebug"
#include "myglwidget.h"
#include <cmath>
ROI::ROI(QWidget *parent,ROI_type type)
    :   m_roiRect(0, 0, 0, 0),
        m_type(type),
        m_parent(parent),
        m_bPainterPressed(false),
        m_bMovedPressed(false),
        m_bScalePressed(false),
        m_emCurDir(EmDirection::DIR_NONE)
{
    pen0.setColor(Qt::yellow);
    pen0.setWidth(EDGE_WIDTH);
    pen1.setColor(Qt::red);
    pen1.setWidth(1);
    pen2.setColor(Qt::blue);
    pen2.setWidth(1);
    myGLwidget *ptr=(myGLwidget *)m_parent;
    data=(float(*)[640])ptr->m_Tdata;//[row][col]
}
ROI::~ROI()
{
    m_parent=NULL;
}
/**
 * @brief				判断鼠标的位置
 * @param point         鼠标的位置
 *
 * @return				EmDirection 方向
 */
EmDirection ROI::region(const QPoint & point)
{
    int mouseX = point.x();
    int mouseY = point.y();
    
    if(m_type == ROI_POINT)
    {
        int px = m_roiRect.x();
        int py = m_roiRect.y();
        if(abs(mouseX - px) <= POINT_PADDING && abs(mouseY - py) <= POINT_PADDING)
        {
            m_parent->setCursor(Qt::OpenHandCursor);
            return DIR_MIDDLE;
        }
        return DIR_NONE;
    }
    
    int ret=0;
    QPoint roiTopLeft = m_roiRect.topLeft();
    QPoint roiBottomRight = m_roiRect.bottomRight();
    if (mouseX > roiTopLeft.x()-CORPADDING && mouseX < roiBottomRight.x() + CORPADDING)
        ret+=1;
    else return DIR_NONE;
    if ( mouseY > roiTopLeft.y()-CORPADDING && mouseY < roiBottomRight.y()+CORPADDING)
        ret+=1<<2;
    else return DIR_NONE;

    if (mouseX > roiTopLeft.x() + CORPADDING)
        ret+=1;
    if (mouseY > roiTopLeft.y()+CORPADDING)
        ret+=1<<2;

    if (mouseX > roiBottomRight.x() - CORPADDING)
        ret+=1;
    if (mouseY > roiBottomRight.y() - CORPADDING)
        ret+=1<<2;
    switch (ret) {
    case DIR_LEFTTOP:
    case DIR_RIGHTBOTTOM:
        m_parent->setCursor(Qt::SizeFDiagCursor);
        break;
    case DIR_RIGHTTOP:

    case DIR_LEFTBOTTOM:
        m_parent->setCursor(Qt::SizeBDiagCursor);
        break;
    case DIR_LEFT:
    case DIR_RIGHT:
        m_parent->setCursor(Qt::SizeHorCursor);
        break;
    case DIR_TOP:
    case DIR_BOTTOM:
        m_parent->setCursor(Qt::SizeVerCursor);
        break;
    default:
        break;
    }
    return (EmDirection)ret;
}
/**
 * @brief				缩放矩形
 * @param mousePoint    鼠标的位置
 *
 * @return				void
 */
void ROI::Scale_Rect(const QPoint & mousePoint)
{
    QRect newRect(m_roiRect.topLeft(), m_roiRect.bottomRight());

    //根据当前的缩放状态来改变矩形的位置大小信息
    switch (m_emCurDir)
    {
    case DIR_LEFT:
        newRect.setLeft(mousePoint.x());
        break;
    case DIR_RIGHT:
        newRect.setRight(mousePoint.x());
        break;
    case DIR_TOP:
        newRect.setTop(mousePoint.y());
        break;
    case DIR_BOTTOM:
        newRect.setBottom(mousePoint.y());
        break;
    case DIR_LEFTTOP:
        newRect.setTopLeft(mousePoint);
        break;
    case DIR_LEFTBOTTOM:
        newRect.setBottomLeft(mousePoint);
        break;
    case DIR_RIGHTTOP:
        newRect.setTopRight(mousePoint);
        break;
    case DIR_RIGHTBOTTOM:
        newRect.setBottomRight(mousePoint);
        break;
    default:
        return;
    }

    if (newRect.width() < MIN_WIDTH || newRect.height() < MIN_HEIGHT)
    {
        //缩放的大小限制
        return;
    }

    m_roiRect = newRect;
    m_moveStartPoint = mousePoint;  //更新鼠标的起始位置
}

/**
 * @brief				绘制矩形
 * @param mousePoint    鼠标的位置
 *
 * @return				void
 */
void ROI::Create_Rect(const QPoint & mousePoint)
{
    m_parent->setCursor(Qt::ArrowCursor);

    if(m_type == ROI_POINT)
    {
        m_roiRect.setX(mousePoint.x());
        m_roiRect.setY(mousePoint.y());
        m_roiRect.setSize(QSize(1, 1));
        return;
    }

    int width = mousePoint.x() - m_paintStartPoint.x();
    int height = mousePoint.y() - m_paintStartPoint.y();

    if (width < 0 && height < 0)
    {
        m_roiRect.setX(mousePoint.x());
        m_roiRect.setY(mousePoint.y());
    }
    else if (width < 0)
    {
        m_roiRect.setX(mousePoint.x());
        m_roiRect.setY(m_paintStartPoint.y());
    }
    else if (height < 0)
    {
        m_roiRect.setX(m_paintStartPoint.x());
        m_roiRect.setY(mousePoint.y());
    }
    else
    {
        m_roiRect.setX(m_paintStartPoint.x());
        m_roiRect.setY(m_paintStartPoint.y());
    }

    m_roiRect.setSize(QSize(abs(width), abs(height)));
}

/**
 * @brief				绘制矩形
 * @param mousePoint    鼠标的位置
 *
 * @return				void
 */
void ROI::Move_Rect(const QPoint & mousePoint)
{
    m_parent->setCursor(Qt::ClosedHandCursor);

    int width = mousePoint.x() - m_moveStartPoint.x();
    int height = mousePoint.y() - m_moveStartPoint.y();

    if(m_type == ROI_POINT)
    {
        m_roiRect.setX(m_roiRect.x() + width);
        m_roiRect.setY(m_roiRect.y() + height);
    }
    else
    {
        QRect ret;
        ret.setX(m_roiRect.x() + width);
        ret.setY(m_roiRect.y() + height);
        ret.setSize(m_roiRect.size());
        m_roiRect = ret;
    }
    m_moveStartPoint = mousePoint;
}
void ROI::ResponseMousePressEV(const QPoint &point)
{
    EmDirection dir = region(point);     //获取鼠标当前的位置
    if (dir == DIR_MIDDLE)
    {
        m_parent->setCursor(Qt::ClosedHandCursor);
        m_moveStartPoint=point;
        m_bMovedPressed = true;
    }
    else if (dir == DIR_NONE)
    {
        m_parent->setCursor(Qt::ArrowCursor);
        m_bPainterPressed = true;
        m_paintStartPoint= point;
        if(m_type == ROI_POINT)
        {
            m_roiRect.setX(point.x());
            m_roiRect.setY(point.y());
            m_roiRect.setSize(QSize(1, 1));
            Analyst_Rect();
            m_parent->update();
        }
    }
    else
    {
        m_moveStartPoint=point;
        m_bScalePressed = true;
        m_emCurDir = dir;
    }
}
void ROI::ResponseMouseMoveEV(const QPoint &point)
{
    //根据鼠标的位置设置当前的鼠标形状
    EmDirection dir = region(point);
    if (dir == DIR_NONE)
    {
        m_parent->setCursor(Qt::ArrowCursor);
    }
    else if (dir == DIR_MIDDLE)
    {
        m_parent->setCursor(Qt::OpenHandCursor);
    }
}
void ROI::ResponseMousePressAndMoveEV(const QPoint &point)
{
    if (m_bPainterPressed)
    {
        //正在绘制状态
        Analyst_Rect();
        Create_Rect(point);
    }
    else if (m_bMovedPressed)
    {
        //正在移动状态
        Analyst_Rect();
        Move_Rect(point);
    }
    else if (m_bScalePressed)
    {
        //正在缩放大小状态
        Analyst_Rect();
        Scale_Rect(point);
    }
    //更新界面
    m_parent->update();
}
void ROI::ResponseMouseReleaseEV(const QPoint &point)
{
    //判断鼠标是否在矩形中
    if (m_roiRect.contains(point))
    {
        //松开鼠标前是否正在拖放
        if (m_bMovedPressed)
        {
            m_parent->setCursor(Qt::OpenHandCursor);
        }
        else
        {
            m_parent->setCursor(Qt::ArrowCursor);
        }
    }

    m_paintStartPoint = QPoint();
    m_bMovedPressed = false;
    m_bPainterPressed = false;
    m_bScalePressed = false;
}
/**
 * @brief 绘制当前ROI
 * @param painter
 * @param isCurrent
 */
void ROI::Draw(QPainter &painter,bool isCurrent) const
{

    switch (m_type) {
    case ROI_RECT:{
        QPolygon polygon;
        int maxPoints[]={m_max.x(),m_max.y(),m_max.x()-4,m_max.y()+8,m_max.x()+4,m_max.y()+8};
        int minPoints[]={m_min.x(),m_min.y(),m_min.x()-4,m_min.y()-8,m_min.x()+4,m_min.y()-8};

        painter.setPen(pen1);
        painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
        polygon.setPoints(3,maxPoints);
        painter.drawPolygon(polygon);
        painter.setPen(pen2);
        painter.setBrush(QBrush(Qt::blue,Qt::SolidPattern));
        polygon.setPoints(3,minPoints);
        painter.drawPolygon(polygon);
        painter.setBrush(Qt::NoBrush);
        if(isCurrent)
            painter.setPen(QPen(Qt::yellow, 3));
        else
            painter.setPen(QPen(Qt::green, 2));
        painter.drawRect(m_roiRect);
        painter.setPen(Qt::green);
        painter.drawText(m_roiRect,"\tMax:"+QString::number(m_maxVal)
                         +"\tMin:"+QString::number(m_minVal)
                         +"\tAver:"+ QString::number(m_averVal));
        break;
    }
    case ROI_POINT:{
        int px = m_roiRect.x();
        int py = m_roiRect.y();
        
        if(isCurrent)
            painter.setPen(QPen(Qt::yellow, 3));
        else
            painter.setPen(QPen(Qt::green, 2));
        
        painter.drawLine(px - 8, py, px + 8, py);
        painter.drawLine(px, py - 8, px, py + 8);
        
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        painter.drawEllipse(px - 3, py - 3, 6, 6);
        
        painter.setPen(Qt::green);
        painter.drawText(px - 20, py + 18, QString::number(m_maxVal) + "°C");
        break;
    }
    default:
        return;

    }

}
bool ROI::contains(const QPoint &point)const
{
    if(m_type == ROI_POINT)
    {
        int px = m_roiRect.x();
        int py = m_roiRect.y();
        return (abs(point.x() - px) <= POINT_PADDING && abs(point.y() - py) <= POINT_PADDING);
    }
    return m_roiRect.contains(point);
}
bool ROI::operator==(const ROI &other)
{
    return(m_type==other.m_type&&(&m_type==&other.m_type));
}
void ROI::Analyst_Rect()
{

    if(data==NULL)
        return;
    m_averVal=0;
    switch (m_type) {
    case ROI_RECT:
    {
        float cnt=0;
        int x=fmax(m_roiRect.x(),0);
        int y=fmax(m_roiRect.y(),0);
        int cols=fmin((m_roiRect.right()-x),640-x);
        int rows=fmin(m_roiRect.bottom()-y,512-y);
        m_maxVal=m_minVal=data[y][x];
        for(int c=0;c<cols;c++)
        {
            for(int r=0;r<rows;r++)
            {
                if(data[y+r][x+c]>m_maxVal)
                {
                    m_maxVal = data[y+r][x+c];
                    m_max.setX(x+c);
                    m_max.setY(y+r);
                }
                if(data[y+r][x+c]<m_minVal)
                {
                    m_minVal = data[y+r][x+c];
                    m_min.setX(x+c);
                    m_min.setY(y+r);
                }
                cnt+=data[y+r][x+c];
            }
            m_averVal+= (cnt/rows);
            cnt=0;
        }
        m_averVal/=cols;
        break;
    }
    case ROI_POINT:
    {
        int x = qBound(0, m_roiRect.x(), 639);
        int y = qBound(0, m_roiRect.y(), 511);
        m_maxVal = m_minVal = m_averVal = data[y][x];
        m_max.setX(x);
        m_max.setY(y);
        m_min.setX(x);
        m_min.setY(y);
        break;
    }
    default:
        return;
    }
}
