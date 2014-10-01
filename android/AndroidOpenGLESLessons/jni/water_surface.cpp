#include "stdafx.h"

const float pitch = 2.0f / tileMax;
const float heightUnit = 0.02f;
const float rippleSpeed = 0.7f;
const float loopTime = 20.0;

WaterSurface waterSurface;

struct TexFiles
{
	const char *name;
	bool clamp;
};

TexFiles texFiles[] = {
	{ "hyomu_sm_1.jpg", true },
	{ "hyomu_sm_2.jpg", true },
	{ "hyomu_sm_3.jpg", true },
	{ "sphere.jpg", true },
};

TexMan::TMID texId[dimof(texFiles)];

static Vec3 MakePos(int x, int z, float hmap[vertMax][vertMax])
{
	float height = hmap[std::max(0,std::min(tileMax, x))][std::max(0,std::min(tileMax, z))];
	return Vec3(((float)x - tileMax / 2) * pitch, height, ((float)z - tileMax / 2) * pitch);
}

float WaterSurface::GetVertHeight(int x, int z)
{
	if (x < 0 || x > tileMax) {
		return 0;
	}
	if (z < 0 || z > tileMax) {
		return 0;
	}
	return vertHeightInfo[x][z].height;
}

float WaterSurface::GetHeightByPos(Vec2 pos)
{
	pos = (pos + Vec2(1, 1)) / 2 * tileMax;
	Vec2 fracPart = frac(pos);
	Vec2 intPart = floor(pos);
	float left = lerp(GetVertHeight((int)intPart.x, (int)intPart.y), GetVertHeight((int)intPart.x, (int)intPart.y + 1), fracPart.y);
	float right = lerp(GetVertHeight((int)intPart.x + 1, (int)intPart.y), GetVertHeight((int)intPart.x + 1, (int)intPart.y + 1), fracPart.y);
	return lerp(left, right, fracPart.x);
}

void WaterSurface::CreateRipple(Vec2 pos1)
{
	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			Vec2 pos2 = Vec2((float)x, (float)z) / tileMax * 2 - Vec2(1, 1);
			VertHeightInfo& info = vertHeightInfo[x][z];
			float dist = length(pos1 - pos2);
			info.height += std::max(0.0f, 1.0f - dist * 5) * 0.13f;
			info.height = std::min(0.4f, std::max(-0.4f, info.height));
		}
	}
}

