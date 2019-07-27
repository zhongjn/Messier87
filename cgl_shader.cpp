/*************************************************************************\

  Copyright 2007 Zhejiang University.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
   fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL ZHEJIANG UNIVERSITY HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  ZHEJIANG UNIVERSITY SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  Authors:  Min Tang, Ruo-feng Tong
  EMail:    {tang_m, trf}@zju.edu.cn

\**************************************************************************/


#include <iostream>
#include <fstream>
#include <assert.h>
#include "cgl_shader.h"

using namespace std;

void checkGLError()
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        printf("error - %s\n", (char*)gluErrorString(error));
    }
}

CGLShader::CGLShader(const char* vfile, const char* ffile)
{
    if (vfile) cout << "loading vertex shader: " << vfile << endl;
    if (ffile) cout << "loading fragment shader: " << ffile << endl;

    programObject = glCreateProgramObjectARB();

    // define search path for GLSL shaders file

    if (vfile) {
        string filename = vfile;
        if (filename == "") {
            printf("Unable to load %s, exiting...\n", vfile);
            cleanExit(-1);
        }

        std::ifstream in(filename);
        std::string contents((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());

        addShader(contents.c_str(), GL_VERTEX_SHADER_ARB);
    }

    if (ffile) {
        string filename = ffile;
        if (filename == "")
        {
            printf("Unable to load %s, exiting...\n", ffile);
            cleanExit(-1);
        }

        std::ifstream in(filename);
        std::string contents((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());

        addShader(contents.c_str(), GL_FRAGMENT_SHADER_ARB);
    }

    // make sure position attrib is bound to location 0
    //## glBindAttribLocationARB(programObject, 0, "position");

    glLinkProgramARB(programObject);

    GLint linked = false;
    glGetObjectParameterivARB(programObject, GL_OBJECT_LINK_STATUS_ARB, &linked);
    if (!linked) {
        printInfoLog(programObject);
        cout << "Shaders failed to link, exiting..." << endl;
        cleanExit(-1);
    }
    
    glValidateProgramARB(programObject);

    GLint validated = false;
    glGetObjectParameterivARB(programObject, GL_OBJECT_VALIDATE_STATUS_ARB, &validated);
    if (!validated) {
        printInfoLog(programObject);
        cout << "Shaders failed to validate, exiting..." << endl;
        cleanExit(-1);
    }
}

CGLShader::~CGLShader()
{
    if (programObject)
        glDeleteObjectARB(programObject);
}

void CGLShader::cleanExit(int exitval)
{
    if (programObject)
        glDeleteObjectARB(programObject);

    if (exitval == 0) { exit(0); }
    else { exit(exitval); }
}

void CGLShader::printInfoLog(GLhandleARB object)
{
    int maxLength = 0;
    glGetObjectParameterivARB(object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

    char* infoLog = new char[maxLength];
    glGetInfoLogARB(object, maxLength, &maxLength, infoLog);

    cout << infoLog << endl;
}

void CGLShader::addShader(const GLcharARB * shaderSource, GLenum shaderType)
{
    assert(programObject != 0);
    assert(shaderSource != 0);
    assert(shaderType != 0);

    GLhandleARB object = glCreateShaderObjectARB(shaderType);
    assert(object != 0);

    glShaderSourceARB(object, 1, &shaderSource, NULL);

    // compile vertex shader object
    glCompileShaderARB(object);

    // check if shader compiled
    GLint compiled = 0;
    glGetObjectParameterivARB(object, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

    if (!compiled)
    {
        printInfoLog(object);
    }

    // attach vertex shader to program object
    glAttachObjectARB(programObject, object);

    // delete vertex object, no longer needed
    glDeleteObjectARB(object);
    
    checkGLError();
}

void CGLShader::enable() {
    glUseProgramObjectARB(programObject);
}

void CGLShader::disable() {
    glUseProgramObjectARB(0);
}

