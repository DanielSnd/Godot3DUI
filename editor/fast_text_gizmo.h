
#ifndef FAST_TEXT_GIZMO_PLUGIN_H
#define FAST_TEXT_GIZMO_PLUGIN_H

#include "editor/plugins/editor_plugin.h"
#include "editor/scene/3d/node_3d_editor_gizmos.h"

class FastTextGizmoPlugin : public EditorNode3DGizmoPlugin {
	GDCLASS(FastTextGizmoPlugin, EditorNode3DGizmoPlugin);

public:

	bool has_gizmo(Node3D *p_spatial) override;
	String get_gizmo_name() const override;
	int get_priority() const override;
	bool can_be_hidden() const override;
	void redraw(EditorNode3DGizmo *p_gizmo) override;

	FastTextGizmoPlugin();
};

#endif // FAST_TEXT_GIZMO_PLUGIN_H
