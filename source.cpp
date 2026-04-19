#include <GL/freeglut.h>
#include <cmath>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>

float carX = 0.0f;
float roadOffsetX = 0.0f;
float elapsedMs = 0.0f;     // timer var
clock_t lastTime;           // timer var
float miles= 0.0f;
bool isPaused = false;
bool movingForward = false;
bool movingBackward = false;

const float ROAD_SPEED = 0.15f;
const float UNIT_TO_MILES = 0.01f;


//  function for drawing text
void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

//timer box
void drawTimerBox(const std::string& text) {
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-0.95f, 0.85f);
        glVertex2f(-0.55f, 0.85f);
        glVertex2f(-0.55f, 0.95f);
        glVertex2f(-0.95f, 0.95f);
    glEnd();

    glColor3f(1, 1, 1);
    drawText(-0.93f, 0.88f, text);
}

//miles box
void drawMilesBox(float y, const std::string& text) {
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-0.95f, y);
        glVertex2f(-0.55f, y);
        glVertex2f(-0.55f, y + 0.1f);
        glVertex2f(-0.95f, y + 0.1f);
    glEnd();

    glColor3f(1, 1, 1);
    drawText(-0.93f, y + 0.03f, text);
}

void updateTimer(int) {
  if(!isPaused){
    clock_t current = clock();
    float delta = float(current - lastTime) * 1000.0f / CLOCKS_PER_SEC;
    lastTime = current;
    elapsedMs += delta; }
    glutTimerFunc(1, updateTimer, 0);
}

void drawRectangle(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i++) {
        float theta = i * 3.14159f / 180.0f;
        glVertex2f(cx + r * cos(theta), cy + r * sin(theta));
    }
    glEnd();
}

void drawRoadandStripsAnimation() {
    float segmentWidth = 2.0f;
    float offset = fmod(roadOffsetX, segmentWidth);
    if (offset < 0) offset += segmentWidth;

    for (int i = -1; i <= 2; ++i) {
        float x_start = i * segmentWidth - offset;
        float x_end = x_start + segmentWidth;

   //road base
        glColor3f(0.15f, 0.15f, 0.15f);
        glBegin(GL_QUADS);
            glVertex2f(x_start, -0.4f);
            glVertex2f(x_end, -0.4f);
            glVertex2f(x_end, 0.4f);
            glVertex2f(x_start, 0.4f);
        glEnd();
   //strips
        if (movingForward || movingBackward) {
            glColor3f(0.35f, 0.35f, 0.35f);
            for (float sx = x_start; sx < x_end; sx += 0.35f) {
                glBegin(GL_QUADS);
                glVertex2f(sx, -0.4f);
                glVertex2f(sx + 0.18f, -0.4f);
                glVertex2f(sx + 0.18f, 0.4f);
                glVertex2f(sx, 0.4f);
                glEnd();
            }
        }

    // Dashed center line
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
        glLineWidth(6);
        glBegin(GL_LINES);
            glColor3f(1,1,1);  // White
            glVertex2f(x_start, 0.0f);
            glVertex2f(x_end, 0.0f);
        glEnd();
        glDisable(GL_LINE_STIPPLE);


    }
}

void drawCar() {
    glColor3f(1.0f, 0.0f, 0.0f);
    drawRectangle(carX - 0.25f, -0.22f, carX + 0.25f, 0.12f);
    glColor3f(0.8f, 0.0f, 0.0f);
    drawRectangle(carX - 0.12f, 0.12f, carX + 0.12f, 0.32f);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawCircle(carX - 0.12f, -0.25f, 0.09f);
    drawCircle(carX + 0.12f, -0.25f, 0.09f);

}

void drawGround() {
    glBegin(GL_QUADS);
        glColor3f(0.3f, 0.7f, 0.3f);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.4f);
        glVertex2f(-1.0f, -0.4f);
    glEnd();
}




void updateGameLogic(int value) {
    if (!isPaused && (movingForward || movingBackward)) {
        float deltaTime = 16.0f / 1000.0f;
        float distanceUnits = ROAD_SPEED;
        miles += distanceUnits * UNIT_TO_MILES;

        if (movingForward) {
            roadOffsetX += ROAD_SPEED;
        } else if (movingBackward) {
            roadOffsetX -= ROAD_SPEED;
        }
    }

    glutTimerFunc(16, updateGameLogic, 0);
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (isPaused) return;

    movingForward = false;
    movingBackward = false;

    switch (key) {
        case GLUT_KEY_RIGHT:
            movingForward = true;
            break;
        case GLUT_KEY_LEFT:
            movingBackward = true;
            break;
    }
}

void specialKeysUp(int key, int x, int y) {
    if (key == GLUT_KEY_RIGHT || key == GLUT_KEY_LEFT) {
        movingForward = false;
        movingBackward = false;
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case ' ':
        case 'p':
        case 'P':
            isPaused = !isPaused;
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawGround();
    drawRoadandStripsAnimation();
    drawCar();

    //timer position
    std::stringstream ss;
    ss << "Time: " << (int)elapsedMs << " ms";
    drawTimerBox(ss.str());
    std::stringstream milesS;
    milesS << "Miles: " << std::fixed << std::setprecision(2) << miles;  //Mile with 2 decimals
    glColor3f(1.0f, 1.0f, 1.0f);
    drawMilesBox(0.73f, milesS.str());

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Car Game + Timer");

    glClearColor(0.6f, 0.85f, 1.0f, 1.0f);
    lastTime = clock();  //  initialize timer

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, updateGameLogic, 0);
    glutTimerFunc(0, updateTimer, 0);  // start timer

    glutMainLoop();
    return 0;
}
