//
//  CObject.cpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#include "rt.h"
#include "CObject.hpp"


/// \fn intersect(CRay ray)
/// \brief Computes intersection between ray and sphere.
/// \param ray Ray parameters.
/// \return Distance from camera position to the closest intersection point, or negative value.
///
float CSphere::intersect(const CRay& ray) {
    float t = -1;

    auto v = ray.pos - pos;

    auto A = glm::dot(ray.dir, ray.dir);
    auto B = glm::dot((2.0f * v), ray.dir);
    auto C = glm::dot(v, v) - pow(r,2);

    auto delta = pow(B,2) - 4 * A * C;
    float t1 = (-B + sqrt(delta))/(2 * A);
    float t2 = (-B - sqrt(delta))/(2 * A);
    if(t1 > 0 && t2 > 0) { return std::min(t1, t2); }
    if( t1 > 0) { return t1; }
    if( t2 > 0) { return t2; }
    return t;
}


/// Normal vector to the sphere surface
/// \fn normal(glm::vec3 hit_pos)
/// \brief Surface normal vector at the intersection point.
/// \param hit_pos Intersection point.
/// \return Normal vector parameters.
///
glm::vec3 CSphere::normal(const glm::vec3& hit_pos) {
    auto n = glm::normalize(hit_pos - pos);
    return n;
}


/// Computes texture mapping coordinates (u,v).
/// \param normal_vec Normalized normal vector at intersection point.
/// \return (u,v) texture coordinates in <0,1> range.
glm::vec2 CSphere::textureMapping(const glm::vec3& normal_vec) {
    glm::vec2 uv = {0,0};
    uv.x = 0.5 + glm::asin(normal_vec.x)/glm::radians(180.00);
    uv.y  = 0.5 - glm::asin(normal_vec.y)/glm::radians(180.00);
    return uv;
}



/// \fn intersect(CRay ray)
/// \brief Computes intersection between triangle and sphere.
/// \param ray Ray parameters.
/// \return Distance from camera position to the closest intersection point, or negative value.
///
float CTriangle::intersect(const CRay& ray) {
    float t = -1;
    glm::vec3 bary;
    bool intersect = glm::intersectRayTriangle(ray.pos, ray.dir, v0,v1,v2, bary);
    bary.z = 1.0f - bary.x - bary.y;
  //  std::string out;
  //  PRINT_VEC3(out,bary);
  //  std::cout<<out<<std::endl;

 //   std::cout<< " JEST/NIE MA " << intersect << std::endl;
    if (!intersect) { return t; }

    //wersja alternatywna
    glm::vec3 cartesian = bary.x * v0 + bary.y * v1 + bary.z * v2;
    t = glm::distance(cartesian, ray.pos);


    return t;

}

/*
float CTriangle::intersect(const CRay& ray) {
    float t = -1;  // Domyślna wartość, gdy nie ma przecięcia
    glm::vec3 barycentric;  // Zmienna do przechowywania współrzędnych barycentrycznych

    // Funkcja glm::intersectRayTriangle oblicza przecięcie promienia z trójkątem
    bool intersect = glm::intersectRayTriangle(ray.pos, ray.dir, v0, v1, v2, barycentric);
    barycentric.z = 1.0f - barycentric.x - barycentric.y;
    // Wypisz wynik działania funkcji - czy jest przecięcie
    //std::cout << "Intersection exists: " << (intersect ? "Yes" : "No") << std::endl;
    //if(intersect) { std::cout<<"ok!"<<std::endl; }

    if (!intersect) {
        return t;  // Zwróć -1 jeśli nie ma przecięcia
    } else {
        // Oblicz punkt przecięcia w przestrzeni kartezjańskiej
        glm::vec3 cartesian = barycentric.x * v0 + barycentric.y * v1 + barycentric.z * v2;
        // Oblicz odległość od pozycji promienia do punktu przecięcia
        t = glm::distance(ray.pos, cartesian);
        return t;
    }
}
*/



/// \fn normal(glm::vec3 hit_pos)
/// \brief Surface normal vector at the intersection point.
/// \param hit_pos Intersection point (not used for triangle).
/// \return Normal vector parameters.
///
glm::vec3 CTriangle::normal(const glm::vec3& hit_pos) {
    auto u = v1 - v0;
    auto v = v2 - v0;
    auto n = glm::normalize(glm::cross(u,v));

    return n;
}



