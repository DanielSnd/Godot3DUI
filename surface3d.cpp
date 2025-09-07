#include "surface3d.h"

Vector<Surface3D*> Surface3D::_surfaces;


#ifdef TOOLS_ENABLED
bool Surface3D::has_gizmo_plugin = false;
#endif

Surface3D::Surface3D() {

}

Vector<Surface3D*> Surface3D::get_all_surfaces() {
    return _surfaces;
}

void Surface3D::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY) {
#ifdef TOOLS_ENABLED
            if (!has_gizmo_plugin && Engine::get_singleton()->is_editor_hint()) {
                has_gizmo_plugin = true;
                Ref<Surface3DGizmoPlugin> gizmo_plugin;
                gizmo_plugin.instantiate();
                Node3DEditor::get_singleton()->add_gizmo_plugin(gizmo_plugin);
            }
#endif
    }
    if (p_what == NOTIFICATION_ENTER_TREE) {
        _surfaces.push_back(this);
    } else if (p_what == NOTIFICATION_EXIT_TREE) {
        _surfaces.erase(this);
    }
    if (p_what == NOTIFICATION_TRANSFORM_CHANGED) {
#ifdef TOOLS_ENABLED
        update_gizmos();
#endif
    }
}

void Surface3D::_bind_methods() {
    // Bind the GDScript-callable methods
    ClassDB::bind_method(D_METHOD("raycast", "ray_origin", "ray_direction", "max_distance"), &Surface3D::_raycast, DEFVAL(INFINITY));
    ClassDB::bind_method(D_METHOD("closest_surface_point", "point", "max_distance"), &Surface3D::_closest_surface_point, DEFVAL(INFINITY));
    
    // Bind getters for hit results
    ClassDB::bind_method(D_METHOD("get_last_hit_point"), &Surface3D::get_last_hit_point);
    ClassDB::bind_method(D_METHOD("get_last_hit_normal"), &Surface3D::get_last_hit_normal);
    ClassDB::bind_method(D_METHOD("get_last_hit_distance"), &Surface3D::get_last_hit_distance);
    ClassDB::bind_method(D_METHOD("is_last_hit_valid"), &Surface3D::is_last_hit_valid);
}

bool Surface3D::_raycast(const Vector3& ray_origin, const Vector3& ray_direction, float max_distance) {
    Ray ray(ray_origin, ray_direction.normalized());
    bool result = raycast(ray, _last_hit, max_distance);
    return result;
}

bool Surface3D::_closest_surface_point(const Vector3& point, float max_distance) {
    bool result = closest_surface_point(point, _last_hit, max_distance);
    return result;
}

Vector<Vector3> Surface3D::_get_gizmo_lines() const {
    return get_gizmo_lines();
}

PlaneSurface3D::PlaneSurface3D() {
}

void PlaneSurface3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("project_point_to_plane", "point"), &PlaneSurface3D::project_point_to_plane);
    ClassDB::bind_method(D_METHOD("distance_to_plane", "point"), &PlaneSurface3D::distance_to_plane);
    ClassDB::bind_method(D_METHOD("is_point_on_plane_surface", "plane_point"), &PlaneSurface3D::is_point_on_plane_surface);
}

bool PlaneSurface3D::raycast(const Ray& ray, SurfaceHit& hit, float max_distance) {
    hit.valid = false;
    
    Vector3 normal = get_global_transform().basis.get_column(2);

    float denominator = normal.dot(ray.direction);
    if (Math::is_zero_approx(denominator)) {
        return false;
    }
    
    Vector3 to_plane = get_global_transform().origin - ray.origin;
    float t = to_plane.dot(normal) / denominator;
    
    if (t < 0.0f || t > max_distance) {
        return false;
    }
    
    Vector3 hit_point = ray.origin + ray.direction * t;
    
    if (!validate_plane_hit(hit_point)) {
        return false;
    }
    
    hit.point = hit_point;
    hit.normal = normal;
    hit.distance = t;
    hit.valid = true;
    return true;
}

bool PlaneSurface3D::closest_surface_point(const Vector3& point, SurfaceHit& hit, float max_distance) {
    hit.valid = false;
    
    Vector3 projected_point = project_point_to_plane(point);
    
    float distance = point.distance_to(projected_point);
    
    if (distance > max_distance) {
        return false;
    }
    
    if (!validate_plane_hit(projected_point)) {
        return false;
    }
    
    hit.point = projected_point;
    hit.normal = get_global_transform().basis.get_column(2);
    hit.distance = distance;
    hit.valid = true;
    return true;
}

bool PlaneSurface3D::is_point_on_plane_surface(const Vector3& plane_point) const {
    return validate_plane_hit(plane_point);
}

