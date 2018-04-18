#include "stdafx.h"
#include "String.h"

//////////////////////////////////////////////////////////////////////////
///@brief ���ڿ� �ڸ���
///@param orgin : ���� ���ڿ�
///@param tok : �ڸ� ������ �Ǵ� ���ڿ�
///@return �Ϸ�� ���ڿ� �迭
//////////////////////////////////////////////////////////////////////////
void String::SplitString(vector<string>* result, string origin, string tok)
{
	result->clear();

	int cutAt = 0; //�ڸ� ��ġs
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //�ڸ��� ��ġ�� 0����ũ��
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //�ڸ��� ���� ���� �ִٸ�
		result->push_back(origin.substr(0, cutAt));
}

void String::SplitString(vector<wstring>* result, wstring origin, wstring tok)
{
	result->clear();

	int cutAt = 0; //�ڸ� ��ġs
	while ((cutAt = (int)origin.find_first_of(tok)) != origin.npos)
	{
		if (cutAt > 0) //�ڸ��� ��ġ�� 0����ũ��
			result->push_back(origin.substr(0, cutAt));

		origin = origin.substr(cutAt + 1);
	}

	if (origin.length() > 0) //�ڸ��� ���� ���� �ִٸ�
		result->push_back(origin.substr(0, cutAt));
}

//////////////////////////////////////////////////////////////////////////
///@brief ������ ��ü ��ο��� ��ο� ���ϸ����� �и�
///@param fullPath : ��ü ���
///@param filePath : ���
///@param fileName : ���ϸ�
//////////////////////////////////////////////////////////////////////////
void String::SplitFilePath(string fullPath, string * filePath, string * fileName)
{
	size_t index = fullPath.find_last_of('/');

	if (filePath != NULL)
		*filePath = fullPath.substr(0, index + 1);

	if (fileName != NULL)
		*fileName = fullPath.substr(index + 1, fullPath.length());
}

void String::SplitFilePath(wstring fullPath, wstring* filePath/*=NULL*/, wstring* fileName/*=NULL*/)
{
	size_t index = fullPath.find_last_of('/');

	if (filePath != NULL)
		*filePath = fullPath.substr(0, index + 1);

	if (fileName != NULL)
		*fileName = fullPath.substr(index + 1, fullPath.length());
}

//////////////////////////////////////////////////////////////////////////
///@brief ������ ��ü ��ο��� ��ο� ���ϸ����� �и�
///@param fullPath : ��ü ���
///@param fileName : ���ϸ�
//////////////////////////////////////////////////////////////////////////
void String::SplitFileNameOnly(string fullPath, string * fileName)
{
	SplitFilePath(fullPath, NULL, fileName);

	size_t index = fileName->find_last_of('.');

	if (fileName != NULL)
		*fileName = fileName->substr(0, index);
}

void String::SplitFileNameOnly(wstring fullPath, wstring * fileName)
{
	SplitFilePath(fullPath, NULL, fileName);

	size_t index = fileName->find_last_of('.');

	if (fileName != NULL)
		*fileName = fileName->substr(0, index);
}


//////////////////////////////////////////////////////////////////////////
///@brief ���� ���ڿ��� ������ üũ
///@param str : üũ�Ϸ��� ���ڿ�
///@param comp : ���� �񱳹��ڿ�
//////////////////////////////////////////////////////////////////////////
bool String::StartsWith(string str, string comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

bool String::StartsWith(wstring str, wstring comp)
{
	wstring::size_type index = str.find(comp);
	if (index != wstring::npos && (int)index == 0)
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
///@brief �ش� ���ڿ��� ���ԵǾ� �ִ���
///@param str : üũ�Ϸ��� ���ڿ�
///@param comp : �񱳹��ڿ�
//////////////////////////////////////////////////////////////////////////
bool String::Contain(string str, string comp)
{
	size_t found = str.find(comp);

	return found != wstring::npos;
}

bool String::Contain(wstring str, wstring comp)
{
	size_t found = str.find(comp);

	return found != wstring::npos;
}


//////////////////////////////////////////////////////////////////////////
///@brief �ش� ���ڿ����� comp ���ڸ� rep�� ����
///@param str : üũ�Ϸ��� ���ڿ�
///@param comp : �񱳹��ڿ�
///@param rep : �ٲܹ��ڿ�
//////////////////////////////////////////////////////////////////////////
void String::Replace(string * str, string comp, string rep)
{
	string temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

void String::Replace(wstring* str, wstring comp, wstring rep)
{
	wstring temp = *str;

	size_t start_pos = 0;
	while ((start_pos = temp.find(comp, start_pos)) != wstring::npos)
	{
		temp.replace(start_pos, comp.length(), rep);
		start_pos += rep.length();
	}

	*str = temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief string���� wstring������ ����
///@param value : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
wstring String::StringToWString(string value)
{
	wstring temp = L"";
	temp.assign(value.begin(), value.end());

	return temp;
}

//////////////////////////////////////////////////////////////////////////
///@brief wstring���� string������ ����
///@param value : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
string String::WStringToString(wstring value)
{
	string temp = "";
	temp.assign(value.begin(), value.end());

	return temp;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///@brief wstring���� string������ ����
///@param value : ��ȯ�� ���ڿ�
///@return ��ȯ �Ϸ� ���ڿ�
//////////////////////////////////////////////////////////////////////////
const char * String::ToCSharp(string value)
{
	UINT size = value.size() + 1;
	char* str = (char *)LocalAlloc(LPTR, value.size() + 1);
	strcpy_s(str, size, value.c_str());

	return (const char *)str;
}