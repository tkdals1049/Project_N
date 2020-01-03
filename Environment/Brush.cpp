#include "stdafx.h"
#include "Plane.h"
#include "Brush.h"

Brush::Brush(Plane* plane)
	: plane(plane)
	, vertexBuffer(plane->vertexBuffer)
	, vertex(plane->vertex)
	, vertexCount(plane->vertexCount)
	, indexBuffer(plane->indexBuffer)
	, index(plane->index)
	, width(plane->width), height(plane->height)
	, size(2)
	, floor(5.0f)
	, color(D3DXVECTOR4(0, 0, 0, 0.8f))
	, mode(0), way(1), type(0), num(0),brushOn(0)
{
}

Brush::~Brush()
{
}

void Brush::Reset()
{
	vertex=plane->vertex;
	vertexCount=plane->vertexCount;
	width=plane->width; 
	height=plane->height;

}

void Brush::Update(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	position = CheckMouseIndex(origin, direction);
	if (position.x != -1 && position.y != -1)
	{
		Box box;
		box.left = position.x - plane->position.x - size;
		box.top = position.y - plane->position.z + size;
		box.right = position.x - plane->position.x + size;
		box.bottom = position.y - plane->position.z - size;

		if (box.left<0) box.left = 0;
		if (box.top >= height) box.top = (float)height;
		if (box.right >= width) box.right = (float)width;
		if (box.bottom<0) box.bottom = 0;

		if (Mouse::Get()->Press(0)&&brushOn!=0)
		{
			if (mode == 0)
			{
				if (Keyboard::Get()->Press(VK_SHIFT))
					way = -1;
				else way = 1;
				ControlHeight(&box);
			}
			else if (mode == 1)
			{
				AdjustHeight(&box);
			}
			else if (mode == 2)
			{
				Coloring(&box);
			}
			else if (mode == 3)
			{
				Splatting(&box);
			}
		}
	}
}

void Brush::Render()
{
}

D3DXVECTOR2 Brush::CheckMouseIndex(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index0 = (width + 1) * z + x;
			UINT index1 = (width + 1) * (z + 1) + x;
			UINT index2 = (width + 1) * z + x + 1;
			UINT index3 = (width + 1) * (z + 1) + x + 1;

			D3DXVECTOR3 v0 = vertex[index0].position;
			D3DXVECTOR3 v1 = vertex[index1].position;
			D3DXVECTOR3 v2 = vertex[index2].position;
			D3DXVECTOR3 v3 = vertex[index3].position;

			float u, v, distance;

			BOOL check;
			check = D3DXIntersectTri
			(
				&v0, &v1, &v2
				, &origin, &direction
				, &u, &v, &distance
			);
			if (check) return D3DXVECTOR2(v0.x, v0.z);
			check = D3DXIntersectTri
			(
				&v3, &v1, &v2
				, &origin, &direction
				, &u, &v, &distance
			);
			if (check) return D3DXVECTOR2(v3.x, v3.z);
		}//for(x)
	}//for(y)
	return D3DXVECTOR2(-1, -1);
}

void Brush::AdjustHeight(Box* box)
{
	for (UINT z = (UINT)box->bottom;z <= box->top;z++)
	{
		for (UINT x = (UINT)box->left;x <= box->right;x++)
		{
			UINT index = z*(width + 1) + x;
			float ano = sqrt(pow(x - (position.x - plane->position.x), 2) + pow(z - (position.y - plane->position.z), 2));

			if (type == 0)vertex[index].position.y = floor;
			else { if (size >= ano)vertex[index].position.y = floor; }
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);
}

void Brush::ControlHeight(Box * box)
{
	for (UINT z = (UINT)box->bottom;z <= box->top;z++)
	{
		for (UINT x = (UINT)box->left;x <= box->right;x++)
		{
			UINT index = z*(width + 1) + x;
			float ano = sqrt(pow(x - (position.x - plane->position.x), 2) + pow(z - (position.y - plane->position.z), 2));

			if (type == 0)vertex[index].position.y += way*Time::Get()->Delta() * 10;
			else { if (size >= ano)vertex[index].position.y += way*Time::Get()->Delta() * 10; }
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);
}

void Brush::Coloring(Box * box)
{
	for (UINT z = (UINT)box->bottom;z <= box->top;z++)
	{
		for (UINT x = (UINT)box->left;x <= box->right;x++)
		{
			UINT index = z*(width + 1) + x;
			float ano = sqrt(pow(x - (position.x - plane->position.x), 2) + pow(z - (position.y - plane->position.z), 2));

			if (type == 0)
			{
				vertex[index].color = color;
			}
			else
			{
				if (size >= ano)
				{
					vertex[index].color = color;
				}
			}
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);
}

void Brush::Splatting(Box * box)
{
	for (UINT z = (UINT)box->bottom;z <= box->top;z++)
	{
		for (UINT x = (UINT)box->left;x <= box->right;x++)
		{
			UINT index = z * (width + 1) + x;
			float ano = sqrt(pow(x - (position.x - plane->position.x), 2) + pow(z - (position.y - plane->position.z), 2));

			if (type == 0)
			{
				switch (num)
				{
					case 0:vertex[index].lerp = D3DXVECTOR4(0.1f, 0, 0, 0);break;
					case 1:if (vertex[index].lerp.x<1)vertex[index].lerp += D3DXVECTOR4(0.1f, 0, 0, 0);break;
					case 2:if (vertex[index].lerp.y<1)vertex[index].lerp += D3DXVECTOR4(0, 0.1f, 0, 0);break;
					case 3:if (vertex[index].lerp.z<1)vertex[index].lerp += D3DXVECTOR4(0, 0, 0.1f, 0);break;
					case 4:if (vertex[index].lerp.w<1)vertex[index].lerp += D3DXVECTOR4(0, 0, 0, 0.1f);break;
				}
			}
			else
			{
				if (size >= ano)
				{
					switch (num)
					{
						case 0:vertex[index].lerp = D3DXVECTOR4(0.1f, 0, 0, 0);break;
						case 1:if (vertex[index].lerp.x<1)vertex[index].lerp += D3DXVECTOR4(0.1f, 0, 0, 0);break;
						case 2:if (vertex[index].lerp.y<1)vertex[index].lerp += D3DXVECTOR4(0, 0.1f, 0, 0);break;
						case 3:if (vertex[index].lerp.z<1)vertex[index].lerp += D3DXVECTOR4(0, 0, 0.1f, 0);break;
						case 4:if (vertex[index].lerp.w<1)vertex[index].lerp += D3DXVECTOR4(0, 0, 0, 0.1f);break;
					}
				}
			}
		}
	}

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*vertexCount, 0);
}