Vector3 PlaneSurface3D::project_point_to_plane(const Vector3& point) const {
    Vector3 normal = get_global_transform().basis.get_column(2).normalized();
    Vector3 to_point = point - get_global_transform().origin;
    float distance_to_plane = to_point.dot(normal);
    return point - normal * distance_to_plane;
}

float PlaneSurface3D::distance_to_plane(const Vector3& point) const {
    Vector3 normal = get_global_transform().basis.get_column(2).normalized();
    Vector3 to_point = point - get_global_transform().origin;
    return Math::abs(to_point.dot(normal));
}

bool PlaneSurface3D::validate_plane_hit(const Vector3& plane_point) const {
    return true;
}

void PlaneSurface3D::_notification(int p_what) {
    Surface3D::_notification(p_what);
}

LimitedPlaneSurface3D::LimitedPlaneSurface3D() : 
    _size(Vector2(1.0f, 1.0f)), 
    _radius(1.0f),
    _bounds_type(BOUNDS_RECTANGLE) {
}
 
void LimitedPlaneSurface3D::_bind_methods() {
    // Bind size properties
    ClassDB::bind_method(D_METHOD("set_size", "size"), &LimitedPlaneSurface3D::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &LimitedPlaneSurface3D::get_size);
 
    // Bind bounds radius
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &LimitedPlaneSurface3D::set_radius);
    ClassDB::bind_method(D_METHOD("get_radius"), &LimitedPlaneSurface3D::get_radius);

    // Bind bounds type
    ClassDB::bind_method(D_METHOD("set_bounds_type", "type"), &LimitedPlaneSurface3D::set_bounds_type);
    ClassDB::bind_method(D_METHOD("get_bounds_type"), &LimitedPlaneSurface3D::get_bounds_type);
 
    // Bind utility methods
    ClassDB::bind_method(D_METHOD("world_to_plane_coordinates", "world_point"), &LimitedPlaneSurface3D::world_to_plane_coordinates);
    ClassDB::bind_method(D_METHOD("plane_to_world_coordinates", "plane_coords"), &LimitedPlaneSurface3D::plane_to_world_coordinates);
    ClassDB::bind_method(D_METHOD("is_within_rectangular_bounds", "plane_coords"), &LimitedPlaneSurface3D::is_within_rectangular_bounds);
    ClassDB::bind_method(D_METHOD("is_within_circular_bounds", "plane_coords"), &LimitedPlaneSurface3D::is_within_circular_bounds);
    ClassDB::bind_method(D_METHOD("get_bounds_rect"), &LimitedPlaneSurface3D::get_bounds_rect);
    ClassDB::bind_method(D_METHOD("get_bounds_radius"), &LimitedPlaneSurface3D::get_bounds_radius);
 
    // Add properties
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bounds_type", PROPERTY_HINT_ENUM, "Rectangle,Circle"), "set_bounds_type", "get_bounds_type");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "size"), "set_size", "get_size");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
 
    // Bind enum
    BIND_ENUM_CONSTANT(BOUNDS_RECTANGLE);
    BIND_ENUM_CONSTANT(BOUNDS_CIRCLE);
}

void LimitedPlaneSurface3D::_notification(int p_what) {
    Surface3D::_notification(p_what);
}

void LimitedPlaneSurface3D::set_size(const Vector2& size) {
    _size = size;
    _validate_size();
#ifdef TOOLS_ENABLED
    update_gizmos();
#endif
}    

void LimitedPlaneSurface3D::set_radius(float radius) {
    _radius = radius;
    _validate_size();
#ifdef TOOLS_ENABLED
    update_gizmos();
#endif
}
 
void LimitedPlaneSurface3D::set_bounds_type(BoundsType type) {
    _bounds_type = type;
#ifdef TOOLS_ENABLED
    update_gizmos();
    notify_property_list_changed();
#endif
}
 
Vector2 LimitedPlaneSurface3D::world_to_plane_coordinates(const Vector3& world_point) const {
     // Project point to plane
    Vector3 plane_point = project_point_to_plane(world_point);
    
    // Get the plane's transform
    Transform3D transform = get_global_transform();
    
    // Calculate vector from plane's origin to the projected point
    Vector3 local_vec = plane_point - transform.origin;
    
    // Project onto the plane's basis vectors to get 2D coordinates
    float x = local_vec.dot(transform.basis.get_column(0));
    float y = local_vec.dot(transform.basis.get_column(1));
    
    return Vector2(x, y);
}
 
Vector3 LimitedPlaneSurface3D::plane_to_world_coordinates(const Vector2& plane_coords) const {
    return get_position() + get_transform().basis.get_column(0) * plane_coords.x + get_transform().basis.get_column(1) * plane_coords.y;
}
 
