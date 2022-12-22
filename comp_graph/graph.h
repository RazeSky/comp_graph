#pragma once
#include "stdio.h"
#include "math.h"
#define WINDOW_SIZE 512
#define BCKG_COLOR 0
#define X 0
#define Y 1
#define Z 2
#define W 3

class point
{
	float* multiply(float _m[4][4])
	{
		float x_ = x;
		float y_ = y;
		float z_ = z;
		float w_ = w;
		float cpy[4] = { x_, y_, z_, w_ };
		float res[4] = { 0,0,0,0 };

		res[X] = cpy[0] * _m[0][0] + cpy[1] * _m[1][0] + cpy[2] * _m[2][0] + cpy[3] * _m[3][0];
		res[Y] = cpy[0] * _m[0][1] + cpy[1] * _m[1][1] + cpy[2] * _m[2][1] + cpy[3] * _m[3][1];
		res[Z] = cpy[0] * _m[0][2] + cpy[1] * _m[1][2] + cpy[2] * _m[2][2] + cpy[3] * _m[3][2];
		res[W] = cpy[0] * _m[0][3] + cpy[1] * _m[1][3] + cpy[2] * _m[2][3] + cpy[3] * _m[3][3];

		x = res[X];
		y = res[Y];
		z = res[Z];
		w = res[W];
		return res;
	}

	void rotateX(float f) {
		f = f * 3.1415 / 180;

		float matrix[4][4] = {
			1, 0, 0, 0,
			0, cos(f), -sin(f), 0,
			0, sin(f), cos(f),
			0, 0, 0, 0, 1 };
		multiply(matrix);
	}

	void rotateY(float f) {
		f = f * 3.1415 / 180;

		float matrix[4][4] = {
			cos(f), 0, -sin(f), 0,
			0, 1, 0, 0,
			sin(f), 0, cos(f), 0,
			0, 0, 0, 1 };
		multiply(matrix);
	}

	void rotateZ(float f) {
		f = f * 3.1415 / 180;

		float matrix[4][4] = {
			cos(f), -sin(f), 0, 0,
			sin(f), cos(f), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };
		multiply(matrix);
	}

	float sx, sy, sz, sw;

public:

	float x, y, z, w;
	point(int _x, int _y, int _z) : x(_x), y(_y), z(_z)
	{
		w = 1;
	}
	point()
	{
		x = 0; y = 0; z = 0; w = 1;
	}


	void move(float _x, float _y, float _z)
	{
		float matrix[4][4] = {
			1,  0,  0,  0,
			0,  1,  0,  0,
			0,  0,  1,  0,
			_x, _y, _z, 1 };

		multiply(matrix);
	}

	void scale(float _x, float _y, float _z) {
		float matrix[4][4] = {
			_x, 0, 0, 0,
			0, _y, 0, 0,
			0, 0, _z, 0,
			0, 0, 0, 1 };

		multiply(matrix);
	}

	void rotate(float _phi, int _axis) {
		if (_axis == X)
			rotateX(_phi);
		if (_axis == Y)
			rotateY(_phi);
		if (_axis == Z)
			rotateZ(_phi);
	}

};



class side
{

public:
	point** v;
	int idx;
	int point_number;

	int color;
	float A, B, C, D;
	float Xmin, Xmax, Ymin, Ymax;

	side()
	{
		color = 8;
	}

	side(point* _A, point* _B, point* _C, int i, int c)
	{
		point_number = 3;
		v = new point * [point_number];
		v[0] = _A;
		v[1] = _B;
		v[2] = _C;

		idx = i;
		color = c;

		update_data();
	}
	side(point* _A, point* _B, point* _C, point* _D, int i, int c)
	{
		point_number = 4;
		v = new point * [point_number];
		v[0] = _A;
		v[1] = _B;
		v[2] = _C;
		v[3] = _D;
		idx = i;
		color = c;
		update_data();
	}
	side(point* _A, point* _B, point* _C, point* _D)
	{
		point_number = 4;
		v = new point * [point_number];
		v[0] = _A;
		v[1] = _B;
		v[2] = _C;
		v[3] = _D;
		idx = 0;
		color = 0;
		
	}

	void set(int i, int c)
	{
		idx = i;
		color = c;
	}

	void update_data()
	{
		int i = 0;
		float M[3] = { v[i]->x, v[i]->y, v[i]->z };
		int ai = i - 1;
		if (ai < 0)
			ai = 2;
		int bi = i + 1;
		if (bi > 2)
			bi = 0;
		float a[3] = { v[ai]->x - v[i]->x, v[ai]->y - v[i]->y, v[ai]->z - v[i]->z };
		float b[3] = { v[bi]->x - v[i]->x, v[bi]->y - v[i]->y, v[bi]->z - v[i]->z };

		A = a[Y] * b[Z] - a[Z] * b[Y];
		B = a[X] * b[Z] - a[Z] * b[X];
		B = -B;
		C = a[X] * b[Y] - a[Y] * b[X];

		D = M[X] * A + M[Y] * B + M[Z] * C;
		D = -D;
		point* cam = new point(0, 0, 0);

		float val = A * cam->x + B * cam->y + C * cam->z + D;
		if (val <= 0)
		{
			A = -A;
			B = -B;
			C = -C;
			D = -D;
		}


		Xmin = WINDOW_SIZE;
		Ymin = WINDOW_SIZE;
		Xmax = -1;
		Ymax = -1;
		for (int i = 0; i < point_number; i++)
		{
			if (v[i]->x < Xmin)
				Xmin = v[i]->x;

			if (v[i]->y < Ymin)
				Ymin = v[i]->y;

			if (v[i]->x >= Xmax)
				Xmax = v[i]->x;

			if (v[i]->y >= Ymax)
				Ymax = v[i]->y;
		}
	}

};