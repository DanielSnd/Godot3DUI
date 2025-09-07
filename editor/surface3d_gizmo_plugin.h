#ifndef SURFACE_3D_GIZMO_PLUGIN_H
#define SURFACE_3D_GIZMO_PLUGIN_H


#ifdef TOOLS_ENABLED
#include "editor/scene/3d/node_3d_editor_gizmos.h"
#include "../surface3d.h"

class Surface3DGizmoPlugin : public EditorNode3DGizmoPlugin {
    GDCLASS(Surface3DGizmoPlugin, EditorNode3DGizmoPlugin);

public:
    bool has_gizmo(Node3D *p_spatial) override;
    String get_gizmo_name() const override;
    int get_priority() const override;
    void redraw(EditorNode3DGizmo *p_gizmo) override;
    Surface3DGizmoPlugin();
};
#endif

#endif // SURFACE_3D_GIZMO_PLUGIN_H
