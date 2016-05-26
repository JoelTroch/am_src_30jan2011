class CTextureBlur
{
public:
	CTextureBlur();
	void Init(int width, int height);
	void BindTexture(int width, int height);
	void DrawQuad(int width, int height,int of);
	void Draw(int width, int height);
	unsigned int g_texture;
	float alpha;
	float r,g,b;
	float of;
};

class CBlur
{
public:
	void InitScreen(void);
	void DrawBlur(void);
	int blur_pos;
	bool AnimateNextFrame(int desiredFrameRate);

	CTextureBlur m_pTextures;
	CTextureBlur m_pScreen;
};

extern CBlur gBlur;