/*
    Copyright 2016-2023 melonDS team

    This file is part of melonDS.

    melonDS is free software: you can redistribute it and/or modify it under
    the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    melonDS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with melonDS. If not, see http://www.gnu.org/licenses/.
*/

#include "GPU_OpenGL.h"

#include <assert.h>
#include <cstdio>
#include <cstring>

#include "NDS.h"
#include "GPU.h"
#include "GPU3D_OpenGL.h"
#include "OpenGLSupport.h"
#include "GPU_OpenGL_shaders.h"
#include "PluginManager.h"

namespace melonDS
{

using namespace OpenGL;

std::optional<GLCompositor> GLCompositor::New() noexcept
{
    assert(glBindAttribLocation != nullptr);
    GLuint CompShader {};

    const char* kCompositorFS_Custom = Plugins::PluginManager::get()->gpuOpenGLFragmentShader();
    if (!OpenGL::CompileVertexFragmentProgram(CompShader, kCompositorVS, (kCompositorFS_Custom == nullptr ? kCompositorFS_Nearest : kCompositorFS_Custom), "CompositorShader",
            {{"vPosition", 0}, {"vTexcoord", 1}}, {{"oColor", 0}}))
            return std::nullopt;

    return { GLCompositor(CompShader) };
}

GLCompositor::GLCompositor(GLuint compShader) noexcept : CompShader(compShader)
{
    CompScaleLoc = glGetUniformLocation(CompShader, "u3DScale");
    Comp3DXPosLoc = glGetUniformLocation(CompShader, "u3DXPos");
    CompIsBottomScreen2DTextureBlackLoc = glGetUniformLocation(CompShader, "IsBottomScreen2DTextureBlack");
    CompIsTopScreen2DTextureBlackLoc = glGetUniformLocation(CompShader, "IsTopScreen2DTextureBlack");
    CompPriorGameSceneLoc = glGetUniformLocation(CompShader, "PriorGameScene");
    CompGameSceneLoc = glGetUniformLocation(CompShader, "GameScene");
    CompAspectRatioLoc = glGetUniformLocation(CompShader, "TopScreenAspectRatio");
    CompShowMapLoc = glGetUniformLocation(CompShader, "ShowMap");
    CompShowTargetLoc = glGetUniformLocation(CompShader, "ShowTarget");
    CompShowMissionGaugeLoc = glGetUniformLocation(CompShader, "ShowMissionGauge");

    glUseProgram(CompShader);
    GLuint screenTextureUniform = glGetUniformLocation(CompShader, "ScreenTex");
    glUniform1i(screenTextureUniform, 0);
    GLuint _3dTextureUniform = glGetUniformLocation(CompShader, "_3DTex");
    glUniform1i(_3dTextureUniform, 1);
    GLuint khUiScaleUniform = glGetUniformLocation(CompShader, "KHUIScale");
    glUniform1i(khUiScaleUniform, 4);

    // all this mess is to prevent bleeding
#define SETVERTEX(i, x, y, offset) \
    CompVertices[i].Position[0] = x; \
    CompVertices[i].Position[1] = y + offset; \
    CompVertices[i].Texcoord[0] = (x + 1.f) * (256.f / 2.f); \
    CompVertices[i].Texcoord[1] = (y + 1.f) * (384.f / 2.f)

    const float padOffset = 1.f/(192*2.f+2.f)*2.f;
    // top screen
    SETVERTEX(0, -1, 1, 0);
    SETVERTEX(1, 1, 0, padOffset);
    SETVERTEX(2, 1, 1, 0);
    SETVERTEX(3, -1, 1, 0);
    SETVERTEX(4, -1, 0, padOffset);
    SETVERTEX(5, 1, 0, padOffset);

    // bottom screen
    SETVERTEX(6, -1, 0, -padOffset);
    SETVERTEX(7, 1, -1, 0);
    SETVERTEX(8, 1, 0, -padOffset);
    SETVERTEX(9, -1, 0, -padOffset);
    SETVERTEX(10, -1, -1, 0);
    SETVERTEX(11, 1, -1, 0);

#undef SETVERTEX

    glGenBuffers(1, &CompVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, CompVertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CompVertices), &CompVertices[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &CompVertexArrayID);
    glBindVertexArray(CompVertexArrayID);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CompVertex), (void*)(offsetof(CompVertex, Position)));
    glEnableVertexAttribArray(1); // texcoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CompVertex), (void*)(offsetof(CompVertex, Texcoord)));

    glGenFramebuffers(CompScreenOutputFB.size(), &CompScreenOutputFB[0]);

    glGenTextures(1, &CompScreenInputTex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, CompScreenInputTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, 256*3 + 1, 192*2, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL);

    glGenTextures(CompScreenOutputTex.size(), &CompScreenOutputTex[0]);
    for (GLuint i : CompScreenOutputTex)
    {
        glBindTexture(GL_TEXTURE_2D, i);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLCompositor::~GLCompositor()
{
    assert(glDeleteFramebuffers != nullptr);

    glDeleteFramebuffers(CompScreenOutputFB.size(), &CompScreenOutputFB[0]);
    glDeleteTextures(1, &CompScreenInputTex);
    glDeleteTextures(CompScreenOutputTex.size(), &CompScreenOutputTex[0]);

    glDeleteVertexArrays(1, &CompVertexArrayID);
    glDeleteBuffers(1, &CompVertexBufferID);

    glDeleteProgram(CompShader);
}


GLCompositor::GLCompositor(GLCompositor&& other) noexcept :
    Scale(other.Scale),
    ScreenH(other.ScreenH),
    ScreenW(other.ScreenW),
    IsBottomScreen2DTextureBlack(other.IsBottomScreen2DTextureBlack),
    IsTopScreen2DTextureBlack(other.IsTopScreen2DTextureBlack),
    PriorGameScene(other.PriorGameScene),
    GameScene(other.GameScene),
    AspectRatio(other.AspectRatio),
    ShowMap(other.ShowMap),
    ShowTarget(other.ShowTarget),
    ShowMissionGauge(other.ShowMissionGauge),
    CompScaleLoc(other.CompScaleLoc),
    Comp3DXPosLoc(other.Comp3DXPosLoc),
    CompIsBottomScreen2DTextureBlackLoc(other.CompIsBottomScreen2DTextureBlackLoc),
    CompIsTopScreen2DTextureBlackLoc(other.CompIsTopScreen2DTextureBlackLoc),
    CompPriorGameSceneLoc(other.CompPriorGameSceneLoc),
    CompGameSceneLoc(other.CompGameSceneLoc),
    CompAspectRatioLoc(other.CompAspectRatioLoc),
    CompShowMapLoc(other.CompShowMapLoc),
    CompShowTargetLoc(other.CompShowTargetLoc),
    CompShowMissionGaugeLoc(other.CompShowMissionGaugeLoc),
    CompVertices(other.CompVertices),
    CompShader(other.CompShader),
    CompVertexBufferID(other.CompVertexBufferID),
    CompVertexArrayID(other.CompVertexArrayID),
    CompScreenInputTex(other.CompScreenInputTex),
    CompScreenOutputTex(other.CompScreenOutputTex),
    CompScreenOutputFB(other.CompScreenOutputFB)
{
    other.CompScreenOutputFB = {};
    other.CompScreenInputTex = {};
    other.CompScreenOutputTex = {};
    other.CompVertexArrayID = {};
    other.CompVertexBufferID = {};
    other.CompShader = {};
}

GLCompositor& GLCompositor::operator=(GLCompositor&& other) noexcept
{
    if (this != &other)
    {
        Scale = other.Scale;
        ScreenH = other.ScreenH;
        ScreenW = other.ScreenW;
        IsBottomScreen2DTextureBlack = other.IsBottomScreen2DTextureBlack;
        IsTopScreen2DTextureBlack = other.IsTopScreen2DTextureBlack;
        PriorGameScene = other.PriorGameScene;
        GameScene = other.GameScene;
        AspectRatio = other.AspectRatio;
        ShowMap = other.ShowMap;
        ShowTarget = other.ShowTarget;
        ShowMissionGauge = other.ShowMissionGauge;
        CompScaleLoc = other.CompScaleLoc;
        Comp3DXPosLoc = other.Comp3DXPosLoc;
        CompIsBottomScreen2DTextureBlackLoc = other.CompIsBottomScreen2DTextureBlackLoc;
        CompIsTopScreen2DTextureBlackLoc = other.CompIsTopScreen2DTextureBlackLoc;
        CompPriorGameSceneLoc = other.CompPriorGameSceneLoc;
        CompGameSceneLoc = other.CompGameSceneLoc;
        CompAspectRatioLoc = other.CompAspectRatioLoc;
        CompShowMapLoc = other.CompShowMapLoc;
        CompShowTargetLoc = other.CompShowTargetLoc;
        CompShowMissionGaugeLoc = other.CompShowMissionGaugeLoc;
        CompVertices = other.CompVertices;

        // Clean up these resources before overwriting them
        glDeleteProgram(CompShader);
        CompShader = other.CompShader;

        glDeleteBuffers(1, &CompVertexBufferID);
        CompVertexBufferID = other.CompVertexBufferID;

        glDeleteVertexArrays(1, &CompVertexArrayID);
        CompVertexArrayID = other.CompVertexArrayID;

        glDeleteTextures(1, &CompScreenInputTex);
        CompScreenInputTex = other.CompScreenInputTex;

        glDeleteTextures(CompScreenOutputTex.size(), &CompScreenOutputTex[0]);
        CompScreenOutputTex = other.CompScreenOutputTex;

        glDeleteFramebuffers(CompScreenOutputFB.size(), &CompScreenOutputFB[0]);
        CompScreenOutputFB = other.CompScreenOutputFB;

        other.CompScreenOutputFB = {};
        other.CompScreenInputTex = {};
        other.CompScreenOutputTex = {};
        other.CompVertexArrayID = {};
        other.CompVertexBufferID = {};
        other.CompShader = {};
    }

    return *this;
}

void GLCompositor::SetIsBottomScreen2DTextureBlack(bool isBlack) noexcept
{
    if (isBlack == IsBottomScreen2DTextureBlack)
        return;

    IsBottomScreen2DTextureBlack = isBlack;
}
void GLCompositor::SetIsTopScreen2DTextureBlack(bool isBlack) noexcept
{
    if (isBlack == IsTopScreen2DTextureBlack)
        return;

    IsTopScreen2DTextureBlack = isBlack;
}
void GLCompositor::SetGameScene(int gameScene) noexcept
{
    if (gameScene == GameScene)
        return;

    PriorGameScene = GameScene;
    GameScene = gameScene;
}
void GLCompositor::SetAspectRatio(float aspectRatio) noexcept
{
    if (aspectRatio == AspectRatio)
        return;

    AspectRatio = aspectRatio;
}
void GLCompositor::SetShowMap(bool showMap) noexcept
{
    if (showMap == ShowMap)
        return;

    ShowMap = showMap;
}
void GLCompositor::SetShowTarget(bool showTarget) noexcept
{
    if (showTarget == ShowTarget)
        return;

    ShowTarget = showTarget;
}
void GLCompositor::SetShowMissionGauge(bool showMissionGauge) noexcept
{
    if (showMissionGauge == ShowMissionGauge)
        return;

    ShowMissionGauge = showMissionGauge;
}
void GLCompositor::SetScaleFactor(int scale) noexcept
{
    if (scale == Scale)
        return;

    Scale = scale;
    ScreenW = 256 * scale;
    ScreenH = (384+2) * scale;

    for (int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, CompScreenOutputTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ScreenW, ScreenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        // fill the padding
        u8* zeroPixels = (u8*) calloc(1, ScreenW*2*scale*4);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 192*scale, ScreenW, 2*scale, GL_RGBA, GL_UNSIGNED_BYTE, zeroPixels);

        GLenum fbassign[] = {GL_COLOR_ATTACHMENT0};
        glBindFramebuffer(GL_FRAMEBUFFER, CompScreenOutputFB[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, CompScreenOutputTex[i], 0);
        glDrawBuffers(1, fbassign);
        free(zeroPixels);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLCompositor::Stop(const GPU& gpu) noexcept
{
    for (int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, CompScreenOutputFB[gpu.FrontBuffer]);

        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLCompositor::RenderFrame(const GPU& gpu, Renderer3D& renderer) noexcept
{
    int backbuf = gpu.FrontBuffer ^ 1;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, CompScreenOutputFB[backbuf]);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glViewport(0, 0, ScreenW, ScreenH);

    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: select more shaders (filtering, etc)
    glUseProgram(CompShader);
    glUniform1ui(CompScaleLoc, Scale);
    glUniform1i(CompIsBottomScreen2DTextureBlackLoc, IsBottomScreen2DTextureBlack ? 1 : 0);
    glUniform1i(CompIsTopScreen2DTextureBlackLoc, IsTopScreen2DTextureBlack ? 1 : 0);
    glUniform1i(CompPriorGameSceneLoc, PriorGameScene);
    glUniform1i(CompGameSceneLoc, GameScene);
    glUniform1f(CompAspectRatioLoc, AspectRatio);
    glUniform1i(CompShowMapLoc, ShowMap ? 1 : 0);
    glUniform1i(CompShowTargetLoc, ShowTarget ? 1 : 0);
    glUniform1i(CompShowMissionGaugeLoc, ShowMissionGauge ? 1 : 0);

    // TODO: support setting this midframe, if ever needed
    glUniform1i(Comp3DXPosLoc, ((int)gpu.GPU3D.GetRenderXPos() << 23) >> 23);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, CompScreenInputTex);

    if (gpu.Framebuffer[backbuf][0] && gpu.Framebuffer[backbuf][1])
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256*3 + 1, 192, GL_RGBA_INTEGER,
                        GL_UNSIGNED_BYTE, gpu.Framebuffer[backbuf][0].get());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 192, 256*3 + 1, 192, GL_RGBA_INTEGER,
                        GL_UNSIGNED_BYTE, gpu.Framebuffer[backbuf][1].get());
    }

    glActiveTexture(GL_TEXTURE1);
    renderer.SetupAccelFrame();

    glBindBuffer(GL_ARRAY_BUFFER, CompVertexBufferID);
    glBindVertexArray(CompVertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 4*3);
}

void GLCompositor::BindOutputTexture(int buf)
{
    glBindTexture(GL_TEXTURE_2D, CompScreenOutputTex[buf]);
}

}
