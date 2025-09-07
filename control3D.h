#pragma once

#include "scene/3d/visual_instance_3d.h"

class Control3D : public GeometryInstance3D {
	GDCLASS(Control3D, GeometryInstance3D);

protected:
	static void _bind_methods();

public:
	Control3D();
};
