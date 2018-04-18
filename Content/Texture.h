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