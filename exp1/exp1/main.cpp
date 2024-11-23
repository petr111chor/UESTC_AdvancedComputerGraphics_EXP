#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

typedef struct _cell {
    int id;
    int x, y;
    float min, max;
    float value;
    float step;
    char* info;
    char* format;
} cell;

cell hsv[3] = {
    { 1, 180, 80, 0.0, 1.0, 0.5, 0.01, "Specifies field of Hue.", "%.2f" },
    { 2, 240, 80, 0.0, 1.0, 0.5, 0.01, "Specifies field of Saturation.", "%.2f" },
    { 3, 300, 80, 0.0, 1.0, 0.5, 0.01, "Specifies field of Value.", "%.2f" },
};

cell rgb[3] = {
    { 4, 180, 180, 0.0, 1.0, 0.5, 0.01, "Specifies field of Red.", "%.2f" },
    { 5, 240, 180, 0.0, 1.0, 0.5, 0.01, "Specifies field of Green.", "%.2f" },
    { 6, 300, 180, 0.0, 1.0, 0.5, 0.01, "Specifies field of Blue.", "%.2f" },
};

enum {
    HSV, RGB
} mode = HSV;

GLint selection = 0;

void redisplay_all(void);
void draw_triangle(float x, float y, float size, float r, float g, float b);
void hsv_to_rgb(float h, float s, float v, float* r, float* g, float* b);
void colorcube(void);
GLuint window, hsv_view, rgb_view, command;
GLuint sub_width = 256, sub_height = 256;

GLvoid* font_style = GLUT_BITMAP_TIMES_ROMAN_10;

static GLfloat theta[] = { 0.0, 0.0, 0.0 };
static GLint axis = 2;

void setfont(char* name, int size) {
    font_style = GLUT_BITMAP_HELVETICA_10;
    if (strcmp(name, "helvetica") == 0) {
        if (size == 12)
            font_style = GLUT_BITMAP_HELVETICA_12;
        else if (size == 18)
            font_style = GLUT_BITMAP_HELVETICA_18;
    }
    else if (strcmp(name, "times roman") == 0) {
        font_style = GLUT_BITMAP_TIMES_ROMAN_10;
        if (size == 24)
            font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    }
    else if (strcmp(name, "8x13") == 0) {
        font_style = GLUT_BITMAP_8_BY_13;
    }
    else if (strcmp(name, "9x15") == 0) {
        font_style = GLUT_BITMAP_9_BY_15;
    }
}

void drawstr(GLuint x, GLuint y, char* format, ...) {
    va_list args;
    char buffer[255], * s;

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    glRasterPos2i(x, y);
    for (s = buffer; *s; s++)
        glutBitmapCharacter(font_style, *s);
}

void cell_draw(cell* cell) {
    glColor3f(0.0, 1.0, 0.5);
    if (selection == cell->id) {
        glColor3f(1.0, 1.0, 0.0);
        drawstr(10, 240, cell->info);
        glColor3f(1.0, 0.0, 0.0);
    }

    drawstr(cell->x, cell->y, cell->format, cell->value);
}

int cell_hit(cell* cell, int x, int y) {
    if (x > cell->x && x < cell->x + 60 &&
        y > cell->y - 30 && y < cell->y + 10)
        return cell->id;
    return 0;
}

void cell_update(cell* cell, int update) {
    if (selection != cell->id)
        return;

    cell->value += update * cell->step;

    if (cell->value < cell->min)
        cell->value = cell->min;
    else if (cell->value > cell->max)
        cell->value = cell->max;
}

void main_reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

#define GAP  25             /* gap between subwindows */
    sub_width = (width - GAP * 3) / 2.0;
    sub_height = (height - GAP * 3) / 2.0;

    glutSetWindow(hsv_view);
    glutPositionWindow(GAP, GAP);
    glutReshapeWindow(sub_width, sub_height);
    glutSetWindow(rgb_view);
    glutPositionWindow(GAP + sub_width + GAP, GAP);
    glutReshapeWindow(sub_width, sub_height);
    glutSetWindow(command);
    glutPositionWindow(GAP, GAP + sub_height + GAP);
    glutReshapeWindow(sub_width + GAP + sub_width, sub_height);
}

void main_display(void) {
    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    setfont("helvetica", 12);
    drawstr(GAP, GAP - 5, "HSV view");
    drawstr(GAP + sub_width + GAP, GAP - 5, "RGB view");
    drawstr(GAP, GAP + sub_height + GAP - 5, "Command manipulation window");
    glutSwapBuffers();
}

void command_reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void command_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);

    setfont("helvetica", 18);

    drawstr(180, hsv[0].y - 40, "Hue");
    drawstr(230, hsv[0].y - 40, "Satur");
    drawstr(300, hsv[0].y - 40, "Value");
    drawstr(40, hsv[0].y, "HSV(");
    drawstr(230, hsv[0].y, ",");
    drawstr(290, hsv[0].y, ",");
    drawstr(350, hsv[0].y, ")");

    drawstr(180, rgb[0].y - 40, "Red");
    drawstr(230, rgb[0].y - 40, "Green");
    drawstr(300, rgb[0].y - 40, "Blue");
    drawstr(40, rgb[0].y, "RGB(");
    drawstr(230, rgb[0].y, ",");
    drawstr(290, rgb[0].y, ",");
    drawstr(350, rgb[0].y, ")");

    cell_draw(&hsv[0]);
    cell_draw(&hsv[1]);
    cell_draw(&hsv[2]);
    cell_draw(&rgb[0]);
    cell_draw(&rgb[1]);
    cell_draw(&rgb[2]);

    if (!selection) {
        glColor3f(1.0, 1.0, 0.0);
        drawstr(10, 240,
            "Click on the arguments and move the mouse to modify values.");
    }

    glutSwapBuffers();
}

