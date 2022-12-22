#pragma once
#include <stdio.h>
#include "graph.h"


class figure
{
	point* mid;

	void calcMiddle()
	{
		mid->x = 0;
		for (int i = 0; i < num; i++)
			mid->x += points[i]->x;

		mid->x /= (float)num;

		mid->y = 0;
		for (int i = 0; i < num; i++)
			mid->y += points[i]->y;

		mid->y /= (float)num;

		mid->z = 0;
		for (int i = 0; i < num; i++)
			mid->z += points[i]->z;

		mid->z /= (float)num;

		mid->w = 1;
	}


public:
	int side_num;
	int num;
	point** points;
	point** points_shadow;
	
	side** f_sides;
	side** f_s_sides;

	figure()
	{
	}

	figure(point** p, int n)
	{
		points = p;
		points_shadow = new point * [n];
		for (int i = 0; i < n; i++)
		{
			points_shadow[i] = new point;
		}
		num = n;
		mid = new point;
	}

	figure(point** p, point** s_p, int n)
	{
		points = p;
		points_shadow = s_p;

		num = n;
		mid = new point;
	}
	figure(point** p, point** p_s, side** s, side** s_s, int vn, int sn)
	{
		points = p;
		points_shadow = new point * [vn];
		num = vn;
		mid = new point;
		f_sides = s;
		f_s_sides = s_s;

		side_num = sn;
	}


	void move(double x, double y, double z)
	{
		for (int i = 0; i < num; i++)
			points[i]->move(x, y, z);
	}

	void scale(double x, double y, double z)
	{
		calcMiddle();
		move(-mid->x, -mid->y, -mid->z);
		for (int i = 0; i < num; i++)
			points[i]->scale(x, y, z);
		move(mid->x, mid->y, mid->z);
	}

	void rotate(double phi, int axis)
	{
		calcMiddle();
		move(-mid->x, -mid->y, -mid->z);
		for (int i = 0; i < num; i++)
			points[i]->rotate(phi, axis);
		move(mid->x, mid->y, mid->z);
	}

	void create_shadow()
	{
		double y = 450;
		point* L = new point(100, 0, 0);
		for (int i = 0; i < num; i++)
		{
			int t = (y - points[i]->y) / (L->y - points[i]->y);
			int x = (L->x - points[i]->x) * t + points[i]->x;
			int z = (L->z - points[i]->z) * t + points[i]->z;
			points_shadow[i]->x = x;
			points_shadow[i]->y = y;
			points_shadow[i]->z = z;
			points_shadow[i]->rotate(20, X);
		}		
	}

	void proj()
	{
		for (int i = 0; i < num; i++)
			points[i]->rotate(20, X);
	}

	void unproj()
	{
		for (int i = 0; i < num; i++)
			points[i]->rotate(-20, X);
	}
};