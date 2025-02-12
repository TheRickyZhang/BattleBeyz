//#include "CameraBehavior.h"
//#include "Beyblade.h"
//
//// Note that this camera is relatively free to move 
//class InteractiveBehavior : public CameraBehavior {
//public:
//    InteractiveBehavior(Beyblade* selectedBeyblade, const glm::vec3& initIntersection)
//        : beyblade(selectedBeyblade),
//        initialIntersection(initIntersection),
//        isHolding(true)
//    {
//        dragOffset = beyblade->getBody()->getCenter().value() - initialIntersection;
//    }
//
//    void update(Camera& camera, float deltaTime, GameEngine* game) override {
//        // If the left mouse button is released, stop dragging.
//        if (!InputManager::getInstance().isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
//            isHolding = false;
//            return;
//        }
//
//        GLFWwindow* window = game->getWindow();
//        double dx, dy;
//        glfwGetCursorPos(window, &dx, &dy);
//        float xpos = static_cast<float>(dx);
//        float ypos = static_cast<float>(dy);
//
//        // Convert the current mouse position to a world-space ray.
//        glm::vec3 rayDir = screenToWorldCoordinates(window, xpos, ypos,
//            camera.getViewMatrix(),
//            game->projection);
//        rayDir = glm::normalize(rayDir);
//
//        // Compute intersection with a horizontal plane at the initial intersection's y.
//        glm::vec3 newIntersection = computeRayPlaneIntersection(camera.position,
//            rayDir,
//            initialIntersection.y);
//
//        // Update beyblade position, preserving the initial offset.
//        glm::vec3 newBeybladePos = newIntersection + dragOffset;
//        beyblade->setPosition(newBeybladePos);
//    }
//
//private:
//    Beyblade* beyblade;              // The object being dragged.
//    bool isHolding;                  // True while left mouse button is held.
//    glm::vec3 initialIntersection;   // The initial intersection point on the drag plane.
//    glm::vec3 dragOffset;            // Offset from the beyblade's center to the intersection.
//};