void world_reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);
}

void world_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 使用HSV值绘制三角形
    float rgb[3];
    hsv_to_rgb(hsv[0].value, hsv[1].value, hsv[2].value, &rgb[0], &rgb[1], &rgb[2]);
    draw_triangle(50, 50, 100, rgb[0], rgb[1], rgb[2]);

    glutSwapBuffers();
}

void screen_reshape(int w, int h) {
    //glViewport(0, 0, width, height);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluOrtho2D(0, width, height, 0);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
            2.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
            2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, -10.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void screen_mouse(int btn, int state, int x, int y) {
    /* mouse callback, selects an axis about which to rotate */
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        axis = 0;
    if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
        axis = 1;
    if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
        axis = 2;

    redisplay_all();

}


void screen_display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //// 使用RGB值绘制三角形
    //draw_triangle(50, 50, 100, rgb[0].value, rgb[1].value, rgb[2].value);

    glLoadIdentity();

    glRotatef(theta[0], 1.0, 0.0, 0.0);
    glRotatef(theta[1], 0.0, 1.0, 0.0);
    glRotatef(theta[2], 0.0, 0.0, 1.0);

    colorcube();

    glutMouseFunc(screen_mouse);

    glutSwapBuffers();
}

void spinCube() {
    /* Idle callback, spin cube 1 degrees about selected axis */
    theta[axis] += 0.5;
    if (theta[axis] > 360.0)
        theta[axis] -= 360.0;

    glutPostRedisplay();
}



int old_y;

void command_mouse(int button, int state, int x, int y) {
    selection = 0;
    if (state == GLUT_DOWN) {
        if (y < 120) {
            mode = HSV;
        }
        else {
            mode = RGB;
        }
        if (mode == HSV) {
            /* mouse should only hit _one_ of the cells, so adding up all
                the hits just propagates a single hit. */
            selection += cell_hit(&hsv[0], x, y);
            selection += cell_hit(&hsv[1], x, y);
            selection += cell_hit(&hsv[2], x, y);
        }
        if (mode == RGB) {
            selection += cell_hit(&rgb[0], x, y);
            selection += cell_hit(&rgb[1], x, y);
            selection += cell_hit(&rgb[2], x, y);
        }
    }

    old_y = y;

    redisplay_all();
}



void command_motion(int x, int y) {
    cell_update(&hsv[0], old_y - y);
    cell_update(&hsv[1], old_y - y);
    cell_update(&hsv[2], old_y - y);
    cell_update(&rgb[0], old_y - y);
    cell_update(&rgb[1], old_y - y);
    cell_update(&rgb[2], old_y - y);

    old_y = y;

    redisplay_all();
}

void hsv_to_rgb(float h, float s, float v, float* r, float* g, float* b) {
    int i;
    float f, p, q, t;
    if (s == 0) {
        *r = *g = *b = v;  // 如果饱和度为0，返回相同的值
        return;
    }
    if (h == 1.0)
        h = 0;
    else
        h *= 6;  // 将h从0-1范围转换为0-6
    i = floor(h);
    f = h - i;  // 计算h的小数部分
    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));
    switch (i % 6) {
    case 0: *r = v; *g = t; *b = p; break;
    case 1: *r = q; *g = v; *b = p; break;
    case 2: *r = p; *g = v; *b = t; break;
    case 3: *r = p; *g = q; *b = v; break;
    case 4: *r = t; *g = p; *b = v; break;
    case 5: *r = v; *g = p; *b = q; break;
    }
}

void draw_triangle(float x, float y, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size / 2, y + size * sqrt(3) / 2);
    glEnd();
}

void redisplay_all(void) {
    glutSetWindow(command);
    glutPostRedisplay();
    glutSetWindow(hsv_view);
    glutPostRedisplay();
    glutSetWindow(rgb_view);
    glutPostRedisplay();
}

void colorcube(void) {
    float step = 0.05;
    glBegin(GL_POINTS);
    for (float x = -1.0; x <= 1.0; x += step)
    {
        for (float y = -1.0; y <= 1.0; y += step)
        {
            for (float z = -1.0; z <= 1.0; z += step)
            {
                glColor3f(x + rgb[0].value / 2.0, y + rgb[1].value / 2.0, z + rgb[2].value / 2.0); // 设置颜色
                glVertex3f(x, y, z);
            }
        }
    }
    glEnd();
}

int main(int argc, char** argv) {
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(512 + GAP * 3, 512 + GAP * 3);
    glutInitWindowPosition(250, 50);
    glutInit(&argc, argv);

    window = glutCreateWindow("HSV-RGB");
    glutReshapeFunc(main_reshape);
    glutDisplayFunc(main_display);

    hsv_view = glutCreateSubWindow(window, GAP, GAP, 256, 256);
    glutReshapeFunc(world_reshape);
    glutDisplayFunc(world_display);

    rgb_view = glutCreateSubWindow(window, GAP + 256 + GAP, GAP, 256, 256);
    glutReshapeFunc(screen_reshape);
    glutDisplayFunc(screen_display);

    command = glutCreateSubWindow(window, GAP + 256 + GAP, GAP + 256 + GAP, 256, 256);
    glutReshapeFunc(command_reshape);
    glutDisplayFunc(command_display);
    glutMotionFunc(command_motion);
    glutMouseFunc(command_mouse);
    glutIdleFunc(spinCube);

    redisplay_all();

    glutMainLoop();

    return 0;
}
