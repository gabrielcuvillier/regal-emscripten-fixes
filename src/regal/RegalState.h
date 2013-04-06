/*
  Copyright (c) 2011-2012 NVIDIA Corporation
  Copyright (c) 2011-2012 Cass Everitt
  Copyright (c) 2012 Scott Nations
  Copyright (c) 2012 Mathias Schott
  Copyright (c) 2012 Nigel Stewart
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __REGAL_STATE_H__
#define __REGAL_STATE_H__

#include "RegalUtil.h"

REGAL_GLOBAL_BEGIN

#include <cstring>    // For memset, memcpy

#include <string>
#include <algorithm>  // For std::swap

#include <boost/print/print_string.hpp>
#include <boost/print/string_list.hpp>

#include "RegalEmu.h"
#include "RegalIff.h"
#include "RegalToken.h"
#include "RegalDispatch.h"

REGAL_GLOBAL_END

REGAL_NAMESPACE_BEGIN

//
// OpenGL State Tracking
//
// Motivating requirements:
//
//  - Emulation of glPushAttrib and glPopAttrib for OpenGL ES and
//    core OpenGL that lack the functionality.
//
//  - OpenGL state capture, browsing, diff and serialization.
//
// See also:
//
//  - Gallium3D
//    http://wiki.freedesktop.org/wiki/Software/gallium
//    http://dri.freedesktop.org/doxygen/gallium/p__state_8h-source.html
//
//  - Tracking Graphics State For Networked Rendering
//    Ian Buck, Greg Humphreys and Pat Hanrahan.
//    Stanford University
//    Proceedings of the 2000 Eurographics/SIGGRAPH Workshop on Graphics Hardware
//    http://graphics.stanford.edu/papers/state_tracking/
//
//  - Chromium: A Stream Processing Framework for Interactive Rendering on Clusters
//    Greg Humphreys, Mike Houston, Ren Ng
//    Stanford University
//    SIGGRAPH 2002
//    http://graphics.stanford.edu/papers/cr/
//

namespace State {

  using   ::boost::print::hex;
  using   ::boost::print::print_string;
  typedef ::boost::print::string_list<std::string> string_list;

  inline static void setEnable(DispatchTable &dt, const GLenum cap, const GLboolean enable)
  {
    RegalAssert(dt.glEnable);
    RegalAssert(dt.glDisable);
    if (enable)
      dt.glEnable(cap);
    else
      dt.glDisable(cap);
  }

  inline static void setEnableIndexedEXT(DispatchTable &dt, const GLenum cap, const GLuint index, const GLboolean enable)
  {
    RegalAssert(dt.glEnable);
    RegalAssert(dt.glDisable);
    if (enable)
      dt.glEnableIndexedEXT(cap,index);
    else
      dt.glDisableIndexedEXT(cap,index);
  }

  inline static void setEnablei(DispatchTable &dt, const GLenum cap, const GLuint index, const GLboolean enable)
  {
    RegalAssert(dt.glEnablei);
    RegalAssert(dt.glDisablei);
    if (enable)
      dt.glEnablei(cap,index);
    else
      dt.glDisablei(cap,index);
  }

  inline static void enableToString(string_list& tmp, const GLboolean b, const char* bEnum, const char *delim = "\n")
  {
      tmp << print_string(b ? "glEnable(" : "glDisable(",bEnum,");",delim);
  }

  inline static void enableiToString(string_list& tmp, const GLboolean b, const char* bEnum, const GLuint index, const char *delim = "\n")
  {
      tmp << print_string(b ? "glEnablei(" : "glDisablei(",bEnum,",",index,");",delim);
  }

  //
  // glPushAttrib(GL_ENABLE_BIT)
  //

  struct Enable
  {
    GLboolean   alphaTest;                 // GL_ALPHA_TEST
    GLboolean   autoNormal;                // GL_AUTO_NORMAL
    GLboolean   blend[REGAL_MAX_DRAW_BUFFERS]; // GL_BLEND
    GLboolean   clipDistance[REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES]; // GL_CLIP_DISTANCEi
    GLenum      clampFragmentColor;        // GL_CLAMP_FRAGMENT_COLOR
    GLenum      clampReadColor;            // GL_CLAMP_READ_COLOR
    GLenum      clampVertexColor;          // GL_CLAMP_VERTEX_COLOR
    GLboolean   colorLogicOp;              // GL_COLOR_LOGIC_OP
    GLboolean   colorMaterial;             // GL_COLOR_MATERIAL
    GLboolean   colorSum;                  // GL_COLOR_SUM
    GLboolean   colorTable;                // GL_COLOR_TABLE
    GLboolean   convolution1d;             // GL_CONVOLUTION_1D
    GLboolean   convolution2d;             // GL_CONVOLUTION_2D
    GLboolean   cullFace;                  // GL_CULL_FACE
    GLboolean   depthClamp;                // GL_DEPTH_CLAMP
    GLboolean   depthTest;                 // GL_DEPTH_TEST
    GLboolean   dither;                    // GL_DITHER
    GLboolean   fog;                       // GL_FOG
    GLboolean   framebufferSRGB;           // GL_FRAMEBUFFER_SRGB
    GLboolean   histogram;                 // GL_HISTOGRAM
    GLboolean   indexLogicOp;              // GL_INDEX_LOGIC_OP
    GLboolean   light[REGAL_FIXED_FUNCTION_MAX_LIGHTS]; // GL_LIGHTi
    GLboolean   lighting;                  // GL_LIGHTING
    GLboolean   lineSmooth;                // GL_LINE_SMOOTH
    GLboolean   lineStipple;               // GL_LINE_STIPPLE
                                           // TODO: GL_MAP1_x where x is a map type (9 x B) (GL_FALSE)
                                           // TODO: GL_MAP2_x where x is a map type (9 x B) (GL_FALSE)
    GLboolean   minmax;                    // GL_MINMAX
    GLboolean   multisample;               // GL_MULTISAMPLE
    GLboolean   normalize;                 // GL_NORMALIZE
    GLboolean   pointSmooth;               // GL_POINT_SMOOTH
    GLboolean   pointSprite;               // GL_POINT_SPRITE
    GLboolean   polygonOffsetFill;         // GL_POLYGON_OFFSET_FILL
    GLboolean   polygonOffsetLine;         // GL_POLYGON_OFFSET_LINE
    GLboolean   polygonOffsetPoint;        // GL_POLYGON_OFFSET_POINT
    GLboolean   polygonSmooth;             // GL_POLYGON_SMOOTH
    GLboolean   polygonStipple;            // GL_POLYGON_STIPPLE
    GLboolean   postColorMatrixColorTable; // GL_POST_COLOR_MATRIX_COLOR_TABLE
    GLboolean   postConvolutionColorTable; // GL_POST_CONVOLUTION_COLOR_TABLE
    GLboolean   programPointSize;          // GL_PROGRAM_POINT_SIZE
    GLboolean   rescaleNormal;             // GL_RESCALE_NORMAL
    GLboolean   sampleAlphaToCoverage;     // GL_SAMPLE_ALPHA_TO_COVERAGE
    GLboolean   sampleAlphaToOne;          // GL_SAMPLE_ALPHA_TO_ONE
    GLboolean   sampleCoverage;            // GL_SAMPLE_COVERAGE
    GLboolean   sampleShading;             // GL_SAMPLE_SHADING
    GLboolean   separable2d;               // GL_SEPARABLE_2D
    GLboolean   scissorTest[REGAL_MAX_VIEWPORTS]; // GL_SCISSOR_TEST
    GLboolean   stencilTest;               // GL_STENCIL_TEST
    GLboolean   texture1d[REGAL_EMU_MAX_TEXTURE_UNITS];      // GL_TEXTURE_1D
    GLboolean   texture2d[REGAL_EMU_MAX_TEXTURE_UNITS];      // GL_TEXTURE_2D
    GLboolean   texture3d[REGAL_EMU_MAX_TEXTURE_UNITS];      // GL_TEXTURE_3D
    GLboolean   textureCubeMap[REGAL_EMU_MAX_TEXTURE_UNITS]; // GL_TEXTURE_CUBE_MAP
    GLboolean   textureGenS[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_S
    GLboolean   textureGenT[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_T
    GLboolean   textureGenR[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_R
    GLboolean   textureGenQ[REGAL_EMU_MAX_TEXTURE_UNITS];    // GL_TEXTURE_GEN_Q
    GLboolean   vertexProgramTwoSide;      // GL_VERTEX_PROGRAM_TWO_SIDE

    inline Enable()
    : alphaTest(GL_FALSE)
    , autoNormal(GL_FALSE)
    , clampFragmentColor(GL_FIXED_ONLY)
    , clampReadColor(GL_FIXED_ONLY)
    , clampVertexColor(GL_TRUE)
    , colorLogicOp(GL_FALSE)
    , colorMaterial(GL_FALSE)
    , colorSum(GL_FALSE)
    , colorTable(GL_FALSE)
    , convolution1d(GL_FALSE)
    , convolution2d(GL_FALSE)
    , cullFace(GL_FALSE)
    , depthClamp(GL_FALSE)
    , depthTest(GL_FALSE)
    , dither(GL_TRUE)
    , fog(GL_FALSE)
    , framebufferSRGB(GL_FALSE)
    , histogram(GL_FALSE)
    , indexLogicOp(GL_FALSE)
    , lighting(GL_FALSE)
    , lineSmooth(GL_FALSE)
    , lineStipple(GL_FALSE)
    , minmax(GL_FALSE)
    , multisample(GL_TRUE)
    , normalize(GL_FALSE)
    , pointSmooth(GL_FALSE)
    , pointSprite(GL_FALSE)
    , polygonOffsetFill(GL_FALSE)
    , polygonOffsetLine(GL_FALSE)
    , polygonOffsetPoint(GL_FALSE)
    , polygonSmooth(GL_FALSE)
    , polygonStipple(GL_FALSE)
    , postColorMatrixColorTable(GL_FALSE)
    , postConvolutionColorTable(GL_FALSE)
    , programPointSize(GL_FALSE)
    , rescaleNormal(GL_FALSE)
    , sampleAlphaToCoverage(GL_FALSE)
    , sampleAlphaToOne(GL_FALSE)
    , sampleCoverage(GL_FALSE)
    , sampleShading(GL_FALSE)
    , separable2d(GL_FALSE)
    , stencilTest(GL_FALSE)
    , vertexProgramTwoSide(GL_FALSE)
    {
      std::memset(blend,GL_FALSE,sizeof(blend));
      std::memset(clipDistance,GL_FALSE,sizeof(clipDistance));
      std::memset(light,GL_FALSE,sizeof(light));
      std::memset(scissorTest,GL_FALSE,sizeof(scissorTest));
      std::memset(texture1d,GL_FALSE,sizeof(texture1d));
      std::memset(texture2d,GL_FALSE,sizeof(texture2d));
      std::memset(texture3d,GL_FALSE,sizeof(texture3d));
      std::memset(textureCubeMap,GL_FALSE,sizeof(textureCubeMap));
      std::memset(textureGenQ,GL_FALSE,sizeof(textureGenQ));
      std::memset(textureGenR,GL_FALSE,sizeof(textureGenR));
      std::memset(textureGenS,GL_FALSE,sizeof(textureGenS));
      std::memset(textureGenT,GL_FALSE,sizeof(textureGenT));

      // TODO: GL_MAP1_x where x is a map type (9 x B) (GL_FALSE)
      // TODO: GL_MAP2_x where x is a map type (9 x B) (GL_FALSE)
    }

    inline Enable(const Enable &other)
    {
      if (this!=&other)
        std::memcpy(this,&other,sizeof(Enable));
    }

    inline Enable &operator=(const Enable &other)
    {
      if (this!=&other)
        std::memcpy(this,&other,sizeof(Enable));
      return *this;
    }

    inline Enable &swap(Enable &other)
    {
      *this = other;
      return *this;
    }

    inline Enable &get(DispatchTable &dt)
    {
      RegalAssert(dt.glIsEnabled);
      RegalAssert(dt.glIsEnabledIndexedEXT);
      RegalAssert(dt.glClampColor);
      alphaTest = dt.glIsEnabled(GL_ALPHA_TEST);
      autoNormal = dt.glIsEnabled(GL_AUTO_NORMAL);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        blend[ii] = dt.glIsEnabledi(GL_BLEND, ii);
      dt.glGetIntegerv(GL_CLAMP_FRAGMENT_COLOR,reinterpret_cast<GLint*>(&clampFragmentColor));
      dt.glGetIntegerv(GL_CLAMP_READ_COLOR,reinterpret_cast<GLint*>(&clampReadColor));
      dt.glGetIntegerv(GL_CLAMP_VERTEX_COLOR,reinterpret_cast<GLint*>(&clampVertexColor));
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES; ii++)
        clipDistance[ii] = dt.glIsEnabled(GL_CLIP_DISTANCE0+ii);
      colorLogicOp  = dt.glIsEnabled(GL_COLOR_LOGIC_OP);
      colorMaterial = dt.glIsEnabled(GL_COLOR_MATERIAL);
      colorMaterial = dt.glIsEnabled(GL_COLOR_MATERIAL);
      colorSum = dt.glIsEnabled(GL_COLOR_SUM);
      colorTable = dt.glIsEnabled(GL_COLOR_TABLE);
      convolution1d = dt.glIsEnabled(GL_CONVOLUTION_1D);
      convolution2d = dt.glIsEnabled(GL_CONVOLUTION_2D);
      cullFace = dt.glIsEnabled(GL_CULL_FACE);
      depthClamp = dt.glIsEnabled(GL_DEPTH_CLAMP);
      depthTest = dt.glIsEnabled(GL_DEPTH_TEST);
      dither = dt.glIsEnabled(GL_DITHER);
      fog = dt.glIsEnabled(GL_FOG);
      framebufferSRGB = dt.glIsEnabled(GL_FRAMEBUFFER_SRGB);
      histogram = dt.glIsEnabled(GL_HISTOGRAM);
      indexLogicOp = dt.glIsEnabled(GL_INDEX_LOGIC_OP);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_LIGHTS; ii++)
        light[ii] = dt.glIsEnabled(GL_LIGHT0+ii);
      lighting = dt.glIsEnabled(GL_LIGHTING);
      lineSmooth = dt.glIsEnabled(GL_LINE_SMOOTH);
      lineStipple = dt.glIsEnabled(GL_LINE_STIPPLE);
      minmax = dt.glIsEnabled(GL_MINMAX);
      multisample = dt.glIsEnabled(GL_MULTISAMPLE);
      normalize = dt.glIsEnabled(GL_NORMALIZE);
      pointSmooth = dt.glIsEnabled(GL_POINT_SMOOTH);
      pointSprite = dt.glIsEnabled(GL_POINT_SPRITE);
      polygonOffsetFill = dt.glIsEnabled(GL_POLYGON_OFFSET_FILL);
      polygonOffsetLine = dt.glIsEnabled(GL_POLYGON_OFFSET_LINE);
      polygonOffsetPoint = dt.glIsEnabled(GL_POLYGON_OFFSET_POINT);
      polygonSmooth = dt.glIsEnabled(GL_POLYGON_SMOOTH);
      polygonStipple = dt.glIsEnabled(GL_POLYGON_STIPPLE);
      postColorMatrixColorTable = dt.glIsEnabled(GL_POST_COLOR_MATRIX_COLOR_TABLE);
      postConvolutionColorTable = dt.glIsEnabled(GL_POST_CONVOLUTION_COLOR_TABLE);
      programPointSize = dt.glIsEnabled(GL_PROGRAM_POINT_SIZE);
      rescaleNormal = dt.glIsEnabled(GL_RESCALE_NORMAL);
      sampleAlphaToCoverage = dt.glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
      sampleAlphaToOne = dt.glIsEnabled(GL_SAMPLE_ALPHA_TO_ONE);
      sampleCoverage = dt.glIsEnabled(GL_SAMPLE_COVERAGE);
      sampleShading = dt.glIsEnabled(GL_SAMPLE_SHADING);
      separable2d = dt.glIsEnabled(GL_SEPARABLE_2D);
      stencilTest = dt.glIsEnabled(GL_STENCIL_TEST);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        scissorTest[ii] = dt.glIsEnabledi(GL_SCISSOR_TEST,ii);
      {
        for (int ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        {
          texture1d[ii]      = dt.glIsEnabledIndexedEXT(GL_TEXTURE_1D,ii);
          texture2d[ii]      = dt.glIsEnabledIndexedEXT(GL_TEXTURE_2D,ii);
          texture3d[ii]      = dt.glIsEnabledIndexedEXT(GL_TEXTURE_3D,ii);
          textureCubeMap[ii] = dt.glIsEnabledIndexedEXT(GL_TEXTURE_CUBE_MAP,ii);
          textureGenS[ii]    = dt.glIsEnabledIndexedEXT(GL_TEXTURE_GEN_S,ii);
          textureGenT[ii]    = dt.glIsEnabledIndexedEXT(GL_TEXTURE_GEN_T,ii);
          textureGenR[ii]    = dt.glIsEnabledIndexedEXT(GL_TEXTURE_GEN_R,ii);
          textureGenQ[ii]    = dt.glIsEnabledIndexedEXT(GL_TEXTURE_GEN_Q,ii);
        }
      }

      return *this;
    }
    inline const Enable &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glClampColor);
      setEnable(dt,GL_ALPHA_TEST,alphaTest);
      setEnable(dt,GL_AUTO_NORMAL,autoNormal);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        setEnablei(dt,GL_BLEND,ii,blend[ii]);
      setEnable(dt,GL_COLOR_LOGIC_OP,colorLogicOp);
      setEnable(dt,GL_COLOR_MATERIAL,colorMaterial);
      dt.glClampColor(GL_CLAMP_FRAGMENT_COLOR,clampFragmentColor);
      dt.glClampColor(GL_CLAMP_READ_COLOR,clampReadColor);
      dt.glClampColor(GL_CLAMP_VERTEX_COLOR,clampVertexColor);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES; ii++)
        setEnable(dt,GL_CLIP_DISTANCE0+ii,clipDistance[ii]);
      setEnable(dt,GL_COLOR_MATERIAL,colorMaterial);
      setEnable(dt,GL_COLOR_SUM,colorSum);
      setEnable(dt,GL_COLOR_TABLE,colorTable);
      setEnable(dt,GL_CONVOLUTION_1D,convolution1d);
      setEnable(dt,GL_CONVOLUTION_2D,convolution2d);
      setEnable(dt,GL_CULL_FACE,cullFace);
      setEnable(dt,GL_DEPTH_CLAMP,depthClamp);
      setEnable(dt,GL_DEPTH_TEST,depthTest);
      setEnable(dt,GL_DITHER,dither);
      setEnable(dt,GL_FOG,fog);
      setEnable(dt,GL_FRAMEBUFFER_SRGB,framebufferSRGB);
      setEnable(dt,GL_HISTOGRAM,histogram);
      setEnable(dt,GL_INDEX_LOGIC_OP,indexLogicOp);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_LIGHTS; ii++)
        setEnable(dt,GL_LIGHT0+ii,light[ii]);
      setEnable(dt,GL_LIGHTING,lighting);
      setEnable(dt,GL_LINE_SMOOTH,lineSmooth);
      setEnable(dt,GL_LINE_STIPPLE,lineStipple);
      setEnable(dt,GL_MINMAX,minmax);
      setEnable(dt,GL_MULTISAMPLE,multisample);
      setEnable(dt,GL_NORMALIZE,normalize);
      setEnable(dt,GL_POINT_SMOOTH,pointSmooth);
      setEnable(dt,GL_POINT_SPRITE,pointSprite);
      setEnable(dt,GL_POLYGON_OFFSET_FILL,polygonOffsetFill);
      setEnable(dt,GL_POLYGON_OFFSET_LINE,polygonOffsetLine);
      setEnable(dt,GL_POLYGON_OFFSET_POINT,polygonOffsetPoint);
      setEnable(dt,GL_POLYGON_SMOOTH,polygonSmooth);
      setEnable(dt,GL_POLYGON_STIPPLE,polygonStipple);
      setEnable(dt,GL_POST_COLOR_MATRIX_COLOR_TABLE,postColorMatrixColorTable);
      setEnable(dt,GL_POST_CONVOLUTION_COLOR_TABLE,postConvolutionColorTable);
      setEnable(dt,GL_PROGRAM_POINT_SIZE,programPointSize);
      setEnable(dt,GL_RESCALE_NORMAL,rescaleNormal);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_COVERAGE,sampleAlphaToCoverage);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_ONE,sampleAlphaToOne);
      setEnable(dt,GL_SAMPLE_COVERAGE,sampleCoverage);
      setEnable(dt,GL_SAMPLE_SHADING,sampleShading);
      setEnable(dt,GL_SEPARABLE_2D,separable2d);
      setEnable(dt,GL_STENCIL_TEST,stencilTest);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        setEnablei(dt,GL_SCISSOR_TEST,ii,scissorTest[ii]);
      {
        for (GLuint ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        {
          setEnableIndexedEXT(dt,GL_TEXTURE_1D,ii,texture1d[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_2D,ii,texture2d[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_3D,ii,texture3d[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_CUBE_MAP,ii,textureCubeMap[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_S,ii,textureGenS[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_T,ii,textureGenT[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_R,ii,textureGenR[ii]);
          setEnableIndexedEXT(dt,GL_TEXTURE_GEN_Q,ii,textureGenQ[ii]);
        }
      }

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, alphaTest, "GL_ALPHA_TEST",delim);
      enableToString(tmp, autoNormal, "GL_AUTO_NORMAL",delim);
      for (GLuint ii=0; ii<REGAL_MAX_DRAW_BUFFERS; ii++)
        enableiToString(tmp, blend[ii], "GL_BLEND", ii,delim);
      tmp << print_string("glClampColor(GL_CLAMP_FRAGMENT_COLOR",Token::toString(clampFragmentColor),");",delim);
      tmp << print_string("glClampColor(GL_CLAMP_READ_COLOR",Token::toString(clampReadColor),");",delim);
      tmp << print_string("glClampColor(GL_CLAMP_VERTEX_COLOR",Token::toString(clampVertexColor),");",delim);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_CLIP_DISTANCES; ii++)
        tmp << print_string(clipDistance[ii] ? "glEnable" : "glDisable","(GL_CLIP_DISTANCE",ii,");",delim);
      enableToString(tmp, colorLogicOp, "GL_COLOR_LOGIC_OP",delim);
      enableToString(tmp, colorMaterial, "GL_COLOR_MATERIAL",delim);
      enableToString(tmp, colorSum, "GL_COLOR_SUM",delim);
      enableToString(tmp, colorTable, "GL_COLOR_TABLE",delim);
      enableToString(tmp, convolution1d, "GL_CONVOLUTION_1D",delim);
      enableToString(tmp, convolution2d, "GL_CONVOLUTION_2D",delim);
      enableToString(tmp, cullFace, "GL_CULL_FACE",delim);
      enableToString(tmp, depthTest, "GL_DEPTH_TEST",delim);
      enableToString(tmp, depthClamp, "GL_DEPTH_CLAMP",delim);
      enableToString(tmp, dither, "GL_DITHER",delim);
      enableToString(tmp, fog, "GL_FOG",delim);
      enableToString(tmp, framebufferSRGB, "GL_FRAMEBUFFER_SRGB",delim);
      enableToString(tmp, histogram, "GL_HISTOGRAM",delim);
      for (int ii=0; ii<REGAL_FIXED_FUNCTION_MAX_LIGHTS; ii++)
        tmp << print_string(light[ii] ? "glEnable" : "glDisable","(GL_LIGHT",ii,");",delim);
      enableToString(tmp, lighting, "GL_LIGHTING",delim);
      enableToString(tmp, lineSmooth, "GL_LINE_SMOOTH",delim);
      enableToString(tmp, lineStipple, "GL_LINE_STIPPLE",delim);
      enableToString(tmp, indexLogicOp, "GL_INDEX_LOGIC_OP",delim);
      enableToString(tmp, minmax, "GL_MINMAX",delim);
      enableToString(tmp, multisample, "GL_MULTISAMPLE",delim);
      enableToString(tmp, normalize, "GL_NORMALIZE",delim);
      enableToString(tmp, pointSmooth, "GL_POINT_SMOOTH",delim);
      enableToString(tmp, pointSprite, "GL_POINT_SPRITE",delim);
      enableToString(tmp, polygonOffsetLine, "GL_POLYGON_OFFSET_LINE",delim);
      enableToString(tmp, polygonOffsetFill, "GL_POLYGON_OFFSET_FILL",delim);
      enableToString(tmp, polygonOffsetPoint, "GL_POLYGON_OFFSET_POINT",delim);

      enableToString(tmp, polygonSmooth, "GL_POLYGON_SMOOTH",delim);
      enableToString(tmp, polygonStipple, "GL_POLYGON_STIPPLE",delim);
      enableToString(tmp, postConvolutionColorTable, "GL_POST_CONVOLUTION_COLOR_TABLE",delim);
      enableToString(tmp, postColorMatrixColorTable, "GL_POST_COLOR_MATRIX_COLOR_TABLE",delim);
      enableToString(tmp, programPointSize, "GL_PROGRAM_POINT_SIZE",delim);
      enableToString(tmp, rescaleNormal, "GL_RESCALE_NORMAL",delim);
      enableToString(tmp, sampleAlphaToCoverage, "GL_SAMPLE_ALPHA_TO_COVERAGE",delim);

      enableToString(tmp, sampleAlphaToOne, "GL_SAMPLE_ALPHA_TO_ONE",delim);
      enableToString(tmp, sampleCoverage, "GL_SAMPLE_COVERAGE",delim);
      enableToString(tmp, sampleShading, "GL_SAMPLE_SHADING",delim);
      enableToString(tmp, separable2d, "GL_SEPARABLE_2D",delim);
      enableToString(tmp, stencilTest, "GL_STENCIL_TEST",delim);
      enableToString(tmp, vertexProgramTwoSide, "GL_VERTEX_PROGRAM_TWO_SIDE",delim);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        enableiToString(tmp, scissorTest[ii], "GL_SCISSOR_TEST", ii,delim);
      {
        for (int ii=0; ii<REGAL_EMU_MAX_TEXTURE_UNITS; ii++)
        {
          tmp << print_string("Texture unit ",ii,": ",texture1d[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_1D);",delim);
          tmp << print_string("Texture unit ",ii,": ",texture2d[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_2D);",delim);
          tmp << print_string("Texture unit ",ii,": ",texture3d[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_3D);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureCubeMap[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_CUBE_MAP);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenS[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_S);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenT[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_T);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenR[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_R);",delim);
          tmp << print_string("Texture unit ",ii,": ",textureGenQ[ii] ? "glEnable" : "glDisable","(GL_TEXTURE_GEN_Q);",delim);
        }
      }
      // TODO: GL_MAP1_x where x is a map type (9 x B) (GL_FALSE)
      // TODO: GL_MAP2_x where x is a map type (9 x B) (GL_FALSE)
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_DEPTH_BUFFER_BIT)
  //

  struct Depth
  {
    GLboolean   enable;
    GLenum      func;
    GLclampd    clear;
    GLboolean   mask;

    inline Depth()
    : enable(GL_FALSE), func(GL_LESS), clear(1.0), mask(GL_TRUE)
    {
    }

    inline Depth &swap(Depth &other)
    {
      std::swap(enable,other.enable);
      std::swap(func,other.func);
      std::swap(clear,other.clear);
      std::swap(mask,other.mask);
      return *this;
    }

    inline void glClearDepthf(GLclampf depth)
    {
      clear = GLclampd(depth);
    }

    inline void glClearDepth(GLclampd depth)
    {
      clear = depth;
    }

    inline void glDepthFunc(GLenum f)
    {
      func = f;
    }

    inline void glDepthMask(GLboolean m)
    {
      mask = m;
    }

    inline Depth &get(DispatchTable &dt)
    {
      RegalAssert(dt.glIsEnabled);
      enable = dt.glIsEnabled(GL_DEPTH_TEST);

      RegalAssert(dt.glGetIntegerv);
      dt.glGetIntegerv(GL_DEPTH_FUNC,reinterpret_cast<GLint *>(&func));

      RegalAssert(dt.glGetFloatv);
      dt.glGetFloatv(GL_DEPTH_CLEAR_VALUE,reinterpret_cast<GLfloat *>(&clear));

      RegalAssert(dt.glGetBooleanv);
      dt.glGetBooleanv(GL_DEPTH_WRITEMASK,&mask);

      return *this;
    }

    inline const Depth &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glEnable);
      RegalAssert(dt.glDisable);
      if (enable)
        dt.glEnable(GL_DEPTH_TEST);
      else
        dt.glDisable(GL_DEPTH_TEST);

      RegalAssert(dt.glDepthFunc);
      dt.glDepthFunc(func);

      RegalAssert(dt.glClearDepth);
      dt.glClearDepth(clear);

      RegalAssert(dt.glDepthMask);
      dt.glDepthMask(mask);

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, enable, "GL_DEPTH_TEST", delim);
      tmp << print_string("glDepthfunc(",Token::toString(func),");",delim);
      tmp << print_string("glClearDepth(",clear,");",delim);
      tmp << print_string("glDepthMask(",mask ? "GL_TRUE" : "GL_FALSE",");",delim);
      return tmp;
    }
  };

  struct StencilFace
  {
    GLenum func;      // glStencilFunc
    GLint  ref;       // glStencilFunc
    GLuint valueMask; // glStencilFunc
    GLuint writeMask; // glStencilMask
    GLenum fail;      // glStencilOp
    GLenum zfail;     // glStencilOp
    GLenum zpass;     // glStencilOp

    inline StencilFace()
    : func     (GL_ALWAYS),
      ref      (0),
      valueMask(~0u),
      writeMask(~0u),
      fail     (GL_KEEP),
      zfail    (GL_KEEP),
      zpass    (GL_KEEP)
    {
    }

    inline StencilFace &swap(StencilFace &other)
    {
      std::swap(func,     other.func);
      std::swap(ref,      other.ref);
      std::swap(valueMask,other.valueMask);
      std::swap(writeMask,other.writeMask);
      std::swap(fail,     other.fail);
      std::swap(zfail,    other.zfail);
      std::swap(zpass,    other.zpass);
      return *this;
    }

    inline StencilFace &get(DispatchTable &dt, GLenum face)
    {
      RegalAssert(dt.glGetIntegerv);
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_FUNC            : GL_STENCIL_BACK_FUNC,            reinterpret_cast<GLint *>(&func)     );
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_REF             : GL_STENCIL_BACK_REF,             &ref                                 );
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_VALUE_MASK      : GL_STENCIL_BACK_VALUE_MASK,      reinterpret_cast<GLint *>(&valueMask));
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_WRITEMASK       : GL_STENCIL_BACK_WRITEMASK,       reinterpret_cast<GLint *>(&writeMask));
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_FAIL            : GL_STENCIL_BACK_FAIL,            reinterpret_cast<GLint *>(&fail)     );
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_PASS_DEPTH_FAIL : GL_STENCIL_BACK_PASS_DEPTH_FAIL, reinterpret_cast<GLint *>(&zfail)    );
      dt.glGetIntegerv(face==GL_FRONT ? GL_STENCIL_PASS_DEPTH_PASS : GL_STENCIL_BACK_PASS_DEPTH_PASS, reinterpret_cast<GLint *>(&zpass)    );
      return *this;
    }

    inline const StencilFace &set(DispatchTable &dt, GLenum face) const
    {
      RegalAssert(dt.glStencilFuncSeparate);
      dt.glStencilFuncSeparate(face,func,ref,valueMask);

      RegalAssert(dt.glStencilMaskSeparate);
      dt.glStencilMaskSeparate(face,writeMask);

      RegalAssert(dt.glStencilOpSeparate);
      dt.glStencilOpSeparate(face,fail,zfail,zpass);

      return *this;
    }

    inline std::string toString(GLenum face,const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glStencilFuncSeparate(",Token::toString(face),",",Token::toString(func),",",ref,",0x",hex(valueMask),");",delim);
      tmp << print_string("glStencilMaskSeparate(",Token::toString(face),",0x",hex(writeMask),");",delim);
      tmp << print_string("glStencilOpSeparate(",Token::toString(face),",",Token::toString(fail),",",Token::toString(zfail),",",Token::toString(zpass),");",delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_STENCIL_BUFFER_BIT)
  //

  struct Stencil
  {
    GLboolean   enable;
    GLint       clear;
    StencilFace front;
    StencilFace back;

    inline Stencil()
    : enable(GL_FALSE), clear(0)
    {
    }

    inline Stencil &swap(Stencil &other)
    {
      std::swap(enable,other.enable);
      std::swap(clear,other.clear);
      front.swap(other.front);
      back.swap(other.back);
      return *this;
    }

    inline void glClearStencil(GLint s)
    {
      clear = s;
    }

    inline void glStencilFunc(GLenum func, GLint ref, GLuint mask)
    {
      front.func      = back.func      = func;
      front.ref       = back.ref       = ref;
      front.valueMask = back.valueMask = mask;
    }

    inline void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
    {
      switch (face)
      {
        case GL_FRONT:
          front.func      = func;
          front.ref       = ref;
          front.valueMask = mask;
          break;
        case GL_BACK:
          back.func      = func;
          back.ref       = ref;
          back.valueMask = mask;
          break;
        case GL_FRONT_AND_BACK:
          front.func      = back.func      = func;
          front.ref       = back.ref       = ref;
          front.valueMask = back.valueMask = mask;
          break;
        default:
          RegalAssert(face==GL_FRONT || face==GL_BACK || face==GL_FRONT_AND_BACK);
          break;
      }
    }

    inline void glStencilMask(GLuint mask)
    {
      front.writeMask = back.writeMask = mask;
    }

    inline void glStencilMaskSeparate(GLenum face, GLuint mask)
    {
      switch (face)
      {
        case GL_FRONT:            front.writeMask = mask;                  break;
        case GL_BACK:             back.writeMask  = mask;                  break;
        case GL_FRONT_AND_BACK:   front.writeMask = back.writeMask = mask; break;
        default:
          RegalAssert(face==GL_FRONT || face==GL_BACK || face==GL_FRONT_AND_BACK);
          break;
      }
    }

    inline void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
    {
      front.fail  = back.fail  = fail;
      front.zfail = back.zfail = zfail;
      front.zpass = back.zpass = zpass;
    }

    inline void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
    {
      switch (face)
      {
        case GL_FRONT:
          front.fail  = fail;
          front.zfail = zfail;
          front.zpass = zpass;
          break;
        case GL_BACK:
          back.fail  = fail;
          back.zfail = zfail;
          back.zpass = zpass;
          break;
        case GL_FRONT_AND_BACK:
          front.fail  = back.fail  = fail;
          front.zfail = back.zfail = zfail;
          front.zpass = back.zpass = zpass;
          break;
        default:
          RegalAssert(face==GL_FRONT || face==GL_BACK || face==GL_FRONT_AND_BACK);
          break;
      }
    }

    inline Stencil &get(DispatchTable &dt)
    {
      RegalAssert(dt.glIsEnabled);
      enable = dt.glIsEnabled(GL_STENCIL_TEST);
      RegalAssert(dt.glGetIntegerv);
      dt.glGetIntegerv(GL_STENCIL_CLEAR_VALUE,&clear);
      front.get(dt,GL_FRONT);
      back .get(dt,GL_BACK );   // What about GL < 2.0 ?
      return *this;
    }

    inline const Stencil &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glEnable);
      RegalAssert(dt.glDisable);
      if (enable)
        dt.glEnable(GL_STENCIL_TEST);
      else
        dt.glDisable(GL_STENCIL_TEST);

      RegalAssert(dt.glClearStencil);
      dt.glClearStencil(clear);

      front.set(dt,GL_FRONT);
      back .set(dt,GL_BACK);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, enable, "GL_STENCIL_TEST", delim);
      tmp << print_string("glClearStencil(",clear,");",delim);
      tmp << front.toString(GL_FRONT,delim);
      tmp << front.toString(GL_BACK,delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_POLYGON_BIT)
  //

  struct Polygon
  {
    GLboolean   cullEnable;
    GLenum      cull;
    GLenum      frontFace;
    GLenum      frontMode;
    GLenum      backMode;
    GLboolean   smoothEnable;
    GLboolean   stippleEnable;
    GLboolean   offsetFill;
    GLboolean   offsetLine;
    GLboolean   offsetPoint;
    GLfloat     factor;
    GLfloat     units;

    inline Polygon()
    : cullEnable(GL_FALSE), cull(GL_BACK), frontFace(GL_CCW), frontMode(GL_FILL), backMode(GL_FILL),
      smoothEnable(GL_FALSE), stippleEnable(GL_FALSE), offsetFill(GL_FALSE), offsetLine(GL_FALSE), offsetPoint(GL_FALSE),
      factor(0), units(0)
    {
    }

    inline Polygon &swap(Polygon &other)
    {
      std::swap(cullEnable,other.cullEnable);
      std::swap(cull,other.cull);
      std::swap(frontFace,other.frontFace);
      std::swap(frontMode,other.frontMode);
      std::swap(backMode,other.backMode);
      std::swap(smoothEnable,other.smoothEnable);
      std::swap(stippleEnable,other.stippleEnable);
      std::swap(offsetFill,other.offsetFill);
      std::swap(offsetLine,other.offsetLine);
      std::swap(offsetPoint,other.offsetPoint);
      std::swap(factor,other.factor);
      std::swap(units,other.units);
      return *this;
    }

    inline void glCullFace(GLenum mode)
    {
      cull = mode;
    }

    inline void glFrontFace(GLenum mode)
    {
      frontFace = mode;
    }

    inline void glPolygonMode(GLenum f, GLenum mode)
    {
      switch (f)
      {
        case GL_FRONT:          frontMode = mode;            break;
        case GL_BACK:           backMode  = mode;            break;
        case GL_FRONT_AND_BACK: frontMode = backMode = mode; break;
        default:                                             break;
      }
    }

    inline void glPolygonOffset(GLfloat f, GLfloat u)
    {
      factor = f;
      units  = u;
    }

    inline Polygon &get(DispatchTable &dt)
    {
      RegalAssert(dt.glIsEnabled);
      cullEnable = dt.glIsEnabled(GL_CULL_FACE);

      RegalAssert(dt.glGetIntegerv);
      dt.glGetIntegerv(GL_CULL_FACE_MODE,reinterpret_cast<GLint *>(&cull));
      dt.glGetIntegerv(GL_FRONT_FACE,reinterpret_cast<GLint *>(&frontFace));
      dt.glGetIntegerv(GL_POLYGON_MODE,reinterpret_cast<GLint *>(&frontMode));
      dt.glGetIntegerv(GL_FRONT_FACE,reinterpret_cast<GLint *>(&frontFace));

      smoothEnable  = dt.glIsEnabled(GL_POLYGON_SMOOTH);
      stippleEnable = dt.glIsEnabled(GL_POLYGON_STIPPLE);
      offsetFill    = dt.glIsEnabled(GL_POLYGON_OFFSET_FILL);
      offsetLine    = dt.glIsEnabled(GL_POLYGON_OFFSET_LINE);
      offsetPoint   = dt.glIsEnabled(GL_POLYGON_OFFSET_POINT);

      RegalAssert(dt.glGetFloatv);
      dt.glGetFloatv(GL_POLYGON_OFFSET_FACTOR,&factor);
      dt.glGetFloatv(GL_POLYGON_OFFSET_UNITS,&units);

      return *this;
    }

    inline const Polygon &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glEnable);
      RegalAssert(dt.glDisable);
      if (cullEnable)
        dt.glEnable(GL_CULL_FACE);
      else
        dt.glDisable(GL_CULL_FACE);

      RegalAssert(dt.glCullFace);
      dt.glCullFace(cull);

      RegalAssert(dt.glFrontFace);
      dt.glFrontFace(frontFace);

      RegalAssert(dt.glPolygonMode);
      dt.glPolygonMode(GL_FRONT,frontMode);
      dt.glPolygonMode(GL_BACK,backMode);

      if (smoothEnable)
        dt.glEnable(GL_POLYGON_SMOOTH);
      else
        dt.glDisable(GL_POLYGON_SMOOTH);

      if (stippleEnable)
        dt.glEnable(GL_POLYGON_STIPPLE);
      else
        dt.glDisable(GL_POLYGON_STIPPLE);

      if (offsetFill)
        dt.glEnable(GL_POLYGON_OFFSET_FILL);
      else
        dt.glDisable(GL_POLYGON_OFFSET_FILL);

      if (offsetLine)
        dt.glEnable(GL_POLYGON_OFFSET_LINE);
      else
        dt.glDisable(GL_POLYGON_OFFSET_LINE);

      if (offsetPoint)
        dt.glEnable(GL_POLYGON_OFFSET_POINT);
      else
        dt.glDisable(GL_POLYGON_OFFSET_POINT);

      RegalAssert(dt.glPolygonOffset);
      dt.glPolygonOffset(factor,units);

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, cullEnable, "GL_CULL_FACE", delim);
      tmp << print_string("glCullFace(",Token::toString(cull),");",delim);
      tmp << print_string("glFrontFace(",Token::toString(frontFace),");",delim);
      tmp << print_string("glPolygonMode(GL_FRONT,",Token::toString(frontMode),");",delim);
      tmp << print_string("glPolygonMode(GL_BACK,",Token::toString(backMode),");",delim);
      enableToString(tmp, smoothEnable, "GL_POLYGON_SMOOTH", delim);
      enableToString(tmp, stippleEnable, "GL_POLYGON_STIPPLE", delim);
      enableToString(tmp, offsetFill, "GL_POLYGON_OFFSET_FILL", delim);
      enableToString(tmp, offsetLine, "GL_POLYGON_OFFSET_LINE", delim);
      enableToString(tmp, offsetPoint, "GL_POLYGON_OFFSET_POINT", delim);
      return tmp;
    }
  };

  //
  // glPushAttrib(GL_TRANSFORM_BIT)
  //

  struct ClipPlaneEquation
  {
    GLdouble data[4];

    inline ClipPlaneEquation()
    {
      data[0] = data[1] = data[2] = data[3] = 0;
    }

    bool operator!= (const ClipPlaneEquation& other) const
    {
      return (data[0] != other.data[0]) || (data[1] != other.data[1]) || (data[2] != other.data[2]) || (data[3] != other.data[3]);
    }
  };

  struct ClipPlane
  {
    GLboolean enabled;
    ClipPlaneEquation equation;

    inline ClipPlane()
    : enabled(false)
    {
    }

    inline ClipPlane &swap(ClipPlane &other)
    {
      std::swap(enabled,other.enabled);
      std::swap(equation,other.equation);
      return *this;
    }
  };

  struct Transform
  {
    // This state matches glspec43.compatability.20120806.pdf Table 23.10,
    // except possibly extended a bit to allow for extra clip planes.

    ClipPlane   clipPlane[REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES];
    GLenum      matrixMode;
    GLboolean   normalize;
    GLboolean   rescaleNormal;
    GLboolean   depthClamp;

    inline Transform()
    : matrixMode(GL_MODELVIEW), normalize(GL_FALSE), rescaleNormal(GL_FALSE), depthClamp(GL_FALSE)
    {
    }

    inline size_t maxPlanes() const
    {
      return sizeof(clipPlane)/sizeof(ClipPlane);
    }

    inline Transform &swap(Transform &other)
    {
      std::swap_ranges(clipPlane,clipPlane+REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES,other.clipPlane);
      std::swap(matrixMode,other.matrixMode);
      std::swap(normalize,other.normalize);
      std::swap(rescaleNormal,other.rescaleNormal);
      std::swap(depthClamp,other.depthClamp);
      return *this;
    }

    inline void glMatrixMode(GLenum mode)
    {
      matrixMode = mode;
    }

    inline void glClipPlane(GLenum plane, const GLdouble* equation)
    {
      GLuint planeIndex = plane - GL_CLIP_PLANE0;
      RegalAssert(planeIndex < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES);
      if (planeIndex < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES)
      {
        clipPlane[planeIndex].equation.data[0] = equation[0];
        clipPlane[planeIndex].equation.data[1] = equation[1];
        clipPlane[planeIndex].equation.data[2] = equation[2];
        clipPlane[planeIndex].equation.data[3] = equation[3];
      }
    }

    inline const Transform &transition(DispatchTable &dt, Transform& current) const
    {
      RegalAssert(dt.glEnable);
      RegalAssert(dt.glDisable);
      RegalAssert(dt.glMatrixMode);
      RegalAssert(dt.glClipPlane);

      for (GLint i = 0; i < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES; i++)
      {
        if (current.clipPlane[i].enabled != clipPlane[i].enabled)
          setEnable(dt, GL_CLIP_PLANE0 + i, clipPlane[i].enabled);

        if (current.clipPlane[i].equation != clipPlane[i].equation)
          dt.glClipPlane(GL_CLIP_PLANE0 + i, clipPlane[i].equation.data);
      }

      if (current.matrixMode != matrixMode)
        dt.glMatrixMode(matrixMode);

      if (current.normalize != normalize)
        setEnable(dt, GL_NORMALIZE, normalize);

      if (current.rescaleNormal != rescaleNormal)
        setEnable(dt, GL_RESCALE_NORMAL, rescaleNormal);

      if (current.depthClamp != depthClamp )
        setEnable(dt, GL_DEPTH_CLAMP, depthClamp );

      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      for (GLint i = 0; i < REGAL_FIXED_FUNCTION_MAX_CLIP_PLANES; i++)
      {
        GLenum plane = GL_CLIP_PLANE0 + i;
        tmp << print_string(clipPlane[i].enabled ? "glEnable(" : "glDisable(",Token::toString(plane),")",delim);
        tmp << print_string("glClipPlane(",Token::toString(plane),clipPlane[i].equation.data[0],clipPlane[i].equation.data[1],clipPlane[i].equation.data[2],clipPlane[i].equation.data[3],")",delim);
      }
      tmp << print_string("glMatrixMode(",Token::toString(matrixMode),");",delim);
      enableToString(tmp, normalize, "GL_NORMALIZE", delim);
      enableToString(tmp, rescaleNormal, "GL_RESCALE_NORMAL", delim);
      enableToString(tmp, depthClamp, "GL_DEPTH_CLAMP", delim);
      return tmp;
    }
  };

  struct Hint
  {
    GLenum perspectiveCorrection;
    GLenum pointSmooth;
    GLenum lineSmooth;
    GLenum polygonSmooth;
    GLenum fog;
    GLenum generateMipmap;
    GLenum textureCompression;
    GLenum fragmentShaderDerivative;

    inline Hint()
    : perspectiveCorrection(GL_DONT_CARE)
    , pointSmooth(GL_DONT_CARE)
    , lineSmooth(GL_DONT_CARE)
    , polygonSmooth(GL_DONT_CARE)
    , fog(GL_DONT_CARE)
    , generateMipmap(GL_DONT_CARE)
    , textureCompression(GL_DONT_CARE)
    , fragmentShaderDerivative(GL_DONT_CARE)
    {
    }

    inline Hint &swap(Hint &other)
    {
      std::swap(perspectiveCorrection, other.perspectiveCorrection);
      std::swap(pointSmooth, other.pointSmooth);
      std::swap(lineSmooth, other.lineSmooth);
      std::swap(polygonSmooth, other.polygonSmooth);
      std::swap(fog, other.fog);
      std::swap(generateMipmap, other.generateMipmap);
      std::swap(textureCompression, other.textureCompression);
      std::swap(fragmentShaderDerivative, other.fragmentShaderDerivative);
      return *this;
    }

    inline void glHint(GLenum target, GLenum mode)
    {
      switch (target)
      {
        case GL_PERSPECTIVE_CORRECTION_HINT: perspectiveCorrection = mode; break;
        case GL_POINT_SMOOTH_HINT: pointSmooth = mode; break;
        case GL_LINE_SMOOTH_HINT: lineSmooth = mode; break;
        case GL_POLYGON_SMOOTH_HINT: polygonSmooth = mode; break;
        case GL_FOG_HINT: fog = mode; break;
        case GL_GENERATE_MIPMAP_HINT: generateMipmap = mode; break;
        case GL_TEXTURE_COMPRESSION_HINT: textureCompression = mode; break;
        case GL_FRAGMENT_SHADER_DERIVATIVE_HINT: fragmentShaderDerivative = mode; break;
        default: break;
      }
    }

    inline Hint &get(DispatchTable &dt)
    {
      RegalAssert(dt.glGetIntegerv);
      dt.glGetIntegerv(GL_PERSPECTIVE_CORRECTION_HINT,reinterpret_cast<GLint *>(&perspectiveCorrection));
      dt.glGetIntegerv(GL_POINT_SMOOTH_HINT,reinterpret_cast<GLint *>(&pointSmooth));
      dt.glGetIntegerv(GL_LINE_SMOOTH_HINT,reinterpret_cast<GLint *>(&lineSmooth));
      dt.glGetIntegerv(GL_POLYGON_SMOOTH_HINT,reinterpret_cast<GLint *>(&polygonSmooth));
      dt.glGetIntegerv(GL_FOG_HINT,reinterpret_cast<GLint *>(&fog));
      dt.glGetIntegerv(GL_GENERATE_MIPMAP_HINT,reinterpret_cast<GLint *>(&generateMipmap));
      dt.glGetIntegerv(GL_TEXTURE_COMPRESSION_HINT,reinterpret_cast<GLint *>(&textureCompression));
      dt.glGetIntegerv(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,reinterpret_cast<GLint *>(&fragmentShaderDerivative));
      return *this;
    }

    inline const Hint &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glHint);
      dt.glHint(GL_PERSPECTIVE_CORRECTION_HINT, perspectiveCorrection);
      dt.glHint(GL_POINT_SMOOTH_HINT, pointSmooth);
      dt.glHint(GL_LINE_SMOOTH_HINT, lineSmooth);
      dt.glHint(GL_POLYGON_SMOOTH_HINT, polygonSmooth);
      dt.glHint(GL_FOG_HINT, fog);
      dt.glHint(GL_GENERATE_MIPMAP_HINT, generateMipmap);
      dt.glHint(GL_TEXTURE_COMPRESSION_HINT, textureCompression);
      dt.glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, fragmentShaderDerivative);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glHint(GL_PERSPECTIVE_CORRECTION_HINT,",perspectiveCorrection,");",delim);
      tmp << print_string("glHint(GL_POINT_SMOOTH_HINT,",pointSmooth,");",delim);
      tmp << print_string("glHint(GL_LINE_SMOOTH_HINT,",lineSmooth,");",delim);
      tmp << print_string("glHint(GL_POLYGON_SMOOTH_HINT,",polygonSmooth,");",delim);
      tmp << print_string("glHint(GL_FOG_HINT,",fog,");",delim);
      tmp << print_string("glHint(GL_GENERATE_MIPMAP_HINT,",generateMipmap,");",delim);
      tmp << print_string("glHint(GL_TEXTURE_COMPRESSION_HINT,",textureCompression,");",delim);
      tmp << print_string("glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,",fragmentShaderDerivative,");",delim);
      return tmp;
    }
  };

  struct List
  {
    GLuint base;

    inline List()
    : base(0)
    {
    }

    inline List &swap(List &other)
    {
      std::swap(base,other.base);
      return *this;
    }

    void glListBase( GLuint b )
    {
      base = b;
    }

    inline List &get(DispatchTable &dt)
    {
      RegalAssert(dt.glGetIntegerv);
      dt.glGetIntegerv(GL_LIST_BASE,reinterpret_cast<GLint *>(&base));
      return *this;
    }

    inline const List &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glStencilFuncSeparate);
      dt.glListBase(base);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glListBase(",base,");",delim);
      return tmp;
    }
  };

  struct AccumBuffer
  {
    GLfloat clear[4];

    inline AccumBuffer()
    {
      clear[0] = clear[1] = clear[2] = clear[3] = 0;
    }

    inline AccumBuffer &swap(AccumBuffer &other)
    {
      std::swap_ranges(clear,clear+4,other.clear);
      return *this;
    }

    void glClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
      clear[0] = r;
      clear[1] = g;
      clear[2] = b;
      clear[3] = a;
    }

    inline AccumBuffer &get(DispatchTable &dt)
    {
      RegalAssert(dt.glGetFloatv);
      dt.glGetFloatv(GL_ACCUM_CLEAR_VALUE,&(clear[0]));
      return *this;
    }

    inline const AccumBuffer &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glClearAccum);
      dt.glClearAccum(clear[0],clear[1],clear[2],clear[3]);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glClearAccum(",clear[0],",",clear[1],",",clear[2],",",clear[3],");",delim);
      return tmp;
    }
  };

  struct Scissor
  {
    GLboolean   scissorTest[REGAL_MAX_VIEWPORTS];   // GL_SCISSOR_TEST
    GLint       scissorBox[REGAL_MAX_VIEWPORTS][4]; // GL_SCISSOR_BOX

    inline Scissor()
    {
      std::memset(scissorTest,GL_FALSE,sizeof(scissorTest));
      std::memset(scissorBox,0,sizeof(scissorBox));
    }

    inline Scissor(const Scissor &other)
    {
      if (this!=&other)
        std::memcpy(this,&other,sizeof(Scissor));
    }

    inline Scissor &operator=(const Scissor &other)
    {
      if (this!=&other)
        std::memcpy(this,&other,sizeof(Scissor));
      return *this;
    }

    inline Scissor &swap(Scissor &other)
    {
      *this = other;
      return *this;
    }

    void glScissorArrayv( GLuint first, GLsizei count, const GLint *v )
    {
      GLuint last = first + count;
      if (last > REGAL_MAX_VIEWPORTS)
        last = REGAL_MAX_VIEWPORTS;
      for (GLuint ii = first; ii < last; ii++)
      {
        scissorBox[ii][0] = v[0];
        scissorBox[ii][1] = v[1];
        scissorBox[ii][2] = v[2];
        scissorBox[ii][3] = v[3];
        v += 4;
      }
    }

    void glScissorIndexed( GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        scissorBox[index][0] = left;
        scissorBox[index][1] = bottom;
        scissorBox[index][2] = width;
        scissorBox[index][3] = height;
      }
    }

    void glScissorIndexedv( GLuint index, const GLint *v )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        scissorBox[index][0] = v[0];
        scissorBox[index][1] = v[1];
        scissorBox[index][2] = v[2];
        scissorBox[index][3] = v[3];
      }
    }

    void glScissor( GLint left, GLint bottom, GLsizei width, GLsizei height )
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        scissorBox[ii][0] = left;
        scissorBox[ii][1] = bottom;
        scissorBox[ii][2] = width;
        scissorBox[ii][3] = height;
      }
    }

    inline Scissor &get(DispatchTable &dt)
    {
      RegalAssert(dt.glIsEnabledi);
      RegalAssert(dt.glGetIntegeri_v);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
      {
        scissorTest[ii] = dt.glIsEnabledi(GL_SCISSOR_TEST,ii);
        dt.glGetIntegeri_v(GL_SCISSOR_BOX, ii, &scissorBox[ii][0]);
      }
      return *this;
    }

    inline const Scissor &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glScissorArrayv);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        setEnablei(dt,GL_SCISSOR_TEST,ii,scissorTest[ii]);
      dt.glScissorArrayv(0, REGAL_MAX_VIEWPORTS, &scissorBox[0][0]);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        enableiToString(tmp, scissorTest[ii], "GL_SCISSOR_TEST", ii, delim);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        tmp << print_string("glScissorIndexed(",ii,",",scissorBox[ii][0],",",scissorBox[ii][1],",",scissorBox[ii][2],",",scissorBox[ii][3],");",delim);
      return tmp;
    }
  };

  struct Viewport
  {
    GLfloat  viewport[REGAL_MAX_VIEWPORTS][4]; // GL_VIEWPORT
    GLclampd depthRange[REGAL_MAX_VIEWPORTS][2]; // GL_DEPTH_RANGE

    inline Viewport()
    {
      std::memset(viewport,0,sizeof(viewport));
      std::memset(depthRange,0,sizeof(depthRange));
    }

    inline Viewport &swap(Viewport &other)
    {
      std::swap_ranges(&viewport[0][0],&viewport[0][0]+(REGAL_MAX_VIEWPORTS*4),&other.viewport[0][0]);
      std::swap_ranges(&depthRange[0][0],&depthRange[0][0]+(REGAL_MAX_VIEWPORTS*2),&other.depthRange[0][0]);
      return *this;
    }

    void glViewportArrayv( GLuint first, GLsizei count, const GLfloat *v )
    {
      GLuint last = first + count;
      if (last > REGAL_MAX_VIEWPORTS)
        last = REGAL_MAX_VIEWPORTS;
      for (GLuint ii = first; ii < last; ii++)
      {
        viewport[ii][0] = v[0];
        viewport[ii][1] = v[1];
        viewport[ii][2] = v[2];
        viewport[ii][3] = v[3];
        v += 4;
      }
    }

    void glViewportIndexedf( GLuint index, GLfloat x, GLfloat y, GLfloat w, float h )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        viewport[index][0] = x;
        viewport[index][1] = y;
        viewport[index][2] = w;
        viewport[index][3] = h;
      }
    }

    void glViewportIndexedfv( GLuint index, const GLfloat *v )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        viewport[index][0] = v[0];
        viewport[index][1] = v[1];
        viewport[index][2] = v[2];
        viewport[index][3] = v[3];
      }
    }

    void glViewport( GLint x, GLint y, GLsizei w, GLsizei h )
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        viewport[ii][0] = static_cast<GLfloat>(x);
        viewport[ii][1] = static_cast<GLfloat>(y);
        viewport[ii][2] = static_cast<GLfloat>(w);
        viewport[ii][3] = static_cast<GLfloat>(h);
      }
    }

    void glDepthRangeArrayv( GLuint first, GLsizei count, const GLdouble *v )
    {
      GLuint last = first + count;
      if (last > REGAL_MAX_VIEWPORTS)
        last = REGAL_MAX_VIEWPORTS;
      for (GLuint ii = first; ii < last; ii++)
      {
        depthRange[ii][0] = v[0];
        depthRange[ii][1] = v[1];
        v += 2;
      }
    }

    void glDepthRangeIndexed( GLuint index, GLdouble n, GLdouble f )
    {
      if (index < REGAL_MAX_VIEWPORTS)
      {
        depthRange[index][0] = n;
        depthRange[index][1] = f;
      }
    }

    void glDepthRange( GLdouble n, GLdouble f )
    {
      for (GLuint ii = 0; ii < REGAL_MAX_VIEWPORTS; ii++)
      {
        depthRange[ii][0] = n;
        depthRange[ii][1] = f;
      }
    }

    void glDepthRangef( GLfloat n, GLfloat f )
    {
      glDepthRange( n, f );
    }

    inline Viewport &get(DispatchTable &dt)
    {
      RegalAssert(dt.glGetFloati_v);
      RegalAssert(dt.glGetDoublei_v);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
      {
        dt.glGetFloati_v(GL_VIEWPORT, ii, &viewport[ii][0]);
        dt.glGetDoublei_v(GL_DEPTH_RANGE, ii, &depthRange[ii][0]);
      }
      return *this;
    }

    inline const Viewport &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glDepthRangeArrayv);
      RegalAssert(dt.glViewportArrayv);
      dt.glDepthRangeArrayv(0, REGAL_MAX_VIEWPORTS, &depthRange[0][0] );
      dt.glViewportArrayv(0, REGAL_MAX_VIEWPORTS, &viewport[0][0] );
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        tmp << print_string("glViewportIndexedf(",ii,",",viewport[ii][0],",",viewport[ii][1],",",viewport[ii][2],",",viewport[ii][3],");",delim);
      for (GLuint ii=0; ii<REGAL_MAX_VIEWPORTS; ii++)
        tmp << print_string("glDepthRangeIndexed(",ii,",",depthRange[ii][0],",",depthRange[ii][1],");",delim);
      return tmp;
    }
  };

  struct Line
  {
    GLfloat     width;          // GL_LINE_WIDTH
    GLboolean   smooth;         // GL_LINE_SMOOTH
    GLboolean   stipple;        // GL_LINE_STIPPLE
    GLint       stippleRepeat;  // GL_LINE_STIPPLE_REPEAT
    GLushort    stipplePattern; // GL_LINE_STIPPLE_PATTERN

    inline Line()
    : width(1.0)
    , smooth(GL_FALSE)
    , stipple(GL_FALSE)
    , stippleRepeat(1)
    , stipplePattern(static_cast<GLushort>(~0))
    {
    }

    inline Line &swap(Line &other)
    {
      std::swap(width,other.width);
      std::swap(smooth,other.smooth);
      std::swap(stipple,other.stipple);
      std::swap(stippleRepeat,other.stippleRepeat);
      std::swap(stipplePattern,other.stipplePattern);
      return *this;
    }

    void glLineWidth(GLfloat w)
    {
      width = w;
    }

    void glLineStipple( GLint repeat, GLushort pattern )
    {
      stippleRepeat = repeat;
      stipplePattern = pattern;
    }

    inline Line &get(DispatchTable &dt)
    {
      RegalAssert(dt.glGetFloatv);
      RegalAssert(dt.glIsEnabled);
      RegalAssert(dt.glGetIntegerv);
      dt.glGetFloatv(GL_LINE_WIDTH,&width);
      smooth = dt.glIsEnabled(GL_LINE_SMOOTH);
      stipple = dt.glIsEnabled(GL_LINE_STIPPLE);
      dt.glGetIntegerv(GL_LINE_STIPPLE_PATTERN,&stippleRepeat);
      stipplePattern = static_cast<GLushort>(stippleRepeat);
      dt.glGetIntegerv(GL_LINE_STIPPLE_REPEAT,&stippleRepeat);
      return *this;
    }

    inline const Line &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glLineWidth);
      RegalAssert(dt.glLineStipple);
      dt.glLineWidth(width);
      setEnable(dt,GL_LINE_SMOOTH,smooth);
      setEnable(dt,GL_LINE_STIPPLE,stipple);
      dt.glLineStipple(stippleRepeat, stipplePattern);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      tmp << print_string("glLineWidth(",width,");",delim);
      enableToString(tmp, smooth, "GL_LINE_SMOOTH", delim);
      enableToString(tmp, stipple, "GL_LINE_STIPPLE", delim);
      tmp << print_string("glLineStipple(",stippleRepeat,",0x",hex(stipplePattern),");",delim);
      return tmp;
    }
  };

  struct Multisample
  {
    GLboolean   multisample;               // GL_MULTISAMPLE
    GLboolean   sampleAlphaToCoverage;     // GL_SAMPLE_ALPHA_TO_COVERAGE
    GLboolean   sampleAlphaToOne;          // GL_SAMPLE_ALPHA_TO_ONE
    GLboolean   sampleCoverage;            // GL_SAMPLE_COVERAGE
    GLclampf    sampleCoverageValue;       // GL_SAMPLE_COVERAGE_VALUE
    GLboolean   sampleCoverageInvert;      // GL_SAMPLE_COVERAGE_INVERT
    GLboolean   sampleShading;             // GL_SAMPLE_SHADING
    GLfloat     minSampleShadingValue;     // GL_MIN_SAMPLE_SHADING_VALUE

    inline Multisample()
    : multisample(GL_TRUE)
    , sampleAlphaToCoverage(GL_FALSE)
    , sampleAlphaToOne(GL_FALSE)
    , sampleCoverage(GL_FALSE)
    , sampleCoverageValue(1)
    , sampleCoverageInvert(GL_FALSE)
    , sampleShading(GL_FALSE)
    , minSampleShadingValue(0)
    {
    }

    inline Multisample &swap(Multisample &other)
    {
      std::swap(multisample,other.multisample);
      std::swap(sampleAlphaToCoverage,other.sampleAlphaToCoverage);
      std::swap(sampleAlphaToOne,other.sampleAlphaToOne);
      std::swap(sampleCoverage,other.sampleCoverage);
      std::swap(sampleCoverageValue,other.sampleCoverageValue);
      std::swap(sampleCoverageInvert,other.sampleCoverageInvert);
      std::swap(sampleShading,other.sampleShading);
      std::swap(minSampleShadingValue,other.minSampleShadingValue);
      return *this;
    }

    void glSampleCoverage( GLfloat value, GLboolean invert )
    {
      sampleCoverageValue = value;
      sampleCoverageInvert = invert;
    }

    void glMinSampleShading( GLfloat value )
    {
      minSampleShadingValue = value;
    }

    inline Multisample &get(DispatchTable &dt)
    {
      RegalAssert(dt.glIsEnabled);
      RegalAssert(dt.glGetFloatv);
      multisample = dt.glIsEnabled(GL_MULTISAMPLE);
      sampleAlphaToCoverage = dt.glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
      sampleAlphaToOne = dt.glIsEnabled(GL_SAMPLE_ALPHA_TO_ONE);
      sampleCoverage = dt.glIsEnabled(GL_SAMPLE_COVERAGE);
      dt.glGetFloatv(GL_SAMPLE_COVERAGE_VALUE,&sampleCoverageValue);
      sampleCoverageInvert = dt.glIsEnabled(GL_SAMPLE_COVERAGE_INVERT);
      sampleShading = dt.glIsEnabled(GL_SAMPLE_SHADING);
      dt.glGetFloatv(GL_MIN_SAMPLE_SHADING_VALUE,&minSampleShadingValue);
      return *this;
    }

    inline const Multisample &set(DispatchTable &dt) const
    {
      RegalAssert(dt.glSampleCoverage);
      RegalAssert(dt.glMinSampleShading);
      setEnable(dt,GL_MULTISAMPLE,multisample);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_COVERAGE,sampleAlphaToCoverage);
      setEnable(dt,GL_SAMPLE_ALPHA_TO_ONE,sampleAlphaToOne);
      setEnable(dt,GL_SAMPLE_COVERAGE,sampleCoverage);
      dt.glSampleCoverage(sampleCoverageValue, sampleCoverageInvert);
      setEnable(dt,GL_SAMPLE_SHADING,sampleShading);
      dt.glMinSampleShading(minSampleShadingValue);
      return *this;
    }

    inline std::string toString(const char *delim = "\n") const
    {
      string_list tmp;
      enableToString(tmp, multisample, "GL_MULTISAMPLE", delim);
      enableToString(tmp, sampleAlphaToCoverage, "GL_SAMPLE_ALPHA_TO_COVERAGE", delim);
      enableToString(tmp, sampleAlphaToOne, "GL_SAMPLE_ALPHA_TO_ONE", delim);
      enableToString(tmp, sampleCoverage, "GL_SAMPLE_COVERAGE", delim);
      tmp << print_string("glSampleCoverage(",sampleCoverageValue,",",sampleCoverageInvert,");",delim);
      enableToString(tmp, sampleShading, "GL_SAMPLE_SHADING", delim);
      tmp << print_string("glMinSampleShading(",minSampleShadingValue,");",delim);
      return tmp;
    }
  };
}

REGAL_NAMESPACE_END

#endif
