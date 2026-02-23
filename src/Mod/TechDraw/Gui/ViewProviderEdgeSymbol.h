#ifndef TechDrawGui_ViewProviderEdgeSymbol_h_
#define TechDrawGui_ViewProviderEdgeSymbol_h_

#include <QMenu>
#include "ViewProviderSymbol.h"

namespace TechDrawGui {

class TechDrawGuiExport ViewProviderEdgeSymbol : public ViewProviderSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderEdgeSymbol);

public:
    ViewProviderEdgeSymbol();
    ~ViewProviderEdgeSymbol() override = default;

    void updateData(const App::Property* prop) override;
    
    bool setEdit(int ModNum) override;
    bool doubleClicked() override;

protected:
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
};

} // namespace TechDrawGui
#endif
