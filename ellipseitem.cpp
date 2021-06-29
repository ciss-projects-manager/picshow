#include "ellipseitem.h"
#include <QtMath>
#include <QDebug>

EllipseItem::EllipseItem(QGraphicsItem *parent):
    m_ShapeType(RECTANGLE),
    m_bResize(false),
    m_oldRect(0,0,100,100),
    m_bRotate(false),
    m_RotateAngle(0),
    m_StateFlag(DEFAULT_FLAG),
    QGraphicsEllipseItem(parent)
{
    this->setZValue(++frontZ);
    this->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
    this->setFlags(ItemIsSelectable | ItemIsMovable);

    setRectSize(m_oldRect);
    setToolTip("Click and drag me!");
    setCursor(Qt::ArrowCursor);
    pPointFofSmallRotateRect = new QPointF[4];
    SetRotate(0);
}

EllipseItem::~EllipseItem()
{
    delete []pPointFofSmallRotateRect;
    pPointFofSmallRotateRect = nullptr;
}

QRectF EllipseItem::boundingRect() const//用来控制本item绘制区域
{
    QPainterPath path;
    path.addPolygon(m_oldRectPolygon);
    path.addPolygon(m_SmallRotatePolygon);
    return path.boundingRect();
}

void EllipseItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen Pen;
    Pen= QPen(Qt::black);
    Pen.setWidth(1);
    painter->setPen(Pen);

    if(m_ShapeType == CIRCLE)
    {
        QTransform trans0;
        QPainterPath path0;
        trans0.translate(m_RotateCenter.x(),m_RotateCenter.y());
        trans0.rotate(m_RotateAngle,Qt::ZAxis);
        trans0.translate(-m_RotateCenter.x(),-m_RotateCenter.y());
        path0.addEllipse(m_oldRect);
        path0 = trans0.map(path0);  // 将pathTemp旋转m_RotateAngle角度
        painter->drawPath(path0);  // drawPolygon(m_oldRectPolygon);

        //绘制旋转圆形标记

        Pen.setColor(Qt::green);
        painter->setPen(Pen);
        QPointF pf = getSmallRotateRectCenter(m_oldRectPolygon[0],m_oldRectPolygon[1]);
        QRectF rect = QRectF(pf.x()-10,pf.y()-10,20,20);
        painter->drawEllipse(rect);  // 绘制圆形
        painter->drawPoint(pf);  // 绘制点

        // 绘制椭圆长轴、短轴
        Pen.setColor(Qt::black);
        Pen.setStyle(Qt::DotLine);
        painter->setPen(Pen);
        painter->drawLine((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(3).x())/2,((m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(3).y())/2),
                         (m_oldRectPolygon.at(1).x()+m_oldRectPolygon.at(2).x())/2,((m_oldRectPolygon.at(1).y()+m_oldRectPolygon.at(2).y())/2)); // 长轴
        painter->drawLine((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(1).x())/2,((m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(1).y())/2), // 短轴
                          (m_oldRectPolygon.at(2).x()+m_oldRectPolygon.at(3).x())/2,((m_oldRectPolygon.at(2).y()+m_oldRectPolygon.at(3).y())/2));
    }
}

void EllipseItem::setRectSize(QRectF mrect, bool bResetRotateCenter)
{
    m_oldRect = mrect;
    if(bResetRotateCenter)
    {
        m_RotateCenter.setX(m_oldRect.x()+m_oldRect.width()/2);
        m_RotateCenter.setY(m_oldRect.y()+m_oldRect.height()/2);
    }
    m_oldRectPolygon = getRotatePolygonFromRect(m_RotateCenter,m_oldRect,m_RotateAngle);

    m_insicedRectf = QRectF(m_oldRect.x()+8,m_oldRect.y()+8,m_oldRect.width()-16,m_oldRect.height()-16);
    m_insicedPolygon =getRotatePolygonFromRect(m_RotateCenter,m_insicedRectf,m_RotateAngle);

    m_leftRectf = QRectF(m_oldRect.x(),m_oldRect.y(),8,m_oldRect.height()-8);
    m_leftPolygon = getRotatePolygonFromRect(m_RotateCenter,m_leftRectf,m_RotateAngle);

    m_topRectf = QRectF(m_oldRect.x()+8,m_oldRect.y(),m_oldRect.width()-8,8);
    m_topPolygon = getRotatePolygonFromRect(m_RotateCenter,m_topRectf,m_RotateAngle);

    m_rightRectf = QRectF(m_oldRect.right()-8,m_oldRect.y()+8,8,m_oldRect.height()-16);
    m_rightPolygon = getRotatePolygonFromRect(m_RotateCenter,m_rightRectf,m_RotateAngle);

    m_bottomRectf = QRectF(m_oldRect.x(),m_oldRect.bottom()-8,m_oldRect.width()-8,8);
    m_bottomPolygon = getRotatePolygonFromRect(m_RotateCenter,m_bottomRectf,m_RotateAngle);

    m_SmallRotateRect = getSmallRotateRect(mrect.topLeft(),mrect.topRight());//矩形正上方的旋转标记矩形
    m_SmallRotatePolygon = getRotatePolygonFromRect(m_RotateCenter,m_SmallRotateRect,m_RotateAngle);

}

