#include "object.hh"

Object::Object(Model model, Texture texture, glm::vec3 position, glm::vec3 rotation, 
        glm::vec3 velocity, glm::vec3 angularVelocity) : Entity(position, rotation) {
    this->model = model.getReference();
    this->texture = texture.getReference();
    this->velocity = velocity;
    this->angularVelocity = angularVelocity;

    //TODO ensure this is only done once per loading(static encapsulation)

    shader = ACGL::OpenGL::ShaderProgramCreator("HelloWorld").attributeLocations(
            model.getReference()->getAttributeLocations()).create();
    shader->use();
}

Object::~Object() {
}

void Object::render() {
    shader->setTexture("uTexture", texture, 0);
}
