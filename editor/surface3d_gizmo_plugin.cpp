
#ifdef TOOLS_ENABLED

#include "surface3d_gizmo_plugin.h"
#include "editor/editor_node.h"
#include "editor/settings/editor_settings.h"
#include "scene/3d/mesh_instance_3d.h"

bool Surface3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
    return Object::cast_to<Surface3D>(p_spatial) != nullptr;
}

String Surface3DGizmoPlugin::get_gizmo_name() const {
    return "Surface3D";
}

int Surface3DGizmoPlugin::get_priority() const {
    return -1;
}

void Surface3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
    Surface3D *surface = Object::cast_to<Surface3D>(p_gizmo->get_node_3d());
    if (!surface) {
        return;
    }

    p_gizmo->clear();

    // Get the container size
    Vector<Vector3> lines = surface->get_gizmo_lines();

	Ref<Material> material = get_material("main_material", p_gizmo);

    p_gizmo->add_lines(lines, material, false, Color(0.8, 0.8, 0.8, 0.5));
	p_gizmo->add_collision_segments(lines);
}

Surface3DGizmoPlugin::Surface3DGizmoPlugin() {
    create_material("main_material", Color(0.8, 0.8, 0.8, 0.5));
}

#endif