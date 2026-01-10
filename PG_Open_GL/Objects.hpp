//
//  Objects.hpp
//  OpenGL Advances Lighting
//
#ifndef Objects_hpp
#define Objects_hpp

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model3D.hpp"
#include "Shader.hpp"

namespace gps {

    // Simple Axis-Aligned Bounding Box for collision detection
    struct BoundingBox {
        glm::vec3 min;
        glm::vec3 max;

        bool contains(const glm::vec3& point) const {
            return (point.x >= min.x && point.x <= max.x) &&
                (point.y >= min.y && point.y <= max.y) &&
                (point.z >= min.z && point.z <= max.z);
        }
    };

    // Data for a single tree instance in the world
    struct TreeInstance {
        glm::vec3 position;
        int typeIndex;   // 0, 1, or 2 (determines which .obj to use)
        float scale;
        float rotationY; 
    };

    class Scene {
    public:
        Scene();

        void loadModels();
        bool checkCollision(const glm::vec3& position);
        void draw(Shader& shader, const glm::mat4& view, float angleY);
        void drawLightCube(Shader& shader, const glm::mat4& view, const glm::mat4& lightRotation, const glm::vec3& lightDir);

    private:
        //Static Models
        Model3D nanosuit;
        Model3D mario;
        Model3D creeper;
        Model3D zombie;
        Model3D ground;
        Model3D lightCube;
        Model3D diamondOres;

        //Tree Models (Pointers to avoid copy issues)
        std::vector<Model3D*> treeModels;

        //Scene Data
        std::vector<TreeInstance> forestInstances; 
        std::vector<BoundingBox> collisionBoxes;

        // Helpers
        void initCollision();
        void setNormalMatrix(Shader& shader, const glm::mat4& model, const glm::mat4& view);
        BoundingBox calculateAABB(const glm::mat4& modelMatrix, glm::vec3 localMin, glm::vec3 localMax);
    };
}

#endif 