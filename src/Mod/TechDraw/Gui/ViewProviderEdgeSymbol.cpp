#include "PreCompiled.h"

#include <Gui/Control.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <QMessageBox>

#include <Mod/TechDraw/App/DrawEdgeSymbol.h>

#include "TaskEdgeSymbol.h"
#include "ViewProviderEdgeSymbol.h"

using namespace TechDrawGui;
using namespace TechDraw;

PROPERTY_SOURCE(TechDrawGui::ViewProviderEdgeSymbol, TechDrawGui::ViewProviderSymbol)

ViewProviderEdgeSymbol::ViewProviderEdgeSymbol()
{
    sPixmap = "TechDraw_EdgeSymbol";
}

void ViewProviderEdgeSymbol::updateData(const App::Property* prop)
{
    ViewProviderSymbol::updateData(prop);
}

bool ViewProviderEdgeSymbol::setEdit(int ModNum)
{
    if (ModNum != ViewProvider::Default ) {
        return ViewProviderSymbol::setEdit(ModNum);
    }
    
    Gui::TaskView::TaskDialog *dlg = Gui::Control().activeDialog();
    if (dlg) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Task in progress"),
            QObject::tr("Close active task dialog and try again"));
        return true;
    }

    auto feature = dynamic_cast<TechDraw::DrawEdgeSymbol*>(getViewObject());
    if (feature) {
        Gui::Control().showDialog(new TechDrawGui::TaskDlgEdgeSymbol(feature));
    }
    
    return true;
}

bool ViewProviderEdgeSymbol::doubleClicked()
{
    setEdit(ViewProvider::Default);
    return true;
}

void ViewProviderEdgeSymbol::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    ViewProviderSymbol::setupContextMenu(menu, receiver, member);
}
