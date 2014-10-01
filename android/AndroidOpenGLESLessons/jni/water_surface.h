const int tileMax = 64;
const int vertMax = tileMax + 1;

struct WaterVert
{
	Vec3 pos;
	float upperHeight;
	float leftHeight;
};

struct VertHeightInfo
{
	float height;
	float velocity;
};

struct WaterRipple
{
	WaterRipple()
	{
		generatedTime = -10000;
	}
	double generatedTime;
	Vec2 centerPos;
};

class WaterSurface
{
//	TexMan::TMID texId;
	ShaderMan::SMID shaderId;
	int lines;
	void UpdateVert(std::vector<WaterVert>& vert, double dt);
	void Update();
	VertHeightInfo vertHeightInfo[vertMax][vertMax];
	WaterRipple ripples[2];
	int ripplesNext;
	double elapsedTime;
	double lastTime;
	double nextTime;
	GLuint vbo, ibo;
	int nIndi;
	float GetVertHeight(int x, int z);
	float GetHeightByPos(Vec2 pos);
	void MakeInitialVertBuf();
public:
	WaterSurface();
	~WaterSurface();
	void Destroy();
	void Init();
	void Draw();
	void CreateRipple(Vec2 pos);
};

extern WaterSurface waterSurface;
