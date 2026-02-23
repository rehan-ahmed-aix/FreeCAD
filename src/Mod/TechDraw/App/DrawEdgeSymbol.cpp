#include "PreCompiled.h"
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Base/Console.h>
#include "DrawEdgeSymbol.h"
#include "DrawEdgeSymbolPy.h"
#include "DrawLeaderLine.h"
#include <sstream>

using namespace TechDraw;

PROPERTY_SOURCE(TechDraw::DrawEdgeSymbol, TechDraw::DrawViewSymbol)

DrawEdgeSymbol::DrawEdgeSymbol()
{
    static const char *group = "Edge Symbol";

    ADD_PROPERTY_TYPE(Leader, (nullptr), group, (App::PropertyType)(App::Prop_None), "Parent Leader");
    ADD_PROPERTY_TYPE(EdgeType, ("Undefined"), group, App::Prop_None, "Type of edge (Undefined, External, Internal)");
    ADD_PROPERTY_TYPE(UpperDeviation, (""), group, App::Prop_None, "Upper deviation text");
    ADD_PROPERTY_TYPE(LowerDeviation, (""), group, App::Prop_None, "Lower deviation text");

    Caption.setStatus(App::Property::Hidden, true);
    Scale.setStatus(App::Property::Hidden, true);
    ScaleType.setStatus(App::Property::Hidden, true);
    Rotation.setStatus(App::Property::Hidden, true);
    X.setStatus(App::Property::Hidden, true);
    Y.setStatus(App::Property::Hidden, true);
    Symbol.setStatus(App::Property::ReadOnly, true);
}

short DrawEdgeSymbol::mustExecute() const
{
    if (Leader.isTouched() || EdgeType.isTouched() || UpperDeviation.isTouched() || LowerDeviation.isTouched()) {
        return 1;
    }
    return DrawViewSymbol::mustExecute();
}

App::DocumentObjectExecReturn *DrawEdgeSymbol::execute()
{
    std::string svg = buildSvg();
    Symbol.setValue(svg);
    
    auto leader = dynamic_cast<TechDraw::DrawLeaderLine*>(Leader.getValue());
    if (leader) {
        Base::Vector3d tail = leader->getTailPoint();
        X.setValue(tail.x);
        Y.setValue(tail.y);
    }
    
    return DrawViewSymbol::execute();
}

void DrawEdgeSymbol::onChanged(const App::Property* prop)
{
    if (prop == &Leader || prop == &EdgeType || prop == &UpperDeviation || prop == &LowerDeviation) {
        requestPaint();
    }
    DrawViewSymbol::onChanged(prop);
}

std::string DrawEdgeSymbol::buildSvg()
{
    std::stringstream svgStream;
    svgStream << "<?xml version='1.0'?>\n";
    svgStream << "<svg width='100' height='100'>\n";
    
    std::string type = EdgeType.getValue();
    std::string upper = UpperDeviation.getValue();
    std::string lower = LowerDeviation.getValue();

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
    return svgStream.str();
}

PyObject *DrawEdgeSymbol::getPyObject()
{
    if (PythonObject.is(Py::_None())) {
        PythonObject = Py::Object(new DrawEdgeSymbolPy(this), true);
    }
    return Py::new_reference_to(PythonObject);
}

namespace App {
PROPERTY_SOURCE_TEMPLATE(TechDraw::DrawEdgeSymbolPython, TechDraw::DrawEdgeSymbol)
template<> const char* TechDraw::DrawEdgeSymbolPython::getViewProviderName() const {
    return "TechDrawGui::ViewProviderEdgeSymbol";
}

template class TechDrawExport FeaturePythonT<TechDraw::DrawEdgeSymbol>;
}
