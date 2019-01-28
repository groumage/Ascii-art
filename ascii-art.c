#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define max_figs 10

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
	double angle;
} circle_t;

typedef struct {
	double square_radius_outside;
	double square_radius_inside;
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
	caracteristic_t caracteristic;
} figure_t;

typedef struct {
	unsigned int width;
	unsigned int height;
	double grain;
	figure_t figs[max_figs];
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

// return the invert rotate of a point by and angle theta
coord_t inv_rotate_point (coord_t p, double theta) {
	coord_t c = {p.x * cos(-theta) - p.y * sin(-theta), p.x * sin(-theta) + p.y * cos(-theta)};
	return c;
}

// return a circle with a radius radius and all other parameter by default
figure_t create_circle (double radius) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = circle;
	f.caracteristic.circle.square_radius = radius * radius;	
	return f;
}

// return a rectangle with a width width, a height height and all other parameter by default
figure_t create_rectangle (double width, double height) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = rectangle;
	f.caracteristic.rectangle.half_width = width/2;
	f.caracteristic.rectangle.half_height = height/2;
	return f;
}

figure_t create_line (double length) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = line;
	f.caracteristic.line.half_length = length/2;
	return f;
}

figure_t create_disc (double radius_inside, double radius_outside) {
	figure_t f;
	f.color = -1;
	f.symbol = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = disc;
	f.caracteristic.disc.square_radius_inside = radius_inside * radius_inside;
	f.caracteristic.disc.square_radius_outside = radius_outside * radius_outside;
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

figure_t translate (figure_t f, double dx, double dy) {
	f.center_gravity = coordinate(f.center_gravity.x + dx, f.center_gravity.y + dy);
	return f;
}

figure_t rotate (figure_t f, double dt) {
	f.rotation_angle = f.rotation_angle + dt;
	return f;
}

double absolute (double p) {
  if (p < 0) return -p;
  else return p;
}

color_t intersect (coord_t p, figure_t f, double grain) {
	coord_t p1 = inv_translate_point(p, f.center_gravity);
	coord_t p2 = inv_rotate_point(p1, f.rotation_angle);
	if (f.shape == circle)
		if (p2.x * p2.x + p2.y * p2.y <= f.caracteristic.circle.square_radius)
			return f.symbol;
	if (f.shape == rectangle)
		if (absolute(p2.x) <= f.caracteristic.rectangle.half_width && absolute(p2.y) <= f.caracteristic.rectangle.half_height)
			return f.symbol;
	if (f.shape == line)
		if (absolute(p2.x) <= f.caracteristic.line.half_length && absolute(p2.y) <= grain)
			return f.symbol;
	if (f.shape == disc)
		if (p2.x * p2.x + p2.y * p2.y <= f.caracteristic.disc.square_radius_outside &&
			f.caracteristic.disc.square_radius_inside <= p2.x * p2.x + p2.y * p2.y)
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
	if (img.nb_figs <= max_figs - 1) {
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
	for (int i = 0; i < img.width; i++) {
		for (int j = 0; j < img.height; j++) {
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

int main() {
  figure_t f;
  image_t img = image(30,30,1);

  //f = rotate(f, 90 * M_PI / 180);

  /*f = create_circle(10);
  f = translate(f, 15, 15);
  f = color(f, -1);
  f = symbol(f, '0');
  img = append(img,f);*/

  f = create_line(19);
  f = translate(f, 16, 20);
  f = rotate(f, -45 * M_PI / 180);
  f = color(f, yellow);
  f = symbol(f, '*');
  img = append(img,f);

  f = create_rectangle(3,10);
  f = translate(f, 10, 8);
  f = color(f, yellow);
  f = symbol(f, 'T');
  img = append(img,f);

  f = create_rectangle(10,3);
  f = translate(f, 17, 8);
  f = color(f, yellow);
  f = symbol(f, 'T');
  img = append(img,f);

  f = create_disc(4,6);
  f = translate(f, 16, 20);
  f = symbol(f, 'O');
  f = color(f, yellow);
  img = append(img,f);
 
  paint(img);
  return EXIT_SUCCESS;
}