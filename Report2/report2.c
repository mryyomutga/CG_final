/*
 * Computer Graphics final report
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>	// isspace()
#include <GL/glut.h>

/* texture(256x256) */
#define imageWidth 256
#define imageHeight 256

/* texture info */
unsigned char texImage[imageHeight][imageWidth][3];
unsigned char mouseFlag = GL_FALSE;			// mouse flag

/* 各部位の時間変化における回転角 */
static double l_leg = 0.0;
static double r_leg = 0.0;
static double l_arms[2] = {0.0, 0.0};
static double r_arms[2] = {0.0, 0.0};

static double px = 0.0, pz = 0.0;			// 物体の移動量
static int dir = 0;							// ロボットの進行方向
static double front = 0.0;					// ロボットの向き
static int lim = 1;							// 距離の上限
static int state =1;						// プログラムの状態

static double theta[4] = {0.0, 0.0, 0.0, 0.0};	// 球体の回転

static int	xStart, yStart;					// start position when drug begins
static double xAngle = 0.0, yAngle = 0.0;	// view angle

static double cx = 3.0, cy = 1.5, cz = 3.0;	// カメラの座標
static double mx = 0.0, my = 0.0, mz = 0.0;	// カメラの移動量
static int lo = 0;							// 視点の状態
static double scl = 1.0;					// カメラの倍率
static int samplingTime = 30;				// サンプリング数
/* Material(chrome) */
GLfloat robot_ambient[] = {0.25, 0.25, 0.25, 1,0};
GLfloat robot_diffuse[] = {0.4, 0.4, 0.4, 1.0};
GLfloat robot_specular[] = {0.774597, 0.774597, 0.774597, 1.0};
GLfloat robot_shininess = 0.6;
//=====================================================
/* user function */
void myField(void);
void mySolidSphere(double x, double y, double z);
void mySphere(void);
void myBox(double x, double y, double z);
void myRobot(void);
/* system(OpenGL) */
void mySetLight(void);
void myTimer(int value);
void setUpTexture(char* filename);
void readPPMImage(char* filename);
void mySetMenu(void);
void getValueFromMenu(int value);
void myMouseFunc(int button, int state, int x, int y);
void myMouseMotion(int x, int y);
void myKeyboard(unsigned char key, int x, int y);
void myInit(char *progname);
void myDisplay(void);
void myReshape(int width, int height);
//=====================================================
/* フィールドの作成 */
void myField(void){
	double tg = .0, tw = 1.0;
	/* ポリゴンの座標 */
	double gr[][2] = {{ -23.0, -23.0 },
					  {  23.0, -23.0 },
					  {  23.0,  23.0 },
					  { -23.0,  23.0 }};
	double wl[][2] = {{ -23.0,  -1.1 },
					  {  23.0,  -1.1 },
				  	  {  23.0, 23.0 },
				  	  { -23.0, 23.0 }};
	/* ポリゴンの材質 */
	float g_amb[]  = {1.0,  1.0,  1.0, 1.0};
	float g_dif[]  = {1.0,  1.0,  1.0, 1.0};
	float g_spc[]  = {1.0,  1.0,  1.0, 1.0};
	float ug_amb[] = {0.4,  0.15, 0.1, 1.0};
	float ug_dif[] = {0.8,  0.3,  0.2, 1.0};
	float ug_spc[] = {1.0,  1.0,  1.0, 1.0};
	float w_amb[]  = {0.25, 0.25, 0.4, 1.0};
	float w_dif[]  = {0.5,  0.5,  0.8, 1.0};
	float w_spc[]  = {1.0,  1.0,  1.0, 1.0};

	/* Ground */
	setUpTexture("monotile.ppm");
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, g_amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_dif);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, g_spc);
		glTranslated(0.0, -0.6, 0.0);
		glRotated(-90, 1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0, 0.0);	glVertex2dv(gr[0]);
			glTexCoord2d(0.0, tg);	glVertex2dv(gr[1]);
			glTexCoord2d(tg, tg);	glVertex2dv(gr[2]);
			glTexCoord2d(tg, 0.0);	glVertex2dv(gr[3]);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	/* Under Ground */
	setUpTexture("ground.ppm");
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ug_amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ug_dif);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ug_spc);

		glTranslated(0.0, -1.15, 0.0);
		glRotated(90, 1.0, 0.0, 0.0);
		glBegin(GL_QUADS);
			glTexCoord2d(0.0, 0.0);	glVertex2dv(gr[0]);
			glTexCoord2d(0.0, tg);	glVertex2dv(gr[1]);
			glTexCoord2d(tg, tg);	glVertex2dv(gr[2]);
			glTexCoord2d(tg, 0.0);	glVertex2dv(gr[3]);
		glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	/* Wall */
	setUpTexture("wall.ppm");
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, w_amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, w_dif);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, w_spc);
		glPushMatrix();
			glTranslated(0.0, 0.0, -23.0);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0);	glVertex2dv(wl[0]);
				glTexCoord2d(0.0, tw);	glVertex2dv(wl[1]);
				glTexCoord2d(tw, tw);	glVertex2dv(wl[2]);
				glTexCoord2d(tw, 0.0);	glVertex2dv(wl[3]);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslated(0.0, 0.0, 23.0);
			glRotated(180, 0.0, 1.0, 0.0);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0);	glVertex2dv(wl[0]);
				glTexCoord2d(0.0, tw);	glVertex2dv(wl[1]);
				glTexCoord2d(tw, tw);	glVertex2dv(wl[2]);
				glTexCoord2d(tw, 0.0);	glVertex2dv(wl[3]);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslated(23.0, 0.0, 0.0);
			glRotated(-90, 0.0 ,1.0, 0.0);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0);	glVertex2dv(wl[0]);
				glTexCoord2d(0.0, tw);	glVertex2dv(wl[1]);
				glTexCoord2d(tw, tw);	glVertex2dv(wl[2]);
				glTexCoord2d(tw, 0.0);	glVertex2dv(wl[3]);
			glEnd();
		glPopMatrix();
		glPushMatrix();
			glTranslated(-23.0, 0.0, 0.0);
			glRotated(90, 0.0 ,1.0, 0.0);
			glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0);	glVertex2dv(wl[0]);
				glTexCoord2d(0.0, tw);	glVertex2dv(wl[1]);
				glTexCoord2d(tw, tw);	glVertex2dv(wl[2]);
				glTexCoord2d(tw, 0.0);	glVertex2dv(wl[3]);
			glEnd();
		glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