bool LimitedPlaneSurface3D::is_within_rectangular_bounds(const Vector2& plane_coords) const {
    float half_width = _size.x * 0.5f;
    float half_height = _size.y * 0.5f;
    return plane_coords.x >= -half_width && plane_coords.x <= half_width &&
           plane_coords.y >= -half_height && plane_coords.y <= half_height;
}
 
bool LimitedPlaneSurface3D::is_within_circular_bounds(const Vector2& plane_coords) const {
    float radius = get_bounds_radius();
    return plane_coords.length_squared() <= radius * radius;
}
 
Rect2 LimitedPlaneSurface3D::get_bounds_rect() const {
    return Rect2(-_size * 0.5f, _size);
}
 
float LimitedPlaneSurface3D::get_bounds_radius() const {
    return _radius;
}
 
bool LimitedPlaneSurface3D::validate_plane_hit(const Vector3& plane_point) const {
    // Get the plane's transform
    Transform3D transform = get_global_transform();
    
    // Convert world point to plane coordinates
    Vector2 plane_coords = world_to_plane_coordinates(plane_point);
    
    // Check bounds based on type
    switch (_bounds_type) {
        case BOUNDS_RECTANGLE:
            return is_within_rectangular_bounds(plane_coords);
        case BOUNDS_CIRCLE:
            return is_within_circular_bounds(plane_coords);
        default:
            return true;
    }
}
 
void LimitedPlaneSurface3D::_validate_size() {
    // Ensure size is positive
    _size.x = MAX(_size.x, 0.001f);
    _size.y = MAX(_size.y, 0.001f);
    _radius = MAX(_radius, 0.001f);
}

Vector<Vector3> PlaneSurface3D::get_gizmo_lines() const {
    Vector<Vector3> lines;
    lines.push_back(Vector3(0, 0, 0));
    lines.push_back(Vector3(1, 0, 0));
    lines.push_back(Vector3(1, 1, 0));
    lines.push_back(Vector3(0, 1, 0));
    lines.push_back(Vector3(0, 0, 0));
    lines.push_back(Vector3(0, 0, 0.25));
    return lines;
}

Vector<Vector3> LimitedPlaneSurface3D::get_gizmo_lines() const {
    Vector<Vector3> lines;
    
    if (_bounds_type == BOUNDS_RECTANGLE) {
        Vector2 half_size = _size * 0.5f;
        
        // Create rectangle corners in local space (X-Y plane)
        Vector3 corner1 = Vector3(-half_size.x, -half_size.y, 0); // Bottom-left
        Vector3 corner2 = Vector3( half_size.x, -half_size.y, 0); // Bottom-right
        Vector3 corner3 = Vector3( half_size.x,  half_size.y, 0); // Top-right
        Vector3 corner4 = Vector3(-half_size.x,  half_size.y, 0); // Top-left
        
        lines.push_back(corner1); lines.push_back(corner2); // Bottom edge
        lines.push_back(corner2); lines.push_back(corner3); // Right edge
        lines.push_back(corner3); lines.push_back(corner4); // Top edge
        lines.push_back(corner4); lines.push_back(corner1); // Left edge
        
        // Draw line from center forward
        lines.push_back(Vector3(0, 0, 0));
        lines.push_back(Vector3(0, 0, 0.25));
        
    } else if (_bounds_type == BOUNDS_CIRCLE) {
        // Circle bounds in local space (X-Y plane)
        float radius = get_bounds_radius();
        const int segments = 32; // Number of segments for circle approximation
        
        for (int i = 0; i < segments; i++) {
            float angle1 = (float)i / segments * Math::TAU;
            float angle2 = (float)(i + 1) / segments * Math::TAU;
            
            Vector3 point1 = Vector3(Math::cos(angle1) * radius, Math::sin(angle1) * radius, 0);
            Vector3 point2 = Vector3(Math::cos(angle2) * radius, Math::sin(angle2) * radius, 0);
            
            lines.push_back(point1);
            lines.push_back(point2);
        }

        lines.push_back(Vector3(0, 0, 0));
        lines.push_back(Vector3(0, 0, 0.25));
    }
    
    return lines;
}


void LimitedPlaneSurface3D::_validate_property(PropertyInfo &p_property) const {
    if (p_property.name == "size" && _bounds_type == BOUNDS_CIRCLE) {
        p_property.usage = PROPERTY_USAGE_NONE;
    }
    if (p_property.name == "radius" && _bounds_type != BOUNDS_CIRCLE) {
        p_property.usage = PROPERTY_USAGE_NONE;
    }
}
