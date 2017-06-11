#include <stdio.h>
#include <cstdlib>

#include "ghost.h"
#include "loaderOBJ.h"

#define mGhost1 "modeleBlend/ghost1.obj"
#define mGhost2 "modeleBlend/ghost2.obj"

using namespace glm;

Ghost::Ghost(Map* &mapa,colision_length &colision_length,int id) {
    bool res = loadOBJ(mGhost1, this->TEMPvertices, this->TEMPuvs, this->TEMPnormals,this->TEMPvCount,colision_length);
    if(!res) {
        fprintf(stderr,"Nie uda³o siê wczytaæ!");
        exit(1);
    }
    position = vec3(1.0,1.0,1.0);
    rotation = vec3(0.0,0.0,0.0);
    scale = vec3(1.0,1.0,1.0);
    speed = 2;
    rotation_speed = PI/2;
    struct colision_length unusable;
    res = loadOBJ(mGhost2, this->TEMPvertices2, this->TEMPuvs2, this->TEMPnormals2,this->TEMPvCount2,unusable);
    if(!res) {
        fprintf(stderr,"Nie uda³o siê wczytaæ!");
        exit(1);
    }
    position2 = position;
    rotation2 = rotation;
    scale2 = scale;
    speed2 = 6;
    rotation_temp = rotation2.y;
    findPosition(mapa,id);
}

Ghost::~Ghost() {
    TEMPvertices.clear();
    TEMPuvs.clear();
    TEMPnormals.clear();
    TEMPvertices2.clear();
    TEMPuvs2.clear();
    TEMPnormals2.clear();
}

// znajduje poczatkowa pozycje pacmana na mapie
void Ghost::findPosition(Map* &mapa,int id) {
    for(int i=0; i<WYSOKOSC_MAPY; i++) {
        for(int j=0; j<SZEROKOSC_MAPY; j++) {
            if(mapa->mapa[i][j] == id) {
                this->position = vec3((float)i,0.75,(float)j);
            }
        }
    }
}


void Ghost::drawSolid(GLuint &tex,mat4 &V) {
    this->drawSolid_1(tex,V);
    this->drawSolid_2(tex,V);
}

