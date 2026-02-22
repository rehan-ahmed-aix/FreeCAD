#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <cmath>
#include "TaskEdgeSymbol.h"
#include "ui_TaskEdgeSymbol.h"

#include <App/Application.h>
#include <App/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Mod/TechDraw/App/DrawPage.h>
#include <Mod/TechDraw/App/DrawUtil.h>
#include <Mod/TechDraw/App/DrawViewSymbol.h>

#include "QGIView.h"
#include "ViewProviderSymbol.h"
#include "ZVALUE.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

using namespace Gui;
using namespace TechDraw;
using namespace TechDrawGui;

EdgeSvgString::EdgeSvgString(int width, int height)
{
    svgStream << "<?xml version='1.0'?>\n";
    svgStream << "<svg width='" << width << "' height='" << height << "' viewBox='0 0 120 120' xmlns='http://www.w3.org/2000/svg'>\n";
}

void EdgeSvgString::addLine(double xStart, double yStart, double xEnd, double yEnd, double width)
{
    svgStream << "<path stroke='#000' stroke-width='" << width << "' d='";
    svgStream << " M" << xStart << ", " << yStart;
    svgStream << " L" << xEnd << ", " << yEnd;
    svgStream << "' />\n";
}

void EdgeSvgString::addText(double xText, double yText, const std::string& text, double size, const std::string& anchor)
{
    svgStream << "<text x='" << xText << "' y='" << yText;
    svgStream << "' style='font-size:" << size << "px; font-family: sans-serif;' text-anchor='" << anchor << "'>" << text << "</text>\n";
}



void EdgeSvgString::addArrow(double xStart, double yStart, double xEnd, double yEnd)
{
    // Draw the leader line
    svgStream << "<g><path d='M" << xStart << "," << yStart << " L" << xEnd << "," << yEnd 
              << "' stroke='#000000' stroke-width='1' /></g>\n";
              
    // Calculate angle for the arrow head
    double angle = atan2(yEnd - yStart, xEnd - xStart) * 180.0 / 3.14159265358979323846;
    
    // Draw an explicit triangle for the arrow head (since QtSvg doesn't support markers)
    // The triangle points back towards the start
    svgStream << "<g transform='translate(" << xEnd << "," << yEnd << ")'> ";
    svgStream << "<path transform='rotate(" << angle << ")' d='M 0.0,0.0 -12.0,3.0 -12.0,-3.0 0.0,0.0 Z' ";
    svgStream << "fill='#000000' stroke='#000000' stroke-linejoin='miter' /></g>\n";
}

std::string EdgeSvgString::finish()
{
    svgStream << "</svg>\n";
    return svgStream.str();
}

TaskEdgeSymbol::TaskEdgeSymbol(const std::string &ownerName) :
    ui(new Ui_TaskEdgeSymbol)
{
    App::Document *doc = Gui::Application::Instance->activeDocument()->getDocument();
    if (doc) {
        owner = doc->getObject(ownerName.c_str());
        std::string subName;
        if (!owner) {
            size_t dot = ownerName.rfind('.');
            if (dot != std::string::npos) {
                subName = ownerName.substr(dot + 1);
                owner = doc->getObject(ownerName.substr(0, dot).c_str());
            }
        }

        auto page = dynamic_cast<TechDraw::DrawPage *>(owner);
        if (page) {
            placement.x = page->getPageWidth()/2.0;
            placement.y = page->getPageHeight()/2.0;
        }

        auto viewPart = dynamic_cast<TechDraw::DrawViewPart *>(owner);
        if (viewPart && !subName.empty()) {
            std::string subType = DrawUtil::getGeomTypeFromName(subName);
            if (subType == "Vertex") {
                TechDraw::VertexPtr vertex = viewPart->getVertex(subName);
                if (vertex) {
                    placement = vertex->point();
                }
            }
            else if (subType == "Edge") {
                TechDraw::BaseGeomPtr edge = viewPart->getEdge(subName);
                if (edge) {
                    placement = edge->getMidPoint();
                }
            }
            else if (subType == "Face") {
                TechDraw::FacePtr face = viewPart->getFace(subName);
                if (face) {
                    placement = face->getCenter();
                }
            }
            placement = DrawUtil::invertY(placement);
        }
    }

    ui->setupUi(this);
    
    symbolScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(symbolScene);

    connect(ui->cbEdgeType, &QComboBox::currentTextChanged, this, &TaskEdgeSymbol::onParametersChanged);
    connect(ui->leUpper, &QLineEdit::textChanged, this, &TaskEdgeSymbol::onParametersChanged);
    connect(ui->leLower, &QLineEdit::textChanged, this, &TaskEdgeSymbol::onParametersChanged);
    connect(ui->cbLeader, &QCheckBox::toggled, this, &TaskEdgeSymbol::onParametersChanged);

    onParametersChanged();
}

