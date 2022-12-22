#pragma once
#include "../libwinbgi/src/graphics.h"
#include <stdio.h>
#include <queue>
#include <iostream>
#include "graph.h"
#include "figures.h"

#define MAX_DEPTH -1000000
class window
{
public:
	int xs, ys, size;

	window() {};
	window(float x, float y, float s) : xs(x), ys(y), size(s) {};

	void dot(int x_isx, int y_isx, int fill_color)
	{
		if (x_isx < xs || y_isx < ys || x_isx >= xs+size || y_isx >= ys + size)
			return;

		if (getpixel(x_isx, y_isx) != fill_color)
		{
			putpixel(x_isx, y_isx, fill_color);
			dot(x_isx + 1, y_isx, fill_color);
			dot(x_isx, y_isx + 1, fill_color);
			dot(x_isx - 1, y_isx, fill_color);
			dot(x_isx, y_isx - 1, fill_color);
		}
		return;
	}
	void colour(int fill_color)
	{
		int midx = xs + size / 2;
		int midy = ys + size / 2; 
		dot(midx, midy, fill_color);

	}
};

class sides
{
public:
	int pol_num;
	side** planes;

	sides()
	{
		pol_num = 0;
		planes = nullptr;
	}

	sides(int _number, side** _planes)
	{
		planes = _planes;
		pol_num = _number;
	}

	void update()
	{
		for (int i = 0; i < pol_num; i++)
			planes[i]->update_data();
	}

};

class vertexs
{
public:
	point** points;
	int num;

	vertexs(int _number, point** _points)
	{
		points = _points;
		num = _number;
	}
};




class warnok
{
	vertexs* all_points;
	sides* all_sides;
	figure p1;
	figure p2;
	point* d1;
	point* d2;

	int frame_size;

	void sizing(int _i, window* _w, bool* _isNotVisible)
	{
		int Xleft = _w->xs;
		int Xright = _w->xs + _w->size - 1;
		int Ydown = _w->ys;
		int Yup = _w->ys + _w->size - 1;

		*_isNotVisible = true;

		if (all_sides->planes[_i]->Xmin > Xright) {
			*_isNotVisible = false;
		}
		if (all_sides->planes[_i]->Xmax < Xleft) {
			*_isNotVisible = false;
		}

		if (all_sides->planes[_i]->Ymin > Yup) {
			*_isNotVisible = false;
		}
		if (all_sides->planes[_i]->Ymax < Ydown) {
			*_isNotVisible = false;
		}

	}



	void zatravka(window* _w, int _color)
	{
		_w->colour(_color);
		setcolor(0);
	}

	void visibility(point* p, point* p1, point* p2, int* pointSeen)
	{
		int r1 = (p->x - p1->x) * (p2->y - p1->y);
		int r2 = (p->y - p1->y) * (p2->x - p1->x);
		int r3 = r1 - r2;

		if (r3 > 0)
			*pointSeen = 1;
		if (r3 == 0)
			*pointSeen = 0;
		if (r3 < 0)
			*pointSeen = -1;
	}

	void getz(int i, window* _w, int* z)
	{
		point* mid = new point;

		mid->x = _w->xs + _w->size / 2;
		mid->y = _w->ys + _w->size / 2;

		if (all_sides->planes[i]->C == 0)
		{
			for (int j = 1; j < all_sides->pol_num; j++)
				*z = max(all_points->points[j]->z, all_points->points[j - 1]->z);
		}
		else
		{
			float A = all_sides->planes[i]->A;
			float B = all_sides->planes[i]->B;
			float C = all_sides->planes[i]->C;
			float D = all_sides->planes[i]->D;
			*z = -(A * mid->x + B * mid->y + D) / C;
		}
	}


	void coverage(window* _w, int* number)
	{
		int Zmax = MAX_DEPTH;
		int num = 0;

		point* okno_center = new point;

		okno_center->x = _w->xs + _w->size / 2;
		okno_center->y = _w->ys + _w->size / 2;

		for (int i = 0; i < all_sides->pol_num; i++)
		{
			int idx = all_sides->planes[i]->idx;
			int isSeen = 0;

			for (int j = 0; j < all_sides->planes[i]->point_number - 1; j++)
			{
				d1->x = all_points->points[idx]->x;
				d1->y = all_points->points[idx]->y;
				d2->x = all_points->points[idx + 1]->x;
				d2->y = all_points->points[idx + 1]->y;
				visibility(okno_center, d1, d2, &isSeen);
				if (isSeen < 0)
					break;
				idx++;
			}

			if (isSeen >= 0)
			{
				d1->x = all_points->points[idx]->x;
				d1->y = all_points->points[idx]->y;
				int start_idx = all_sides->planes[i]->idx;
				d2->x = all_points->points[start_idx]->x;
				d2->y = all_points->points[start_idx]->y;

				visibility(okno_center, d1, d2, &isSeen);

				if (isSeen >= 0)
				{
					int z = 0;
					getz(i, _w, &z);
					if (z >= Zmax)
					{
						Zmax = z;
						*number = i;
					}
				}
			}

		}
	}

