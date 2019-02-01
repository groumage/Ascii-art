#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define MAX_FIGS 20

typedef enum {
	black,
	red,
	green,
	yellow,
	blue,
} color_t;

typedef struct {
	double x;
	double y;
} coord_t;

typedef enum {
	circle,
	rectangle,
	line,
	disc,
} shape_t;

typedef struct {
	double square_radius;
} circle_t;

typedef struct {
	circle_t circle_inside;
	circle_t circle_outside;
} disc_t;

typedef struct {
	double half_width;
	double half_height;
} rectangle_t;

typedef struct {
	double half_length;
} line_t;

typedef union {
	circle_t circle;
	rectangle_t rectangle;
	line_t line;
	disc_t disc;
} caracteristic_t;

typedef struct {
	color_t color;
	char symbol;
	coord_t center_gravity;
	double rotation_angle;
	shape_t shape;
	double angle;
	caracteristic_t caracteristic;
} figure_t;

typedef struct {
	unsigned int width;
	unsigned int height;
	double grain;
	figure_t figs[MAX_FIGS];
	int nb_figs;
} image_t;

// print a struct coord
void print_coord (coord_t c) {
	printf("(%f, %f)", c.x, c.y);
}

// return a struct coord with x coord and y coord
coord_t coordinate (double x, double y) {
	coord_t c = {x, y};
	return c;
}

// return the invert translate of a point p by a vector v
coord_t inv_translate_point (coord_t p, coord_t v) {
	coord_t c = {p.x - v.x, p.y - v.y};
	return c;
}

// return the translate of a point p by a vector v
coord_t translate_point (coord_t p, coord_t v) {
	coord_t c = {p.x + v.x, p.y + v.y};
	return c;
}

// return the invert rotate of a point by and angle theta
coord_t inv_rotate_point (coord_t p, double theta) {
	coord_t c = {p.x * cos(-theta) - p.y * sin(-theta), p.x * sin(-theta) + p.y * cos(-theta)};
	return c;
}

// return a circle with a radius of radius and all other parameters to default
figure_t create_circle (double radius) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.angle = 360;
	f.shape = circle;
	f.caracteristic.circle.square_radius = radius * radius;
	return f;
}

// return a rectangle with a width of width, a height of height and all other parameters to default
figure_t create_rectangle (double width, double height) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = rectangle;
	f.angle = 360;
	f.caracteristic.rectangle.half_width = width/2;
	f.caracteristic.rectangle.half_height = height/2;
	return f;
}

// return a line with a length of length and all other parameters to default
figure_t create_line (double length) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = line;
	f.angle = 360;
	f.caracteristic.line.half_length = length/2;
	return f;
}

// return a disc with a radius of radius_outside and a thickness of (radius_outside - radious_inside)
figure_t create_disc (double radius_inside, double radius_outside) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = disc;
	f.angle = 360;
	f.caracteristic.disc.circle_inside.square_radius = radius_inside * radius_inside;
	f.caracteristic.disc.circle_outside.square_radius = radius_outside * radius_outside;
	return f;
}

figure_t color (figure_t f, color_t c) {
	f.color = c;
	return f;
}

figure_t symbol (figure_t f, char c) {
	f.symbol = c;
	return f;
}

figure_t angle (figure_t f, double a) {
	f.angle = a;
	return f;
}

figure_t translate (figure_t f, double dx, double dy) {
	f.center_gravity = coordinate(f.center_gravity.x + dx, f.center_gravity.y + dy);
	return f;
}

figure_t rotate (figure_t f, double dt) {
	f.rotation_angle = f.rotation_angle + dt;
	return f;
}

// return the absolute value of a number
double absolute (double p) {
  if (p < 0) return -p;
  else return p;
}

