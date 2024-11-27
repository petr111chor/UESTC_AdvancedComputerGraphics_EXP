#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <GL/freeglut.h>

//�������������
GLuint texGround;
GLuint texWall;
GLuint texCeiling;

static int menuActive = 0;

#define BMP_Header_Length 54  //ͼ���������ڴ���е�ƫ����
static GLfloat angle = 0.0f;   //��ת�Ƕ�
static GLfloat zPosition = 0.0f;
static GLint mouseOldX, mouseOldY; // ���ľ�λ��
static GLfloat angleMouse = 0.0f;   // �����Ƶ���ת�Ƕ�
static GLfloat zPositionMouse = 0.0f; // �����Ƶ�Zλ��
static GLfloat pitchAngle = 0.0f; // �洢������

// ����power_of_two�����ж�һ�������ǲ���2����������
int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

/* ����load_texture
* ��ȡһ��BMP�ļ���Ϊ����
* ���ʧ�ܣ�����0������ɹ�������������
*/
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// ���ļ������ʧ�ܣ�����
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;
	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;

		total_bytes = line_bytes * height;
	}

	// �����������ֽ��������ڴ�
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// ��ȡ��������
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);

		return 0;
	}

	// �Ծ;ɰ汾�ļ��ݣ����ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������
	// ��ͼ���߳�����OpenGL�涨�����ֵ��Ҳ����
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			// �涨���ź��µĴ�СΪ�߳���������
			const GLint new_width = 256;
			const GLint new_height = 256;

			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// ����ÿ����Ҫ���ֽ��������ֽ���
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;

			new_total_bytes = new_line_bytes * new_height;

			// �����ڴ�
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// ������������
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// ����һ���µ�������
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// ���µ������������������������
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
	GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels); */

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //�ָ�֮ǰ�������

	free(pixels);

	return texture_ID;
}

void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);

	GLfloat aspect = (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(35.5, aspect, 1.0, 150.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutPostRedisplay();
}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	//�ƶ�����ת
	glTranslatef(0.0f, 0.0f, zPosition);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, 0.0f, zPositionMouse);
	glRotatef(pitchAngle, 1.0f, 0.0f, 0.0f); // �����仯
	glRotatef(angleMouse, 0.0f, 1.0f, 0.0f);

	for (GLfloat z = -60.0f; z <= 0.0f; z += 10.0f)
	{
		//�󶨵ذ�������Ƶذ壬ע��glBeindTexture��glBegin��glEnd������Ч��
		glBindTexture(GL_TEXTURE_2D, texGround);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, -10.0f, z);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, -10.0f, z + 10.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, -10.0f, z + 10.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(10.0f, -10.0f, z);

		glEnd();

		//���컨������
		glBindTexture(GL_TEXTURE_2D, texCeiling);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, 10.0f, z);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, 10.0f, z + 10.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, z + 10.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, z);
		glEnd();

		//��שǽ������
		glBindTexture(GL_TEXTURE_2D, texWall);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10.0f, -10.0f, z);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-10.0f, 10.0f, z);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-10.0f, 10.0f, z + 10.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10.0f, -10.0f, z + 10.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(10.0f, -10.0f, z);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(10.0f, 10.0f, z);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(10.0f, 10.0f, z + 10.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(10.0f, -10.0f, z + 10.0f);
		glEnd();
	}

	//   GLclampf prioritize[TEXTURENUM] = {0.0f, 0.0f, 1.0f};
	//   glPrioritizeTextures(TEXTURENUM, textureName, prioritize);
	//   GLboolean isResident[TEXTURENUM];
	//   if (glAreTexturesResident(TEXTURENUM, textureName, isResident))
	//   {
	//     printf("all texture is resident\n");
	//   }
	//   else
	//   {
	//     printf("texture resident is : %d %d %d", isResident[0], isResident[1], isResident[2]);
	//   }
	glPopMatrix();

	glutSwapBuffers();
}

void specialKey(GLint key, GLint x, GLint y)
{
	if (key == GLUT_KEY_UP)
	{
		zPosition += 0.5f;
	}

	if (key == GLUT_KEY_DOWN)
	{
		zPosition -= 0.5f;
	}

	if (key == GLUT_KEY_LEFT)
	{
		angle -= 0.5f;
	}

	if (key == GLUT_KEY_RIGHT)
	{
		angle += 0.5f;
	}
	if (angle > 360.0f)
	{
		angle = 0.0f;
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (menuActive) {
		// ����˵��򿪣��򲻸������λ��
		return;
	}
	if (button == GLUT_LEFT_BUTTON && button != GLUT_MIDDLE_BUTTON && button != GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseOldX = x;
			mouseOldY = y;
		}
	}
}

void motion(int x, int y) {
	GLfloat deltaX = (GLfloat)(x - mouseOldX);
	GLfloat deltaY = (GLfloat)(y - mouseOldY);

	if (deltaX != 0.0f || deltaY != 0.0f) {
		angleMouse += deltaX * 0.1f; // ��������ƶ���������ת�Ƕȣ���X�ᣩ

		// ��Ӹ����Ǵ�������ʹ��deltaY�����ƣ���ֵ��ʾ���Ͽ�����ֵ��ʾ���¿�
		GLfloat pitch = deltaY * 0.1f; // ���Ը�����Ҫ����������
		pitchAngle += pitch; 

		// ���Ƹ����ǵķ�Χ����ֹ��ת
		if (pitchAngle > 89.0f) pitchAngle = 89.0f;
		if (pitchAngle < -89.0f) pitchAngle = -89.0f;

		mouseOldX = x;
		mouseOldY = y;
		glutPostRedisplay(); // ���»��Ƴ���
	}
}


//�л���ͬ������ģʽ
void processMenu(int value)
{
	menuActive = 1;
	//   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
	switch (value)
	{
	case 0:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case 1:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;
	case 2:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		break;
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		break;
	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		break;
	case 5:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		break;
		//   case 6:
		//     if (gltIsExtSupported("GL_EXT_texture_filter_anisotropic"))
		//     {

		//       //�����������Թ���
		//       GLfloat fLargest;
		//       glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
		//       printf("anisotropic:%f\n", fLargest);
		//       glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);
		//     }
		//     break;
	default:
		break;
	}

	menuActive = 0;

	glutPostRedisplay();
}

void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//������Ȳ���,���������棬����󻭵�ǽ����ǰ����
	glEnable(GL_DEPTH_TEST);

	// ��������
	glEnable(GL_TEXTURE_2D);

	//��������
	texGround = load_texture("resource/ground.bmp");
	texWall = load_texture("resource/wall.bmp");
	texCeiling = load_texture("resource/ceiling.bmp");
}

int main(int argc, char* argv[])
{
	// GLUT��ʼ��
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("OpenGL Mipmap Demo");

	init();

	//�ص�����
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKey);
	// �����Ҽ������˵�
	glutCreateMenu(processMenu);
	glutAddMenuEntry("GL_NEAREST", 0);
	glutAddMenuEntry("GL_LINEAR", 1);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", 2);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 3);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 4);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
	// glutAddMenuEntry("ANISOTROPIC", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMouseFunc(mouse); // ע����갴���¼�������
	glutMotionFunc(motion); // ע������ƶ��¼�������

	glutMainLoop();

	return 0;
}