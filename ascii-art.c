#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>		

typedef char color_t;

typedef struct {
	double x;
	double y;
} coord_t;

typedef enum {
	circle,
	rectangle,
	line,
} shape_t;

typedef struct {
	double square_radius;
} circle_t;

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
} caracteristic_t;

typedef struct {
	color_t color;
	coord_t center_gravity;
	double rotation_angle;
	shape_t shape;
	caracteristic_t caracteristic;
} figure_t;

typedef struct {
	unsigned int width;
	unsigned int height;
	double grain;
	figure_t figs[10];
	int nb_figs;
} image_t;

void print_coord (coord_t c) {
	printf("(%f, %f)", c.x, c.y);
}

coord_t coordinate (double x, double y) {
	coord_t c = {x, y};
	return c;
}

coord_t inv_translate_point (coord_t p, coord_t v) {
	coord_t c = {p.x - v.x, p.y - v.y};
	return c;
}

coord_t inv_rotate_point (coord_t p, double theta) {
	coord_t c = {p.x * cos(-theta) - p.y * sin(-theta), p.x * sin(-theta) + p.y * cos(-theta)};
	return c;
}

figure_t create_circle (double radius) {
	figure_t f;
	f.color = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = circle;
	f.caracteristic.circle.square_radius = radius * radius;	
	return f;
}

figure_t create_rectangle (double width, double height) {
	figure_t f;
	f.color = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = rectangle;
	f.caracteristic.rectangle.half_width = width/2;
	f.caracteristic.rectangle.half_height = height/2;
	return f;
}

figure_t create_line (double length) {
	figure_t f;
	f.color = '+';
	f.center_gravity = coordinate(0,0);
	f.rotation_angle = 0;
	f.shape = line;
	f.caracteristic.line.half_length = length/2;
	return f;
}

figure_t color (figure_t f, color_t c) {
	f.color = c;
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
	if (f.shape == circle) {
		if (p2.x * p2.x + p2.y * p2.y <= f.caracteristic.circle.square_radius)
			return f.color;
		else
			return 0;
	} else if (f.shape == rectangle) {
		if (absolute(p2.x) <= f.caracteristic.rectangle.half_width && absolute(p2.y) <= f.caracteristic.rectangle.half_height)
			return f.color;
		else
			return 0;
	} else {
		if (absolute(p2.x) <= f.caracteristic.line.half_length && absolute(p2.y) <= grain)
			return f.color;
		else
			return 0;
	}
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
	if (img.nb_figs <= 8) {
		img.figs[img.nb_figs] = f;
		img.nb_figs = img.nb_figs + 1;
	}
	return img;
}

void paint (image_t img) {
	for (int i = 0; i < img.width; i++) {
		for (int j = 0; j < img.height; j++) {
			bool is_print = false;
			for (int k = 0; k < img.nb_figs; k++) {
				color_t tmp = intersect(coordinate(i, j), img.figs[k], img.grain);
				if (tmp != 0) {
					printf("%c", tmp);
					is_print = true;
					k = img.nb_figs;
				}
			}
			if (is_print == false) {
				printf(" ");
			}
		}
		printf("\n");
	}
}

int main() {
  figure_t f;
  image_t img = image(50,50,0.5);
 
  f = create_line(30);
  f = translate(f, 18, 18);
  f = rotate(f, -60 * M_PI / 180);
  f = color(f, '*');
  img = append(img,f);
 
  f = create_rectangle(10,7);
  f = translate(f, 10, 8);
  f = rotate(f, 45 * M_PI / 180);
  img = append(img,f);

  f = create_circle(12);
  f = translate(f, 20, 20);
  f = color(f, 'O');
  img = append(img,f);

  paint(img);
  return EXIT_SUCCESS;
}