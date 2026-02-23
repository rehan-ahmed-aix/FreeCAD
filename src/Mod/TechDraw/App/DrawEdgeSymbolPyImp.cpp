#include "PreCompiled.h"
#include "DrawEdgeSymbol.h"
#include <Mod/TechDraw/App/DrawEdgeSymbolPy.h>
#include <Mod/TechDraw/App/DrawEdgeSymbolPy.cpp>

using namespace TechDraw;

std::string DrawEdgeSymbolPy::representation() const
{
    return std::string("<DrawEdgeSymbol object>");
}

PyObject *DrawEdgeSymbolPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int DrawEdgeSymbolPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
