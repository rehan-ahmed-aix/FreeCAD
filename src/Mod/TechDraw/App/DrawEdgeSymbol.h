#ifndef TechDraw_DrawEdgeSymbol_h_
#define TechDraw_DrawEdgeSymbol_h_

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>

#include "DrawViewSymbol.h"


namespace TechDraw
{

class TechDrawExport DrawEdgeSymbol : public TechDraw::DrawViewSymbol
{
    PROPERTY_HEADER_WITH_OVERRIDE(TechDraw::DrawEdgeSymbol);

public:
    DrawEdgeSymbol();
    ~DrawEdgeSymbol() override = default;

    App::PropertyLink         Leader;
    App::PropertyString       EdgeType;
    App::PropertyString       UpperDeviation;
    App::PropertyString       LowerDeviation;

    short mustExecute() const override;
    App::DocumentObjectExecReturn *execute() override;

    const char* getViewProviderName() const override {
        return "TechDrawGui::ViewProviderEdgeSymbol";
    }
    PyObject *getPyObject() override;

    App::PropertyLink *getOwnerProperty() override { return &Leader; }

protected:
    void onChanged(const App::Property* prop) override;
    std::string buildSvg();
};

using DrawEdgeSymbolPython = App::FeaturePythonT<DrawEdgeSymbol>;

} //namespace TechDraw
#endif
