class BgTransferer
{
	double elapsedTime;
	double lastTime;
public:
	BgTransferer();
	void Init();
	void GetBGs(int& bg1, int& bg2);
	float GetTransferTime();
	void Update();
};

extern BgTransferer bgTransferer;
