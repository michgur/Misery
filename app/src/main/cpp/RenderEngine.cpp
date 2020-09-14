//
// Created by micha on 8/28/2020.
//

#include "RenderEngine.h"
#include <GLES3/gl3.h>
#include <pthread.h>
#include <ctime>
#include <EGL/egl.h>
#include <android/native_window.h>
#include "assets.h"

void render(uint entity, float delta) {
    uint material = *Misery::ecs.getComponent<uint>(entity, "material");
    int shader = *Misery::ecs.getComponent<int>(material, "shader");
    glUseProgram(shader);

    int mvp = glGetUniformLocation(shader, "mvp");
    int diffuse = glGetUniformLocation(shader, "diffuse");

    matrix4f m = Misery::ecs.getComponent<Transform>(entity, "transform")->toMatrix();
    m = Misery::renderContext.projection * (Misery::renderContext.camera.toMatrix() * m);
    glUniformMatrix4fv(mvp, 1, true, m[0]);
    glActiveTexture(GL_TEXTURE0);
    uint texture = *Misery::ecs.getComponent<int>(material, "diffuse");
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(diffuse, 0);

    auto* mesh = (Misery::Mesh*) *Misery::ecs.getComponent<long>(entity, "mesh");
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->size, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void RenderEngine::setViewSize(uint w, uint h) {
    width = w;
    height = h;

    projection = Misery::createProjectionMatrix(std::acos(0), width, height, 0.1, 1000);
}

void RenderEngine::createEGLContext() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    ASSERT(display, "could not get EGL display! error %i", eglGetError());

    int v_major, v_minor;
    ASSERT(eglInitialize(display, &v_major, &v_minor), "could not initialize EGL! error %i", eglGetError());

    int config_attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16, EGL_STENCIL_SIZE, 0,
            EGL_NONE
    };
    int num_configs = 0;
    EGLConfig config = nullptr;
    ASSERT(eglChooseConfig(display, config_attribs, &config, 1, &num_configs) || num_configs,
           "could not choose EGL configuration! error %i", eglGetError());

    int context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3 };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    int err = eglGetError();
    ASSERT(err == EGL_SUCCESS, "could not create EGL context! error %i", err);

    surface = eglCreateWindowSurface(display, config, window, nullptr);
    ASSERT(surface, "could not create EGL surface! error %i", eglGetError());

    ASSERT(eglMakeCurrent(display, surface, surface, context), "EGL make current failed! error %i", eglGetError());
}
void RenderEngine::initOpenGL() {
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
}

void RenderEngine::renderThread() {
    createEGLContext();
    initOpenGL();

#define SECOND 1000000000L
    timespec last {};
    long timer = 0L;
    long frameTime = SECOND / FRAME_CAP;
    uint fps = 0;

    // render loop
    while (true) {
        // validate EGL context
        if(context != eglGetCurrentContext() || surface != eglGetCurrentSurface(EGL_DRAW)) {
            int err = eglGetError();
            if (err != EGL_SUCCESS) LOGE("EGL context / surface not current. error %i", err);
            LOGI("trying to make current EGL context");
            ASSERT(eglMakeCurrent(display, surface, surface, context), "EGL make current failed! error %i", eglGetError());
        }

        // wait for frame
        timespec now {};
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
        if (timer >= SECOND) {
            LOGI("fps: %i", fps);
            fps = 0;
            timer = 0;
        }
        timespec diff { now.tv_sec - last.tv_sec, now.tv_nsec - last.tv_nsec };
        if (diff.tv_nsec < frameTime || diff.tv_sec > 0) {
            nanosleep(&diff, nullptr);
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);
        }

        // draw
        glClear(GL_DEPTH_BUFFER_BIT + GL_COLOR_BUFFER_BIT);

        // swap buffers
        ASSERT(eglSwapBuffers(display, surface), "could not swap buffers! error %i", eglGetError());

        // update times
        fps++;
        timer += diff.tv_nsec;
        last = now;
    }

    ANativeWindow_release(window);
}

void RenderEngine::start(AAssetManager* am, ANativeWindow* win) {
    this->assetManager = am;
    this->window = win;
    pthread_t thread;
    pthread_create(&thread, nullptr, RenderEngine::renderThread, this);
}

void* RenderEngine::renderThread(void *ths) {
    ((RenderEngine*) ths)->renderThread();
    pthread_exit(nullptr);
}
