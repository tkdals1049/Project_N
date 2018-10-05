#pragma once

class Texture
{
	public:
	static Texture* Get();
	static void Delete();
	string GetTexturePath(int num);
	string Convert(string file);
	const char** GetTextureList(){return c_texturelist;}
	UINT GetTexturePathSize() { return textureList.size(); }
	void Refresh();

	void LoadTexture(wstring fileName, ID3D11Texture2D ** texture);

	void LoadPixel(ID3D11Texture2D * texture, UINT ** colors);

	private:
	static Texture* instance;
	 Texture();
	 void Search(string path);
	 ~Texture();
	 struct TextureList
	 {
		string file;
		string path;
	 };
	vector<TextureList> textureList;
	const char** c_texturelist;
};