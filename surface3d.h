#pragma once

#include "scene/3d/node_3d.h"
#include "core/math/vector3.h"
#include "core/math/math_funcs.h"

#ifdef TOOLS_ENABLED
#include "editor/settings/editor_settings.h"
#include "editor/scene/3d/node_3d_editor_plugin.h"
#include "editor/surface3d_gizmo_plugin.h"
#endif

class Surface3D : public Node3D {
    GDCLASS(Surface3D, Node3D);

protected:
    static void _bind_methods();
    void _notification(int p_what);
    
    static Vector<Surface3D*> _surfaces;

#ifdef TOOLS_ENABLED
    static bool has_gizmo_plugin;
#endif

public:
    Surface3D();

    struct Ray {
        Vector3 origin;
        Vector3 direction;
        
        Ray() {}
        Ray(const Vector3& p_origin, const Vector3& p_direction) : origin(p_origin), direction(p_direction) {}
    };

    struct SurfaceHit {
        Vector3 point;
        Vector3 normal;
        float distance;
        bool valid;
        SurfaceHit() : distance(0.0f), valid(false) {}
    };

    // Pure virtual methods that must be implemented by derived classes
    virtual bool raycast(const Ray& ray, SurfaceHit& hit, float max_distance = INFINITY) = 0;
    virtual bool closest_surface_point(const Vector3& point, SurfaceHit& hit, float max_distance = INFINITY) = 0;

    Vector<Vector3> _get_gizmo_lines() const;
    bool _raycast(const Vector3& ray_origin, const Vector3& ray_direction, float max_distance = INFINITY);
    bool _closest_surface_point(const Vector3& point, float max_distance = INFINITY);
    
    // Getters for the last hit result
    Vector3 get_last_hit_point() const { return _last_hit.point; }
    Vector3 get_last_hit_normal() const { return _last_hit.normal; }
    float get_last_hit_distance() const { return _last_hit.distance; }
    bool is_last_hit_valid() const { return _last_hit.valid; }

    static Vector<Surface3D*> get_all_surfaces();

    virtual Vector<Vector3> get_gizmo_lines() const = 0;

private:
    SurfaceHit _last_hit;
};


class PlaneSurface3D : public Surface3D {
    GDCLASS(PlaneSurface3D, Surface3D);
 
protected:
    static void _bind_methods();
    void _notification(int p_what);
 
public:
    PlaneSurface3D();
    virtual ~PlaneSurface3D() = default;
 
    // Surface3D interface implementation
    virtual bool raycast(const Ray& ray, SurfaceHit& hit, float max_distance = INFINITY) override;
    virtual bool closest_surface_point(const Vector3& point, SurfaceHit& hit, float max_distance = INFINITY) override;
 
    // Utility methods (useful for derived classes)
    virtual bool is_point_on_plane_surface(const Vector3& plane_point) const;
    Vector3 project_point_to_plane(const Vector3& point) const;
    float distance_to_plane(const Vector3& point) const;
    virtual Vector<Vector3> get_gizmo_lines() const override;
 
protected:
    // Helper method for derived classes to override hit validation
    virtual bool validate_plane_hit(const Vector3& plane_point) const;
};


class LimitedPlaneSurface3D : public PlaneSurface3D {
    GDCLASS(LimitedPlaneSurface3D, PlaneSurface3D);
 
public:
    enum BoundsType {
        BOUNDS_RECTANGLE,
        BOUNDS_CIRCLE
    };
 
protected:
    static void _bind_methods();
    void _notification(int p_what);
    Vector2 _size;
    float _radius;
    BoundsType _bounds_type;
 
public:
    LimitedPlaneSurface3D();
    virtual ~LimitedPlaneSurface3D() = default;
 
    // Size properties
    void set_size(const Vector2& size);
    Vector2 get_size() const { return _size; }
    void set_radius( float p_radius );
    float get_radius() const { return _radius; }
 
    // Bounds type
    void set_bounds_type(BoundsType type);
    BoundsType get_bounds_type() const { return _bounds_type; }
    
    const Vector3 world_up = Vector3(0, 1, 0);
    const Vector3 world_right = Vector3(1, 0, 0);
    const Vector3 world_forward = Vector3(0, 0, 1);

    // Utility methods
    Vector2 world_to_plane_coordinates(const Vector3& world_point) const;
    Vector3 plane_to_world_coordinates(const Vector2& plane_coords) const;
    bool is_within_rectangular_bounds(const Vector2& plane_coords) const;
    bool is_within_circular_bounds(const Vector2& plane_coords) const;
 

    // Get bounds information
    Rect2 get_bounds_rect() const;
    float get_bounds_radius() const;
    virtual Vector<Vector3> get_gizmo_lines() const override;
 
protected:
    virtual bool validate_plane_hit(const Vector3& plane_point) const override;
    void _validate_property(PropertyInfo &p_property) const;
 
private:
    void _validate_size();
};
 
VARIANT_ENUM_CAST(LimitedPlaneSurface3D::BoundsType);