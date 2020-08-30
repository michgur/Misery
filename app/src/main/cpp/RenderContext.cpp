//
// Created by micha on 8/28/2020.
//

#include "RenderContext.h"
#include <GLES3/gl3.h>
#include "assets.h"

void render(Entity &e, float delta) {
    uint material = *Misery::ecs.getComponent<uint>(e.id, "material");
    int shader = *Misery::ecs.getComponent<int>(material, "shader");
    glUseProgram(shader);

    int mvp = glGetUniformLocation(shader, "mvp");
    int diffuse = glGetUniformLocation(shader, "diffuse");

    matrix4f m = Misery::ecs.getComponent<Transform>(e.id, "transform")->toMatrix();
    m = Misery::renderContext.projection * (Misery::renderContext.camera.toMatrix() * m);
    glUniformMatrix4fv(mvp, 1, true, m[0]);
    glActiveTexture(GL_TEXTURE0);
    uint texture = *Misery::ecs.getComponent<int>(material, "diffuse");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(diffuse, 0);

    auto* mesh = (Misery::Mesh*) *Misery::ecs.getComponent<long>(e.id, "mesh");
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

namespace Misery { RenderContext renderContext { .render = render }; }

void RenderContext::setViewSize(uint w, uint h) {
    width = w;
    height = h;

    projection = Misery::createProjectionMatrix(std::acos(0), width, height, 0.1, 1000);
}
