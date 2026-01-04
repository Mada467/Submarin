#include "glos.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include <math.h>

float posMisc = 0.0f;
GLUquadricObj* pQuadric;

// Pește compus din sferă (corp) și triunghi (coadă)
void deseneazaPeste(float x, float y, float r, float g, float b, float viteza) {
    glPushMatrix();
    float xAnimat = fmod(x + posMisc * viteza, 1.4f) - 0.2f; // Mișcare continuă
    glTranslatef(xAnimat, y + sin(posMisc * 4) * 0.03f, 0.0f);

    glColor3f(r, g, b);
    glPushMatrix(); // Corpul
    glScalef(1.5f, 0.7f, 1.0f);
    gluSphere(pQuadric, 0.045, 20, 20);
    glPopMatrix();

    glBegin(GL_TRIANGLES); // Coada
    glVertex2f(-0.04f, 0.0f);
    glVertex2f(-0.09f, 0.04f);
    glVertex2f(-0.09f, -0.04f);
    glEnd();
    glPopMatrix();
}

void CALLBACK display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    // 1. DEFINIM ZONA DIN INTERIORUL HUBLOULUI ÎN STENCIL
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Nu desenăm culori încă
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.0f);
    gluDisk(pQuadric, 0, 0.35, 64, 1); // Aceasta este gaura
    glPopMatrix();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // 2. DESENĂM SCENA MARINĂ (DOAR în interiorul cercului)
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // Fundal Apă
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.2f, 0.4f, 1.0f);
    glRectf(0, 0, 1, 1);

    // Nisip în mișcare
    glPushMatrix();
    float nisipShift = fmod(posMisc * 0.05f, 0.4f);
    glTranslatef(-nisipShift, 0, 0);
    glColor3f(0.7f, 0.6f, 0.4f);
    glRectf(-0.5, 0.0, 1.5, 0.25); // Nisipul nu va ieși din cerc datorită Stencil
    glPopMatrix();

    // Cei 3 pești
    deseneazaPeste(0.1, 0.6, 1.0, 0.5, 0.0, 0.3); // Portocaliu
    deseneazaPeste(0.4, 0.4, 1.0, 0.1, 0.1, 0.5); // Roșu
    deseneazaPeste(0.7, 0.3, 0.9, 0.9, 0.1, 0.2); // Galben

    // Bule
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        float yB = fmod(posMisc * 0.4f + i * 0.2f, 0.7f) + 0.2f;
        glTranslatef(0.2f + i * 0.15f, yB, 0);
        gluDisk(pQuadric, 0, 0.012, 12, 1);
        glPopMatrix();
    }

    // 3. DESENĂM INTERIORUL SUBMARINULUI (DOAR în exteriorul cercului)
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glColor3f(0.05f, 0.05f, 0.07f); // Negru/Gri închis (pereții submarinului)
    glRectf(0, 0, 1, 1);

    // 4. DESENĂM RAMA DE LEMN (Peste tot)
    glDisable(GL_STENCIL_TEST);
    glColor3f(0.35f, 0.18f, 0.05f); // Maro lemn
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.0f);
    gluDisk(pQuadric, 0.35, 0.46, 64, 1); // Rama propriu-zisă

    // Nituri pe ramă
    glColor3f(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < 360; i += 45) {
        float rad = i * 3.1415f / 180.0f;
        glPushMatrix();
        glTranslatef(cos(rad) * 0.41f, sin(rad) * 0.41f, 0);
        gluDisk(pQuadric, 0, 0.015, 12, 1);
        glPopMatrix();
    }
    glPopMatrix();

    auxSwapBuffers();
}

void CALLBACK miscare(void) {
    posMisc += 0.0001f;
    display();
}

void myinit(void) {
    pQuadric = gluNewQuadric();
    gluQuadricDrawStyle(pQuadric, GLU_FILL);
}

void CALLBACK myReshape(GLsizei w, GLsizei h) {
    if (!h) return;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    auxInitDisplayMode(AUX_DOUBLE | AUX_RGB | AUX_STENCIL);
    auxInitPosition(0, 0, 600, 600);
    auxInitWindow("Submarin - Vedere Finala");
    myinit();
    auxReshapeFunc(myReshape);
    auxIdleFunc(miscare);
    auxMainLoop(display);
    return(0);
}