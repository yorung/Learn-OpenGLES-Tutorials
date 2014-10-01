#include "stdafx.h"

ShaderMan shaderMan;

#ifdef _MSC_VER
#include "path_utils.h"
#endif

static GLuint CompileShader(int type, const char *fileName)
{
	GLuint shader = glCreateShader(type);

#ifdef _MSC_VER
	void* img = LoadFile((GetExecutableDirectory() + "/../../assets/" + fileName).c_str());
#else
	void* img = LoadFile(fileName);
#endif
	glShaderSource(shader, 1, (const char**)&img, NULL);
	glCompileShader(shader);
	free(img);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == 0) {
		int len;
		int len2;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		GLchar* buf = (GLchar*)malloc(len);
		glGetShaderInfoLog(shader, len, &len2, buf);
		aflog("result=%d (%s)%s", result, fileName, buf);
		free(buf);
	}

	return shader;
}

static bool CreateProgram(const char* name, GLuint* id)
{
	char buf[256];
	snprintf(buf, dimof(buf), "shaders/%s.vs", name);
	int vertexShader = CompileShader(GL_VERTEX_SHADER, buf);
	snprintf(buf, dimof(buf), "shaders/%s.fs", name);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, buf);
	int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glAttachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);
	glLinkProgram(program);
	*id = program;
	return true;
}

static void ApplyElements(GLuint program, const InputElement decl[], int nDecl)
{
	int stride = 0;
	for (int i = 0; i < nDecl; i++) {
		const InputElement& d = decl[i];
		stride += d.size * sizeof(float);
	}
	for (int i = 0; i < nDecl; i++) {
		const InputElement& d = decl[i];
		int h = glGetAttribLocation(program, d.name);
		glEnableVertexAttribArray(h);
		glVertexAttribPointer(h, d.size, GL_FLOAT, GL_FALSE, stride, (void*)d.offset);
	}
}

ShaderMan::SMID ShaderMan::Create(const char *name, const InputElement elements[], int numElements)
{
	NameToId::iterator it = nameToId.find(name);
	if (it != nameToId.end())
	{
		return it->second;
	}

	Effect effect;
	memset(&effect, 0, sizeof(effect));

	CreateProgram(name, &effect.program);
	effect.elements = elements;
	effect.numElements = numElements;

	effects[effect.program] = effect;
	return nameToId[name] = effect.program;
}

void ShaderMan::Destroy()
{
	for (Effects::iterator it = effects.begin(); it != effects.end(); it++)
	{
		glDeleteProgram(it->second.program);
	}
	effects.clear();
	nameToId.clear();
}

void ShaderMan::Apply(SMID id)
{
	Effect& it = effects[id];
	glUseProgram(it.program);
	ApplyElements(it.program, it.elements, it.numElements);
}
