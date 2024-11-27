#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <GL/freeglut.h>

//定义纹理对象编号
GLuint texGround;
GLuint texWall;
GLuint texCeiling;

static int menuActive = 0;

#define BMP_Header_Length 54  //图像数据在内存块中的偏移量
static GLfloat angle = 0.0f;   //旋转角度
static GLfloat zPosition = 0.0f;
static GLint mouseOldX, mouseOldY; // 鼠标的旧位置
static GLfloat angleMouse = 0.0f;   // 鼠标控制的旋转角度
static GLfloat zPositionMouse = 0.0f; // 鼠标控制的Z位置
static GLfloat pitchAngle = 0.0f; // 存储俯仰角

// 函数power_of_two用于判断一个整数是不是2的整数次幂
int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

/* 函数load_texture
* 读取一个BMP文件作为纹理
* 如果失败，返回0，如果成功，返回纹理编号
*/
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// 打开文件，如果失败，返回
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;
	// 读取文件中图象的宽度和高度
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// 计算每行像素所占字节数，并根据此数据计算总像素字节数
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;

		total_bytes = line_bytes * height;
	}

	// 根据总像素字节数分配内存
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// 读取像素数据
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);

		return 0;
	}

	// 对就旧版本的兼容，如果图象的宽度和高度不是的整数次方，则需要进行缩放
	// 若图像宽高超过了OpenGL规定的最大值，也缩放
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			// 规定缩放后新的大小为边长的正方形
			const GLint new_width = 256;
			const GLint new_height = 256;

			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// 计算每行需要的字节数和总字节数
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;

			new_total_bytes = new_line_bytes * new_height;

			// 分配内存
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// 进行像素缩放
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// 分配一个新的纹理编号
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// 绑定新的纹理，载入纹理并设置纹理参数
	// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
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
	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定

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

	//移动和旋转
	glTranslatef(0.0f, 0.0f, zPosition);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);

	glTranslatef(0.0f, 0.0f, zPositionMouse);
	glRotatef(pitchAngle, 1.0f, 0.0f, 0.0f); // 俯仰变化
	glRotatef(angleMouse, 0.0f, 1.0f, 0.0f);

	for (GLfloat z = -60.0f; z <= 0.0f; z += 10.0f)
	{
		//绑定地板纹理绘制地板，注意glBeindTexture在glBegin和glEnd中是无效的
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

		//绑定天花板纹理
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

		//绑定砖墙的纹理
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
		// 如果菜单打开，则不更新鼠标位置
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
		angleMouse += deltaX * 0.1f; // 根据鼠标移动量调整旋转角度（绕X轴）

		// 添加俯仰角处理，这里使用deltaY来控制，正值表示向上看，负值表示向下看
		GLfloat pitch = deltaY * 0.1f; // 可以根据需要调整灵敏度
		pitchAngle += pitch; 

		// 限制俯仰角的范围，防止翻转
		if (pitchAngle > 89.0f) pitchAngle = 89.0f;
		if (pitchAngle < -89.0f) pitchAngle = -89.0f;

		mouseOldX = x;
		mouseOldY = y;
		glutPostRedisplay(); // 重新绘制场景
	}
}


//切换不同的纹理模式
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

		//       //开启各向异性过滤
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

	//开启深度测试,消除隐藏面，避免后画的墙画到前面来
	glEnable(GL_DEPTH_TEST);

	// 启用纹理
	glEnable(GL_TEXTURE_2D);

	//加载纹理
	texGround = load_texture("resource/ground.bmp");
	texWall = load_texture("resource/wall.bmp");
	texCeiling = load_texture("resource/ceiling.bmp");
}

int main(int argc, char* argv[])
{
	// GLUT初始化
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(400, 400);
	glutCreateWindow("OpenGL Mipmap Demo");

	init();

	//回调函数
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(specialKey);
	// 创建右键弹出菜单
	glutCreateMenu(processMenu);
	glutAddMenuEntry("GL_NEAREST", 0);
	glutAddMenuEntry("GL_LINEAR", 1);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_NEAREST", 2);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_NEAREST", 3);
	glutAddMenuEntry("GL_NEAREST_MIPMAP_LINEAR", 4);
	glutAddMenuEntry("GL_LINEAR_MIPMAP_LINEAR", 5);
	// glutAddMenuEntry("ANISOTROPIC", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMouseFunc(mouse); // 注册鼠标按下事件处理函数
	glutMotionFunc(motion); // 注册鼠标移动事件处理函数

	glutMainLoop();

	return 0;
}