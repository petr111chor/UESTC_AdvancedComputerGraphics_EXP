#include <GL/freeglut.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.14159265358979323846

// ��Դ����
float lightPosition[] = { 0.0f, 3.0f, 2.0f, 1.0f };  // ��Դλ��
float lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };   // ������
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // �����
float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // �����
// ��Դ��ɫ
float lightColor[3] = { 1.0f, 1.0f, 1.0f }; 
// ���¹�Դλ�õĲ���
float lightMoveSpeed = 0.5f;
// ���¹�Դ��ɫ�Ĳ���
float colorChangeSpeed = 0.1f;

// �ӵ����
float eyeDistance = 2.0f; // ��������ĳ�ʼ����
float eyeAngleX = 0.0f;   // ��X����ת
float eyeAngleY = 0.0f;   // ��Y����ת

void initOpenGL() {
    glClearColor(0.0f, 0.1f, 0.1f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    // ���ù���
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

    // �����ӵ�λ��
    float eyeX = eyeDistance * sin(eyeAngleY) * cos(eyeAngleX);
    float eyeY = eyeDistance * sin(eyeAngleX);
    float eyeZ = eyeDistance * cos(eyeAngleY) * cos(eyeAngleX);

    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    // ���¹�Դλ�ú���ɫ
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);

    // ���ʶ���
    GLfloat no_mat[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat no_shininess[] = { 0.0f };
    GLfloat low_shininess[] = { 5.0f };
    GLfloat high_shininess[] = { 100.0f };
    GLfloat mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };

    // ��һ��
    // �����������޻�����;����
    glPushMatrix();
    glTranslatef(-2.0f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ���������;���⣬���е͸߹⣬���޻�����
    glPushMatrix();
    glTranslatef(-0.5f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ���������;���⣬���к����ĸ߹⣬���޻�����
    glPushMatrix();
    glTranslatef(1.0f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // �����������Է��⣬���޻����;��淴���
    glPushMatrix();
    glTranslatef(2.5f, 1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // �ڶ���
    // ���������ͻ����⣬���޾��淴���
    glPushMatrix();
    glTranslatef(-2.0f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ��������⡢������;���⣬���е͸߹�
    glPushMatrix();
    glTranslatef(-0.5f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ��������⡢������;���⣬���к����ĸ߹�
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ��������⡢��������Է��⣬���޾����
    glPushMatrix();
    glTranslatef(2.5f, 0.0f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ������
    // ��������������ɫ�Ļ����⣬���޾����
    glPushMatrix();
    glTranslatef(-2.0f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ��������������ɫ�Ļ������Լ�����⣬���е͸߹�
    glPushMatrix();
    glTranslatef(-0.5f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ��������������ɫ�Ļ������Լ�����⣬���к����ĸ߹�
    glPushMatrix();
    glTranslatef(1.0f, -1.5f, -7.0f);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    drawSphere(0.0f, 0.0f, 0.0f, 0.5f, 30, 30);
    glPopMatrix();

    // ��������������ɫ�Ļ������Լ��Է��⣬���޾����
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
    case 'q': // ��ǰ�ƶ���Դ
        lightPosition[2] -= lightMoveSpeed;
        break;
    case 'w': // ����ƶ���Դ
        lightPosition[2] += lightMoveSpeed;
        break;
    case 'e': // ���Ӻ�ɫ����
        lightColor[0] = fmin(lightColor[0] + colorChangeSpeed, 1.0f);
        break;
    case 'r': // ���ٺ�ɫ����
        lightColor[0] = fmax(lightColor[0] - colorChangeSpeed, 0.0f);
        break;
    case 't': // ������ɫ����
        lightColor[1] = fmin(lightColor[1] + colorChangeSpeed, 1.0f);
        break;
    case 'y': // ������ɫ����
        lightColor[1] = fmax(lightColor[1] - colorChangeSpeed, 0.0f);
        break;
    case 'u': // ������ɫ����
        lightColor[2] = fmin(lightColor[2] + colorChangeSpeed, 1.0f);
        break;
    case 'i': // ������ɫ����
        lightColor[2] = fmax(lightColor[2] - colorChangeSpeed, 0.0f);
        break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP: // �����ƶ���Դ
        lightPosition[1] += lightMoveSpeed;
        break;
    case GLUT_KEY_DOWN: // �����ƶ���Դ
        lightPosition[1] -= lightMoveSpeed;
        break;
    case GLUT_KEY_LEFT: // �����ƶ���Դ
        lightPosition[0] -= lightMoveSpeed;
        break;
    case GLUT_KEY_RIGHT: // �����ƶ���Դ
        lightPosition[0] += lightMoveSpeed;
        break;
    }
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    static int lastX = 0, lastY = 0;
    int deltaX = x - lastX;
    int deltaY = y - lastY;

    eyeAngleY += deltaX * 0.01f; // ˮƽ��ת
    eyeAngleX -= deltaY * 0.01f; // ��ֱ��ת

    // ���ƴ�ֱ�Ƕ�
    if (eyeAngleX > PI/2) eyeAngleX = PI/2;
    if (eyeAngleX < -PI/2) eyeAngleX = -PI/2;

    lastX = x;
    lastY = y;

    glutPostRedisplay();
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        eyeDistance -= 0.5f; // ����
    }
    else {
        eyeDistance += 0.5f; // ��Զ
    }

    if (eyeDistance < 1.0f) eyeDistance = 1.0f; // ������С����
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
    glutCreateWindow("ʵ���");

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
