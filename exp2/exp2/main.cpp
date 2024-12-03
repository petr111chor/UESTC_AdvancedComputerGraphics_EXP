#include <GL/freeglut.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.14159265358979323846

// 光源设置
float lightPosition[] = { 0.0f, 3.0f, 2.0f, 1.0f };  // 光源位置
float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };   // 环境光
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // 漫射光
float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 镜面光
// 光源颜色
float lightColor[3] = { 1.0f, 1.0f, 1.0f }; 
// 更新光源位置的步长
float lightMoveSpeed = 0.5f;
// 更新光源颜色的步长
float colorChangeSpeed = 0.1f;

// 视点控制
float eyeDistance = 2.0f; // 距离球体的初始距离
float eyeAngleX = 0.0f;   // 绕X轴旋转
float eyeAngleY = 0.0f;   // 绕Y轴旋转

void initOpenGL() {
    glClearColor(0.0f, 0.1f, 0.1f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // 设置光照
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void drawSphere(float x, float y, float z, float radius, int slices, int stacks) {
    glPushMatrix();
    glTranslatef(x, y, z);
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluSphere(quadric, radius, slices, stacks);
    gluDeleteQuadric(quadric);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 计算视点位置
    float eyeX = eyeDistance * sin(eyeAngleY) * cos(eyeAngleX);
    float eyeY = eyeDistance * sin(eyeAngleX);
    float eyeZ = eyeDistance * cos(eyeAngleY) * cos(eyeAngleX);

    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // 更新光源位置和颜色
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    // 材质定义
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat no_shininess[] = { 0.0f };
    GLfloat low_shininess[] = { 5.0f };
    GLfloat high_shininess[] = { 100.0f };
    GLfloat mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };

    // 第一行
    // 有漫反射光而无环境光和镜面光
    glPushMatrix();
    glTranslatef(-2.0f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光和镜面光，并有低高光，而无环境光
    glPushMatrix();
    glTranslatef(-0.5f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光和镜面光，并有很亮的高光，而无环境光
    glPushMatrix();
    glTranslatef(1.0f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光和自发光，而无环境和镜面反射光
    glPushMatrix();
    glTranslatef(2.5f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 第二行
    // 有漫反射光和环境光，而无镜面反射光
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光、环境光和镜面光，且有低高光
    glPushMatrix();
    glTranslatef(-0.5f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光、环境光和镜面光，且有很亮的高光
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光、环境光和自发光，而无镜面光
    glPushMatrix();
    glTranslatef(2.5f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 第三行
    // 有漫反射光和有颜色的环境光，而无镜面光
    glPushMatrix();
    glTranslatef(-2.0f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光和有颜色的环境光以及镜面光，且有低高光
    glPushMatrix();
    glTranslatef(-0.5f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光和有颜色的环境光以及镜面光，且有很亮的高光
    glPushMatrix();
    glTranslatef(1.0f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // 有漫反射光和有颜色的环境光以及自发光，而无镜面光
    glPushMatrix();
    glTranslatef(2.5f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q': // 向前移动光源
        lightPosition[2] -= lightMoveSpeed;
        break;
    case 'w': // 向后移动光源
        lightPosition[2] += lightMoveSpeed;
        break;
    case 'e': // 增加红色分量
        lightColor[0] = fmin(lightColor[0] + colorChangeSpeed, 1.0f);
        break;
    case 'r': // 减少红色分量
        lightColor[0] = fmax(lightColor[0] - colorChangeSpeed, 0.0f);
        break;
    case 't': // 增加绿色分量
        lightColor[1] = fmin(lightColor[1] + colorChangeSpeed, 1.0f);
        break;
    case 'y': // 减少绿色分量
        lightColor[1] = fmax(lightColor[1] - colorChangeSpeed, 0.0f);
        break;
    case 'u': // 增加蓝色分量
        lightColor[2] = fmin(lightColor[2] + colorChangeSpeed, 1.0f);
        break;
    case 'i': // 减少蓝色分量
        lightColor[2] = fmax(lightColor[2] - colorChangeSpeed, 0.0f);
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: // 向上移动光源
        lightPosition[1] += lightMoveSpeed;
        break;
    case GLUT_KEY_DOWN: // 向下移动光源
        lightPosition[1] -= lightMoveSpeed;
        break;
    case GLUT_KEY_LEFT: // 向左移动光源
        lightPosition[0] -= lightMoveSpeed;
        break;
    case GLUT_KEY_RIGHT: // 向右移动光源
        lightPosition[0] += lightMoveSpeed;
        break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    static int lastX = 0, lastY = 0;
    int deltaX = x - lastX;
    int deltaY = y - lastY;

    eyeAngleY += deltaX * 0.01f; // 水平旋转
    eyeAngleX -= deltaY * 0.01f; // 垂直旋转

    // 限制垂直角度
    if (eyeAngleX > PI/2) eyeAngleX = PI/2;
    if (eyeAngleX < -PI/2) eyeAngleX = -PI/2;

    lastX = x;
    lastY = y;

    glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        eyeDistance -= 0.5f; // 拉近
    }
    else {
        eyeDistance += 0.5f; // 拉远
    }

    if (eyeDistance < 1.0f) eyeDistance = 1.0f; // 限制最小距离
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("实验二");

    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);      
    glutSpecialFunc(specialKeys);     
    glutMotionFunc(mouseMotion);  
    glutMouseWheelFunc(mouseWheel); 
    glutMainLoop();

    return 0;
}
