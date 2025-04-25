#define STB_IMAGE_IMPLEMENTATION 
#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846
#include <cmath>
#include "stb_image.h"
#include <iostream>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>  // para malloc y free
#include <string>
float vel = 5.0;

GLuint texturaID[21];

void cargarTextura(std::string nom, int id) {
    int ancho, alto, nrChannels;
    unsigned char* data = stbi_load(nom.c_str(), &ancho, &alto, &nrChannels, 0);
    if (!data) {
        std::cerr << "Error al cargar la textura: " << nom << std::endl;
        return;
    }

    std::cout << "Textura cargada: " << nom << " (ID: " << id << ")" << std::endl; // Depuración

    glGenTextures(1, &texturaID[id]);
    glBindTexture(GL_TEXTURE_2D, texturaID[id]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum formato = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, formato, ancho, alto, 0, formato, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}

// Ángulos de rotación para cada planeta
float angSol = 142.0, angMerc = 357.0, angVenus = 89.0, angTierra = 234.0, angLuna = 17.0, angSLuna = 305.0, angJLuna1 = 66.0, angTitan = 188.0;
float angMar = 271.0, angJup = 33.0, angSat = 119.0, angUran = 298.0, angNep = 75.0, angJLuna2 = 222.0, angJLuna3 = 340.0, angJLuna4 = 156.0;
const int NUM_STARS = 500; // Número de estrellas
float stars[NUM_STARS][3];   // Coordenadas de las estrellas

// Variable de tiempo para el mensaje
bool showWelcome = true;
int welcomeTimer = 0;
int selectedPlanetId = -1; // -1 significa que no hay planeta seleccionado

// Variables para controlar la visibilidad del cuadro de información
bool showInfo = false;

// Función para mostrar texto en la pantalla
void renderBitmapString(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y); // Establecer la posición del texto
    while (*string) {
        glutBitmapCharacter(font, *string); // Dibujar cada caracter
        string++;
    }
}

// Función para convertir el mensaje a mayúsculas
void toUpperCase(char* str) {
    while (*str) {
        *str = toupper(*str);  // Convertir cada carácter a mayúscula
        str++;
    }
}

void init(void) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = { 0.0f, 0.0f, 0.0f, 1.0f };  // Luz en el centro (sol)
    GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // Ambient light (low intensity)
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Diffuse light (brightness)
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // Specular highlight

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // Cargar texturas de los planetas
    cargarTextura("sol.bmp", 0);
    cargarTextura("mercurio.bmp", 1);
    cargarTextura("venus.bmp", 2);
    cargarTextura("tierra.bmp", 3);
    cargarTextura("luna.bmp", 4);
    cargarTextura("marte.bmp", 5);
    cargarTextura("jupiter.bmp", 6);
    cargarTextura("saturno.bmp", 7);
    cargarTextura("urano.bmp", 8);
    cargarTextura("neptuno.bmp", 9);
    cargarTextura("anillos.bmp", 10);

    //Cargar textura de los flyers
    cargarTextura("SolFlyer.bmp", 11); //0
    cargarTextura("MercurioFlyer.bmp", 12); //1
    cargarTextura("VenusFlyer.bmp", 13); //2
    cargarTextura("TierraFlyer.bmp", 14); //3
    cargarTextura("MarteFlyer.bmp", 15); //4
    cargarTextura("JupiterFlyer.bmp", 16); //5
    cargarTextura("SaturnoFlyer.bmp", 17); //6
    cargarTextura("UranoFlyer.bmp", 18); //7
    cargarTextura("NeptunoFlyer.bmp", 19); //8
    cargarTextura("message.bmp", 20); //9

}
void initStars() {
    for (int i = 0; i < NUM_STARS; i++) {
        stars[i][0] = (rand() % 2000 - 1000) / 2.0f; // X
        stars[i][1] = (rand() % 2000 - 1000) / 2.0f; // Y
        stars[i][2] = (rand() % 2000 - 1000) / 2.0f; // Z
    }
}

