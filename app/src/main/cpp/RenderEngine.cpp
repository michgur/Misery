//
// Created by micha on 8/28/2020.
//

#include "RenderEngine.h"
#include <GLES3/gl3.h>
#include <GLES/gl.h>
#include <pthread.h>
#include <ctime>
#include <EGL/egl.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "assets.h"

#define SECOND 1000000000L

void RenderEngine::setViewSize(uint w, uint h) {
    width = w;
    height = h;

    projection = Misery::createProjectionMatrix(std::acos(0), width, height, 0.1, 1000);
}

namespace Misery { RenderEngine renderContext(ecs); }

void RenderEngine::createEGLContext() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    ASSERT(display, "error: eglGetDisplay() failed 0x%04x", eglGetError());

    int v_major, v_minor;
    ASSERT(eglInitialize(display, &v_major, &v_minor), "error: eglInitialize() failed 0x%04x", eglGetError());

    int config_attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16, EGL_STENCIL_SIZE, 0,
            EGL_NONE
    };
    int num_configs, i;
    eglChooseConfig(display, config_attribs, nullptr, 0, &num_configs);
    auto* configs = new EGLConfig[num_configs];
    ASSERT(eglChooseConfig(display, config_attribs, configs, 1, &num_configs) && num_configs,
           "error: eglChooseConfig() failed 0x%04x", eglGetError());
    for (i = 0; i < num_configs; i++) {
        auto attr = [&](int attr, int value) {
            int res;
            eglGetConfigAttrib(display, configs[i], attr, &res);
            return res >= value;
        };
        if (attr(EGL_RED_SIZE, 8) && attr(EGL_BLUE_SIZE, 8) && attr(EGL_GREEN_SIZE, 8)
            && attr(EGL_DEPTH_SIZE, 16) && attr(EGL_STENCIL_SIZE, 0))
            break;
    }
    ASSERT(i < num_configs, "Error: eglChooseConfig() returned no valid configs");

    int context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    context = eglCreateContext(display, configs[i], EGL_NO_CONTEXT, context_attribs);
    int err = eglGetError();
    ASSERT(err == EGL_SUCCESS, "error: eglCreateContext() failed 0x%04x", err);

    surface = eglCreateWindowSurface(display, configs[i], window, nullptr);
    if (surface == nullptr || surface == EGL_NO_SURFACE) {
        int error = eglGetError();
        if (error == EGL_BAD_NATIVE_WINDOW)
            LOGE("error: createWindowSurface() returned EGL_BAD_NATIVE_WINDOW.");
        ASSERT(false, "error: createWindowSurface() failed 0x%04x", error);
    }

    ASSERT(eglMakeCurrent(display, surface, surface, context), "error: eglMakeCurrent() failed 0x%04x", eglGetError());

    ASSERT(eglQuerySurface(display, surface, EGL_WIDTH, (int*) &width) &&
        eglQuerySurface(display, surface, EGL_HEIGHT, (int*) &height),
        "could not query surface size! error 0x%04x", eglGetError());
}
void RenderEngine::initOpenGL() {
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1, 0, 1, 1);
    glViewport(0, 0, width, height);
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
//    createEGLContext();
    initOpenGL();

    timespec last {};
    long timer = 0L;
    long frameTime = SECOND / FRAME_CAP;
    uint fps = 0;

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
#define RANDF (float) std::rand() / (float) RAND_MAX
            glClearColor(RANDF, RANDF, RANDF, 1.0f);
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

        float delta = (float) diff.tv_nsec / SECOND + diff.tv_sec;
        // load assets
        assetLoader.load();

        // draw
        glClear(GL_DEPTH_BUFFER_BIT + GL_COLOR_BUFFER_BIT);
        for (uint entity : entities) render(entity, delta);

        // swap buffers
        ASSERT(eglSwapBuffers(display, surface), "could not swap buffers! error 0x%04x", eglGetError());

        // count frames
        fps++;
        timer += diff.tv_nsec + SECOND * diff.tv_sec;
    }
}

void RenderEngine::start(AAssetManager* am, ANativeWindow* win) {
    this->assetLoader.setAssetManager(am);
    this->window = win;
    pthread_create(&thread, nullptr, RenderEngine::renderThread, this);
}

void RenderEngine::start(EGLDisplay* d, EGLContext* c, EGLSurface* s, AAssetManager* am, ANativeWindow* win) {
    this->assetLoader.setAssetManager(am);
    this->window = win;
    this->display = d;
    this->context = c;
    this->surface = s;
//    renderThread();
}

void* RenderEngine::renderThread(void *ths) {
    ((RenderEngine*) ths)->renderThread();
    pthread_exit(nullptr);
}

void RenderEngine::kill() {
    pthread_kill(thread, 0);
    ANativeWindow_release(window);
}

void RenderEngine::render(uint entity, float) {
    uint material = *ecs.getComponent<uint>(entity, "material");
    auto id = *ecs.getComponent<AssetID*>(material, "shader");
    uint shader = *id->get();
    glUseProgram(shader);

    int mvp = glGetUniformLocation(shader, "mvp");
    int diffuse = glGetUniformLocation(shader, "diffuse");

    matrix4f m = ecs.getComponent<Transform>(entity, "transform")->toMatrix();
    m = projection * (camera.toMatrix() * m);
    glUniformMatrix4fv(mvp, 1, true, m[0]);
    glActiveTexture(GL_TEXTURE0);
    uint texture = *(*ecs.getComponent<AssetID*>(material, "diffuse"))->get();
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(diffuse, 0);

    uint* mesh = (*ecs.getComponent<AssetID*>(entity, "mesh"))->get();
    glBindVertexArray(mesh[0]);
    glDrawElements(GL_TRIANGLES, mesh[1], GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void RenderEngine::drawFrame() {
    eglSwapBuffers(display, surface);
    //    assetLoader.load();
//
//    // draw
//    glClear(GL_DEPTH_BUFFER_BIT + GL_COLOR_BUFFER_BIT);
//    for (uint entity : entities) render(entity, 0);
}