void TaskEdgeSymbol::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void TaskEdgeSymbol::onParametersChanged()
{
    updatePreview();
}

void TaskEdgeSymbol::updatePreview()
{
    symbolScene->clear();
    std::string svg = buildSvg();
    QByteArray ba(svg.c_str(), svg.length());
    QGraphicsSvgItem* svgItem = new QGraphicsSvgItem();
    QSvgRenderer* renderer = new QSvgRenderer(ba, svgItem);
    svgItem->setSharedRenderer(renderer);
    symbolScene->addItem(svgItem);
    ui->graphicsView->fitInView(svgItem, Qt::KeepAspectRatio);
}

std::string TaskEdgeSymbol::buildSvg()
{
    EdgeSvgString svg(100, 100);
    
    std::string type = ui->cbEdgeType->currentText().toStdString();
    std::string upper = ui->leUpper->text().toStdString();
    std::string lower = ui->leLower->text().toStdString();
    bool hasLeader = ui->cbLeader->isChecked();

    if (hasLeader) {
        svg.addArrow(40, 65, 13, 90);
    }

    // Basic layout for ISO 13715
    svg.addLine(40, 65, 100, 65, 1); // Reference line

    if (type == "External") {
        svg.addLine(55, 60, 95, 60, 3); // Horizontal corner
        svg.addLine(55, 12, 55, 62, 3); // Vertical corner
        if (!upper.empty()) svg.addText(35, 30, upper, 18, "end");
        if (!lower.empty()) svg.addText(35, 55, lower, 18, "end");
    } else if (type == "Internal") {
        svg.addLine(55, 12, 95, 12, 3);
        svg.addLine(95, 12, 95, 62, 3);
        if (!upper.empty()) svg.addText(75, 40, upper, 18, "middle");
        if (!lower.empty()) svg.addText(75, 55, lower, 18, "middle");
    } else {
        // Undefined / Basic
        svg.addLine(55, 40, 75, 12, 3);
        if (!upper.empty()) svg.addText(45, 30, upper, 18, "end");
        if (!lower.empty()) svg.addText(45, 55, lower, 18, "end");
    }

    return svg.finish();
}

bool TaskEdgeSymbol::accept()
{
    Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Edge Symbol"));
    App::Document *doc = Gui::Application::Instance->activeDocument()->getDocument();
    auto* symbol = doc->addObject<TechDraw::DrawViewSymbol>("EdgeSymbol");
    symbol->Symbol.setValue(buildSvg());
    symbol->Rotation.setValue(ui->leAngle->text().toDouble());

    auto view = dynamic_cast<TechDraw::DrawView *>(owner);
    symbol->Owner.setValue(view);
    symbol->X.setValue(placement.x);
    symbol->Y.setValue(placement.y);

    auto viewProvider = dynamic_cast<ViewProviderSymbol *>(QGIView::getViewProvider(symbol));
    if (viewProvider) {
        viewProvider->StackOrder.setValue(ZVALUE::DIMENSION);
    }

    auto page = dynamic_cast<TechDraw::DrawPage *>(owner);
    if (!page && view) {
        page = view->findParentPage();
    }
    if (page) {
        page->addView(symbol);
    }

    Gui::Command::commitCommand();
    return true;
}

bool TaskEdgeSymbol::reject()
{
    return true;
}

TaskDlgEdgeSymbol::TaskDlgEdgeSymbol(const std::string &ownerName)
    : TaskDialog()
{
    widget  = new TaskEdgeSymbol(ownerName);
    taskbox = new Gui::TaskView::TaskBox(Gui::BitmapFactory().pixmap("actions/TechDraw_EdgeSymbol"),
                                             widget->windowTitle(), true, nullptr);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
}

TaskDlgEdgeSymbol::~TaskDlgEdgeSymbol()
{
}

bool TaskDlgEdgeSymbol::accept()
{
    widget->accept();
    return true;
}

bool TaskDlgEdgeSymbol::reject()
{
    widget->reject();
    return true;
}

#include <Mod/TechDraw/Gui/moc_TaskEdgeSymbol.cpp>