// Función para dibujar texto en 3D
void drawText3D(float x, float y, float z, void* font, const char* text) {
    glRasterPos3f(x, y, z); // Posición del texto en el espacio 3D
    while (*text) {
        glutBitmapCharacter(font, *text); // Dibujar cada carácter
        text++;
    }
}
void drawSun(float tamaño, int texturaID) {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);

    gluSphere(quad, tamaño, 30, 30);

    gluDeleteQuadric(quad);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

}


// Dibujar un planeta en su órbita
void drawPlanet(float distancia, float tamaño, float angulo, int texturaID) {
    glPushMatrix();
    glRotatef(angulo, 0.0f, 1.0f, 0.0f);
    glTranslatef(distancia, 0.0f, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);

    gluSphere(quad, tamaño, 30, 30);

    gluDeleteQuadric(quad);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}


void drawMoon(float angMoon, GLuint textura) {
    glPushMatrix();
    glRotatef(angTierra, 0.0f, 1.0f, 0.0f);  // Rotar con la Tierra
    glTranslatef(90.0f, 0.0f, 0.0f);       // Trasladar a la Tierra

    glPushMatrix();
    glRotatef(angMoon, 0.0f, 1.0f, 0.0f); // Rotar la Luna alrededor de la Tierra
    glTranslatef(12.0f, 0.0f, 0.0f);     // Distancia de la Luna a la Tierra
    drawPlanet(0, 1.5f, 0, textura);
    glPopMatrix();

    glPopMatrix();
}

void drawJMoon(float angJMoon, GLuint textura) {
    glPushMatrix();
    glRotatef(angJup, 0.0f, 1.0f, 0.0f); // Rotar con Júpiter
    glTranslatef(160.0f, 0.0f, 0.0f);   // Trasladar a Júpiter

    glPushMatrix();
    glRotatef(angJMoon, 0.0f, 1.0f, 0.0f); // Rotar la luna alrededor de Júpiter
    glTranslatef(23.0f, 0.0f, 0.0f);      // Distancia de la luna a Júpiter
    drawPlanet(0, 1.4f, 0, textura);
    glPopMatrix();

    glPopMatrix();
}

void drawTitan(float angTitan, GLuint textura) {
    glPushMatrix();
    glRotatef(angSat, 0.0f, 1.0f, 0.0f); // Rotar con Saturno
    glTranslatef(220.0f, 0.0f, 0.0f);   // Trasladar a Saturno

    glPushMatrix();
    glRotatef(angTitan, 0.0f, 1.0f, 0.0f); // Rotar Titan alrededor de Saturno
    glTranslatef(26.0f, 0.0f, 0.0f);      // Distancia de Titan a Saturno
    drawPlanet(0, 2.0f, 0, textura);
    glPopMatrix();

    glPopMatrix();
}

void drawStars(int numStars) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f); // Color blanco para las estrellas

    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        glVertex3f(stars[i][0], stars[i][1], stars[i][2]);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void drawRing() {
    int num_segments = 100;  // Número de segmentos para hacer el anillo suave
    float innerRadius = 10.0f; // Radio interno del anillo
    float outerRadius = 18.0f; // Radio externo del anillo

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments);
        float x = cos(theta);
        float y = sin(theta);

        glVertex3f(innerRadius * x, innerRadius * y, 0.0f);
        glVertex3f(outerRadius * x, outerRadius * y, 0.0f);
    }
    glEnd();
}


void drawSaturnRings(GLuint texturaAnillos) {
    glPushMatrix();
    glRotatef(angSat, 0.0f, 1.0f, 0.0f); // Asegurar que los anillos giren con Saturno
    glTranslatef(220.0f, 0.0f, 0.0f);   // Ubicar en Saturno

    glPushMatrix();
    glRotatef(90, 1.0f, 0.0f, 0.0f); // Ajustar la orientación del anillo
    glScalef(1.3f, 1.3f, 0.05f); // Ajustar dimensiones del anillo

    glEnable(GL_TEXTURE_2D); // Activar texturas
    glBindTexture(GL_TEXTURE_2D, texturaAnillos); // Aplicar la textura de los anillos
    glColor3f(1.0f, 1.0f, 1.0f); // Asegurar que el color no afecte la textura
    drawRing();
    glDisable(GL_TEXTURE_2D); // Desactivar texturas después de dibujar

    glPopMatrix();

    glPopMatrix();
}




