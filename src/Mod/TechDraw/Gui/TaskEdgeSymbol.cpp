#include "PreCompiled.h"
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QComboBox>
#include <QLineEdit>
#include <cmath>
#include <sstream>

#include "TaskEdgeSymbol.h"
#include "ui_TaskEdgeSymbol.h"

#include <App/Application.h>
#include <App/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Application.h>
#include <Gui/Document.h>

#include <Mod/TechDraw/App/DrawEdgeSymbol.h>
#include <Mod/TechDraw/App/DrawLeaderLine.h>
#include <Mod/TechDraw/App/DrawPage.h>
#include <Mod/TechDraw/App/DrawUtil.h>

#include "ViewProviderEdgeSymbol.h"
#include "ZVALUE.h"
#include "QGIView.h"
#include "DrawGuiUtil.h"
#include <Base/Console.h>

using namespace Gui;
using namespace TechDraw;
using namespace TechDrawGui;

TaskEdgeSymbol::TaskEdgeSymbol(TechDraw::DrawLeaderLine *leader) :
    ui(new Ui_TaskEdgeSymbol), m_symbol(nullptr), m_leader(leader), m_createMode(true)
{
    init();
}

TaskEdgeSymbol::TaskEdgeSymbol(TechDraw::DrawEdgeSymbol *symbol) :
    ui(new Ui_TaskEdgeSymbol), m_symbol(symbol), m_leader(nullptr), m_createMode(false)
{
    if (m_symbol) {
        m_leader = dynamic_cast<TechDraw::DrawLeaderLine*>(m_symbol->Leader.getValue());
    }
    init();
}

void TaskEdgeSymbol::init()
{
    ui->setupUi(this);
    
    symbolScene = new QGraphicsScene(this);
    ui->graphicsView->setScene(symbolScene);

    if (m_symbol) {
        ui->cbEdgeType->setCurrentText(QString::fromStdString(m_symbol->EdgeType.getValue()));
        ui->leUpper->setText(QString::fromStdString(m_symbol->UpperDeviation.getValue()));
        ui->leLower->setText(QString::fromStdString(m_symbol->LowerDeviation.getValue()));
        ui->leAngle->setText(QString::number(m_symbol->Rotation.getValue()));
    }

    connect(ui->cbEdgeType, &QComboBox::currentTextChanged, this, &TaskEdgeSymbol::onParametersChanged);
    connect(ui->leUpper, &QLineEdit::textChanged, this, &TaskEdgeSymbol::onParametersChanged);
    connect(ui->leLower, &QLineEdit::textChanged, this, &TaskEdgeSymbol::onParametersChanged);

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
    
    std::stringstream svgStream;
    svgStream << "<?xml version='1.0'?>\n";
    svgStream << "<svg width='100' height='100'>\n";
    
    std::string type = ui->cbEdgeType->currentText().toStdString();
    std::string upper = ui->leUpper->text().toStdString();
    std::string lower = ui->leLower->text().toStdString();

    auto addLine = [&svgStream](int x1, int y1, int x2, int y2, int w) {
        svgStream << "<path stroke='#000' stroke-width='" << w << "' d=' M" << x1 << ", " << y1 << " L" << x2 << ", " << y2 << "' />\n";
    };
    auto addText = [&svgStream](int x, int y, const std::string& txt, int sz, const std::string& anchor) {
        svgStream << "<text x='" << x << "' y='" << y << "' style='font-size:" << sz << "px; font-family: sans-serif;' text-anchor='" << anchor << "'>" << txt << "</text>\n";
    };

    addLine(40, 65, 100, 65, 1);

    if (type == "External") {
        addLine(55, 60, 95, 60, 3);
        addLine(55, 12, 55, 62, 3);
        if (!upper.empty()) addText(35, 30, upper, 18, "end");
        if (!lower.empty()) addText(35, 55, lower, 18, "end");
    } else if (type == "Internal") {
        addLine(55, 12, 95, 12, 3);
        addLine(95, 12, 95, 62, 3);
        if (!upper.empty()) addText(75, 40, upper, 18, "middle");
        if (!lower.empty()) addText(75, 55, lower, 18, "middle");
    } else {
        addLine(55, 40, 75, 12, 3);
        if (!upper.empty()) addText(45, 30, upper, 18, "end");
        if (!lower.empty()) addText(45, 55, lower, 18, "end");
    }

    svgStream << "</svg>\n";
    std::string svg = svgStream.str();
    
    QByteArray ba(svg.c_str(), svg.length());
    QGraphicsSvgItem* svgItem = new QGraphicsSvgItem();
    QSvgRenderer* renderer = new QSvgRenderer(ba, svgItem);
    svgItem->setSharedRenderer(renderer);
    symbolScene->addItem(svgItem);
    ui->graphicsView->fitInView(svgItem, Qt::KeepAspectRatio);
}

void TaskEdgeSymbol::updateFeature()
{
    if (!m_symbol) return;
    
    m_symbol->EdgeType.setValue(ui->cbEdgeType->currentText().toStdString());
    m_symbol->UpperDeviation.setValue(ui->leUpper->text().toStdString());
    m_symbol->LowerDeviation.setValue(ui->leLower->text().toStdString());
    m_symbol->Rotation.setValue(ui->leAngle->text().toDouble());
}

bool TaskEdgeSymbol::accept()
{
    if (m_createMode) {
        Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Create Edge Symbol"));
        
        App::Document *doc = Gui::Application::Instance->activeDocument()->getDocument();
        m_symbol = doc->addObject<TechDraw::DrawEdgeSymbol>("EdgeSymbol");
        
        m_symbol->Leader.setValue(m_leader);
        
        auto page = dynamic_cast<TechDraw::DrawPage *>(m_leader->findParentPage());
        if (!page) {
            page = TechDrawGui::DrawGuiUtil::findPage(nullptr);
        }
        
        if (page) {
            page->addView(m_symbol);
        } else {
            Base::Console().warning("EdgeSymbol: Could not find a suitable TechDraw Page to attach to.\n");
        }
        
        updateFeature();

        auto viewProvider = dynamic_cast<ViewProviderEdgeSymbol *>(QGIView::getViewProvider(m_symbol));
        if (viewProvider) {
            viewProvider->StackOrder.setValue(ZVALUE::DIMENSION);
        }

        Gui::Command::commitCommand();
        if (m_symbol) m_symbol->recomputeFeature();
        if (page) page->recomputeFeature();
    } else {
        Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Edit Edge Symbol"));
        updateFeature();
        Gui::Command::commitCommand();
        if (m_symbol) m_symbol->recomputeFeature();
    }
    
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.ActiveDocument.resetEdit()");
    return true;
}

bool TaskEdgeSymbol::reject()
{
    Gui::Command::doCommand(Gui::Command::Gui, "App.activeDocument().recompute()");
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.ActiveDocument.resetEdit()");
    return true;
}

TaskDlgEdgeSymbol::TaskDlgEdgeSymbol(TechDraw::DrawLeaderLine *leader)
    : TaskDialog()
{
    widget  = new TaskEdgeSymbol(leader);
    taskbox = new Gui::TaskView::TaskBox(Gui::BitmapFactory().pixmap("actions/TechDraw_EdgeSymbol"),
                                             widget->windowTitle(), true, nullptr);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
}

TaskDlgEdgeSymbol::TaskDlgEdgeSymbol(TechDraw::DrawEdgeSymbol *symbol)
    : TaskDialog()
{
    widget  = new TaskEdgeSymbol(symbol);
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