/* ブロックの作成 */
void myBox(double x, double y, double z){
	glScaled(x,y,z);
	glutSolidCube(1.0);
}
/* 球体の作成(Solid) */
void mySolidSphere(double x, double y, double z){
	/* 球体の材質 */
	float sph_amb[] = {0.5, 0.5, 0.5, 1.0};
	float sph_dif[] = {1.0, 1.0, 1.0, 1.0};
	float sph_spc[] = {1.0 ,1.0, 1.0, 1.0};

	glPushMatrix();
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sph_amb);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sph_dif);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sph_spc);

		glTranslated(x, y, z);
		glRotated(theta[0], 1.0, 1.0, 0.0);
		glRotated(theta[1], 0.0, 1.0, 1.0);
		glRotated(theta[2], 1.0, 0.0, 1.0);
		glutSolidSphere(0.5, 128, 64);
	glPopMatrix();
}
/* 球体の設定 */
void mySphere(void){
	glPushMatrix();
		mySolidSphere(0.0, -0.5, 0.0);

		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		setUpTexture("sphere_pattern1.ppm");
			glPushMatrix();
				glRotated(theta[0]*lim/10, 0.0, 1.0, 0.0);
				mySolidSphere( 3.5*2+2, -0.5,  3.5*2+2);
				mySolidSphere( 3.5*2+2, -0.5, -3.5*2-2);
				mySolidSphere(-3.5*2-2, -0.5, -3.5*2-2);
				mySolidSphere(-3.5*2-2, -0.5,  3.5*2+2);
			glPopMatrix();
			glPushMatrix();
				glRotated(-theta[1]*lim/10, 0.0, 1.0, 0.0);
				mySolidSphere( 3.5*2+2, 4.4,  3.5*2+2);
				mySolidSphere( 3.5*2+2, 4.4, -3.5*2-2);
				mySolidSphere(-3.5*2-2, 4.4, -3.5*2-2);
				mySolidSphere(-3.5*2-2, 4.4,  3.5*2+2);
			glPopMatrix();
		glPopMatrix();
		glPushMatrix();
		setUpTexture("sphere_pattern2.ppm");
			glPushMatrix();
				glRotated(theta[2]*lim/10, 0.0, 1.0, 0.0);
				mySolidSphere( 3.5*4-2, -0.5,  3.5*4-2);
				mySolidSphere( 3.5*4-2, -0.5, -3.5*4+2);
				mySolidSphere(-3.5*4+2, -0.5, -3.5*4+2);
				mySolidSphere(-3.5*4+2, -0.5,  3.5*4-2);
			glPopMatrix();
			glPushMatrix();
				glRotated(-theta[3]*lim/10, 0.0, 1.0, 0.0);
				mySolidSphere( 3.5*4-2, 4.4,  3.5*4-2);
				mySolidSphere( 3.5*4-2, 4.4, -3.5*4+2);
				mySolidSphere(-3.5*4+2, 4.4, -3.5*4+2);
				mySolidSphere(-3.5*4+2, 4.4,  3.5*4-2);
			glPopMatrix();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
/* ロボットの作成 */
void myRobot(void){
	glEnable(GL_TEXTURE_2D);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, robot_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, robot_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, robot_specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, robot_shininess);
	glPushMatrix();
	/* Head */
		glPushMatrix();
			setUpTexture("robot_head.ppm");
			glTranslated(0.0, 0.8, 0.0);
			myBox(0.3, 0.3, 0.2);
		glPopMatrix();
	/* Body */
		glPushMatrix();
			setUpTexture("robot_body.ppm");
			glTranslated(0.0, 0.3, 0.0);
			myBox(0.4, 0.6, 0.3);
		glPopMatrix();
	/* Arms */
		/* Right */
		glPushMatrix();
			/* shoulder */
			setUpTexture("robot_arm1.ppm");
			glTranslated(0.28, 0.45, 0.0);
			glRotated(r_arms[0], 1.0, 0.0, 0.0);
			myBox(0.16, 0.3, 0.16);
			/* elbow */
			glPushMatrix();
				setUpTexture("robot_arm2.ppm");
				glTranslated(0.0, -1, 0.0);
				glRotated(r_arms[1], 1.0, 0.0, 0.0);
				myBox(1.0,1.0,1.0);
			glPopMatrix();
		glPopMatrix();
		/* Left */
		glPushMatrix();
			/* shoulder */
			setUpTexture("robot_arm1.ppm");
			glTranslated(-0.28, 0.45, 0.0);
			glRotated(l_arms[0], 1.0, 0.0, 0.0);
			myBox(0.16, 0.3, 0.16);
			/* elbow */
			glPushMatrix();
				setUpTexture("robot_arm2.ppm");
				glTranslated(0.0, -1.0, 0.0);
				glRotated(l_arms[1], 1.0, 0.0, 0.0);
				myBox(1.0, 1.0, 1.0);
			glPopMatrix();
		glPopMatrix();
	/* Legs */
		setUpTexture("robot_leg.ppm");
		/* Right */
		glPushMatrix();
			glTranslated(0.1, -0.3, 0.0);
			glRotated(r_leg, 1.0, 0.0, 0.0);
			myBox(0.2, 0.6, 0.2);
		glPopMatrix();
		/* Left */
		glPushMatrix();
			glTranslated(-0.1, -0.3, 0.0);
			glRotated(l_leg, 1.0, 0.0, 0.0);
			myBox(0.2, 0.6, 0.2);
		glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
/* 光源の設定 */
void mySetLight(void){
	// light1 SpotLight white
	float light1_position[]  = { 23.0, 22.4, 23.0, 1.0};
	float light1_ambient[]   = {  0.5,  0.5,  0.5, 1.0};
	float light1_diffuse[]   = {  1.0,  1.0,  1.0, 1.0};
	float light1_specular[]  = {  1.0,  1.0,  1.0, 1.0};
	float light1_direction[] = { -1.0, -0.471404, -1.0};
	// light2 SpotLight Red
	float light2_position[]  = {-23.0, 22.4, -23.0, 1.0};
	float light2_ambient[]   = {  0.5,  0.0,   0.0, 1.0};
	float light2_diffuse[]   = {  1.0,  0.0,   0.0, 1.0};
	float light2_specular[]  = {  1.0,  0.0,   0.0, 1.0};
	float light2_direction[] = {  1.0,  -0.471404,  1.0};
	// light3 SpotLight Green
	float light3_position[]  = { 23.0, 22.4, -23.0, 1.0};
	float light3_ambient[]   = {  0.0,  0.5,   0.0, 1.0};
	float light3_diffuse[]   = {  0.0,  1.0,   0.0, 1.0};
	float light3_specular[]  = {  0.0,  1.0,   0.0, 1.0};
	float light3_direction[] = { -1.0,  -0.471404,  1.0};
	// light4 SpotLight Blue
	float light4_position[]  = {-23.0, 22.4, 23.0, 1.0};
	float light4_ambient[]   = {  0.0,  0.0,  0.5, 1.0};
	float light4_diffuse[]   = {  0.0,  0.0,  1.0, 1.0};
	float light4_specular[]  = {  0.0,  0.0,  1.0, 1.0};
	float light4_direction[] = {  1.0, -0.471404, -1.0};
	// SpotLight option
	float spot_cutoff = 90.0;
	float spot_exponent = 32;

	/* Set up LIGHT1 */
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_direction);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_cutoff);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exponent);
	/* Set up LIGHT2 */
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
	glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2_direction);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spot_cutoff);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, spot_exponent);
	/* Set up LIGHT3 */
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
	glLightfv(GL_LIGHT3, GL_AMBIENT, light3_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light3_specular);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light3_direction);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, spot_cutoff);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, spot_exponent);
	/* Set up LIGHT4 */
	glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
	glLightfv(GL_LIGHT4, GL_AMBIENT, light4_ambient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light4_specular);
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, light4_direction);
	glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, spot_cutoff);
	glLightf(GL_LIGHT4, GL_SPOT_EXPONENT, spot_exponent);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	glEnable(GL_LIGHT1);		// enable the 1st light
	glEnable(GL_LIGHT2);		// enable the 2th light
	glEnable(GL_LIGHT3);		// enable the 3st light
	glEnable(GL_LIGHT4);		// enable the 4th light
}
/* タイマーコールバック */
void myTimer(int value) {
	if (value == 1){
		glutTimerFunc(samplingTime, myTimer, 1);
		/* run状態の判定 */
		if(state){
			static double rad, ang;
			ang = sin(rad);
			theta[0] += 1;
			theta[1] += 2;
			theta[2] += 3;
			theta[3] += 4;

			if(dir == 3){
				r_arms[0] = ang*30;
				r_arms[1] = ang*10;
				l_arms[0] = -ang*30;
				l_arms[1] = -ang*5;
				r_leg = -ang*30;
				l_leg = ang*30;
				pz += 0.1*lim;
				front = 180;
				rad += 0.5;
			if(rad > 360){
					rad -= 360;
				}
				if(pz > 3.5*lim){
					dir--;
				}
			}
			else if(dir == 2){
				r_arms[0] = ang*30;
				r_arms[1] = ang*10;
				l_arms[0] = -ang*30;
				l_arms[1] = -ang*5;
				r_leg = -ang*30;
				l_leg = ang*30;
				px -= 0.1*lim;
				front = -90;
				rad += 0.5;
				if(rad > 360){
					rad -= 360;
				}
				if(px < -3.5*lim){
					dir--;
				}
			}
			else if(dir == 1){
				r_arms[0] = -ang*30;
				r_arms[1] = -ang*5;
				l_arms[0] = ang*30;
				l_arms[1] = ang*10;
				r_leg = ang*30;
				l_leg = -ang*30;
				pz -= 0.1*lim;
				front = 180;
				rad -= 0.5;
				if(rad < -360){
					rad += 360;
				}
				if(pz < -3.5*lim){
					dir--;
				}
			}
			else if(dir == 0){
				r_arms[0] = -ang*30;
				r_arms[1] = -ang*5;
				l_arms[0] = ang*30;
				l_arms[1] = ang*10;
				r_leg = ang*30;
				l_leg = -ang*30;
				px += 0.1*lim;
				front = 90;
				rad -= 0.5;
				if(rad < -360){
					rad += 360;
				}
				if(px > 3.5*lim){
					dir = 3;
				}
			}
		}
		glutPostRedisplay();
	}
}
/* テクスチャの設定 */
void setUpTexture(char* filename){
	readPPMImage(filename);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, texImage);

	/* automatic map */
	glTexGenf(GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glTexGenf(GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
}
/* PPMファイルの読み込み */
void readPPMImage(char* filename){
	FILE *fp;
	errno_t error;
	if(error = fopen_s(&fp, filename, "r")) {
		fprintf(stderr, "Cannot open ppm file %s\n", filename);
		exit(1);
	}
	fread(texImage, 1, imageWidth*imageHeight * 3, fp);	// read RGB data
	fclose(fp);
}
/* メニュー情報 */
void mySetMenu(void) {
	glutCreateMenu(getValueFromMenu);
	glutAddMenuEntry("run", 1);
	glutAddMenuEntry("stop", 2);
	glutAddMenuEntry("x 1.0", 3);
	glutAddMenuEntry("x 0.5", 4);
	glutAddMenuEntry("x 0.25", 5);
	glutAddMenuEntry("Overlook", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
/* メニュー動作 */
void getValueFromMenu(int value) {
	glLoadIdentity();
	switch (value) {
		case 1:
			state = 1;
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 2:
			state = 0;
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 3:
			lo = 0;
			cx = 3.0*1.0; mx = 0.0;
			cy = 1.5*1.0; my = 0.0;
			cz = 3.0*1.0; mz = 0.0;
			lim = 1.0;
			gluLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			break;
		case 4:
			lo = 0;
			cx = 3.0*2.0; mx = 0.0;
			cy = 1.5*2.0; my = 0.0;
			cz = 3.0*2.0; mz = 0.0;
			lim = 2;
			gluLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			break;
		case 5:
			lo = 0;
			cx = 3.0*4.0; mx = 0.0;
			cy = 1.5*4.0; my = 0.0;
			cz = 3.0*4.0; mz = 0.0;
			lim = 4;
			gluLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			break;
		case 6:
			lo = 1;
			cx = 0.0;  mx = 0.0;
			cy = 23.0; my = 0.0;
			cz = 0.0;  mz = 0.0;
			xAngle = 0.0;
			yAngle = 0.0;
			gluLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
	}
}
/* クリック動作 */
void myMouseFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		xStart = x;
		yStart = y;
		mouseFlag = GL_TRUE;
	}
	else {
		mouseFlag = GL_FALSE;
	}
}
/* ドラッグ動作 */
void myMouseMotion(int x, int y) {
	int xdis, ydis;
	double th = 0.5;

	if (mouseFlag == GL_FALSE)	return;
	xdis = x - xStart;
	ydis = y - yStart;
	xAngle += (double)ydis * th;
	yAngle += (double)xdis * th;
	xStart = x;
	yStart = y;
	glutPostRedisplay();
}
/* キーボード動作 */
void myKeyboard (unsigned char key, int x, int y){
	glLoadIdentity();
	switch (key) {
		case 'w':
			if(((cx+mx) >= -20.0) || ((cz+mz) >= -20.0)){
				mx -= 0.125;
				mz -= 0.125;
			}
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 'd':
			if(((cx+mx) <= 20.0) || ((cz+mz) >= -20.0)){
				mx += 0.125;
				mz -= 0.125;
			}
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 's':
			if(((cx+mx) <= 20.0) || ((cz+mz) <= 20.0)){
				mx += 0.125;
				mz += 0.125;
			}
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 'a':
			if(((cx+mx) >= -20.0) || ((cz+mz) <= 20.0)){
				mx -= 0.125;
				mz += 0.125;
			}
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 'h':
			if((cy+my) <= 20.0){
				my += 0.125;
			}
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 'l':
			if((cy+my) >= 1.5){
				my -= 0.125;
			}
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else 		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
		case 27:
			exit(0);
			break;
		default:
			if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
			else 		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
			break;
	}
}
/* キーが離れたときの動作 */
void keyboardup(unsigned char key, int x, int y){
	glLoadIdentity();
	if(lo == 0)	gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 0.0, 1.0, 0.0);
	else		gluLookAt(cx+mx, cy+my, cz+mz, mx, my, mz, 1.0, 0.0, 0.0);
}
/* Init */
void myInit(char *progname) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(progname);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.1, 0.15, 0.225, 0.0);
}
/* 描画関数 */
void myDisplay(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);	// z-buffer
	glEnable(GL_LIGHTING);		// shading
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	mySetLight();

	glPushMatrix();
		/* 視点の回転 */
		glRotated(yAngle, 0.0, 1.0, 0.0);
		glRotated(xAngle, 0.0, 0.0, 1.0);

		/* Field */
		myField();

		/* Robot */
		glPushMatrix();
			glTranslated(px, 0.05, pz);
			glRotated(front, 0.0, 1.0, 0.0);
			myRobot();
		glPopMatrix();

		/* Sphere */
		mySphere();

	glPopMatrix();
	glFlush();

	glDisable(GL_NORMALIZE);
	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}
void myReshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (double)width / (double)height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(3.0+mx, 1.5+my, 3.0+mz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
int main(int argc, char** argv){
	glutInit(&argc, argv);
	myInit("Robot Animation");
	// light
	mySetLight();
	// menu
	mySetMenu();
	// keyboard
	glutKeyboardFunc(myKeyboard);
	// keyboard up
	glutKeyboardUpFunc(keyboardup);
	// mouse
	glutMouseFunc(myMouseFunc);
	// mouse motion
	glutMotionFunc(myMouseMotion);
	// Reshape
	glutReshapeFunc(myReshape);
	// timer call back
	glutTimerFunc(samplingTime, myTimer, 1);

	glutDisplayFunc(myDisplay);
	glutMainLoop();
	return 0;
}
