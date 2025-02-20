//
//  CScene.cpp
//  rt
//
//  Created by Radoslaw Mantiuk on 22/01/2023.
//

#include "rt.h"

#include<fstream>

/// \fn create(void)
/// \brief Adds components to the scene.
///
void CScene::create() {

    lightList.clear(); // clears vector with the light data
    objectList.clear(); // clears vector of pointers to objects

    // add camera, light sources, and objects

    cam.eyep ={ 0,0,10};
    cam.lookp={0,0,0};
    cam.up={0,1,0};
    cam.fov = 50;
    cam.width=500;
    cam.height=400;


    CSphere* sphere1 = new CSphere({-2.5,1.3,-3}, 1);
    CSphere* sphere2 = new CSphere({0,0,0}, 1.6);
    objectList.push_back(sphere1);
    objectList.push_back(sphere2);

    CLight light1(glm::vec3(-3,-2, 8));
    light1.color = {0.6, 0.6, 0.6};
    lightList.push_back(light1);

    //CTriangle* triangle1 = new CTriangle({3,3,-5},{-3,3,-10},{-3,-3,-8});

    //objectList.push_back(triangle1);

    /*  cam.eyep ={ 2,5,10};
      cam.lookp={-2,-1,0};
      cam.up={0,1,0};
      cam.fov = 50;
      cam.width=500;
      cam.height=400;
      */
}
void CScene::create(const glm::vec3& eyep, const glm::vec3& lookp, const glm::vec3& up, const float fov, const float width,  const float height) {

    lightList.clear(); // clears vector with the light data
    objectList.clear(); // clears vector of pointers to objects

    cam.eyep = eyep;
    cam.lookp= lookp;
    cam.up= up;
    cam.fov = fov;
    cam.width= width;
    cam.height= height;

    CLight light(glm::vec3(-3,-2, 8));
    light.color = {0.6, 0.6, 0.6};
    lightList.push_back(light);

}

