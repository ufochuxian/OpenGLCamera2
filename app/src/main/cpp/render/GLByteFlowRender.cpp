//
// Created by ByteFlow on 2019/4/15.
//

#include <LogUtil.h>
#include <gtc/matrix_transform.hpp>
#include <unistd.h>
#include "GLByteFlowRender.h"
#include "GLUtils.h"
#include "GLShaders.h"

static const float VERTICES_COORS[] =
		{
				-1.f, 1.f,
				-1.f, -1.f,
				1.f, 1.f,
				1.f, -1.f
		};

static const float TEXTURE_COORS[] =
		{
				0, 0,
				0, 1,
				1, 0,
				1, 1
		};

GLByteFlowRender::GLByteFlowRender() :
		m_Program(0),
		m_VertexShader(0),
		m_FragShader(0),
		m_YTextureId(0),
		m_UTextureId(0),
		m_VTextureId(0),
		m_VertexCoorHandle(0),
		m_TextureCoorHandle(0),
		m_YTextureHandle(0),
		m_UTextureHandle(0),
		m_VTextureHandle(0),
		m_TextureSizeHandle(0),
		m_MVPHandle(0),
		m_MVPMatrix(1.0f),
		m_ShaderIndex(0)
{
	LOGCATE("GLByteFlowRender::GLByteFlowRender");
	m_IsProgramChanged = true;
	m_IsProgramChanged = false;
}

GLByteFlowRender::~GLByteFlowRender()
{
	LOGCATE("GLByteFlowRender::~GLByteFlowRender");

}

int GLByteFlowRender::Init(int initType)
{
	LOGCATE("GLByteFlowRender::Init");
	m_FragShaders.push_back(kFragmentShader);
	m_FragShaders.push_back(kFragmentShader1);
	m_FragShaders.push_back(kFragmentShader2);
	m_FragShaders.push_back(kFragmentShader3);
	m_FragShaders.push_back(kFragmentShader4);
	m_FragShaders.push_back(kFragmentShader5);
	m_FragShaders.push_back(kFragmentShader6);
	m_FragShaders.push_back(kFragmentShader7);
	m_FragShaders.push_back(kFragmentShader8);
	m_FragShaders.push_back(kFragmentShader9);
	m_FragShaders.push_back(kFragmentShader10);
	m_FragShaders.push_back(kFragmentShader11);
	m_FragShaders.push_back(kFragmentShader12);
	return 0;
}

int GLByteFlowRender::UnInit()
{
	LOGCATE("GLByteFlowRender::UnInit");
	ByteFlowFrameUtil::FreeFrame(&m_RenderBufFrame);
	//DeleteTextures();
	//GLUtils::DeleteProgram(m_Program);
	m_FragShaders.clear();

	return 0;
}

void GLByteFlowRender::UpdateFrame(uint8_t *pBuffer, int width, int height)
{
	LOGCATE("GLByteFlowRender::UpdateFrame");
	ByteFlowFrame frame;
	frame.width = width;
	frame.height = height;
	frame.yPitch = width;
	frame.uPitch = width / 2;
	frame.vPitch = width / 2;
	frame.pYPlane = pBuffer;
	frame.pUPlane = frame.pYPlane + width * height;
	frame.pVPlane = frame.pUPlane + width * height / 4;

	if (frame.width != m_RenderBufFrame.width || frame.height != m_RenderBufFrame.height)
	{
		if (m_RenderBufFrame.pYPlane != NULL)
		{
			ByteFlowFrameUtil::FreeFrame(&m_RenderBufFrame);
		}
		memset(&m_RenderBufFrame, 0, sizeof(ByteFlowFrame));
		m_RenderBufFrame.width = width;
		m_RenderBufFrame.height = height;
		ByteFlowFrameUtil::AllocFrame(&m_RenderBufFrame);
	}

	ByteFlowFrameUtil::CopyFrame(&frame, &m_RenderBufFrame);

}

void GLByteFlowRender::SetTransformMatrix(float translateX, float translateY, float scaleX, float scaleY, int degree, int mirror)
{
	LOGCATE("GLByteFlowRender::SetTransformMatrix translateX = %f, translateY = %f, scaleX = %f, scaleY = %f, degree = %d, mirror = %d",
			translateX, translateY, scaleX, scaleY, degree, mirror);

	m_TransformMatrix.degree = degree;
	m_TransformMatrix.mirror = mirror;
	m_IsProgramChanged = true;

}

void GLByteFlowRender::SetShaderIndex(int shaderIndex)
{
	LOGCATE("GLByteFlowRender::SetShaderIndex shaderIndex = %d", shaderIndex);
	m_ShaderIndex = shaderIndex;
	m_IsShaderChanged = true;

}

