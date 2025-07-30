#pragma once

#include "core/object/ref_counted.h"

class Control3D : public RefCounted {
	GDCLASS(Control3D, RefCounted);

protected:
	static void _bind_methods();

public:
	Control3D();
};
