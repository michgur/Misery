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

    initOpenGL();
}
void RenderEngine::initOpenGL() {
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(1, 0, 1, 1);
    glViewport(0, 0, width, height);

    projection = Misery::createProjectionMatrix(90, width, height, 0.1f, 1000);
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
    timespec last {};
    long timer = 0L;
    long frameTime = SECOND / FRAME_CAP;
    uint fps = 0;

    bool running = true;

    // render loop
    while (running) {
        pthread_mutex_lock(&mutex);
        switch (interrupt) {
            case MISERY_INT_SURFACE:
                createEGLContext();
                initOpenGL();
                break;
            case MISERY_INT_KILL:
                running = false;
                destroyEGLContext();
                break;
            case MISERY_INT_NONE:
            default:
                break;
        }
        interrupt = MISERY_INT_NONE;

        if (display) {
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
        pthread_mutex_unlock(&mutex);
    }
}

void RenderEngine::start() {
    LOGI("starting render thread (%lu)", thread);
    pthread_mutex_init(&mutex, nullptr);
    pthread_create(&thread, nullptr, RenderEngine::renderThread, this);
}

void* RenderEngine::renderThread(void *ths) {
    ((RenderEngine*) ths)->renderThread();
    return nullptr;
}

void RenderEngine::kill() {
    pthread_mutex_lock(&mutex);
    LOGI("killing render thread (%lu)", thread);
    interrupt = MISERY_INT_KILL;
    pthread_mutex_unlock(&mutex);

    pthread_join(thread, nullptr);
    pthread_mutex_destroy(&mutex);
    LOGI("render thread stopped");
}

void RenderEngine::render(uint entity, float) {
    uint material = *ecs.getComponent<uint>(entity, "material");
    uint shader = *(*ecs.getComponent<AssetID*>(material, "shader"))->get();
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

void RenderEngine::destroyEGLContext() {
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);

    ANativeWindow_release(window);
}

void RenderEngine::setSurface(AAssetManager* assetManager, ANativeWindow* nativeWindow) {
    pthread_mutex_lock(&mutex);
    LOGI("render thread (%lu) setting surface %p", thread, nativeWindow);
    assetLoader.setAssetManager(assetManager);
    window = nativeWindow;
    interrupt = MISERY_INT_SURFACE;
    pthread_mutex_unlock(&mutex);
}

void RenderEngine::releaseSurface() { ANativeWindow_release(window); }