void WaterSurface::UpdateVert(std::vector<WaterVert>& vert, double dt)
{
	vert.resize(vertMax * vertMax);

	double tm0 = GetTime();
	/*
	struct RandWave {
		float degreePerSec;
		float xShift;
		float xMul;
		float timeMul;
		float strength;
	} static randWave[10];
	static bool t;
	if (!t) {
		t = true;
		for (int i = 0; i < dimof(randWave); i++) {
			RandWave& r = randWave[i];
			r.degreePerSec = (float)Random() * 15.0f - 7.5f;
			r.xShift = (float)Random();
			r.xMul = 2 + powf(2, 1 + 3 * Random());
			r.timeMul = 2 + powf(2, 0.5f + 2.0f * Random());
			r.strength = 0.005f * powf(2, 0.1f + 0.3f * Random());
		}
	}
	*/
	float hmap[vertMax][vertMax];
	memset(hmap, 0, sizeof(hmap));

	float average[vertMax][vertMax];
	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
		//	float dist = dt * 5 * (1 - powf(Random(), 2));
		//	Vec2 pos = Vec2((float)x, (float)z) / tileMax * 2 - Vec2(1, 1);
		//	average[x][z] = GetHeightByPos(pos + Vec2(dist, 0)) + GetHeightByPos(pos + Vec2(0, dist)) + GetHeightByPos(pos - Vec2(dist, 0)) + GetHeightByPos(pos - Vec2(0, dist));
			average[x][z] = GetVertHeight(x + 1, z) + GetVertHeight(x, z + 1) + GetVertHeight(x - 1, z) + GetVertHeight(x, z - 1);
			average[x][z] /= 4;
		}
	}

	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			Vec2 pos = Vec2((float)x, (float)z) / tileMax * 2 - Vec2(1, 1);
			VertHeightInfo& info = vertHeightInfo[x][z];
			info.velocity += average[x][z] - info.height;
			info.velocity *= 0.99f;//powf(0.2f, dt / 2);
			info.height += info.velocity;
			hmap[x][z] = info.height * 0.8f;
		}
	}

	/*
	for (int z = 0; z <= tileMax; z++) {
		for (int x = 0; x <= tileMax; x++) {
			Vec2 pos = Vec2((float)x, (float)z) / tileMax * 2 - Vec2(1, 1);
			for (int i = 0; i < dimof(ripples); i++) {
				const WaterRipple& r = ripples[i];
				float lifeTime = (float)(elapsedTime - r.generatedTime);
				float timeAfterArrived = lifeTime - length(r.centerPos - pos) / rippleSpeed;
				float h = timeAfterArrived > 0 ? (float)sin(timeAfterArrived * XM_2PI * repeat) * heightUnit : 0;
				h *= std::min(1.0f, powf(0.5f, lifeTime / halflife));

				hmap[x][z] += h;
			}
#if 0
			for (int i = 0; i < dimof(randWave); i++) {
				const RandWave& r = randWave[i];
				float rad = (elapsedTime * r.degreePerSec) * XM_2PI / 180;
				Vec2 posRot = pos * cosf(rad) + Vec2(-pos.y, pos.x) * sinf(rad);
				hmap[x][z] += sinf(r.xShift + posRot.x * r.xMul + elapsedTime * r.timeMul) * r.strength;
			}
#endif
		}
	}
*/
	double tm1 = GetTime();

	for (int z = 0; z <= tileMax; z++) {
		float leftHeight = 0;
		for (int x = 0; x <= tileMax; x++) {
			WaterVert& v = vert[x + z * vertMax];
			v.pos = MakePos(x, z, hmap);
			v.leftHeight = leftHeight;
			v.upperHeight = z > 0 ? vert[x + (z - 1) * vertMax].pos.y : 0;
			leftHeight = v.pos.y;
		}
	}
/*
	for (int z = 0; z <= tileMax; z++) {
		Vec3 left = MakePos(-1, z, hmap);
		for (int x = 0; x <= tileMax; x++) {
			WaterVert& v = vert[x + z * vertMax];
			Vec3 upper = MakePos(x, z - 1, hmap);
			v.normal = cross(v.pos - left, upper - v.pos);
			left = v.pos;
		}
	}*/
	double tm2 = GetTime();

//	aflog("%f %f\n", tm1 - tm0, tm2 - tm1);
//	__android_log_print(ANDROID_LOG_DEBUG, "MyTag", "%f %f\n", tm1 - tm0, tm2 - tm1);

}

WaterSurface::WaterSurface()
{
	elapsedTime = 0;
	ripplesNext = 0;
	ibo = 0;
	lastTime = 0;
	lines = 0;
	nextTime = 0;
	nIndi = 0;
	shaderId = -1;
	vbo = 0;
}

WaterSurface::~WaterSurface()
{
	Destroy();
}

void WaterSurface::Destroy()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}

void WaterSurface::MakeInitialVertBuf()
{
	std::vector<short> indi;
	std::vector<WaterVert> vert;
	UpdateVert(vert, 0);

	for (int z = 0; z < tileMax; z++) {
		if (z != 0) {
			indi.push_back(z * vertMax);
		}
		indi.push_back(z * vertMax);
		for (int x = 0; x < tileMax; x++) {
			indi.push_back((z + 1) * vertMax + x);
			indi.push_back(z * vertMax + x + 1);
		}
		indi.push_back((z + 1) * vertMax + vertMax - 1);
		if (z != tileMax - 1) {
			indi.push_back((z + 1) * vertMax + vertMax - 1);
		}
	}

	lines = indi.size() / 2;
	nIndi = indi.size();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(WaterVert), &vert[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ARRAY_BUFFER, ibo);
	glBufferData(GL_ARRAY_BUFFER, indi.size() * sizeof(short), &indi[0], GL_STATIC_DRAW);
}

