#include "glos.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include <math.h>
#include <stdio.h>

float posMisc = 0.0f;
GLUquadricObj* pQuadric;
GLuint texturi[1]; // Vectorul care retine ID-ul texturii

// --- FUNCTIE INCARCARE TEXTURA ---
void IncarcaTextura(const char* caleFisier, int indexTextura) {
    BITMAP bmp;
    // Incarcam imaginea de pe disc
    HBITMAP hBmp = (HBITMAP)LoadImageA(NULL, caleFisier, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if (hBmp != NULL) {
        GetObject(hBmp, sizeof(bmp), &bmp);
        glGenTextures(1, &texturi[indexTextura]);
        glBindTexture(GL_TEXTURE_2D, texturi[indexTextura]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Repetam textura pentru a acoperi toata rama
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight,
            0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
        DeleteObject(hBmp);
    }
}

// --- DESENARE PESTE (Corp: Sfera, Coada: Triunghi) ---
void deseneazaPeste(float x, float y, float r, float g, float b, float viteza) {
    glPushMatrix();
    float xAnimat = fmod(x + posMisc * viteza, 1.4f) - 0.2f;
    glTranslatef(xAnimat, y + sin(posMisc * 4) * 0.03f, 0.0f);

    glColor3f(r, g, b);
    glPushMatrix(); // Corpul pestei
    glScalef(1.5f, 0.7f, 1.0f);
    gluSphere(pQuadric, 0.045, 20, 20);
    glPopMatrix();

    glBegin(GL_TRIANGLES); // Coada pestei
    glVertex2f(-0.04f, 0.0f);
    glVertex2f(-0.09f, 0.04f);
    glVertex2f(-0.09f, -0.04f);
    glEnd();
    glPopMatrix();
}

void CALLBACK display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    // 1. STENCIL: Masca pentru "gaura" hubloului
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.0f);
    gluDisk(pQuadric, 0, 0.35, 64, 1);
    glPopMatrix();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // 2. SCENA MARINA (Se vede doar prin gaura)
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    // Fundal Apa
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.2f, 0.4f, 1.0f);
    glRectf(0, 0, 1, 1);

    // Nisip miscator
    glPushMatrix();
    float nisipShift = fmod(posMisc * 0.05f, 0.4f);
    glTranslatef(-nisipShift, 0, 0);
    glColor3f(0.7f, 0.6f, 0.4f);
    glRectf(-0.5, 0.0, 1.5, 0.25);
    glPopMatrix();

    // Pestii
    deseneazaPeste(0.1, 0.6, 1.0, 0.5, 0.0, 0.3);
    deseneazaPeste(0.4, 0.4, 1.0, 0.1, 0.1, 0.5);
    deseneazaPeste(0.7, 0.3, 0.9, 0.9, 0.1, 0.2);

    // Bule
    glColor4f(1.0f, 1.0f, 1.0f, 0.4f);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        float yB = fmod(posMisc * 0.4f + i * 0.2f, 0.7f) + 0.2f;
        glTranslatef(0.2f + i * 0.15f, yB, 0);
        gluDisk(pQuadric, 0, 0.012, 12, 1);
        glPopMatrix();
    }

    // 3. INTERIOR SUBMARIN (Peretele negru din jurul hubloului)
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.05f, 0.05f, 0.07f);
    glRectf(0, 0, 1, 1);

    // 4. RAMA CU TEXTURA DE LEMN (Peste tot)
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturi[0]);
    glColor3f(1.0f, 1.0f, 1.0f); // Alb pentru a nu colora textura

    // Setam generarea automata a coordonatelor pentru disk
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.0f);
    gluDisk(pQuadric, 0.35, 0.46, 64, 1);
    glPopMatrix();

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);

    // 5. NITURI METALICE (Negre)
    glColor3f(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < 360; i += 45) {
        float rad = i * 3.1415f / 180.0f;
        glPushMatrix();
        glTranslatef(0.5f + cos(rad) * 0.41f, 0.5f + sin(rad) * 0.41f, 0);
        gluDisk(pQuadric, 0, 0.015, 12, 1);
        glPopMatrix();
    }

    auxSwapBuffers();
}

void CALLBACK miscare(void) {
    posMisc += 0.0001f;
    display();
}

void myinit(void) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    pQuadric = gluNewQuadric();
    gluQuadricDrawStyle(pQuadric, GLU_FILL);

    // --- INCARCARE TEXTURA ---
    // Aici poti pune numele fisierului tau .bmp
    IncarcaTextura("hublou.bmp", 0);
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
    auxInitWindow("Submarin - Final cu Textura");
    myinit();
    auxReshapeFunc(myReshape);
    auxIdleFunc(miscare);
    auxMainLoop(display);
    return(0);
}