	void draw()
	{
		std::vector<window*> stack;
		window* frame = new window(0, 0, frame_size);

		stack.push_back(frame);

		bool notEmpty;
		while (stack.size() != 0)
		{
			window* wind = stack.back();

			stack.pop_back();

			notEmpty = false;
			int idx = 0;
			while (idx < all_sides->pol_num && notEmpty == false)
			{
				sizing(idx, wind, &notEmpty);
				idx++;
			}

			if (notEmpty)
			{
				if (wind->size > 1)
				{
					int s = wind->size / 2;
					int Xst = wind->xs;
					int Yst = wind->ys;

					window* newOkno1 = new window(Xst + s, Yst + s, s);
					window* newOkno2 = new window(Xst, Yst + s, s);
					window* newOkno3 = new window(Xst + s, Yst, s);
					window* newOkno4 = new window(Xst, Yst, s);

					stack.push_back(newOkno1);
					stack.push_back(newOkno2);
					stack.push_back(newOkno3);
					stack.push_back(newOkno4);
				}
				else
				{
					int num = -1;
					coverage(wind, &num);
					if (num >= 0)
						zatravka(wind, all_sides->planes[num]->color);
					else
						zatravka(wind, BCKG_COLOR);
				}
			}
			else
				zatravka(wind, BCKG_COLOR);

		}

	}

	void draw_scene()
	{
		p1.create_shadow();
		p1.proj();
		p2.create_shadow();
		p2.proj();

		all_sides->update();

		clearviewport();
		draw();

		p1.unproj();
		p2.unproj();
	}
	void menu()
	{
		initwindow(frame_size, frame_size);
		p1.scale(0.2, 0.2, 0.2);
		p1.move(0, 40, 0);
		p2.move(200, 70, 0);
		char c;
		while (true)
		{
			draw_scene();
			c = getch();
			switch (c)
			{
			case '0':
				closegraph();
				return;

			case '1':
				p1.scale(2, 2, 2);
				break;
			case '2':
				p1.scale(0.5, 0.5, 0.5);
				break;
			case 'd':
				p1.move(30, 0, 0);
				break;
			case 'a':
				p1.move(-30, 0, 0);
				break;

			case 'w':
				p1.move(0, -30, 0);
				break;
			case 's':
				p1.move(0, 30, 0);
				break;
			case 'q':
				p1.move(0, 0, 30);
				break;
			case 'e':
				p1.move(0, 0, -30);
				break;

			case 'r':
				p1.rotate(30, X);
				break;
			case 'f':
				p1.rotate(-30, X);
				break;
			case 'c':
				p1.rotate(30, Y);
				break;
			case 'v':
				p1.rotate(-30, Y);
				break;
			case 'z':
				p1.rotate(30, Z);
				break;
			case 'x':
				p1.rotate(-30, Z);
				break;


			case '6':
				p2.scale(2, 2, 2);
				break;
			case '7':
				p2.scale(0.5, 0.5, 0.5);
				break;
			case 'k':
				p2.move(30, 0, 0);
				break;
			case 'h':
				p2.move(-30, 0, 0);
				break;
			case 'u':
				p2.move(0, -30, 0);
				break;
			case 'j':
				p2.move(0, 30, 0);
				break;
			case 'y':
				p2.move(0, 0, 30);
				break;
			case 'i':
				p2.move(0, 0, -30);
				break;
			case 'b':
				p2.rotate(30, X);
				break;
			case 'n':
				p2.rotate(-30, X);
				break;
			case 'm':
				p2.rotate(30, Y);
				break;
			case ',':
				p2.rotate(-30, Y);
				break;
			case 'o':
				p2.rotate(30, Z);
				break;
			case 'l':
				p2.rotate(-30, Z);
				break;

			default:
				break;
			}
		}
		closegraph();
	}

public:

	warnok()
	{
		frame_size = WINDOW_SIZE;
		d1 = new point;
		d2 = new point;
	}

