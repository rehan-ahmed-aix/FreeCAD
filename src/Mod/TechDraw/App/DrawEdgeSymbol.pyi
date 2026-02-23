# SPDX-License-Identifier: LGPL-2.1-or-later

from __future__ import annotations

from Base.Metadata import export
from TechDraw.DrawViewSymbol import DrawViewSymbol


@export(
    Include="Mod/TechDraw/App/DrawEdgeSymbol.h",
    Namespace="TechDraw",
    FatherInclude="Mod/TechDraw/App/DrawViewSymbolPy.h",
)
class DrawEdgeSymbol(DrawViewSymbol):
    """
    Feature for adding edge symbols to leader lines

    Author: FreeCAD community
    License: LGPL-2.1-or-later
    """
