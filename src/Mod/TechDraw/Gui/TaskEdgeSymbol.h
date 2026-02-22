#ifndef TECHDRAWGUI_TASKEDGESYMBOL_H
#define TECHDRAWGUI_TASKEDGESYMBOL_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <QWidget>
#include <Base/Vector3D.h>
#include <sstream>
#include <memory>
#include <vector>

class QGraphicsScene;
class QGraphicsPixmapItem;

namespace App {
class DocumentObject;
}

namespace TechDrawGui
{
class Ui_TaskEdgeSymbol;

class SvgString
{
    std::stringstream svgStream;

public:
    SvgString(int width, int height);
    void addLine(double xStart, double yStart, double xEnd, double yEnd, double width=1.0);
    void addText(double xText, double yText, const std::string& text, double size=18.0, const std::string& anchor="middle");
    void addArrow(double xStart, double yStart, double xEnd, double yEnd);
    std::string finish();
};

class TaskEdgeSymbol : public QWidget
{
    Q_OBJECT

public:
    explicit TaskEdgeSymbol(const std::string &ownerName);
    ~TaskEdgeSymbol() override = default;

    virtual bool accept();
    virtual bool reject();

protected:
    void changeEvent(QEvent *event) override;

private:
    std::string buildSvg();
    void updatePreview();

    App::DocumentObject *owner;
    Base::Vector3d placement;
    
    QGraphicsScene* symbolScene;
    std::unique_ptr<Ui_TaskEdgeSymbol> ui;
    QColor getPenColor();

private Q_SLOTS:
    void onParametersChanged();

};

class TaskDlgEdgeSymbol : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgEdgeSymbol(const std::string &ownerName);
    ~TaskDlgEdgeSymbol() override;

    void open() override {}
    void clicked(int) override {}
    bool accept() override;
    bool reject() override;
    bool isAllowedAlterDocument() const override { return false; }

private:
    TaskEdgeSymbol* widget;
    Gui::TaskView::TaskBox* taskbox;
};

} // namespace TechDrawGui

#endif // TECHDRAWGUI_TASKEDGESYMBOL_H
