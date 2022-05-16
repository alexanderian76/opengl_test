//#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926532
#define DR 0.0174533 // one degree in radians
//#else
//#include <GL/glut.h>
//#endif

//To compile input: g++ main.cpp -framework OpenGL -framework GLUT 



float px, py, pdx, pdy, pa; // player position

void DrawPlayer() {
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px, py);
    glEnd();


    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px + pdx * 5, py + pdy * 5);
    glEnd();

}



int mapX = 8;
int mapY = 8;
int mapS = 64;

int map[] = {
    1,1,1,1,1,1,1,1,
    1,0,0,1,0,0,0,1,
    1,0,0,1,0,0,0,1,
    1,0,0,1,0,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1
};


void DrawMap2D() {
    int x, y, x0, y0;
    for(y = 0; y < mapY; y++) {
        for(x = 0; x < mapX; x++) {
            if(map[y*mapX + x] == 1) {
                glColor3f(1, 1, 1);
            }
            else {
                glColor3f(0, 0, 0);
            }
            x0 = x * mapS;
            y0 = y * mapS;
            glBegin(GL_QUADS);
            glVertex2i(x0 + 1, y0 + 1);
            glVertex2i(x0 + 1, y0 + mapS - 1);
            glVertex2i(x0 + mapS - 1, y0 + mapS - 1);
            glVertex2i(x0 + mapS - 1, y0 + 1);
            glEnd();
        }
    }
}


float Dist(float ax, float ay, float bx, float by, float ang) {
    return sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
}


void DrawRays3D() {
    
    int r, mx, my, mp, dof;
    float rx, ry, ra, x0, y0, disT;
    
    ra = pa - DR * 30;
    if(ra < 0) {
        ra += 2 * PI;
    }
    if(ra > 2 * PI) {
        ra -= 2 * PI;
    }

    for(r = 0; r < 60; r++) {
        //Check horizontal lines
        float disH = 1000000;
        float hx = px;
        float hy = py;
        dof = 0;
        float aTan = -1 / tan(ra);
        if(ra > PI) {
            ry = (((int)py >> 6) << 6) - 0.0001;
            rx = (py - ry) * aTan + px;
            y0 = -64;
            x0 = -y0 * aTan;
        }
        if(ra < PI) {
            ry = (((int)py >> 6) << 6) + 64;
            rx = (py - ry) * aTan + px;
            y0 = 64;
            x0 = -y0 * aTan;
        }
        if(ra == 0 || ra == PI) {
            rx = px;
            ry = py;
           // printf( "%f\n", rx);
           // printf("%f\n", ry);
            dof = 8;
        }
        while(dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if(mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                hx = rx;
                hy = ry;
                disH = Dist(px, py, hx, hy, ra);
                dof = 8; // hit the wall
            }
            else {
                rx += x0;
                ry += y0;
                dof += 1;
            }
        }
       /* glColor3f(1, 0, 0);
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();*/
         //Check vertical lines
        float disV = 1000000;
        float vx = px;
        float vy = py;
        dof = 0;
        float nTan = -tan(ra);
        if(ra > PI / 2 && ra < 3 * PI / 2) {
            rx = (((int)px >> 6) << 6) - 0.0001;
            ry = (px - rx) * nTan + py;
            x0 = -64;
            y0 = -x0 * nTan;
        }
        if(ra < PI / 2 || ra > 3 * PI / 2) {
            rx = (((int)px >> 6) << 6) + 64;
            ry = (px - rx) * nTan + py;
            x0 = 64;
            y0 = -x0 * nTan;
        }
        if(ra == 0 || ra == PI) {
            rx = px;
            ry = py;
           // printf( "%f\n", rx);
           // printf("%f\n", ry);
            dof = 8;
        }
        while(dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if(mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                vx = rx;
                vy = ry;
                disV = Dist(px, py, vx, vy, ra);
                dof = 8; // hit the wall
            }
            else {
                rx += x0;
                ry += y0;
                dof += 1;
            }
        }
        if(disH > disV) {
            glColor3f(0.9, 0, 0);
            rx = vx;
            ry = vy;
            disT = disV;
        }
        else {
            glColor3f(0.7, 0, 0);
            rx = hx;
            ry = hy;
            disT = disH;
        }
        
        glLineWidth(1);
        glBegin(GL_LINES);
        glVertex2i(px, py);
        glVertex2i(rx, ry);
        glEnd();

        // Drawing 3D scene 
        float ca = pa - ra;
        if(ca < 0) {
        ca += 2 * PI;
        }
        if(ca > 2 * PI) {
            ca -= 2 * PI;
        }
        disT = disT * cos(ca); // this is fixing fisheyes
        float lineH = mapS * 320 / disT;
        
        if(lineH > 320) {
            lineH = 320;
        }
        float lineO = 160 - lineH / 2;
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r * 8 + 530, lineO);
        glVertex2i(r * 8 + 530, lineH + lineO);
        glEnd();
        ra += DR;
        if(ra < 0) {
        ra += 2 * PI;
        }
        if(ra > 2 * PI) {
            ra -= 2 * PI;
        }
    }
}






void buttons(unsigned char key, int x, int y) {
    if(key == 'a') {
       // px -= 10;
        pa -= 0.1;
        if(pa < 0) {
            pa += 2*PI;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    }
    if(key == 'w') {
        px += pdx;
        py += pdy;
    }
    if(key == 's') {
        px -= pdx;
        py -= pdy;
    }
    if(key == 'd') {
         pa += 0.1;
        if(pa > 2*PI) {
            pa -= 2*PI;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    }
    glutPostRedisplay();
}

// Clears the current window and draws a triangle.
void display() {

    // Set every pixel in the frame buffer to the current clear color.
    glClear(GL_COLOR_BUFFER_BIT);

    // Drawing is done by specifying a sequence of vertices.  The way these
    // vertices are connected (or not connected) depends on the argument to
    // glBegin.  GL_POLYGON constructs a filled polygon.
    glBegin(GL_POLYGON);
    glColor3f(1, 0, 0); glVertex3f(-0.6, -0.75, 0.5);
    glColor3f(0, 1, 0); glVertex3f(0.6, -0.75, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0.75, 0);

    glEnd();
    DrawMap2D();
    DrawRays3D();
    DrawPlayer();
    glutSwapBuffers();

    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();
}


void init() {
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 1024, 512, 0);
    px = 300;
    py = 300;
    pdx = cos(pa) * 5;
    pdy = sin(pa) * 5;
}

// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char** argv) {

    // Use a single buffered window in RGB mode (as opposed to a double-buffered
    // window or color-index mode).
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    // Position window at (80,80)-(480,380) and give it a title.
    glutInitWindowPosition(80, 80);
    glutInitWindowSize(1024, 512);
    glutCreateWindow("A Simple Triangle");
    init();
    // Tell GLUT that whenever the main window needs to be repainted that it
    // should call the function display().
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    
    // Tell GLUT to start reading and processing events.  This function
    // never returns; the program only exits when the user closes the main
    // window or kills the process.
    glutMainLoop();
}