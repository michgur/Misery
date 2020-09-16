//
// Created by micha on 8/28/2020.
//

#ifndef MISERY_RENDERENGINE_H
#define MISERY_RENDERENGINE_H

#include "Transform.h"
#include <assimp/matrix4x4.inl>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <android/asset_manager.h>
#include <queue>
#include <future>
#include <android/surface_texture.h>
#include "ECS.h"
#include "assets.h"

/** This class is responsible for rendering all of the entities.
 * It runs on a separate thread which also hosts an OpenGL context.
 * It also is responsible for loading assets that require OpenGL to be loaded */
class RenderEngine {
    pthread_t thread;
    ANativeWindow* window;
    EGLContext context;
    EGLSurface surface;
    EGLDisplay display;
    void createEGLContext();
    void initOpenGL();

    void renderThread();
    static void* renderThread(void* ths);

public:
    inline RenderEngine(void (*render)(uint, float)) : render(render) {}

    uint FRAME_CAP = 30;
    uint width = 0, height = 0;
    matrix4f projection;
    Transform camera;
    AssetLoader assetLoader {};

    void (*render)(uint, float);
    void setViewSize(uint w, uint h);

    void start(AAssetManager* assetManager, ANativeWindow* window);
    void kill();
};

namespace Misery { extern RenderEngine renderContext; }

#endif //MISERY_RENDERENGINE_H
