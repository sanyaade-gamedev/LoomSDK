/*
 * ===========================================================================
 * Loom SDK
 * Copyright 2011, 2012, 2013
 * The Game Engine Company, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * ===========================================================================
 */

#pragma once

#define GFX_DEBUG 1

#ifdef GFX_DEBUG
#define GFX_OPENGL_CHECK 1
#endif

#include <SDL.h>

#ifdef LOOM_RENDERER_OPENGLES2
#include "SDL_opengles2.h"
#else
#include "SDL_opengl.h"
#endif

#include <stdint.h>

#include "loom/common/core/assert.h"
#include "loom/common/core/log.h"
namespace GFX {
    lmDeclareLogGroup(gGFXLogGroup);
}

#ifdef GFX_OPENGL_CHECK
#ifdef _WIN32
#include <intrin.h>
#endif
#endif

namespace GFX
{

    typedef struct GL_Context
    {

#ifdef _WIN32
#define GFX_CALL __stdcall
#else
#define GFX_CALL
#endif

#if GFX_OPENGL_CHECK

#define GFX_PREFIX gfx_internal_
#define GFX_PREFIX_CALL_INTERNAL_CONCAT(prefix, func, args) prefix ## func args
#define GFX_PREFIX_CALL_INTERNAL(prefix, func, args) GFX_PREFIX_CALL_INTERNAL_CONCAT(prefix, func, args)
#define GFX_PREFIX_CALL(func, args) GFX_PREFIX_CALL_INTERNAL(GFX_PREFIX, func, args)

#define GFX_PROC_BEGIN(ret, func, params) \
        ret (GFX_CALL *GFX_PREFIX_CALL(func,)) params; \
        ret func params {

#ifdef _WIN32
#define GFX_PROC_BREAK __debugbreak();
#else
#define GFX_PROC_BREAK
#endif

#define GFX_PROC_MID(func) \
        GLenum error = GFX_PREFIX_CALL(glGetError, ()); \
        switch (error) { \
            case GL_NO_ERROR: break; \
            case GL_OUT_OF_MEMORY: lmLogWarn(gGFXLogGroup, "OpenGL reported to be out of memory"); break; \
            case 0x0507 /* GL_CONTEXT_LOST in OpenGL 4.5 */: lmLogWarn(gGFXLogGroup, "OpenGL reported context loss"); break; \
            default: \
                const char* errorName; \
                switch (error) { \
                    case GL_INVALID_ENUM: errorName = "GL_INVALID_ENUM"; break; \
                    case GL_INVALID_VALUE: errorName = "GL_INVALID_VALUE"; break; \
                    case GL_INVALID_OPERATION: errorName = "GL_INVALID_OPERATION"; break; \
                    case 0x0503 /* GL_STACK_OVERFLOW */: errorName = "GL_STACK_OVERFLOW"; break; \
                    case 0x0504 /* GL_STACK_UNDERFLOW */: errorName = "GL_STACK_UNDERFLOW"; break; \
                    case GL_INVALID_FRAMEBUFFER_OPERATION: errorName = "GL_INVALID_FRAMEBUFFER_OPERATION"; break; \
                    default: errorName = "Unknown error"; \
                } \
                lmLogError(gGFXLogGroup, "OpenGL error: %s (0x%04x)", errorName, error); \
                GFX_PROC_BREAK \
                lmAssert(error, "OpenGL error, see above for details."); \
        } \

#define GFX_PROC_VOID(func, params, args) \
        GFX_PROC_BEGIN(void, func, params) \
            GFX_PREFIX_CALL(func, args); \
            GFX_PROC_MID(func) \
        }

#define GFX_PROC(ret, func, params, args) \
        GFX_PROC_BEGIN(ret, func, params) \
            ret returnValue = GFX_PREFIX_CALL(func, args); \
            GFX_PROC_MID(func) \
            return returnValue; \
        }

#else

#define GFX_PROC(ret, func, params, args) ret (GFX_CALL *func) params;
#define GFX_PROC_VOID(func, params, args) GFX_PROC(void, func, params, args)

#endif

#include "gfxGLES2EntryPoints.h"
#undef GFX_PROC
#undef GFX_PROC_VOID
    } GL_Context;


/** 
  *  Graphics subsystem class in charge of initializing bgfx graphics and handling context loss
  */    
class Graphics
{

public:
    
    static GL_Context *context()
    {
        return &_context;
    }

    static void initialize();

    static bool isInitialized()
    {
        return sInitialized;
    }

    static void reset(int width, int height, uint32_t flags = 0);

    static void shutdown();
    
    static bool queryExtension(char *extName);

    static void beginFrame();

    static void endFrame();

    static void handleContextLoss();

    static inline uint32_t getCurrentFrame() { return sCurrentFrame; }

    static inline void setNativeSize(int width, int height)
    {
        sWidth = width;
        sHeight = height;
    }
    
    static inline int getWidth() { return sWidth; }
    static inline int getHeight() { return sHeight; }

    static void setViewTransform(float *view, float *proj);

    static void setDebug(int flags);
    static void screenshot(const char *path);
    static void setFillColor(int color);
    static int getFillColor();


    // Set a clip rect specified by the provided parameters
    static void setClipRect(int x, int y, int width, int height);

    // Reset clip rect
    static void clearClipRect();

private:

    // Once the Graphics system is initialized, this will be true!
    static bool sInitialized;

    // If we're currently in a OpenGL context loss situation (the application has changed orientation, etc), 
    // this will be true.  Once we're recovering the graphics subsystem will need to recreate vertex/index buffers, 
    // texture resources, etc
    static bool sContextLost;    

    // The current width of the graphics device
    static int sWidth;

    // The current height of the graphics device
    static int sHeight;

    // The flags used to create the graphics device( see bgfx.h BGFX_RESET_ for a list of flags )
    static uint32_t sFlags;

    // The current fill color used when clearing the color buffer
    static int sFillColor;

    // The current view number being rendered
    static int sView;

    // The current frame counter
    static uint32_t sCurrentFrame;

    // Opaque platform data, such as HWND
//    static void *sPlatformData[3];

    // Internal method used to initialize platform data 
//    static void initializePlatform();

    // If set, at next opportunity we will store a screenshot to this path and clear it.
    static char pendingScreenshot[1024];
    
    static GL_Context _context;

};
}
