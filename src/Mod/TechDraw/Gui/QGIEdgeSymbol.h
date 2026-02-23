#ifndef DRAWINGGUI_QGRAPHICSITEMEDGESYMBOL_H
#define DRAWINGGUI_QGRAPHICSITEMEDGESYMBOL_H

#include <Mod/TechDraw/TechDrawGlobal.h>
#include <QGraphicsItem>
#include <QPointF>
#include "QGIUserTypes.h"
#include "QGIView.h"

class QGraphicsSvgItem;

namespace TechDraw {
class DrawEdgeSymbol;
class DrawLeaderLine;
}

namespace TechDrawGui
{

class TechDrawGuiExport QGIEdgeSymbol : public QGIView
{
    Q_OBJECT

public:
    enum {Type = UserType::QGIWeldSymbol + 100};

    explicit QGIEdgeSymbol();
    ~QGIEdgeSymbol() override = default;

    int type() const override { return Type;}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;

    void drawBorder() override;
    void updateView(bool update = false) override;

    TechDraw::DrawEdgeSymbol* getFeature();
    TechDraw::DrawLeaderLine* getLeader();

    QPointF getTailPoint();
    double getLastSegAngle();

private:
    void draw();

    QGraphicsSvgItem* m_svgItem;
};

} // namespace TechDrawGui

#endif
