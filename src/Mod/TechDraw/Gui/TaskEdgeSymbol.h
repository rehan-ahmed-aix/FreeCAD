#ifndef TECHDRAWGUI_TASKEDGESYMBOL_H
#define TECHDRAWGUI_TASKEDGESYMBOL_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <QWidget>
#include <QGraphicsScene>
#include <memory>

namespace TechDraw {
    class DrawEdgeSymbol;
    class DrawLeaderLine;
}

namespace TechDrawGui
{
class Ui_TaskEdgeSymbol;

class TaskEdgeSymbol : public QWidget
{
    Q_OBJECT

public:
    explicit TaskEdgeSymbol(TechDraw::DrawLeaderLine *leader);
    explicit TaskEdgeSymbol(TechDraw::DrawEdgeSymbol *symbol);
    ~TaskEdgeSymbol() override = default;

    virtual bool accept();
    virtual bool reject();

protected:
    void changeEvent(QEvent *event) override;

private:
    void init();
    void updatePreview();
    void updateFeature();

    TechDraw::DrawEdgeSymbol* m_symbol;
    TechDraw::DrawLeaderLine* m_leader;
    bool m_createMode;
    
    QGraphicsScene* symbolScene;
    std::unique_ptr<Ui_TaskEdgeSymbol> ui;

private Q_SLOTS:
    void onParametersChanged();

};

class TaskDlgEdgeSymbol : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgEdgeSymbol(TechDraw::DrawLeaderLine *leader);
    explicit TaskDlgEdgeSymbol(TechDraw::DrawEdgeSymbol *symbol);
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