QPainterPath EllipseItem::shape() const//用来控制检测碰撞collide和鼠标点击hit响应区域
{
    QPainterPath path;
    path.addPolygon(m_oldRectPolygon);
    path.addPolygon(m_SmallRotatePolygon);
    return path;
}

QPainterPath EllipseItem::getCollideShape()
{
    QPainterPath path;
    if(m_ShapeType==RECTANGLE)
    {
        path.addPolygon(m_oldRectPolygon);
    }
    else if(m_ShapeType == CIRCLE)
    {
        QPainterPath pathTemp;
        pathTemp.addEllipse(m_oldRect);
        QTransform trans;
        trans.translate(m_RotateCenter.x(),m_RotateCenter.y());
        trans.rotate(m_RotateAngle);//QTransform是绕（0,0）点旋转的，所以转之前要先平移到圆心，然后旋转，然后再平移回来
        trans.translate(-m_RotateCenter.x(),-m_RotateCenter.y());
        path = trans.map(pathTemp);
    }
    return path;
}

void EllipseItem ::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton)
    {
        setSelected(true);
        m_startPos = event->pos();  // 鼠标左击时，获取当前鼠标在图片中的坐标，
        if(m_SmallRotatePolygon.containsPoint(m_startPos,Qt::WindingFill))//旋转矩形
        {
            setCursor(Qt::PointingHandCursor);
            m_StateFlag = ROTATE;
        }
        else if(m_insicedPolygon.containsPoint(m_startPos,Qt::WindingFill))  // 在矩形内框区域时按下鼠标，则可拖动图片
        {
            setCursor(Qt::ClosedHandCursor);  // 改变光标形状,手的形状
            m_StateFlag = MOV_RECT;//标记当前为鼠标拖动图片移动状态
        }
        else if(m_leftPolygon.containsPoint(m_startPos,Qt::WindingFill))
        {
            setCursor(Qt::SizeHorCursor);
            m_StateFlag = MOV_LEFT_LINE;//标记当前为用户按下矩形的左边界区域
        }
        else if(m_rightPolygon.containsPoint(m_startPos,Qt::WindingFill))
        {
            setCursor(Qt::SizeHorCursor);
            m_StateFlag = MOV_RIGHT_LINE;//标记当前为用户按下矩形的右边界区域
        }
        else if(m_topPolygon.containsPoint(m_startPos,Qt::WindingFill))
        {
            setCursor(Qt::SizeVerCursor);
            m_StateFlag = MOV_TOP_LINE;//标记当前为用户按下矩形的上边界区域
        }
        else if(m_bottomPolygon.containsPoint(m_startPos,Qt::WindingFill))
        {
            setCursor(Qt::SizeVerCursor);
            m_StateFlag = MOV_BOTTOM_LINE;//标记当前为用户按下矩形的下边界区域
        }
        else
        {
            m_StateFlag = DEFAULT_FLAG;
        }
    }
    else
    {
        QGraphicsItem::mousePressEvent(event);
    }
}

void EllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_StateFlag == ROTATE)
    {
       int nRotateAngle = atan2((event->pos().x()-m_RotateCenter.x()),(event->pos().y()-m_RotateCenter.y()))*180/M_PI;
       SetRotate(180-nRotateAngle);
    }
    else if(m_StateFlag == MOV_RECT)
    {
        QPointF point = (event->pos() - m_startPos);
        moveBy(point.x(), point.y());
        setRectSize(m_oldRect);
        scene()->update();
    }
    else if(m_StateFlag == MOV_LEFT_LINE)
    {
        QPointF pf = QPointF((m_oldRectPolygon.at(1).x()+m_oldRectPolygon.at(2).x())/2,((m_oldRectPolygon.at(1).y()+m_oldRectPolygon.at(2).y())/2));
        //计算到右侧边中点的距离
        qreal dis = sqrt((event->pos().x()-pf.x())*(event->pos().x()-pf.x()) +(event->pos().y()-pf.y())*(event->pos().y()-pf.y()));
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2RT = sqrt((event->pos().x()-m_oldRectPolygon.at(1).x())*(event->pos().x()-m_oldRectPolygon.at(1).x()) +(event->pos().y()-m_oldRectPolygon.at(1).y())*(event->pos().y()-m_oldRectPolygon.at(1).y()));
        if(dis<16||dis2LT>dis2RT)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setLeft(m_oldRect.right()-dis);
            newRect.setRight(m_oldRect.right());
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
        }
    }
    else if(m_StateFlag == MOV_TOP_LINE)
    {
        //底边中点
        QPointF pf = QPointF((m_oldRectPolygon.at(2).x()+m_oldRectPolygon.at(3).x())/2,((m_oldRectPolygon.at(2).y()+m_oldRectPolygon.at(3).y())/2));
        //计算到底边中点的距离
        qreal dis = sqrt((event->pos().x()-pf.x())*(event->pos().x()-pf.x()) +(event->pos().y()-pf.y())*(event->pos().y()-pf.y()));
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2LB = sqrt((event->pos().x()-m_oldRectPolygon.at(3).x())*(event->pos().x()-m_oldRectPolygon.at(3).x()) +(event->pos().y()-m_oldRectPolygon.at(3).y())*(event->pos().y()-m_oldRectPolygon.at(3).y()));
        if(dis<16||dis2LT>dis2LB)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setTop(m_oldRect.bottom()-dis);
            newRect.setBottom(m_oldRect.bottom());
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
        }
    }
    else if(m_StateFlag == MOV_RIGHT_LINE)
    {
        QPointF pf = QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(3).x())/2,((m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(3).y())/2));
        //计算到左侧边中点的距离
        qreal dis = sqrt((event->pos().x()-pf.x())*(event->pos().x()-pf.x()) +(event->pos().y()-pf.y())*(event->pos().y()-pf.y()));
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2RT = sqrt((event->pos().x()-m_oldRectPolygon.at(1).x())*(event->pos().x()-m_oldRectPolygon.at(1).x()) +(event->pos().y()-m_oldRectPolygon.at(1).y())*(event->pos().y()-m_oldRectPolygon.at(1).y()));
        if(dis<16||dis2LT<dis2RT)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setLeft(m_oldRect.left());
            newRect.setRight(m_oldRect.left()+dis);
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
        }
    }
    else if(m_StateFlag == MOV_BOTTOM_LINE)
    {
        //顶边中点
        QPointF pf = QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(1).x())/2,((m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(1).y())/2));
        //计算到底边中点的距离
        qreal dis = sqrt((event->pos().x()-pf.x())*(event->pos().x()-pf.x()) +(event->pos().y()-pf.y())*(event->pos().y()-pf.y()));
        qreal dis2LT = sqrt((event->pos().x()-m_oldRectPolygon.at(0).x())*(event->pos().x()-m_oldRectPolygon.at(0).x()) +(event->pos().y()-m_oldRectPolygon.at(0).y())*(event->pos().y()-m_oldRectPolygon.at(0).y()));
        qreal dis2LB = sqrt((event->pos().x()-m_oldRectPolygon.at(3).x())*(event->pos().x()-m_oldRectPolygon.at(3).x()) +(event->pos().y()-m_oldRectPolygon.at(3).y())*(event->pos().y()-m_oldRectPolygon.at(3).y()));
        if(dis<16||dis2LT<dis2LB)
        {
            return;
        }
        else
        {
            QRectF newRect(m_oldRect);
            newRect.setTop(m_oldRect.top());
            newRect.setBottom(m_oldRect.top()+dis);
            setRectSize(newRect,false);
            m_RotateCenter=QPointF((m_oldRectPolygon.at(0).x()+m_oldRectPolygon.at(2).x())/2,(m_oldRectPolygon.at(0).y()+m_oldRectPolygon.at(2).y())/2);
            m_oldRect.moveCenter(m_RotateCenter);
            setRectSize(m_oldRect);
            scene()->update();//必须要用scene()->update()，不能用update();否则会出现重影
        }
    }
}

void EllipseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCursor(Qt::ArrowCursor);
    if(m_StateFlag == MOV_RECT)
    {
        m_StateFlag = DEFAULT_FLAG;
    }
    else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
}

void EllipseItem::SetRotate(qreal RotateAngle)
{
    m_bRotate = true;
    m_RotateAngle = RotateAngle;
    setRectSize(m_oldRect);
    if(this->scene()!=nullptr)
        this->scene()->update();
}

QPointF EllipseItem::getRotatePoint(QPointF ptCenter, QPointF ptIn, qreal angle)
{
    double dx = ptCenter.x();
    double dy = ptCenter.y();
    double x = ptIn.x();
    double y = ptIn.y();
    double xx,yy;
    xx = (x-dx)*cos(angle*M_PI/180)-(y-dy)*sin(angle*M_PI/180)+dx;
    yy = (x-dx)*sin(angle*M_PI/180)+(y-dy)*cos(angle*M_PI/180)+dy;

    return QPointF(xx,yy);
}

QPolygonF EllipseItem::getRotatePolygonFromRect(QPointF ptCenter, QRectF rectIn, qreal angle)
{
    QVector<QPointF> vpt;
    QPointF pf = getRotatePoint(ptCenter,rectIn.topLeft(),angle);
    vpt.append(pf);
    pf = getRotatePoint(ptCenter,rectIn.topRight(),angle);
    vpt.append(pf);
    pf = getRotatePoint(ptCenter,rectIn.bottomRight(),angle);
    vpt.append(pf);
    pf = getRotatePoint(ptCenter,rectIn.bottomLeft(),angle);
    vpt.append(pf);
    pf = getRotatePoint(ptCenter,rectIn.topLeft(),angle);
    vpt.append(pf);
    return QPolygonF(vpt);
}

QRectF EllipseItem::getSmallRotateRect(QPointF ptA,QPointF ptB)
{
    QPointF pt = getSmallRotateRectCenter(ptA,ptB);
    return QRectF(pt.x()-10,pt.y()-10,20,20);
}
QPointF EllipseItem::getSmallRotateRectCenter(QPointF ptA,QPointF ptB)
{
    QPointF ptCenter = QPointF((ptA.x()+ptB.x())/2,(ptA.y()+ptB.y())/2);//A,B点的中点C
    //中垂线方程式为 y=x*k + b;
    qreal x,y;//旋转图标矩形的中心
    if(abs(ptB.y()-ptA.y())<0.1)
    {
        if(ptA.x()<ptB.x())//矩形左上角在上方
        {
            x = ptCenter.x();
            y = ptCenter.y()-20;
        }
        else//矩形左上角在下方
        {
            x = ptCenter.x();
            y = ptCenter.y()+20;
        }
    }
    else if(ptB.y()>ptA.y())//顺时针旋转0-180
    {
        qreal k = (ptA.x()-ptB.x())/(ptB.y()-ptA.y());//中垂线斜率
        qreal b = (ptA.y()+ptB.y())/2-k*(ptA.x()+ptB.x())/2;
        //求AB线中垂线上离AB中点20个像素的点C的坐标
        x = 20*cos(atan(k))+ptCenter.x();
        y = k*x+b;
    }
    else if(ptB.y()<ptA.y())//顺时针旋转180-360
    {
        qreal k = (ptA.x()-ptB.x())/(ptB.y()-ptA.y());//中垂线斜率
        qreal b = (ptA.y()+ptB.y())/2-k*(ptA.x()+ptB.x())/2;
        //求AB线中垂线上离AB中点20个像素的点C的坐标
        x = -20*cos(atan(k))+ptCenter.x();
        y = k*x+b;
    }
    return QPointF(x,y);
}