// return the angle in degre of the point, reliative to the circle
// this function cimpute a scalar product
double relative_angle (figure_t f, coord_t a) {
	double angle_deg = 0;
	// if the point a has a negativ sinus, the point is tranform to it's opposite
	// this allow calcul of angle above 180 degres
	if (a.y < 0) {
		a.x = -a.x;
		a.y = -a.y;
		angle_deg += 180;
	}
	coord_t o = coordinate(0, 0);
	coord_t b = coordinate(sqrt(f.caracteristic.circle.square_radius), 0);
	coord_t vect_oa = coordinate(a.x - o.x, a.y - o.y);
	coord_t vect_ob = coordinate(b.x - o.x, b.y - o.y);
	double norm_oa = sqrt(vect_oa.x * vect_oa.x + vect_oa.y * vect_oa.y);
	double norm_ob = sqrt(vect_ob.x * vect_ob.x + vect_ob.y * vect_ob.y);
	double cos_angle = (vect_oa.x * vect_ob.x + vect_oa.y * vect_ob.y)/(norm_oa * norm_ob);
	double angle_rad = acos(cos_angle);
	angle_deg += angle_rad * 180/M_PI;
	return angle_deg;
}

color_t intersect (coord_t p, figure_t f, double grain) {
	coord_t a_tmp = inv_translate_point(p, f.center_gravity);
	coord_t a = inv_rotate_point(a_tmp, f.rotation_angle);
	if (a.x == 0 && a.y == 0 && f.shape != disc)
		return f.symbol;
	if (f.shape == circle)
		if (a.x * a.x + a.y * a.y <= f.caracteristic.circle.square_radius)
			if (relative_angle(f, a) <= f.angle)
				return f.symbol;
	if (f.shape == rectangle)
		if (absolute(a.x) <= f.caracteristic.rectangle.half_width && absolute(a.y) <= f.caracteristic.rectangle.half_height)
			if (relative_angle(f, a) <= f.angle)
				return f.symbol;
	if (f.shape == line)
		if (absolute(a.x) <= f.caracteristic.line.half_length && absolute(a.y) <= grain)
			if (relative_angle(f, a) <= f.angle)
				return f.symbol;
	if (f.shape == disc)
		if (a.x * a.x + a.y * a.y <= f.caracteristic.disc.circle_outside.square_radius &&
			f.caracteristic.disc.circle_inside.square_radius <= a.x * a.x + a.y * a.y)
			if (relative_angle(f, a) <= f.angle)
				return f.symbol;
	return 0;
} 

image_t image (unsigned int width, unsigned int height, double grain) {
	image_t img;
	img.width = width;
	img.height = height;
	img.grain = grain;
	img.nb_figs = 0;
	return img;
}

image_t append (image_t img, figure_t f) {
	if (img.nb_figs <= MAX_FIGS - 1) {
		img.figs[img.nb_figs] = f;
		img.nb_figs = img.nb_figs + 1;
	}
	return img;
}

// change the color of which is print on the console
void change_color (color_t c) {
	switch (c) {
		case black:
		printf("\033[30m");
		break;
		case red:
		printf("\033[31m");
		break;
		case green:
		printf("\033[32m");
		break;
		case yellow:
		printf("\033[33m");
		break;
		case blue:
		printf("\033[34m");
		break;
		default:
		printf("\033[0m");
	}
}

void paint (image_t img) {
	for (int i = 0; i < img.height; i++) {
		for (int j = 0; j < img.width; j++) {
			bool is_print = false;
			for (int k = 0; k < img.nb_figs; k++) {
				color_t tmp = intersect(coordinate(i, j), img.figs[k], img.grain);
				if (tmp != 0) {
					change_color(img.figs[k].color);
					printf("%c", tmp);
					change_color(-1);
					is_print = true;
					k = img.nb_figs;
				}
			}
			if (is_print == false)
				printf(" ");
		}
		printf("\n");
	}
}

// TEST ZONE

int main() {
  figure_t f;
  image_t img = image(50,30,0.5);

  f = create_line(30);
  f = translate(f, 15, 20);
  f = rotate(f, -60 * M_PI / 180);
  f = symbol(f, '*');
  f = color(f, red);
  img = append(img,f);
 
  f = create_rectangle(10,7);
  f = translate(f, 7, 10);
  f = rotate(f, 45 * M_PI / 180);
  f = color(f, blue);
  img = append(img,f);
 
  f = create_circle(12);
  f = translate(f, 15, 20);
  f = symbol(f, 'O');
  f = color(f, yellow);
  img = append(img,f);

  paint(img);
  return EXIT_SUCCESS;
}