int GLByteFlowRender::GetShaderIndex()
{
	LOGCATE("GLByteFlowRender::GetShaderIndex");
	return m_ShaderIndex;
}

bool GLByteFlowRender::CreateTextures()
{
	LOGCATE("GLByteFlowRender::CreateTextures");
	GLsizei yWidth = static_cast<GLsizei>(m_RenderBufFrame.yPitch);
	GLsizei yHeight = static_cast<GLsizei>(m_RenderBufFrame.height);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_YTextureId);
	glBindTexture(GL_TEXTURE_2D, m_YTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yWidth, yHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
				 NULL);

	if (!m_YTextureId)
	{
		GLUtils::CheckGLError("Create Y texture");
		return false;
	}

	GLsizei uWidth = static_cast<GLsizei>(m_RenderBufFrame.uPitch);
	GLsizei uHeight = yHeight / 2;

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &m_UTextureId);
	glBindTexture(GL_TEXTURE_2D, m_UTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, uWidth, uHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
				 NULL);

	if (!m_UTextureId)
	{
		GLUtils::CheckGLError("Create U texture");
		return false;
	}

	GLsizei vWidth = static_cast<GLsizei>(m_RenderBufFrame.vPitch);
	GLsizei vHeight = (GLsizei) yHeight / 2;

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &m_VTextureId);
	glBindTexture(GL_TEXTURE_2D, m_VTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, vWidth, vHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
				 NULL);

	if (!m_VTextureId)
	{
		GLUtils::CheckGLError("Create V texture");
		return false;
	}

	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &m_MaskTextureId);
	glBindTexture(GL_TEXTURE_2D, m_MaskTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool GLByteFlowRender::UpdateTextures()
{
	LOGCATE("GLByteFlowRender::UpdateTextures");
	if (m_RenderBufFrame.pYPlane == NULL)
	{
		return false;
	}

	if (!m_YTextureId && !m_UTextureId && !m_VTextureId && !CreateTextures())
	{
		return false;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_YTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_RenderBufFrame.width, (GLsizei)m_RenderBufFrame.height, 0,
				 GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderBufFrame.pYPlane);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_UTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_RenderBufFrame.width >> 1, (GLsizei)m_RenderBufFrame.height >> 1, 0,
				 GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderBufFrame.pUPlane);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_VTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, (GLsizei)m_RenderBufFrame.width >> 1, (GLsizei)m_RenderBufFrame.height >> 1, 0,
				 GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderBufFrame.pVPlane);

	return true;
}

bool GLByteFlowRender::DeleteTextures()
{
	LOGCATE("GLByteFlowRender::DeleteTextures");
	if (m_YTextureId)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_YTextureId);

		m_YTextureId = 0;
	}

	if (m_UTextureId)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_UTextureId);

		m_UTextureId = 0;
	}

	if (m_VTextureId)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &m_VTextureId);

		m_VTextureId = 0;
	}
	return true;
}

void GLByteFlowRender::OnSurfaceCreated()
{
	LOGCATE("GLByteFlowRender::OnSurfaceCreated");
	int nMaxTextureSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &nMaxTextureSize);
	LOGCATE("GLByteFlowRender::OnSurfaceCreated GL_MAX_TEXTURE_SIZE = %d", nMaxTextureSize);
	if(!CreateProgram(kVertexShader, m_FragShaders.at(m_ShaderIndex)))
	{
		LOGCATE("GLByteFlowRender::OnSurfaceCreated create program fail.");
	}
}

