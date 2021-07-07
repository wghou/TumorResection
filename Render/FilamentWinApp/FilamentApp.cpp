#include <memory>

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "FilamentApp.h"

#if !defined(WIN32)
#    include <unistd.h>
#else
#    include <SDL_syswm.h>
#    include <utils/unwindows.h>
#endif


#include <utils/Path.h>

#include <filament/Camera.h>
#include <filament/Material.h>
#include <filament/MaterialInstance.h>
#include <filament/Renderer.h>
#include <filament/RenderableManager.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/View.h>


#include "Cube.h"
#include "NativeWindowHelper.h"

#include "resources/resources.h"

using namespace filament;
using namespace filagui;
using namespace filament::math;
using namespace utils;

FilamentApp& FilamentApp::get() {
    static FilamentApp filamentApp;
    return filamentApp;
}

FilamentApp::FilamentApp() {
    initSDL();
}

FilamentApp::~FilamentApp() {
    SDL_Quit();
}

void FilamentApp::run(const Config& config, SetupCallback setupCallback,
        CleanupCallback cleanupCallback, ImGuiCallback imguiCallback,
        PreRenderCallback preRender, PostRenderCallback postRender,
        size_t width, size_t height) {
    std::unique_ptr<FilamentApp::Window> window(
            new FilamentApp::Window(this, config, config.title, width, height));

    mDepthMaterial = Material::Builder()
            .package(RESOURCES_DEPTHVISUALIZER_DATA, RESOURCES_DEPTHVISUALIZER_SIZE)
            .build(*mEngine);

    mDepthMI = mDepthMaterial->createInstance();

    mDefaultMaterial = Material::Builder()
            .package(RESOURCES_AIDEFAULTMAT_DATA, RESOURCES_AIDEFAULTMAT_SIZE)
            .build(*mEngine);

    mTransparentMaterial = Material::Builder()
            .package(RESOURCES_TRANSPARENTCOLOR_DATA, RESOURCES_TRANSPARENTCOLOR_SIZE)
            .build(*mEngine);

    std::unique_ptr<Cube> cameraCube(new Cube(*mEngine, mTransparentMaterial, {1,0,0}));
    // we can't cull the light-frustum because it's not applied a rigid transform
    // and currently, filament assumes that for culling
    std::unique_ptr<Cube> lightmapCube(new Cube(*mEngine, mTransparentMaterial, {0,1,0}, false));
    mScene = mEngine->createScene();

    window->mMainView->getView()->setVisibleLayers(0x4, 0x4);

    window->mUiView->getView()->setClearTargets(false, false, false);
    window->mUiView->getView()->setRenderTarget(View::TargetBufferFlags::DEPTH_AND_STENCIL);
    window->mUiView->getView()->setPostProcessingEnabled(false);
    window->mUiView->getView()->setShadowsEnabled(false);

    if (config.splitView) {
        auto& rcm = mEngine->getRenderableManager();

        rcm.setLayerMask(rcm.getInstance(cameraCube->getSolidRenderable()), 0x3, 0x2);
        rcm.setLayerMask(rcm.getInstance(cameraCube->getWireFrameRenderable()), 0x3, 0x2);
        cameraCube->mapFrustum(*mEngine, window->mMainCameraMan.getCamera());

        rcm.setLayerMask(rcm.getInstance(lightmapCube->getSolidRenderable()), 0x3, 0x2);
        rcm.setLayerMask(rcm.getInstance(lightmapCube->getWireFrameRenderable()), 0x3, 0x2);

        // Create the camera mesh
        window->mMainCameraMan.setCameraChangedCallback(
                [&cameraCube, &lightmapCube, &window, engine = mEngine](Camera const* camera) {
            cameraCube->mapFrustum(*engine, camera);
            lightmapCube->mapFrustum(*engine,
                    window->mMainView->getView()->getDirectionalLightCamera());
        });

        mScene->addEntity(cameraCube->getWireFrameRenderable());
        mScene->addEntity(cameraCube->getSolidRenderable());

        mScene->addEntity(lightmapCube->getWireFrameRenderable());
        mScene->addEntity(lightmapCube->getSolidRenderable());

        window->mGodView->getView()->setVisibleLayers(0x6, 0x6);
        window->mOrthoView->getView()->setVisibleLayers(0x6, 0x6);

        // only preserve the color buffer for additional views; depth and stencil can be discarded.
        window->mDepthView->getView()->setRenderTarget(View::TargetBufferFlags::DEPTH_AND_STENCIL);
        window->mGodView->getView()->setRenderTarget(View::TargetBufferFlags::DEPTH_AND_STENCIL);
        window->mOrthoView->getView()->setRenderTarget(View::TargetBufferFlags::DEPTH_AND_STENCIL);

        window->mDepthView->getView()->setShadowsEnabled(false);
        window->mGodView->getView()->setShadowsEnabled(false);
        window->mOrthoView->getView()->setShadowsEnabled(false);
    }

    loadIBL(config);
    if (mIBL != nullptr) {
        mIBL->getSkybox()->setLayerMask(0x7, 0x4);
        mScene->setSkybox(mIBL->getSkybox());
        mScene->setIndirectLight(mIBL->getIndirectLight());
    }

    for (auto& view : window->mViews) {
        if (view.get() != window->mUiView) {
            view->getView()->setScene(mScene);
        }
    }

    setupCallback(mEngine, window->mMainView->getView(), mScene);

    bool mousePressed[3] = { false };

	static float last = (double)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
    while (!mClosed) {

        if (!UTILS_HAS_THREADING) {
            mEngine->execute();
        }

        // Allow the app to animate the scene if desired.
        if (mAnimation) {
            double now = (double) SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
			
            mAnimation(mEngine, window->mMainView->getView(), mScene, now);
			last = now;
        }

        // Loop over fresh events twice: first stash them and let ImGui process them, then allow
        // the app to process the stashed events. This is done because ImGui might wish to block
        // certain events from the app (e.g., when dragging the mouse over an obscuring window).
        constexpr int kMaxEvents = 16;
        SDL_Event events[kMaxEvents];
        int nevents = 0;
        while (nevents < kMaxEvents && SDL_PollEvent(&events[nevents]) != 0) {
			const SDL_Event& event = events[nevents];
			switch (event.type) {
			case SDL_QUIT:
				mClosed = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					mClosed = true;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_M) {
					if(mAnimationStep) mAnimationStep(mEngine, window->mMainView->getView(), mScene, 1.0f/30.0f);
				}
				break;
			case SDL_MOUSEWHEEL:
					window->mouseWheel(event.wheel.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
					window->mouseDown(event.button.button, event.button.x, event.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
					window->mouseUp(event.button.x, event.button.y);
				break;
			case SDL_MOUSEMOTION:
					window->mouseMoved(event.motion.x, event.motion.y);
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					window->resize();
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
            nevents++;
        }

        window->mMainCameraMan.updateCameraTransform();

        // TODO: we need better timing or use SDL_GL_SetSwapInterval
        SDL_Delay(16);

        Renderer* renderer = window->getRenderer();

        if (preRender) {
            for (auto const& view : window->mViews) {
                if (view.get() != window->mUiView) {
                    preRender(mEngine, view->getView(), mScene, renderer);
                }
            }
        }

        if (renderer->beginFrame(window->getSwapChain())) {
            for (auto const& view : window->mViews) {
                renderer->render(view->getView());
            }
            renderer->endFrame();
        }

        if (postRender) {
            for (auto const& view : window->mViews) {
                if (view.get() != window->mUiView) {
                    postRender(mEngine, view->getView(), mScene, renderer);
                }
            }
        }
    }

    cleanupCallback(mEngine, window->mMainView->getView(), mScene);

    cameraCube.reset();
    lightmapCube.reset();
    window.reset();

    mIBL.reset();
    mEngine->destroy(mDepthMI);
    mEngine->destroy(mDepthMaterial);
    mEngine->destroy(mDefaultMaterial);
    mEngine->destroy(mTransparentMaterial);
    mEngine->destroy(mScene);
    Engine::destroy(&mEngine);
    mEngine = nullptr;
}

void FilamentApp::loadIBL(const Config& config) {
    if (!config.iblDirectory.empty()) {
        Path iblPath(config.iblDirectory);

        if (!iblPath.exists()) {
            std::cerr << "The specified IBL path does not exist: " << iblPath << std::endl;
            return;
        }

        if (!iblPath.isDirectory()) {
            std::cerr << "The specified IBL path is not a directory: " << iblPath << std::endl;
            return;
        }

        mIBL = std::make_unique<IBL>(*mEngine);
        if (!mIBL->loadFromDirectory(iblPath)) {
            std::cerr << "Could not load the specified IBL: " << iblPath << std::endl;
            mIBL.reset(nullptr);
            return;
        }
    }
}

void FilamentApp::initSDL() {

	if (SDL_Init(SDL_INIT_EVENTS) != 0) {
		std::cerr << "SDL_Init Failure" << std::endl;
	}
    //ASSERT_POSTCONDITION(SDL_Init(SDL_INIT_EVENTS) == 0, "SDL_Init Failure");
}

// ------------------------------------------------------------------------------------------------

FilamentApp::Window::Window(FilamentApp* filamentApp,
        const Config& config, std::string title, size_t w, size_t h)
        : mFilamentApp(filamentApp) {
    const int x = SDL_WINDOWPOS_CENTERED;
    const int y = SDL_WINDOWPOS_CENTERED;
    const uint32_t windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    mWindow = SDL_CreateWindow(title.c_str(), x, y, (int) w, (int) h, windowFlags);

    // Create the Engine after the window in case this happens to be a single-threaded platform.
    // For single-threaded platforms, we need to ensure that Filament's OpenGL context is current,
    // rather than the one created by SDL.
    mFilamentApp->mEngine = Engine::create(config.backend);
    mBackend = config.backend;

    void* nativeWindow = ::getNativeWindow(mWindow);
    void* nativeSwapChain = nativeWindow;

#if defined(__APPLE__)

    void* metalLayer = nullptr;
    if (config.backend == filament::Engine::Backend::METAL) {
        metalLayer = setUpMetalLayer(nativeWindow);
        // The swap chain on Metal is a CAMetalLayer.
        nativeSwapChain = metalLayer;
    }

#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
    if (config.backend == filament::Engine::Backend::VULKAN) {
        // We request a Metal layer for rendering via MoltenVK.
        setUpMetalLayer(nativeWindow);
    }
#endif

#endif

    mSwapChain = mFilamentApp->mEngine->createSwapChain(nativeSwapChain);
    mRenderer = mFilamentApp->mEngine->createRenderer();

    // create cameras
    mCameras[0] = mMainCamera = mFilamentApp->mEngine->createCamera();
    mCameras[1] = mDebugCamera = mFilamentApp->mEngine->createCamera();
    mCameras[2] = mOrthoCamera = mFilamentApp->mEngine->createCamera();
    mCameras[3] = mUiCamera = mFilamentApp->mEngine->createCamera();

    // set exposure
    for (auto camera : mCameras) {
        camera->setExposure(16.0f, 1 / 125.0f, 100.0f);
    }

    // create views
    mViews.emplace_back(mMainView = new CView(*mRenderer, "Main View"));
    if (config.splitView) {
        mViews.emplace_back(mDepthView = new CView(*mRenderer, "Depth View"));
        mViews.emplace_back(mGodView = new GodView(*mRenderer, "God View"));
        mViews.emplace_back(mOrthoView = new CView(*mRenderer, "Ortho View"));
        mDepthView->getView()->setDepthPrepass(View::DepthPrepass::DISABLED);
    }
    mViews.emplace_back(mUiView = new CView(*mRenderer, "UI View"));

    // set-up the camera manipulators
    double3 at(0, 0, 0);
    mMainCameraMan.setCamera(mMainCamera);
    mDebugCameraMan.setCamera(mDebugCamera);
    mMainView->setCamera(mMainCamera);
    mMainView->setCameraManipulator(&mMainCameraMan);
    mUiView->setCamera(mUiCamera);
    if (config.splitView) {
        // Depth view always uses the main camera
        mDepthView->setCamera(mMainCamera);

        // The god view uses the main camera for culling, but the debug camera for viewing
        mGodView->setCamera(mMainCamera);
        mGodView->setGodCamera(mDebugCamera);

        // Ortho view obviously uses an ortho camera
        mOrthoView->setCamera( (Camera *)mMainView->getView()->getDirectionalLightCamera() );

        mDepthView->setCameraManipulator(&mMainCameraMan);
        mGodView->setCameraManipulator(&mDebugCameraMan);
        mOrthoView->setCameraManipulator(&mOrthoCameraMan);
    }

    // configure the cameras
    configureCamerasForWindow();

    mMainCameraMan.lookAt(at + double3{ 4, 0, 0 }, at);
    mDebugCameraMan.lookAt(at + double3{ 4, 0, 0 }, at);
    mOrthoCameraMan.lookAt(at + double3{ 4, 0, 0 }, at);
}

FilamentApp::Window::~Window() {
    mViews.clear();
    for (auto& camera : mCameras) {
        mFilamentApp->mEngine->destroy(camera);
    }
    mFilamentApp->mEngine->destroy(mRenderer);
    mFilamentApp->mEngine->destroy(mSwapChain);
    SDL_DestroyWindow(mWindow);
}

void FilamentApp::Window::mouseDown(int button, ssize_t x, ssize_t y) {
    fixupMouseCoordinatesForHdpi(x, y);
    y = mHeight - y;
    for (auto const& view : mViews) {
        if (view->intersects(x, y)) {
            mEventTarget = view.get();
            view->mouseDown(button, x, y);
            break;
        }
    }
}

void FilamentApp::Window::mouseWheel(ssize_t x) {
    if (mEventTarget) {
        mEventTarget->mouseWheel(x);
    } else {
        for (auto const& view : mViews) {
            if (view->intersects(mLastX, mLastY)) {
                view->mouseWheel(x);
                break;
            }
        }
    }
}

void FilamentApp::Window::mouseUp(ssize_t x, ssize_t y) {
    fixupMouseCoordinatesForHdpi(x, y);
    if (mEventTarget) {
        y = mHeight - y;
        mEventTarget->mouseUp(x, y);
        mEventTarget = nullptr;
    }
}

void FilamentApp::Window::mouseMoved(ssize_t x, ssize_t y) {
    fixupMouseCoordinatesForHdpi(x, y);
    y = mHeight - y;
    if (mEventTarget) {
        mEventTarget->mouseMoved(x, y);
    }
    mLastX = x;
    mLastY = y;
}

void FilamentApp::Window::fixupMouseCoordinatesForHdpi(ssize_t& x, ssize_t& y) const {
    int dw, dh, ww, wh;
    SDL_GL_GetDrawableSize(mWindow, &dw, &dh);
    SDL_GetWindowSize(mWindow, &ww, &wh);
    x = x * dw / ww;
    y = y * dh / wh;
}

void FilamentApp::Window::resize() {
    mFilamentApp->mEngine->destroy(mSwapChain);
    void* nativeWindow = ::getNativeWindow(mWindow);
    void* nativeSwapChain = nativeWindow;

#if defined(__APPLE__)

    void* metalLayer = nullptr;
    if (mBackend == filament::Engine::Backend::METAL) {
        metalLayer = resizeMetalLayer(nativeWindow);
        // The swap chain on Metal is a CAMetalLayer.
        nativeSwapChain = metalLayer;
    }

#if defined(FILAMENT_DRIVER_SUPPORTS_VULKAN)
    if (mBackend == filament::Engine::Backend::VULKAN) {
        resizeMetalLayer(nativeWindow);
    }
#endif

#endif

    mSwapChain = mFilamentApp->mEngine->createSwapChain(nativeSwapChain);
    configureCamerasForWindow();
}

void FilamentApp::Window::configureCamerasForWindow() {

    // Determine the current size of the window in physical pixels.
    uint32_t w, h;
    SDL_GL_GetDrawableSize(mWindow, (int*) &w, (int*) &h);
    mWidth = (size_t) w;
    mHeight = (size_t) h;

    // Compute the "virtual pixels to physical pixels" scale factor that the
    // the platform uses for UI elements.
    int virtualWidth, virtualHeight;
    SDL_GetWindowSize(mWindow, &virtualWidth, &virtualHeight);
    float dpiScaleX = (float) w / virtualWidth;
    float dpiScaleY = (float) h / virtualHeight;

    const float3 at(0, 0, 0);
    const double ratio = double(h) / double(w);

    double near = 0.1;
    double far = 50;
    mMainCamera->setProjection(45.0, double(w) / h, near, far, Camera::Fov::VERTICAL);
    mDebugCamera->setProjection(45.0, double(w) / h, 0.0625, 4096, Camera::Fov::VERTICAL);
    mOrthoCamera->setProjection(Camera::Projection::ORTHO, -3, 3, -3 * ratio, 3 * ratio, near, far);
    mOrthoCamera->lookAt(at + float3{ 4, 0, 0 }, at);
    mUiCamera->setProjection(Camera::Projection::ORTHO,
            0.0, w / dpiScaleX,
            h / dpiScaleY, 0.0,
            0.0, 1.0);

    // We're in split view when there are more views than just the Main and UI views.
    if (mViews.size() > 2) {
        uint32_t vpw = w / 2;
        uint32_t vph = h / 2;
        mMainView->setViewport ({            0,            0,     vpw, vph     });
        mDepthView->setViewport({ int32_t(vpw),            0, w - vpw, vph     });
        mGodView->setViewport  ({ int32_t(vpw), int32_t(vph), w - vpw, h - vph });
        mOrthoView->setViewport({            0, int32_t(vph),     vpw, h - vph });

        mMainView->getCameraManipulator()->updateCameraTransform();
        mDepthView->getCameraManipulator()->updateCameraTransform();
        mGodView->getCameraManipulator()->updateCameraTransform();
        mOrthoView->getCameraManipulator()->updateCameraTransform();
    } else {
        mMainView->setViewport({ 0, 0, w, h });
    }
    mUiView->setViewport({ 0, 0, w, h });
}

// ------------------------------------------------------------------------------------------------

FilamentApp::CView::CView(Renderer& renderer, std::string name)
        : engine(*renderer.getEngine()), mName(name) {
    view = engine.createView();
    view->setClearColor({ 0 });
    view->setName(name.c_str());
}

FilamentApp::CView::~CView() {
    engine.destroy(view);
}

void FilamentApp::CView::setViewport(Viewport const& viewport) {
    mViewport = viewport;
    view->setViewport(viewport);
    if (mCameraManipulator) {
        mCameraManipulator->setViewport(viewport.width, viewport.height);
    }
}

void FilamentApp::CView::mouseDown(int button, ssize_t x, ssize_t y) {
    mLastMousePosition = double2(x, y);
    if (button == 1) {
        mMode = Mode::ROTATE;
    } else if (button == 3) {
        mMode = Mode::TRACK;
    }
}

void FilamentApp::CView::mouseUp(ssize_t x, ssize_t y) {
    mMode = Mode::NONE;
}

void FilamentApp::CView::mouseMoved(ssize_t x, ssize_t y) {
    if (mCameraManipulator) {
        double2 delta = double2(x, y) - mLastMousePosition;
        mLastMousePosition = double2(x, y);
        switch (mMode) {
            case Mode::NONE:
                break;
            case Mode::ROTATE:
                mCameraManipulator->rotate(delta);
                break;
            case Mode::TRACK:
                mCameraManipulator->track(delta);
                break;
        }
    }
}

void FilamentApp::CView::mouseWheel(ssize_t x) {
    if (mCameraManipulator){
        mCameraManipulator->dolly(x);
    }
}

bool FilamentApp::CView::intersects(ssize_t x, ssize_t y) {
    if (x >= mViewport.left && x < mViewport.left + mViewport.width)
        if (y >= mViewport.bottom && y < mViewport.bottom + mViewport.height)
            return true;

    return false;
}

void FilamentApp::CView::setCameraManipulator(CameraManipulator* cm) {
    mCameraManipulator = cm;
}

void FilamentApp::CView::setCamera(Camera* camera) {
    view->setCamera(camera);
}

void FilamentApp::GodView::setGodCamera(Camera* camera) {
    getView()->setDebugCamera(camera);
}