	void start()
	{
		point* A = new point(0, 100, 100);
		point* B = new point(100, 100, 100);
		point* C = new point(100, 100, 0);
		point* D = new point(0, 100, 0);
		point* E = new point(50, 0, 100);
		point* F = new point(150, 0, 100);
		point* G = new point(150, 0, 0);
		point* H = new point(50, 0, 0);

		side* ABFE = new side(A, B, F, E, 0, 10);
		side* BCGF = new side(B, C, G, F, 4, 11);
		side* CDHG = new side(C, D, H, G, 8, 12);
		side* DAEH = new side(D, A, E, H, 12, 13);
		side* EFGH = new side(E, F, G, H, 16, 14);
		side* DCBA = new side(D, C, B, A, 20, 15);
	
		point* s_A = new point(0, 100, 100);
		point* s_B = new point(100, 100, 100);
		point* s_C = new point(100, 100, 0);
		point* s_D = new point(0, 100, 0);
		point* s_E = new point(0, 0, 100);
		point* s_F = new point(100, 0, 100);
		point* s_G = new point(100, 0, 0);
		point* s_H = new point(0, 0, 0);

		side* s_ABFE = new side(s_A, s_B, s_F, s_D, 24, 8);
		side* s_BCGF = new side(s_B, s_C, s_G, s_F, 28, 8);
		side* s_CDHG = new side(s_C, s_D, s_H, s_G, 32, 8);
		side* s_DAEH = new side(s_D, s_A, s_E, s_H, 36, 8);
		side* s_EFGH = new side(s_E, s_F, s_G, s_H, 40, 8);
		side* s_DCBA = new side(s_D, s_C, s_B, s_A, 44, 8);

		point* s_points[4 * 6] = {
			 s_A,  s_B,  s_F,  s_E,
			 s_B,  s_C,  s_G,  s_F,
			 s_C,  s_D,  s_H,  s_G,
			 s_D,  s_A,  s_E,  s_H,
			 s_E,  s_F,  s_G,  s_H,
			 s_D,  s_C,  s_B,  s_A
		};

		point* quad_points[8] = { A, B, C, D, E, F, G, H };
		point* quad_spoints[8] = {
			 s_A,  s_B,  s_C,  s_D,  s_E,  s_F,  s_G,  s_H
		};



		point* I_ = new point(0, 0, 100);
		point* J_ = new point(100, 100, 100);
		point* K_ = new point(100, 100, 0);
		point* L_ = new point(50, 100, 50);

		side* s7 = new side(I_, J_, L_, 48, 11);
		side* s8 = new side(J_, K_, L_, 51, 12);
		side* s9 = new side(K_, I_, L_, 54, 13);
		side* s10 = new side(I_, K_, J_, 57, 14);

		point* sI_ = new point(0, 0, 100);
		point* sJ_ = new point(100, 100, 100);
		point* sK_ = new point(100, 100, 0);
		point* sL_ = new point(50, 100, 50);

		side* ss7 =  new side(sI_, sJ_, sL_, 60, 8);
		side* ss8 = new side(sJ_, sK_, sL_, 63, 8);
		side* ss9 = new side(sK_, sI_, sL_, 66, 8);
		side* ss10 = new side(sL_, sK_, sJ_, 69, 8);

		point* tri_points[4] = { I_, J_, K_, L_ };
		point* tri_spoints[4] = { sI_, sJ_, sK_, sL_ };

		point* points[] = {
			A, B, F, E, 	B, C, G, F, 	C, D, H, G, 	D, A, E, H, 	E, F, G, H, 	D, C, B, A, 
			s_A,  s_B,  s_F,  s_E,	s_B,  s_C,  s_G,  s_F,	s_C,  s_D,  s_H,  s_G,	s_D,  s_A,  s_E,  s_H,	s_E,  s_F,  s_G,  s_H,s_D,  s_C,  s_B,  s_A,
			I_, J_, L_,	J_, K_, L_,	K_, I_, L_,	I_, K_, J_,	sI_, sJ_, sL_,	sJ_, sK_, sL_,	sK_, sI_, sL_,	sI_, sK_, sJ_
		};


		side* planes[] = {	
			ABFE, BCGF, CDHG, DAEH, EFGH,  DCBA,
			s_ABFE, s_BCGF, s_CDHG, s_DAEH, s_EFGH,  s_DCBA,
			s7,	s8,	s9,	s10, 
			ss7,ss8,ss9,ss10
}			;


		all_points = new vertexs(72, points);
		all_sides = new sides(20, planes);

		p1 = *new figure(quad_points, quad_spoints, 8);
		p2 = *new figure(tri_points, tri_spoints, 4);

		menu();
	}

	
};