void update(int value) {
    if (showWelcome) {
        welcomeTimer++;

        if (welcomeTimer > 1380) {
            showWelcome = false; 
            welcomeTimer = 0;     
        }
    }
    else {
        angSol += 0.0f * vel;
        angMerc += 0.005f * vel;
        angVenus += 0.02f * vel;
        angTierra += 0.015f * vel; 
        angMar += 0.011f * vel;
        angJup += 0.04f * vel;
        angSat += 0.015 * vel;
        angUran += 0.01 * vel;
        angNep += 0.009 * vel;
        angLuna += 0.01f * vel;
        angJLuna1 += 0.1f * vel;
        angJLuna2 += 0.15f * vel;
        angJLuna3 += 0.2f * vel;
        angJLuna4 += 0.18f * vel;
        angTitan += 0.2f * vel;
    }

    // Limitar los ángulos de rotación a 360 grados
    if (angLuna > 360) angLuna -= 360;
    if (angJLuna1 > 360) angJLuna1 -= 360;
    if (angJLuna2 > 360) angJLuna2 -= 360;
    if (angJLuna3 > 360) angJLuna3 -= 360;
    if (angJLuna4 > 360) angJLuna4 -= 360;
    if (angTitan > 360) angTitan -= 360;
    if (angSol > 360) angSol -= 360;
    if (angMerc > 360) angMerc -= 360;
    if (angVenus > 360) angVenus -= 360;
    if (angTierra > 360) angTierra -= 360;
    if (angMar > 360) angMar -= 360;
    if (angJup > 360) angJup -= 360;
    if (angSat > 360) angSat -= 360;
    if (angUran > 360) angUran -= 360;
    if (angNep > 360) angNep -= 360;

    glutPostRedisplay(); // Redibujar
    glutTimerFunc(1, update, 0); // Llamada periódica para animación
}

// Función para mostrar el mensaje de bienvenida
void showWelcomeMessage() {
    // Crear el mensaje de bienvenida
    
    char message1[100];
    char message2[100];
    char message3[100];
    char message4[100];

    
    
    sprintf_s(message1, ("Fuimos apenas un parpadeo en la inmensidad del tiempo."));
    sprintf_s(message2, ("Nuestra partida no detuvo el firmamento, ni altero el curso de las estrellas."));
    sprintf_s(message3, ("Eones han pasado, y nuestra huella se ha desvanecido."));
    sprintf_s(message4, ("El universo siguio su danza indiferente, eterno e implacable como si jamas hubiesemos sido."));


    // Convertir el mensaje a mayúsculas
    
    
    toUpperCase(message1);
    toUpperCase(message2);
    toUpperCase(message3);
    toUpperCase(message4);
    

    // Establecer el color del texto 
    glColor3f(0.0f, 1.0f, 1.0f);

    if (showWelcome) {
        drawStars(1000);  // Mostrar las estrellas en el fondo

        // Mostrar el mensaje en la pantalla

        renderBitmapString(-250.0f, 30.0f, GLUT_BITMAP_HELVETICA_18, message1);
        renderBitmapString(-250.0f, 10.0f, GLUT_BITMAP_HELVETICA_18, message2);
        renderBitmapString(-250.0f, -10.0f, GLUT_BITMAP_HELVETICA_18, message3);
        renderBitmapString(-250.0f, -30.0f, GLUT_BITMAP_HELVETICA_18, message4);
        
    }
}
//---------------------------------Crear Interactividad--------------------------------//}

int subWindow;
// Función para dibujar el cuadro de información
void drawInfoBox() {
    if (selectedPlanetId < 0 || selectedPlanetId > 20) return;

    glDisable(GL_LIGHTING); // Desactivar iluminación para el cuadro
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1400, 0, 700); // Coordenadas de la ventana principal

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    float margen = 10.0f; // Margen de 10 píxeles
    float ventana_width = 1400.0f;
    float ventana_height = 700.0f;
    // Tamaño del cuadro
    float width = 600.0f;  // Ancho del cuadro
    float height = 300.0f; // Alto del cuadro

    // Posición del cuadro (esquina superior izquierda)
    float x = margen; // Alineado a la izquierda con margen
    float y = ventana_height - height - margen; // Alineado arriba con margen
    
    

    // Dibujar el cuadro con la textura del flyer
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaID[selectedPlanetId]);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x, y);               // Esquina inferior izquierda
    glTexCoord2f(1, 1); glVertex2f(x + width, y);        // Esquina inferior derecha
    glTexCoord2f(1, 0); glVertex2f(x + width, y + height); // Esquina superior derecha
    glTexCoord2f(0, 0); glVertex2f(x, y + height);       // Esquina superior izquierda
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_LIGHTING); // Reactivar la iluminación
}