void GLByteFlowRender::OnSurfaceChanged(int width, int height)
{
	LOGCATE("GLByteFlowRender::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
	m_ViewportWidth = width;
	m_ViewportHeight = height;
	m_IsProgramChanged = true;
}

void GLByteFlowRender::OnDrawFrame()
{
	LOGCATE("GLByteFlowRender::OnDrawFrame");
	glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_CULL_FACE);

	if (!UpdateTextures() || !UseProgram())
	{
		LOGCATE("GLByteFlowRender::OnDrawFrame skip frame");
		return;
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glDrawArrays(GL_TRIANGLES, 0, m_FragCoors.nCoorsCount);

}

void GLByteFlowRender::UpdateMVPMatrix(glm::mat4 &mat4Matrix, TransformMatrix &transformMatrix)
{
	LOGCATE("GLByteFlowRender::UpdateMVPMatrix");
	float fFactorX = 1.0f;
	float fFactorY = 1.0f;

	if (transformMatrix.mirror == 1)
		fFactorX = -1.0f;
	else if (transformMatrix.mirror == 2)
		fFactorY = -1.0f;

	float fRotate = 0.0f;
	if (transformMatrix.mirror == 0) {
		if (transformMatrix.degree == 270) {
			fRotate = MATH_PI * 0.5;
		} else if (transformMatrix.degree == 180) {
			fRotate = MATH_PI;
		} else if (transformMatrix.degree == 90) {
			fRotate = MATH_PI * 1.5;
		}
	} else if (transformMatrix.mirror == 1) {
		if (transformMatrix.degree == 90) {
			fRotate = MATH_PI * 0.5;
		} else if (transformMatrix.degree == 180) {
			fRotate = MATH_PI;
		} else if (transformMatrix.degree == 270) {
			fRotate = MATH_PI * 1.5;
		}
	}

	float ratio = (float) m_RenderBufFrame.width / m_RenderBufFrame.height;
	glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
	//glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
	//glm::mat4 Projection = glm::perspective(45.0f,4.0f/3.0f,0.1f,100.f);
	glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 1), // Camera is at (0,0,1), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	Model = glm::scale(Model, glm::vec3(fFactorX * transformMatrix.scaleX, fFactorY * transformMatrix.scaleY, 1.0f));
	Model = glm::rotate(Model, fRotate, glm::vec3(0.0f, 0.0f, 1.0f));
	Model = glm::translate(Model, glm::vec3(transformMatrix.translateX, transformMatrix.translateY, 0.0f));

	LOGCATE("SetMVP real: rotate %d,%.2f,%0.5f,%0.5f,%0.5f,%0.5f,", transformMatrix.degree, fRotate,
		 transformMatrix.translateX, transformMatrix.translateY, fFactorX * transformMatrix.scaleX, fFactorY * transformMatrix.scaleY);

	mat4Matrix = Projection * View * Model;// Remember, matrix multiplication is the other way around

}

GLuint GLByteFlowRender::UseProgram()
{
	LOGCATE("GLByteFlowRender::UseProgram");
	if (m_IsShaderChanged)
	{
		GLUtils::DeleteProgram(m_Program);
		CreateProgram(kVertexShader, m_FragShaders.at(m_ShaderIndex));
		m_IsShaderChanged = false;
		m_IsProgramChanged = true;
	}


	if (!m_Program)
	{
		LOGCATE("GLByteFlowRender::UseProgram Could not use program.");
		return 0;
	}

	if (m_IsProgramChanged)
	{
		glUseProgram(m_Program);
		GLUtils::CheckGLError("GLByteFlowRender::UseProgram");

		glVertexAttribPointer(m_VertexCoorHandle, 2, GL_FLOAT, GL_FALSE, 2*4, VERTICES_COORS);
		glEnableVertexAttribArray(m_VertexCoorHandle);

		UpdateMVPMatrix(m_MVPMatrix, m_TransformMatrix);

		glUniformMatrix4fv(m_MVPHandle, 1, GL_FALSE, &m_MVPMatrix[0][0]);

		glUniform1i(m_YTextureHandle, 0);
		glUniform1i(m_UTextureHandle, 1);
		glUniform1i(m_VTextureHandle, 2);
		glUniform1i(m_MaskTextureHandle, 3);
		glVertexAttribPointer(m_TextureCoorHandle, 2, GL_FLOAT, GL_FALSE, 2*4, TEXTURE_COORS);
		glEnableVertexAttribArray(m_TextureCoorHandle);

		if (m_TextureSizeHandle >= 0) {
			GLfloat size[2];
			size[0] = m_RenderBufFrame.width;
			size[1] = m_RenderBufFrame.height;
			glUniform2fv(m_TextureSizeHandle, 1, &size[0]);
		}

		m_IsProgramChanged = false;
	}

	return m_Program;
}

int GLByteFlowRender::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource)
{
	m_Program = GLUtils::CreateProgram(pVertexShaderSource, pFragShaderSource, m_VertexShader,
									   m_FragShader);
	if (!m_Program)
	{
		GLUtils::CheckGLError("Create Program");
		LOGCATE("GLByteFlowRender::CreateProgram Could not create program.");
		return 0;
	}

	m_YTextureHandle = glGetUniformLocation(m_Program, "s_textureY");
	m_UTextureHandle = glGetUniformLocation(m_Program, "s_textureU");
	m_VTextureHandle = glGetUniformLocation(m_Program, "s_textureV");
	m_MaskTextureHandle = glGetUniformLocation(m_Program, "s_textureMask");

	m_TextureSizeHandle = glGetUniformLocation(m_Program, "texSize");
	m_VertexCoorHandle = (GLuint) glGetAttribLocation(m_Program, "position");
	m_TextureCoorHandle = (GLuint) glGetAttribLocation(m_Program, "texcoord");
	m_MVPHandle = glGetUniformLocation(m_Program, "MVP");

	return m_Program;
}