void Ghost::drawSolid_1(GLuint &tex,mat4 &V) {
    glEnable(GL_NORMALIZE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D,tex);
    mat4 M=mat4(1.0f);
    M=translate(M,this->position);
    M=rotate(M,this->rotation.y,vec3(0.0,1.0,0.0));
    glLoadMatrixf(value_ptr(V*M));
    glVertexPointer(3,GL_FLOAT,0,&(this->TEMPvertices[0]));
    glNormalPointer(GL_FLOAT,sizeof(float)*3,&(this->TEMPnormals[0]));
    glTexCoordPointer(2,GL_FLOAT,0,&(this->TEMPuvs[0]));
    float ambient[] = {0,0,0,1};
    float emision[] = {0,0,0,1};
    float diffuse[] = {0.7,0.5,0.5,1};
    float specular[] = {0.5,0.5,0.5,1};
    float shininess = 50;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glDrawArrays(GL_TRIANGLES,0,this->TEMPvCount);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Ghost::drawSolid_2(GLuint &tex,mat4 &V) {
    glEnable(GL_NORMALIZE);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D,tex);
    mat4 M=mat4(1.0f);
    M=translate(M,this->position);
    M=rotate(M,this->rotation.y,vec3(0.0,1.0,0.0));
    this->rotation_temp -= (float)(glfwGetTime()*speed2);
    this->rotation2.y = (float)(this->rotation_temp - 2*PI*(ceil(this->rotation_temp/(2*PI))));
    M=rotate(M,this->rotation2.y,vec3(0.0,1.0,0.0));
    glLoadMatrixf(value_ptr(V*M));
    glVertexPointer(3,GL_FLOAT,0,&(this->TEMPvertices2[0]));
    glNormalPointer(GL_FLOAT,sizeof(float)*3,&(this->TEMPnormals2[0]));
    glTexCoordPointer(2,GL_FLOAT,0,&(this->TEMPuvs2[0]));
    float ambient[] = {0,0,0,1};
    float emision[] = {0,0,0,1};
    float diffuse[] = {0.7,0.5,0.5,1};
    float specular[] = {0.5,0.5,0.5,1};
    float shininess = 50;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emision);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glDrawArrays(GL_TRIANGLES,0,this->TEMPvCount2);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void Ghost::WallDetect(Map* &mapa,colision_length colision_table[],int id) {
    /*
        SCHEMAT DETEKCJI SCIAN
    X /|\
    |   ________   ________   ________
    |  |        | |        | |        |
    |  |        | |        | |        |
    |  |   8   b| |   1    | |b  2    |
    |  |        | |        | |        |
    |  |___a____| |________| |___a____|
    |   ________   ________   ________
    |  |        | |        | |        |
    |  |        | |  PAC   | |        |
    |  |   7    | |  MAN   | |   3    |
    |  |        | |        | |        |
    |  |________| |________| |________|
    |   ________   ________   ________
    |  |   a    | |        | |   a    |
    |  |        | |        | |        |
    |  |   6   b| |   5    | |b  4    |
    |  |        | |        | |        |
    |  |________| |________| |________|
    |
    |------------------------------------->
                                        Z
    */
    int px = (int)(this->position.x+0.5f); // znajduje srodek pola w x
    int pz = (int)(this->position.z+0.5f); // znajduje srodek pola w z
    // dodawanie po 0.5 poniewaz tyle ma nasze pole w grze np. (0,0) pozycja to srodki bedzie na (0.5,0.5)
    /// Kolizja ze sciana nr 1
    if(mapa->mapa[px+1][pz]==1 && px+1>=0 && px+1<WYSOKOSC_MAPY && pz>=0 && pz <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px+1][pz]].toX + colision_table[id].toX
                >= fabs((px+1)-this->position.x)) {
            // zderzenie !
            //printf("Sciana 1\n");
            position.x = px+1 - (colision_table[mapa->mapa[px+1][pz]].toX + colision_table[id].toX);
        }
    }
    /// Kolizja ze sciana nr 3
    if(mapa->mapa[px][pz+1]==1 && px>=0 && px<WYSOKOSC_MAPY && pz+1>=0 && pz+1 <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px][pz+1]].toZ + colision_table[id].toZ
                >= fabs((pz+1)-this->position.z)) {
            // zderzenie !
            //printf("Sciana 3\n");
            position.z = pz+1 - (colision_table[mapa->mapa[px][pz+1]].toZ + colision_table[id].toZ);
        }
    }
    /// Kolizja ze sciana nr 5
    if(mapa->mapa[px-1][pz]==1 && px-1>=0 && px-1<WYSOKOSC_MAPY && pz>=0 && pz <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px-1][pz]].toX + colision_table[id].toX
                >= fabs((px-1)-this->position.x)) {
            // zderzenie !
            //printf("Sciana 5\n");
            position.x = px-1 + (colision_table[mapa->mapa[px-1][pz]].toX + colision_table[id].toX);
        }
    }
    /// Kolizja ze sciana nr 7
    if(mapa->mapa[px][pz-1]==1 && px>=0 && px<WYSOKOSC_MAPY && pz-1>=0 && pz-1 <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px][pz-1]].toZ + colision_table[id].toZ
                >= fabs((pz-1)-this->position.z)) {
            // zderzenie !
            //printf("Sciana 7\n");
            position.z = pz-1 + (colision_table[mapa->mapa[px][pz-1]].toZ + colision_table[id].toZ);
        }
    }
    /// Kolizja ze sciana nr 2
    if(mapa->mapa[px+1][pz+1]==1 && px+1>=0 && px+1<WYSOKOSC_MAPY && pz+1>=0 && pz+1 <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px+1][pz+1]].toX + colision_table[id].toX
                >= fabs((px+1)-this->position.x)
                && colision_table[mapa->mapa[px+1][pz+1]].toZ + colision_table[id].toZ
                >= fabs((pz+1)-this->position.z)) {
            // zderzenie !
            if(fabs((pz+1)-this->position.z) < colision_table[mapa->mapa[px+1][pz+1]].toZ + colision_table[id].toZ
                    && fabs((pz+1)-this->position.z) < fabs((px+1)-this->position.x)) {
                /// Kolizja ze sciana nr 2a
                //printf("Sciana 2a\n");
                position.x = px+1 - (colision_table[mapa->mapa[px+1][pz+1]].toX + colision_table[id].toX);
            } else if(fabs((px+1)-this->position.x) < colision_table[mapa->mapa[px+1][pz+1]].toX + colision_table[id].toX
                      && fabs((px+1)-this->position.x) < fabs((pz+1)-this->position.z)) {
                /// Kolizja ze sciana nr 2b
                //printf("Sciana 2b\n");
                position.z = pz+1 - (colision_table[mapa->mapa[px+1][pz+1]].toZ + colision_table[id].toZ);
            }
        }
    }
    /// Kolizja ze sciana nr 4
    if(mapa->mapa[px-1][pz+1]==1 && px-1>=0 && px-1<WYSOKOSC_MAPY && pz+1>=0 && pz+1 <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px-1][pz+1]].toX + colision_table[id].toX
                >= fabs((px-1)-this->position.x)
                && colision_table[mapa->mapa[px-1][pz+1]].toZ + colision_table[id].toZ
                >= fabs((pz+1)-this->position.z)) {
            // zderzenie !
            if(fabs((pz+1)-this->position.z) < colision_table[mapa->mapa[px-1][pz+1]].toZ + colision_table[id].toZ
                    && fabs((pz+1)-this->position.z) < fabs((px-1)-this->position.x)) {
                /// Kolizja ze sciana nr 4a
                //printf("Sciana 4a\n");
                position.x = px-1 + (colision_table[mapa->mapa[px-1][pz+1]].toX + colision_table[id].toX);
            } else if(fabs((px-1)-this->position.x) < colision_table[mapa->mapa[px-1][pz+1]].toX + colision_table[id].toX
                      && fabs((px-1)-this->position.x) < fabs((pz+1)-this->position.z)) {
                /// Kolizja ze sciana nr 4b
                //printf("Sciana 4b\n");
                position.z = pz+1 - (colision_table[mapa->mapa[px-1][pz+1]].toZ + colision_table[id].toZ);
            }
        }
    }
    /// Kolizja ze sciana nr 6
    if(mapa->mapa[px-1][pz-1]==1 && px-1>=0 && px-1<WYSOKOSC_MAPY && pz-1>=0 && pz-1 <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px-1][pz-1]].toX + colision_table[id].toX
                >= fabs((px-1)-this->position.x)
                && colision_table[mapa->mapa[px-1][pz-1]].toZ + colision_table[id].toZ
                >= fabs((pz-1)-this->position.z)) {
            // zderzenie !
            if(fabs((pz-1)-this->position.z) < colision_table[mapa->mapa[px-1][pz-1]].toZ + colision_table[id].toZ
                    && fabs((pz-1)-this->position.z) < fabs((px-1)-this->position.x)) {
                /// Kolizja ze sciana nr 6a
                //printf("Sciana 6a\n");
                position.x = px-1 + (colision_table[mapa->mapa[px-1][pz-1]].toX + colision_table[id].toX);
            } else if(fabs((px-1)-this->position.x) < colision_table[mapa->mapa[px-1][pz-1]].toX + colision_table[id].toX
                      && fabs((px-1)-this->position.x) < fabs((pz-1)-this->position.z)) {
                /// Kolizja ze sciana nr 6b
                //printf("Sciana 6b\n");
                position.z = pz-1 + (colision_table[mapa->mapa[px-1][pz-1]].toZ + colision_table[id].toZ);
            }
        }
    }
    /// Kolizja ze sciana nr 8
    if(mapa->mapa[px+1][pz-1]==1 && px+1>=0 && px+1<WYSOKOSC_MAPY && pz-1>=0 && pz-1 <SZEROKOSC_MAPY) {
        if(colision_table[mapa->mapa[px+1][pz-1]].toX + colision_table[id].toX
                >= fabs((px+1)-this->position.x)
                && colision_table[mapa->mapa[px+1][pz-1]].toZ + colision_table[id].toZ
                >= fabs((pz-1)-this->position.z)) {
            // zderzenie !
            if(fabs((pz-1)-this->position.z) < colision_table[mapa->mapa[px+1][pz-1]].toZ + colision_table[id].toZ
                    && fabs((pz-1)-this->position.z) < fabs((px+1)-this->position.x)) {
                /// Kolizja ze sciana nr 8a
                //printf("Sciana 8a\n");
                position.x = px+1 - (colision_table[mapa->mapa[px+1][pz-1]].toX + colision_table[id].toX);
            } else if(fabs((px+1)-this->position.x) < colision_table[mapa->mapa[px+1][pz-1]].toX + colision_table[id].toX
                      && fabs((px+1)-this->position.x) < fabs((pz-1)-this->position.z)) {
                /// Kolizja ze sciana nr 8b
                //printf("Sciana 8b\n");
                position.z = pz-1 + (colision_table[mapa->mapa[px+1][pz-1]].toZ + colision_table[id].toZ);
            }
        }
    }
}

