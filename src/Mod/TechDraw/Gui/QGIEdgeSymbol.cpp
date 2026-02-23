#include "PreCompiled.h"

#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <cmath>

#include <Base/Console.h>
#include <Mod/TechDraw/App/DrawEdgeSymbol.h>
#include <Mod/TechDraw/App/DrawLeaderLine.h>
#include <Mod/TechDraw/App/DrawUtil.h>

#include "QGIEdgeSymbol.h"
#include "Rez.h"
#include "ZVALUE.h"

using namespace TechDrawGui;
using namespace TechDraw;

QGIEdgeSymbol::QGIEdgeSymbol()
    : m_svgItem(nullptr)
{
    setHandlesChildEvents(false);
    setCacheMode(QGraphicsItem::NoCache);
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    m_svgItem = new QGraphicsSvgItem();
    m_svgItem->setParentItem(this);
    m_svgItem->setZValue(ZVALUE::DIMENSION);
}

void QGIEdgeSymbol::updateView(bool update)
{
    Q_UNUSED(update);

    auto feature = getFeature();
    if (!feature) {
        return;
    }

    if (feature->isRestoring() || !getLeader()) {
        return;
    }

    draw();
}

void QGIEdgeSymbol::draw()
{
    if (!isVisible()) {
        return;
    }

    auto feature = getFeature();
    if (!feature) {
        return;
    }

    // Remove old SVG item
    if (m_svgItem) {
        delete m_svgItem;
        m_svgItem = nullptr;
    }

    // Create new SVG from the symbol property
    std::string svgContent = feature->Symbol.getValue();
    if (svgContent.empty()) {
        return;
    }

    QByteArray qba(svgContent.c_str(), svgContent.length());
    m_svgItem = new QGraphicsSvgItem();
    QSvgRenderer* renderer = new QSvgRenderer(qba, m_svgItem);
    m_svgItem->setSharedRenderer(renderer);
    m_svgItem->setParentItem(this);
    m_svgItem->setZValue(ZVALUE::DIMENSION);

    // Position at leader's tail point
    QPointF pos = getTailPoint();
    m_svgItem->setPos(pos);

    // Rotate according to leader's last segment angle
    double angle = getLastSegAngle();
    m_svgItem->setTransformOriginPoint(m_svgItem->boundingRect().center());
    m_svgItem->setRotation(-angle);

    drawBorder();
}

QPointF QGIEdgeSymbol::getTailPoint()
{
    auto leader = getLeader();
    if (!leader) return QPointF(0, 0);
    
    Base::Vector3d org = leader->getTailPoint();
    return DU::toQPointF(Rez::guiX(org));
}

double QGIEdgeSymbol::getLastSegAngle()
{
    auto leader = getLeader();
    if (!leader) return 0.0;
    
    auto lastSegDirection = leader->lastSegmentDirection();
    auto lastSegAngleRad = DU::angleWithX(lastSegDirection);
    return Base::toDegrees(lastSegAngleRad);
}

TechDraw::DrawEdgeSymbol* QGIEdgeSymbol::getFeature()
{
    return dynamic_cast<TechDraw::DrawEdgeSymbol*>(getViewObject());
}

TechDraw::DrawLeaderLine* QGIEdgeSymbol::getLeader()
{
    auto feature = getFeature();
    if (!feature) {
        return nullptr;
    }
    return dynamic_cast<TechDraw::DrawLeaderLine*>(feature->Leader.getValue());
}

void QGIEdgeSymbol::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;
    QGIView::paint(painter, &myOption, widget);
}

QRectF QGIEdgeSymbol::boundingRect() const
{
    if (m_svgItem) {
        return m_svgItem->boundingRect();
    }
    return QRectF(0, 0, 10, 10);
}

void QGIEdgeSymbol::drawBorder()
{
}

#include <Mod/TechDraw/Gui/moc_QGIEdgeSymbol.cpp>
