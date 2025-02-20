//
//  CRayTrace.cpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#include "rt.h"


/// \fn compPrimaryRayMatrix(CCamera cam, glm::mat3& m)
/// \brief Computation of the projection matrix.
/// \param cam Camera parameters.
/// \param m Output projection matrix.
///
bool CRayTrace::compPrimaryRayMatrix(const CCamera& cam, glm::mat3& m) {

    auto look = cam.lookp - cam.eyep;
    glm::vec3 u = glm::cross(cam.up, look)/glm::length(glm::cross(cam.up, look));

    glm::vec3 v = glm::cross(u, look)/glm::length(glm::cross(u, look));


    auto h1 = look/glm::length(look);

    auto angle = (cam.fov/2) * 3.14f / 180.0f;
    auto h2 = cam.width / (2*glm::tan(angle));
    auto h3 = (float(cam.width/2)) * u;
    auto h4 = (float(cam.height/2)) * v;

    glm::vec3 o = h1 * h2 - ( h3 + h4);

    m = {u,v,o};

    return true;
}
bool CRayTrace::rayTrace9(const CScene& scene, CRay& ray, COutput& out) {
    CObject* obj;
    float tmin = FLT_MAX;
    float EPS = 0.0001;
    bool is_intersection = false;
    for(auto& object: scene.objectList) {
        float t = object->intersect(ray);
        if(t > EPS and t < tmin) {
            tmin = t;
            is_intersection = true;
        }
    }
    if(is_intersection) { out.col = {0,0.5,0};}
    else { out.col = {0, 0, 0};}

    return true;
}
bool CRayTrace::rayTrace12(const CScene& scene, CRay& ray, COutput& out) {
    CObject* hit_obj;
    float tmin = FLT_MAX;
    float EPS = 0.0001;
    bool is_intersection = false;
    for(auto& object: scene.objectList) {
        float t = object->intersect(ray);
        if(t > EPS and t < tmin) {
            tmin = t;
            hit_obj = object;
            is_intersection = true;
        }
    }
    if (is_intersection == false) { return is_intersection; }

    //compute position of the intersection point p using the ray equation
    auto p = ray.pos + tmin * ray.dir;

    for(auto& light: scene.lightList) {
        //compute and add ambient color component
        out.col = out.col + light.color * hit_obj->matAmbient;

        //compute normal vector at p
        auto n = hit_obj->normal(p);

        //compute vector from p to light position
        auto L = glm::normalize(light.pos - p);

        //compute angle between n and vector to light
        auto cos_angle = glm::dot(n, L);

        if(cos_angle > 0.001) {
            //compute and add diffuse color component
            out.col = out.col + light.color * hit_obj->matDiffuse * cos_angle;

            //compute half angle vector h
            auto h = glm::normalize(L + (-(ray.dir)));

            //compute angle between n and h
            auto cos_beta = glm::dot(n, h);

            if(cos_beta > 0.001) {
                //compute and add specular color component
                out.col = out.col + light.color * hit_obj->matSpecular * glm::pow(cos_beta,hit_obj->matShininess);
            }
        }

    }

    return is_intersection;
}
bool CRayTrace::rayTrace13(const CScene& scene, CRay& ray, COutput& out){
    CObject* hit_obj;
    float tmin = FLT_MAX;
    float EPS = 0.0001;
    bool is_intersection = false;
    for(auto& object: scene.objectList) {
        float t = object->intersect(ray);
        if(t > EPS and t < tmin) {
            tmin = t;
            hit_obj = object;
            is_intersection = true;
        }
    }
    if (is_intersection == false) { return is_intersection; }

    //compute position of the intersection point p using the ray equation
    auto p = ray.pos + tmin * ray.dir;

    for(auto& light: scene.lightList) {
        //compute and add ambient color component
        out.col = out.col + light.color * hit_obj->matAmbient;

        //compute normal vector at p
        auto n = hit_obj->normal(p);

        //compute vector from p to light position
        auto L = glm::normalize(light.pos - p);

        //compute angle between n and vector to light
        auto cos_angle = glm::dot(n, L);

        if(cos_angle > 0.001) {
            //compute and add diffuse color component
            out.col = out.col + light.color * hit_obj->matDiffuse * cos_angle;

            //compute half angle vector h
            auto h = glm::normalize(L + (-(ray.dir)));

            //compute angle between n and h
            auto cos_beta = glm::dot(n, h);

            if(cos_beta > 0.001) {
                //compute and add specular color component
                out.col = out.col + light.color * hit_obj->matSpecular * glm::pow(cos_beta,hit_obj->matShininess);
            }
        }
        if(hit_obj->isTexture) {
            glm::vec2 uv = hit_obj->textureMapping(n);
            glm::vec3 tex_col = CImage::getTexel(hit_obj->texture, uv.x, uv.y);
            out.col = out.col * tex_col;
        }

    }

    return is_intersection;
}
bool CRayTrace::rayTrace14(const CScene& scene, CRay& ray, COutput& out) {
    CObject* hit_obj;
    float tmin = FLT_MAX;
    float EPS = 0.0001;
    bool is_intersection = false;
    for(auto& object: scene.objectList) {
        float t = object->intersect(ray);
        if(t > EPS and t < tmin) {
            tmin = t;
            hit_obj = object;
            is_intersection = true;
        }
    }
    if (is_intersection == false) { return is_intersection; }

    //compute position of the intersection point p using the ray equation
    auto p = ray.pos + tmin * ray.dir;
    CRay shadow_ray;

    for(auto& light: scene.lightList) {
        //compute and add ambient color component
        out.col = out.col + light.color * hit_obj->matAmbient;

        //compute normalized shadow ray from the intersection point towards the light source

        shadow_ray.pos = p;
        shadow_ray.dir = glm::normalize(light.pos - p);

        //for each object in the scene search for intersection with the shadow ray
        bool shadow_intersection = false;
        for(auto& object: scene.objectList) {
            float t = object->intersect(shadow_ray);
            if(t > EPS) {
                shadow_intersection = true;
                break;
            }
        }
        if(shadow_intersection) { continue;}

        //compute normal vector at p
        auto n = hit_obj->normal(p);

        //compute vector from p to light position
        auto L = glm::normalize(light.pos - p);

        //compute angle between n and vector to light
        auto cos_angle = glm::dot(n, L);

        if(cos_angle > 0.001) {
            //compute and add diffuse color component
            out.col = out.col + light.color * hit_obj->matDiffuse * cos_angle;

            //compute half angle vector h
            auto h = glm::normalize(L + (-(ray.dir)));

            //compute angle between n and h
            auto cos_beta = glm::dot(n, h);

            if(cos_beta > 0.001) {
                //compute and add specular color component
                out.col = out.col + light.color * hit_obj->matSpecular * glm::pow(cos_beta,hit_obj->matShininess);
            }
        }
        if(hit_obj->isTexture) {
            glm::vec2 uv = hit_obj->textureMapping(n);
            glm::vec3 tex_col = CImage::getTexel(hit_obj->texture, uv.x, uv.y);
            out.col = out.col * tex_col;
        }

    }

    return is_intersection;
}
bool CRayTrace::rayTrace16(const CScene& scene, CRay& ray, COutput& out) {
    CObject* hit_obj;
    int MAX_RAY_TREE = 1;
    float MIN_RAY_ENERGY = 0.01;
    float tmin = FLT_MAX;
    float EPS = 0.0001;
    bool is_intersection = false;
    for(auto& object: scene.objectList) {
        float t = object->intersect(ray);
        if(t > EPS and t < tmin) {
            tmin = t;
            hit_obj = object;
            is_intersection = true;
        }
    }
    if (is_intersection == false) { return is_intersection; }

    //compute position of the intersection point p using the ray equation
    auto p = ray.pos + tmin * ray.dir;
    CRay shadow_ray;

    for(auto& light: scene.lightList) {
        //compute and add ambient color component
        out.col = out.col + out.energy * light.color * hit_obj->matAmbient;

        //compute normalized shadow ray from the intersection point towards the light source

        shadow_ray.pos = p;
        shadow_ray.dir = glm::normalize(light.pos - p);

        //for each object in the scene search for intersection with the shadow ray
        bool shadow_intersection = false;
        for(auto& object: scene.objectList) {
            float t = object->intersect(shadow_ray);
            if(t > EPS) {
                shadow_intersection = true;
                break;
            }
        }
        if(shadow_intersection) { continue; }

        //compute normal vector at p
        auto n = hit_obj->normal(p);

        //compute vector from p to light position
        auto L = glm::normalize(light.pos - p);

        //compute angle between n and vector to light
        auto cos_angle = glm::dot(n, L);

        if(cos_angle > 0.001) {
            //compute and add diffuse color component
            out.col = out.col + out.energy * light.color * hit_obj->matDiffuse * cos_angle;

            //compute half angle vector h
            auto h = glm::normalize(L + (-(ray.dir)));

            //compute angle between n and h
            auto cos_beta = glm::dot(n, h);

            if(cos_beta > 0.001) {
                //compute and add specular color component
                out.col = out.col + out.energy * light.color * hit_obj->matSpecular * glm::pow(cos_beta,hit_obj->matShininess);
            }
        }
        if(hit_obj->reflectance > 0 and out.tree < MAX_RAY_TREE and out.energy > MIN_RAY_ENERGY) {
            //increments ray tree counter
            out.tree++;
            //decreases the energy
            out.energy = out.energy * hit_obj->reflectance;
            CRay secondary_ray;

            //computes secondary ray
            secondary_ray = reflectedRay(ray, n, p);
            rayTrace16(scene, secondary_ray, out);
        }




        if(hit_obj->isTexture) {
            glm::vec2 uv = hit_obj->textureMapping(n);
            glm::vec3 tex_col = CImage::getTexel(hit_obj->texture, uv.x, uv.y);
            out.col = out.col * tex_col;
        }

    }

    return is_intersection;
}
/// \fn rayTrace(CScene scene, CRay& ray, COutput& out)
/// \brief Traces single ray.
/// \param scene Object with all scene components including a camera.
/// \param ray Ray parameter (primary and secondary rays).
/// \param out Object with output color and parameters used in recursion.
bool CRayTrace::rayTrace(const CScene& scene, CRay& ray, COutput& out) {
    std::vector<float> outs1;
    float tmin = FLT_MAX;
    int MAX_RAY_TREE = 1;
    float MIN_RAY_ENERGY = 0.01;
    float EPS = 0.0001;
    bool is_intersection = false;
    CObject* hit_obj = nullptr;

    // compute t h e c l o s e s t i n t e r s e c t i o n , s t o r e d i s t a n c e ( t_min ) and r e f e r e n c e t o
    // c l o s e s t o b j e c t ( h i t _ o b j )

    for (auto & obj : scene.objectList) {
        float t = obj->intersect(ray);
        if(t > EPS && t < tmin) {
            tmin = t;
            is_intersection = true;
            hit_obj = obj;
        }
    }

    /// returns false if there are no intersections
    if(is_intersection) {
       // out.col = {0,0.5,0};
    }
    if(!is_intersection) {
       // out.col = {0,0.0,0};
        return false;
    }

    /// // compute position of the intersection point p using the ray equation
    auto p = ray.pos + tmin * ray.dir;

    /// for each light source defined in the scene
    for (auto & light : scene.lightList) {
        ///// compute and add am b i e n t c o l o r component
        out.col = out.col + out.energy * light.color * hit_obj->matAmbient;

        CRay shadow_ray;
        shadow_ray.pos = p;
        shadow_ray.dir = glm::normalize(light.pos - p);
        bool is_shadow_intersection = false;

        /// for each object in the scene search for an intersection with the shadow ray
        for (auto & obj : scene.objectList ) {
            float t  = obj->intersect(shadow_ray);
            if( t > 0) {
                is_shadow_intersection = true;
                break;
            }
        }

        if(is_shadow_intersection) { continue; }
    //// compute d i f f u s e and s p r e c u l a r c o l o r c om p o ne n t s b a s e d on i l l u m i n a t i o n e q u a t i o n
    ///
        /// computes normal vector at intersection point
        auto n = hit_obj->normal(p);
        // compute v e c t o r f rom p t o l i g h t p o s i t i o n
        auto L = glm::normalize(light.pos - p);
        // compute a n g l e be twee n n and v e c t o r t o l i g h t
        auto cos_angle = glm::dot(n, L);
        if (cos_angle > 0.001) {
            /// compute and add d i f f u s e c o l o r component
            out.col = out.col + out.energy * (light.color * hit_obj->matDiffuse * cos_angle);
            /// compute h a l f a n g l e v e c t o r h
            auto h = glm::normalize(L + (-ray.dir));
            // compute a n g l e be twee n n and h
            auto cos_beta = glm::dot(n, h);
            if (cos_beta > 0.001) {
                // compute and add s p e c u l a r c o l o r component
                out.col = out.col + out.energy * (light.color * hit_obj->matSpecular * glm::pow(cos_beta, hit_obj->matShininess));
            }

        }



        /// adds texture for textured spheres
        if(hit_obj->isTexture) {
            glm::vec2 uv = hit_obj->textureMapping(n);
            glm::vec3 tex_col = CImage::getTexel(hit_obj->texture, uv.x, uv.y);
            out.col = out.col * tex_col;
        }




    }

    /// if the surface is reflective
    if (hit_obj->reflectance > 0 and out.tree < MAX_RAY_TREE and out.energy > MIN_RAY_ENERGY) {
        out.tree++;
        out.energy = out.energy * hit_obj->reflectance;

        /// computes the secondary ray parameters (reflected ray)
        auto n = hit_obj->normal(p);
        CRay secondary_ray = reflectedRay(ray,n, p);
        /// recursion
        rayTrace(scene, secondary_ray, out);
    }

    return true;
}


/// \fn reflectedRay(CRay ray, glm::vec3 n, glm::vec3 pos)
/// \brief Computes parameters of the ray reflected at the surface point with given normal vector.
/// \param ray Input ray.
/// \param n Surface normal vector.
/// \param pos Position of reflection point.
/// \return Reflected ray.
///
CRay CRayTrace::reflectedRay(const CRay& ray, const glm::vec3& n, const glm::vec3& pos) {
    CRay reflected_ray;
    auto v = ray.dir;
    auto r = v - (2 * glm::dot(v,n)) * n;
    reflected_ray.dir = glm::normalize(r);
    reflected_ray.pos = pos;

    return reflected_ray;
}

