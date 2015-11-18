#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include "GL/glut.h"
#else
#include <GL/freeglut.h>
#endif
#endif

#include <iostream>
#include <math.h>

using namespace std;

const int MAX_PARTICLES = 10000;
const int NUM_NEIGHBOURING_PARTICLES = 40;
const float PI = 3.14159f;
const float RADIUS = 0.1f;
const float PARTICLE_MASS = 1.0f; // Assuming all particles have the same mass
const float REST_DENSITY = 1000;
const float STIFFNESS_PARAMETER = 1000;

struct Particle{
    float x, y, z, vx, vy, vz, ax, ay, az, angle, life, density, pressure;
    
    virtual Particle* find_neighborhood(){
        Particle* particles;
        return particles;
    }
    
    float compute_density(Particle* particles) {
        // See p.35
        
        for(int i = 0; i < NUM_NEIGHBOURING_PARTICLES; i++)
        {
            float dx = x - particles[i].x;
            float dy = y - particles[i].y;
            float dz = z - particles[i].z;
            float distance = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
            if (distance >= 0 && distance < RADIUS)
            {
                // Using poly6 smoothing kernel, see p.30
                density += (315.f / (64.f * PI * powf(RADIUS, 9.f))) *
                powf(powf(RADIUS, 2.f) - powf(distance, 2.f), 3.f);
            }
        }
        
        return PARTICLE_MASS * density;
    }
    
    float compute_pressure(float d) {
        return pressure = STIFFNESS_PARAMETER * (d - REST_DENSITY);
    }
    
    float compute_laplacian_of_pressure(Particle* particles){
        float laplacian = 0;

        // See p.33
        for(int i = 0; i < NUM_NEIGHBOURING_PARTICLES; i++)
        {
            float dx = x - particles[i].x;
            float dy = y - particles[i].y;
            float dz = z - particles[i].z;
            float distance = sqrtf((dx * dx) + (dy * dy) + (dz * dz));
            if (distance >= 0 && distance < RADIUS)
            {
                // Using the gradient of the poly6 smoothing kernel, see p.30
                float sk = (315.f / (64.f * PI * powf(RADIUS, 9.f))) *
                (3.f * powf(powf(RADIUS, 2.f) - powf(distance, 2.f), 2.f)) * (-2.f * distance);
                
                laplacian += (1.f / particles[i].density ) * particles[i].pressure * sk;
            }
        }
        
        laplacian *= PARTICLE_MASS;
        
        return laplacian;
    }
    
    void compute_density_and_pressure(){
     
        for(int i = 0; i < MAX_PARTICLES; i++)
        {
            Particle* neighbors = find_neighborhood();
            
            float d = compute_density(neighbors);
            compute_pressure(d);
            compute_laplacian_of_pressure(neighbors);
        }
    }
    
};

Particle particleContainer[MAX_PARTICLES];



void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27:
            exit(0);
            break;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSolidSphere(0.05, 100, 100);
    glutSwapBuffers();
}

int main(int argc, char * argv[]) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Fluid Simulation");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    
    glutMainLoop();
    
    return 0;
}
