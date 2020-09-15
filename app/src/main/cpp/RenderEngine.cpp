//
// Created by micha on 8/28/2020.
//

#include "RenderEngine.h"
#include <GLES3/gl3.h>
#include <pthread.h>
#include <ctime>
#include <EGL/egl.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "assets.h"

#define SECOND 1000000000L

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

namespace Misery { RenderEngine renderContext(render); }

void RenderEngine::createEGLContext() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    ASSERT(display, "could not get EGL display! error 0x%04x", eglGetError());

    int v_major, v_minor;
    ASSERT(eglInitialize(display, &v_major, &v_minor), "could not initialize EGL! error 0x%04x", eglGetError());

    int config_attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16, EGL_STENCIL_SIZE, 0,
            EGL_NONE
    };
    int num_configs = 0;
    EGLConfig config = nullptr;
    ASSERT(eglChooseConfig(display, config_attribs, &config, 1, &num_configs) || num_configs,
           "could not choose EGL configuration! error 0x%04x", eglGetError());

    int context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    int err = eglGetError();
    ASSERT(err == EGL_SUCCESS, "could not create EGL context! error 0x%04x", err);

    surface = eglCreateWindowSurface(display, config, window, nullptr);
    ASSERT(surface, "could not create EGL surface! error 0x%04x", eglGetError());

    ASSERT(eglMakeCurrent(display, surface, surface, context), "EGL make current failed! error 0x%04x", eglGetError());
}
void RenderEngine::initOpenGL() {
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1, 0, 1, 1);
}

inline void clock_gettimediff(clockid_t clockid, timespec* now, timespec* diff) {
    timespec from { now->tv_sec, now->tv_nsec };
    clock_gettime(clockid, now);

    long ndiff = now->tv_nsec - from.tv_nsec;
    if (ndiff >= 0) {
        diff->tv_nsec = ndiff;
        diff->tv_sec = now->tv_sec - from.tv_sec;
    } else {
        diff->tv_nsec = ndiff + SECOND;
        diff->tv_sec = now->tv_sec - from.tv_sec - 1;
    }
}
void RenderEngine::renderThread() {
    createEGLContext();
    initOpenGL();

    timespec last {};
    long timer = 0L;
    long frameTime = SECOND / FRAME_CAP;
    uint fps = 0;

    assetLoader.loadMesh("pig.obj");
    assetLoader.loadShader("vertex.glsl", "fragment.glsl");

    // render loop
    while (true) {
        // validate EGL context
        if(context != eglGetCurrentContext() || surface != eglGetCurrentSurface(EGL_DRAW)) {
            int err = eglGetError();
            if (err != EGL_SUCCESS) LOGE("EGL context / surface not current. error 0x%04x", err);
            LOGI("trying to make current EGL context");
            ASSERT(eglMakeCurrent(display, surface, surface, context), "EGL make current failed! error 0x%04x", eglGetError());
        }

        // wait for frame
        if (timer >= SECOND) {
            LOGI("fps: %i", fps);
            fps = 0;
            timer = 0;
        }
        timespec diff {};
        clock_gettimediff(CLOCK_MONOTONIC, &last, &diff);
        if (diff.tv_nsec < frameTime) {
            timespec rest { 0, frameTime - diff.tv_nsec };
            while(nanosleep(&rest, &rest));
            clock_gettimediff(CLOCK_MONOTONIC, &last, &rest);
            diff.tv_sec += rest.tv_sec;
            diff.tv_nsec += rest.tv_nsec;
        }

        // load assets
        assetLoader.load();

        // draw
        glClear(GL_DEPTH_BUFFER_BIT + GL_COLOR_BUFFER_BIT);

        // swap buffers
        ASSERT(eglSwapBuffers(display, surface), "could not swap buffers! error 0x%04x", eglGetError());

        // count frames
        fps++;
        timer += diff.tv_nsec + SECOND * diff.tv_sec;
    }

    ANativeWindow_release(window);
}

void RenderEngine::start(AAssetManager* am, ANativeWindow* win) {
    this->assetLoader.setAssetManager(am);
    this->window = win;
    pthread_create(&thread, nullptr, RenderEngine::renderThread, this);
}

void* RenderEngine::renderThread(void *ths) {
    ((RenderEngine*) ths)->renderThread();
    pthread_exit(nullptr);
}

void RenderEngine::kill() {
    pthread_kill(thread, 0);
    ANativeWindow_release(window);
}
