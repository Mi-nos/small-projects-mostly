
#include <iostream>


#include "rt.h"

extern void draw_robot();
extern void draw_triangles();

bool cw2() {
    draw_robot();
    return true;
}
bool cw3() {
    //spójrz na CScene::create();
    return true;
}
bool cw4() {
    CScene scene1;
    CScene scene2;
    scene1.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);
    scene2.create(glm::vec3{2,5,10}, glm::vec3{-2,1,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    glm::mat3 ray_matrix1;
    CRayTrace::compPrimaryRayMatrix(scene1.cam, ray_matrix1);
    PRINT_MAT3(ray_matrix1);

    std::cout<<std::endl;
    glm::mat3 ray_matrix2;
    CRayTrace::compPrimaryRayMatrix(scene2.cam, ray_matrix2);
    PRINT_MAT3(ray_matrix2);

    return true;
}
bool cw5() {
    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    CRayTrace rt;
    CRay ray;
    COutput results;

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    ray.pos = scene.cam.eyep;
    float fx = scene.cam.width/2 - 1 + 0.5;
    float fy = scene.cam.height/2 - 1 + 0.5;

    //float fx = 0.5;
    //float fy = 0.5
    glm::vec3 b = {fx,fy,1};
    ray.dir = glm::normalize( ray_matrix * b);

    ray.dir.x = std::round(ray.dir.x * 100.0f) / 100.0f;
    ray.dir.y = std::round(ray.dir.y * 100.0f) / 100.0f;
    ray.dir.z = std::round(ray.dir.z * 100.0f) / 100.0f;

    std::string output;

    PRINT_VEC3(output, ray.dir);

    return true;
}
bool cw6() {

    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    CRayTrace rt;
    CRay ray;
    COutput results;


    /// creates raster image object
    CImage image(scene.cam.width, scene.cam.height);
    std::vector<std::string> names = {"X", "Y", "Z"};

    /// main loop
    for(int k = 0; k < 3; k++) {
        for (int j = 0; j < scene.cam.height; j++) {
            for (int i = 0; i < scene.cam.width; i++) {
                /// position of the image point
                float fx = (float) i + 0.5f;
                float fy = (float) j + 0.5f;
                glm::vec3 help = {fx, fy, 1};

                ray.pos = scene.cam.eyep;
                ray.dir = glm::normalize(ray_matrix * help);

                //ray.dir.x = std::round(ray.dir.x * 100.0f) / 100.0f;
                //ray.dir.y = std::round(ray.dir.y * 100.0f) / 100.0f;
                //ray.dir.z = std::round(ray.dir.z * 100.0f) / 100.0f;

                glm::vec3 rgb(0.0f, 0.0f, 0.0f);
                rgb[k] = (ray.dir[k] + 1.0f) / 2.0f; //odpowiednio rgb.x, rgb.y i rgb.z
                image.setPixel(i, j, rgb);


            }
        }
        image.save("O" + names[k], true);
        cv::imshow("O" + names[k], image.getImage());
        cv::waitKey();

    }
    return true;
}
bool cw7() {
    //nic tu nie ma, zerknij do CScene::create()
    return true;
}
bool cw8() {
    //implementacja w CObject::CSphere::intersect()
    CRay ray;
    ray.pos = {0,0,10};
    ray.dir = {0.3,0.3,-1};

    CSphere sphere({0,0,0},5);
    float test = sphere.intersect(ray);
    std::cout<<"TEST PRZECIECIA KULI " << test << std::endl;

    return true;
}
bool cw9() {
    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    auto sphere1 = new CSphere({-2.5,1.3,-3}, 1);
    auto sphere2 = new CSphere({0,0,0}, 1.6);
    scene.objectList.push_back(sphere1);
    scene.objectList.push_back(sphere2);

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    CRayTrace rt;
    CRay ray;
    COutput results;


    /// creates raster image object
    CImage image(scene.cam.width, scene.cam.height);

    /// main loop
    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};
            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            //jak to zrobisz, to będziesz miał pixel art zamiast kółka

         //   ray.dir.x = std::round(ray.dir.x * 100.0f) / 100.0f;
          //  ray.dir.y = std::round(ray.dir.y * 100.0f) / 100.0f;
           // ray.dir.z = std::round(ray.dir.z * 100.0f) / 100.0f;

            rt.rayTrace9(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("sphere-intersection.png", false);
    cv::imshow("sphere-intersection.png", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    delete sphere1;
    delete sphere2;

    return true;
}
bool cw10() {
    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    auto triangle = new CTriangle({3,3,-5},{-3,3,-10},{-3,-3,-8});
    scene.objectList.push_back(triangle);



    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    CRayTrace rt;
    CRay ray;
    COutput results;

    float test = triangle->intersect(ray);
    std::cout<<"TEST PRZECIECIA TRÓJKĄTA " << test << std::endl;
    /// creates raster image object
    CImage image(scene.cam.width, scene.cam.height);

    /// main loop
    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};
            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace9(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("triangle-intersection.png", false);
    cv::imshow("triangle-intersection.png", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    delete triangle;

    return true;
}
bool cw11(){
    //CObject::CSphere::normal()
    //CObject::CTriangle::normal() spójrz
    return true;
}
bool cw12() {
    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);
    auto sphere = new CSphere({0,0,0}, 1.6);

    CRayTrace rt;
    CRay ray;
    COutput results;

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    sphere->matAmbient = {0.5,0.0,0.0};
    sphere->matDiffuse = { 0.0, 0.0, 0.0};
    sphere->matSpecular = {0.0,0.0,0.0};
    sphere->matShininess = 30;
    sphere->reflectance = 0;
    scene.objectList.push_back(sphere);

    /// creates raster image object
    CImage image(scene.cam.width, scene.cam.height);

    /// main loop
    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace12(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("Ambient.png", false);
    cv::imshow("Ambient", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    scene.objectList.clear();

    sphere->matAmbient = {0.0,0.0,0.0};
    sphere->matDiffuse = {0.7,0,0};
    sphere->matShininess = 0;

    scene.objectList.push_back(sphere);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace12(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("Diffuse.png", false);
    cv::imshow("Diffuse", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    scene.objectList.clear();

    sphere->matAmbient = {0.0,0.0,0.0};
    sphere->matDiffuse = {0.0,0.0,0.0};
    sphere->matSpecular = {0.7,0.7,0.7};
    sphere->matShininess = 30;

    scene.objectList.push_back(sphere);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace12(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("Specular.png", false);
    cv::imshow("Specular", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    scene.objectList.clear();

    sphere->matAmbient = {0.1,0.0,0.0};
    sphere->matDiffuse = {0.6,0.0,0.0};
    sphere->matSpecular = {0.7,0.7,0.7};

    scene.objectList.push_back(sphere);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace12(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("Blinn-Phong.png", false);
    cv::imshow("Blinn-Phong", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    scene.objectList.clear();

    sphere->matShininess = 10;

    scene.objectList.push_back(sphere);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace12(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("shininese=10.png", false);
    cv::imshow("shininese=10", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    scene.objectList.clear();

    sphere->matShininess = 50;

    scene.objectList.push_back(sphere);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace12(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("shininese=50.png", false);
    cv::imshow("shininese=50", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    delete sphere;
    return true;
}
bool cw13() {
    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);
    auto sphere = new CSphere({0,0,0}, 1.6);

    CRayTrace rt;
    CRay ray;
    COutput results;

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);
    CImage image(scene.cam.width, scene.cam.height);

    sphere->matAmbient = {0.1,0.0,0.0};
    sphere->matDiffuse = {0.6,0.0,0.0};
    sphere->matSpecular = {0.7,0.7,0.7};
    sphere->matShininess = 30;
    sphere->isTexture = true;
    sphere->texture = CImage::createTexture(400,400);

    scene.objectList.push_back(sphere);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace13(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("Textured.png", false);
    cv::imshow("Textured", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    delete sphere;
    return true;

}
bool cw14() {
    CScene scene;
    scene.create(glm::vec3{0,0,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    CRayTrace rt;
    CRay ray;
    COutput results;

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    /// creates raster image object
    CImage image(scene.cam.width, scene.cam.height);

    auto sphere1 = new CSphere({-1,0,3},0.4);
    sphere1->matAmbient = {0,0.1,0};
    sphere1->matDiffuse = {0,0.6,0};
    sphere1->matSpecular = {0.7,0.7,0.7};
    sphere1->matShininess = 30;
    scene.objectList.push_back(sphere1);

    auto sphere2 = new CSphere( {0,0,0},1.6);
    sphere2->matAmbient = {0.1,0,0};
    sphere2->matDiffuse = {0.6,0,0};
    sphere2->matSpecular = {0.7,0.7,0.7};
    sphere2->matShininess = 30;
    scene.objectList.push_back(sphere2);

    auto sphere3 = new CSphere({-3,-2,-2},0.6);
    sphere3->matAmbient = {0,0,0.1};
    sphere3->matDiffuse = {0,0,0.6};
    sphere3->matSpecular = {0.7,0.7,0.7};
    sphere3->matShininess = 30;
    scene.objectList.push_back(sphere3);

    auto triangle1 = new CTriangle({5,5,-5},{-5,5,-5},{-5,-5,-5});
    triangle1->matAmbient = {0.1,0.1,0.1};
    triangle1->matDiffuse = {0.4,0.4,0.4};
    triangle1->matSpecular = {0.0,0.0,0.0};
    triangle1->matShininess = 0;
    scene.objectList.push_back(triangle1);

    auto triangle2 = new CTriangle({5,5,-5},{-5,-5,-5},{5,-5,-5});
    triangle2->matAmbient = {0.1,0.1,0.1};
    triangle2->matDiffuse = {0.4,0.4,0.4};
    triangle2->matSpecular = {0.0,0.0,0.0};
    triangle2->matShininess = 0;
    scene.objectList.push_back(triangle2);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};

            results.col = {0.0, 0.0, 0.0};

            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            rt.rayTrace14(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("Shadows.png", false);
    cv::imshow("Shadows", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    return true;
}
bool cw15() {
    CRayTrace rt;
    CRay ray;
    COutput results;

    ray.pos = {0,0,0};
    ray.dir = {0.5f,0.5f,0.0f};
    glm::vec3 n(0,1.0,0);
    glm::vec3 pos ={0,0,0};
    CRay ray_reflected = rt.reflectedRay(ray,n,pos);
    std::string position;
    std::string direction;
    PRINT_VEC3(position, ray_reflected.pos);
    PRINT_VEC3(direction, ray_reflected.dir);

    return true;

}
bool cw16() {
    CScene scene;
    scene.create(glm::vec3{0,-4,10}, glm::vec3{0,0,0}, glm::vec3{0, 1, 0}, 50, 500, 400);

    CRayTrace rt;
    CRay ray;
    COutput results;

    glm::mat3 ray_matrix;
    CRayTrace::compPrimaryRayMatrix(scene.cam, ray_matrix);

    CImage image(scene.cam.width, scene.cam.height);

    auto sphere = new CSphere{{0,0,0}, 1.6};
    sphere->matAmbient = {0.1,0.0,0.0};
    sphere->matDiffuse = {0.6,0.0,0.0};
    sphere->matSpecular = {0.7,0.7,0.7};
    sphere->matShininess = 30;
    sphere->reflectance = 0;

    scene.objectList.push_back(sphere);

    auto triangle1 = new CTriangle({5,5,-5},{-5,5,-5},{-5,-5,-5});
    triangle1->matAmbient = {0.1,0.1,0.1};
    triangle1->matDiffuse = {0.4,0.4,0.4};
    triangle1->matSpecular = {0.0,0.0,0.0};
    triangle1->matShininess = 0;
    triangle1->reflectance = 1;
    scene.objectList.push_back(triangle1);

    auto triangle2 = new CTriangle({5,5,-5},{-5,-5,-5},{5,-5,-5});
    triangle2->matAmbient = {0.1,0.1,0.1};
    triangle2->matDiffuse = {0.4,0.4,0.4};
    triangle2->matSpecular = {0.0,0.0,0.0};
    triangle2->matShininess = 0;
    triangle2->reflectance = 1;
    scene.objectList.push_back(triangle2);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};
            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            results.col = {0.0, 0.0, 0.0};
            results.energy = 1.0f;
            results.tree = 0;


            rt.rayTrace16(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("reflectance.png", false);
    cv::imshow("reflectance", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;

    delete sphere;
    delete triangle1;
    delete triangle2;
    scene.objectList.clear();

    auto light1 = new CLight{{5,-2,8}};
    light1->color = {0.3,0.3,0.3};

    auto sphere1 = new CSphere{{-1,0,3}, 0.4};
    sphere1->matAmbient = {0.0,0.1,0.0};
    sphere1->matDiffuse = {0.0,0.6,0.0};
    sphere1->matSpecular = {0.7,0.7,0.7};
    sphere1->matShininess = 30;
    sphere1->reflectance = 0;

    scene.objectList.push_back(sphere1);

    auto sphere2 = new CSphere{{0,0,0}, 1.6};
    sphere2->matAmbient = {0.1,0.0,0.0};
    sphere2->matDiffuse = {0.6,0.0,0.0};
    sphere2->matSpecular = {0.7,0.7,0.7};
    sphere2->matShininess = 30;
    sphere2->reflectance = 0;

    scene.objectList.push_back(sphere2);

    auto sphere3 = new CSphere{{-3,-2,-2}, 0.6};
    sphere3->matAmbient = {0.0,0.0,0.1};
    sphere3->matDiffuse = {0.0,0.0,0.6};
    sphere3->matSpecular = {0.7,0.7,0.7};
    sphere3->matShininess = 30;
    sphere3->reflectance = 0;

    scene.objectList.push_back(sphere3);

    auto triangle3 = new CTriangle({5,5,-5},{-5,5,-5},{-5,-5,-5});
    triangle3->matAmbient = {0.1,0.1,0.1};
    triangle3->matDiffuse = {0.4,0.4,0.4};
    triangle3->matSpecular = {0.0,0.0,0.0};
    triangle3->matShininess = 0;
    triangle3->reflectance = 1;

    scene.objectList.push_back(triangle3);

    auto triangle4 = new CTriangle({5,5,-5},{-5,-5,-5},{5,-5,-5});
    triangle4->matAmbient = {0.1,0.1,0.1};
    triangle4->matDiffuse = {0.4,0.4,0.4};
    triangle4->matSpecular = {0.0,0.0,0.0};
    triangle4->matShininess = 0;
    triangle4->reflectance = 1;

    scene.objectList.push_back(triangle4);

    for (int j = 0; j < scene.cam.height; j++) {
        for (int i = 0; i < scene.cam.width; i++) {
            /// position of the image point
            float fx = (float) i + 0.5f;
            float fy = (float) j + 0.5f;
            // computes ray.pos and normalized ray.dir using projection matrix
            glm::vec3 help = {fx, fy, 1};
            ray.pos = scene.cam.eyep;
            ray.dir = glm::normalize(ray_matrix * help);

            results.col = {0.0, 0.0, 0.0};
            results.energy = 1.0f;
            results.tree = 0;


            rt.rayTrace16(scene, ray, results);
            if(results.col.x > 1 or results.col.y > 1 or results.col.z > 1){ results.col = {1, 1, 1};}

            image.setPixel(i, j, results.col);
        }
    }
    image.save("reflectance2.png", false);
    cv::imshow("reflectance2", image.getImage());
    cv::waitKey();
    std::cout << std::endl << std::endl;


    return true;

}
bool cw17() {

    return true;
}

int main (int argc, char * const argv[]) {
    //cw4();
    //cw5();
    //cw6();
    //cw8();
    //cw9();
    //cw10();
    //cw12();
    //cw13();
    //cw14();
    //cw15();
    cw16();
    //cw17();
    return 0;
}


/// Draws two trianges
void draw_triangles() {

    CImage img(1000, 1000);

    glm::vec3 color1(0.9,0.1,0.1); // red
    glm::vec3 color2(0.1,0.9,0.1); // green

    // draw circle
    glm::vec3 pp(0.0,0.0,1.0);
    float radius = 0.1f;
//    img.drawCircle(pp, radius, color1);

    // triangle vertices
    glm::vec3 pp0(0.4, 0.3, 1);
    glm::vec3 pp1(-0.4, 0.3, 1);
    glm::vec3 pp2(0.4, -0.3, 1);

    // draws triangle in 2D
    img.drawLine(pp0, pp1, color1);
    img.drawLine(pp1, pp2, color1);
    img.drawLine(pp2, pp0, color1);

    // translation
    float tX = 0.2f; // OX translation
    float tY = 0.1f; // OY translation
    glm::mat3x3 mTrans {{1,0,0}, {0,1,0}, {tX,tY,1}}; // translation matrix
    PRINT_MAT3(mTrans);

    // translation of vertices
    pp0 = mTrans * pp0;
    pp1 = mTrans * pp1;
    pp2 = mTrans * pp2;

    // draws triangle after translation
    img.drawLine(pp0, pp1, color2);
    img.drawLine(pp1, pp2, color2);
    img.drawLine(pp2, pp0, color2);

    img.save("robot.png");
    cv::imshow("ROBOT", img.getImage());
    cv::waitKey();

}
void drawRectangle(CImage& img, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 color) {

    img.drawLine(p0, p1, color);
    img.drawLine(p1, p2, color);
    img.drawLine(p2, p3, color);
    img.drawLine(p3, p0, color);
}
/// Draws robot.
void draw_robot() {
    CImage img( 1000, 1000);
    glm::vec3 color1(0.9,0.1,0.1);
    glm::vec3 pp0(-0.4, 0.5, 1);
    glm::vec3 pp1(-0.4, 0.0, 1);
    glm::vec3 pp2( 0.4, 0.0, 1);
    glm::vec3 pp3( 0.4, 0.5, 1);
    drawRectangle(img,pp0,pp1,pp2,pp3,color1);

    glm::vec3 pp4(-0.8, 0.5, 1);
    glm::vec3 pp5(-0.8, 0.2, 1);
    glm::vec3 pp6( -0.6, 0.2, 1);
    glm::vec3 pp7( -0.6, 0.5, 1);
    drawRectangle(img,pp4,pp5,pp6,pp7,color1);

    glm::vec3 pp8(-0.4, -0.5, 1);
    glm::vec3 pp9(-0.4, -0.2, 1);
    glm::vec3 pp10( -0.3, -0.2, 1);
    glm::vec3 pp11( -0.3, -0.5, 1);
    drawRectangle(img,pp8,pp9,pp10,pp11,color1);

    glm::vec3 pp12(0.8, 0.5, 1);
    glm::vec3 pp13(0.8, 0.2, 1);
    glm::vec3 pp14(0.6, 0.2, 1);
    glm::vec3 pp15(0.6, 0.5, 1);
    drawRectangle(img,pp12,pp13,pp14,pp15,color1);

    glm::vec3 pp16(0.4, -0.5, 1);
    glm::vec3 pp17(0.4, -0.2, 1);
    glm::vec3 pp18(0.3, -0.2, 1);
    glm::vec3 pp19(0.3, -0.5, 1);
    drawRectangle(img,pp16,pp17,pp18,pp19,color1);

    glm::vec3 pp(0.0,0.7,1.0);
    float radius = 0.1f;
    img.drawCircle(pp, radius, color1);

    img.save("robot.png");
    cv::imshow("ROBOT", img.getImage());
    cv::waitKey();
}