void WaterSurface::Init()
{
	Destroy();

	lastTime = GetTime();

	MakeInitialVertBuf();

	static const InputElement elements[] = {
		{ "vPosition", 3, 0 },
		{ "upperHeight", 1, 12 },
		{ "leftHeight", 1, 16 },
	};
//	texId = texMan.Create("sphere.jpg");
	shaderId = shaderMan.Create("water", elements, dimof(elements));

	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < dimof(texFiles); i++) {
		texId[i] = texMan.Create(texFiles[i].name);
	}
}

void WaterSurface::Update()
{
	const double deltaTimeMax = 0.1;

	double now = GetTime();
	double dt = std::min(deltaTimeMax, now - lastTime);
//	double dt = 0.008;
	elapsedTime += dt;
	lastTime = now;

	static std::vector<WaterVert> vert;

//	vert.clear();
	UpdateVert(vert, dt);
//	static int cnt;
//	if (cnt++ < 60) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vert.size() * sizeof(WaterVert), &vert[0]);
//	}

	if (0) {
//	if (elapsedTime >= nextTime) {
		nextTime = elapsedTime + 0.5 + Random() * 2;
		CreateRipple(Vec2(Random(), Random()) * 4 - Vec2(2, 2));
	}
}

void WaterSurface::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

//	float rad = sinf(time * M_PI) * 0.5f * M_PI;
//	vertOld[0].normal = Vec3(0, 0, 1) * cosf(rad) + Vec3(1, 0, 0) * sinf(rad);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertOld), vertOld);

//	glDrawArrays(GL_TRIANGLE_STRIP, 0, dimof(vert));
	glDrawElements(GL_TRIANGLE_STRIP, nIndi, GL_UNSIGNED_SHORT, 0);
//	glDisableVertexAttribArray(mPositionHandle);

	Update();

	shaderMan.Apply(shaderId);

	glUniform1i(glGetUniformLocation(shaderId, "bg1"), 0);
	glUniform1i(glGetUniformLocation(shaderId, "bg2"), 1);
	glUniform1i(glGetUniformLocation(shaderId, "spheremap"), 2);
	double dummy;
	glUniform1f(glGetUniformLocation(shaderId, "time"), (float)modf(elapsedTime * (1.0f / loopTime), &dummy) * loopTime);
	glUniform1f(glGetUniformLocation(shaderId, "transferTime"), bgTransferer.GetTransferTime());
/*
	{
		int bg1, bg2;
		bgTransferer.GetBGs(bg1, bg2);
		float transT = bgTransferer.GetTransferTime();
		aflog("%f %d %d\n", transT, bg1, bg2);
	}
*/

	//	Mat m = scale(1.5f);
//	Mat m = q2m(Quat(Vec3(1,0,0), M_PI / 180 * time * 60));
	Mat matW = q2m(Quat(Vec3(1,0,0), XM_PI / 180 * -90));
	Mat matP, matV;
	matrixMan.Get(MatrixMan::PROJ, matP);
	matrixMan.Get(MatrixMan::VIEW, matV);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "matW"), 1, GL_FALSE, &matW.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "matV"), 1, GL_FALSE, &matV.m[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "matP"), 1, GL_FALSE, &matP.m[0][0]);

	int bg1, bg2;
	bgTransferer.GetBGs(bg1, bg2);
	glActiveTexture(GL_TEXTURE0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, texId[bg1]);

	glActiveTexture(GL_TEXTURE1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, texId[bg2]);

	glActiveTexture(GL_TEXTURE2);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, texId[3]);
}