//------------------------------------------------------------------------------------//
// Función de visualización
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float angle = glutGet(GLUT_ELAPSED_TIME) * 0.0002f;
    float camX = cos(angle) * 400.0;
    float camZ = sin(angle) * 600.0;

    gluLookAt(camX, 150.0, camZ,  // Cámara girando alrededor del centro
        0.0, 0.0, 0.0,       // Mirando al Sol
        0.0, 1.0, 0.0);      // Manteniendo el eje Y como arriba



    if (showWelcome) {
        showWelcomeMessage(); // Mostrar el mensaje de inicio
    }
    else {
        // Dibujar el sistema solar
        glPushMatrix();
        drawStars(500);
        drawSun(45.0, texturaID[0]);
        glPopMatrix();

        drawPlanet(50.0f, 2.0f, angMerc, texturaID[1]); // Mercurio
        drawPlanet(60.0f, 4.0f, angVenus, texturaID[2]); // Venus
        drawPlanet(90.0f, 6.0f, angTierra, texturaID[3]); // Tierra
        drawPlanet(110.0f, 3.0f, angMar, texturaID[5]); // Marte
        drawPlanet(160.0f, 15.0f, angJup, texturaID[6]); // Júpiter
        drawPlanet(220.0f, 12.0f, angSat, texturaID[7]); // Saturno
        drawPlanet(260.0f, 10.0f, angUran, texturaID[8]); // Urano
        drawPlanet(330.0f, 10.0f, angNep, texturaID[9]); // Neptuno

        drawMoon(angLuna, texturaID[4]); // Luna
        drawJMoon(angJLuna1, texturaID[4]); // JLuna 1
        drawJMoon(angJLuna2, texturaID[4]); // JLuna 2
        drawJMoon(angJLuna3, texturaID[4]); // JLuna 3
        drawJMoon(angJLuna4, texturaID[4]); // JLuna 4
        drawTitan(angTitan, texturaID[4]); // Titan
        drawSaturnRings(texturaID[10]);

        // Dibujar el cuadro de información si es necesario
        if (showInfo) {
            drawInfoBox();
        }
    }

    glutSwapBuffers(); // Intercambiar los buffers para la animación
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

// Función para manejar el teclado
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // Salir con ESC
        exit(0);
    }


    if (key == ' ') { // Ocultar el cuadro de información con la barra espaciadora
        showInfo = false;
        selectedPlanetId = -1; // Reiniciar el ID del planeta seleccionado
    }

    if (key == '0') {
        showInfo = true;
        selectedPlanetId = 11;
    }

    if (key == '1') {
        showInfo = true;
        selectedPlanetId = 12;
    }

    if (key == '2') {
        showInfo = true;
        selectedPlanetId = 13;
    }

    if (key == '3') {
        showInfo = true;
        selectedPlanetId = 14;
    }

    if (key == '4') {
        showInfo = true;
        selectedPlanetId = 15;
    }

    if (key == '5') {
        showInfo = true;
        selectedPlanetId = 16;
    }

    if (key == '6') {
        showInfo = true;
        selectedPlanetId = 17;
    }

    if (key == '7') {
        showInfo = true;
        selectedPlanetId = 18;
    }

    if (key == '8') {
        showInfo = true;
        selectedPlanetId = 19;
    }

    if (key == '9') {
        showInfo = true;
        selectedPlanetId = 20;
    }
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1400, 700);
    glutCreateWindow("Sistema Solar 3D");

    init();
    initStars();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);
    glutMainLoop(); // Solo una llamada a glutMainLoop

    return 0;
}