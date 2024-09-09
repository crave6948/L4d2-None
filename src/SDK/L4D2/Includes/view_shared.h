#pragma once

//https://www.unknowncheats.me/forum/2795309-post4.html
class CViewSetup
{
private:
	char __pad00[16];

public:
	int width;
	int m_nUnscaledWidth;
	int height;
	int m_nUnscaledHeight;

private:
	char __pad01[20];

public:
	float fov;
	float fovViewmodel;

private:
	char __pad02[2052];
};