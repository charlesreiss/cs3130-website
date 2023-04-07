#include <stdlib.h>
#include "life.h"

/*
 * This file is a port to C by Luther Tychonievich  of a file written in C++ 
 * by Charles Reiss.
 * 
 * This file is released as part of a programming assignment and MUST NOT
 * be shared or replicated except within a single students' private workspace
 * and class submission space. If it is found, in whole or in part, to have been
 * shared outside of the class, the sharing is in violation of this license
 * agreement and will be prosecuted accordingly.
 * 
 * See life.h for documentation of these functions.
 */



void LB_initialize(LifeBoard *self, int w, int h) {
    self->width = w;
    self->height = h;
    if (self->cells) free(self->cells);
    self->cells = (LifeCell *)calloc(w*h, sizeof(LifeCell));
}
void LB_clear(LifeBoard *self) {
    self->width = 0;
    self->height = 0;
    if (self->cells) free(self->cells);
    self->cells = NULL;
}

LifeBoard *LB_new(int w, int h) {
    LifeBoard *ans = calloc(1, sizeof(LifeBoard));
    LB_initialize(ans, w, h);
    return ans;
}
LifeBoard *LB_clone(const LifeBoard *other) {
    LifeBoard *ans = LB_new(other->width, other->height);
    for(int i=0; i<ans->width*ans->height; i+=1) ans->cells[i] = other->cells[i];
    return ans;
}
void LB_del(LifeBoard *self) {
    LB_clear(self);
    free(self);
}


LifeCell LB_get(const LifeBoard *self, int x, int y) {
    return self->cells[x + y*self->width];
}
void LB_set(LifeBoard *self, int x, int y, LifeCell value) {
    self->cells[x + y*self->width] = value;
}
void LB_swap(LifeBoard *self, LifeBoard *other) {
    { int tmp = self->width; self->width = other->width; other->width = tmp; }
    { int tmp = self->height; self->height = other->height; other->height = tmp; }
    { LifeCell *tmp = self->cells; self->cells = other->cells; other->cells = tmp; }
}

int LB_equals(const LifeBoard *self, const LifeBoard *other) {
    if (!self || !other) return 0;
    if (self->width != other->width) return 0;
    if (self->height != other->height) return 0;
    for(int i=0; i<self->width*self->height; i+=1)
        if (self->cells[i] != other->cells[i]) return 0;
    return 1;
}

void LB_display(const LifeBoard *self, FILE *to) {
    fprintf(to, "%d %d\n", self->width, self->height);
    for(int y=0; y<self->height; y+=1) {
        for(int x=0; x<self->width; x+=1) {
            fputc(LB_get(self, x, y) ? 'o' : '.', to);
        }
        fputc('\n', to);
    }
}
void LB_load(LifeBoard *self, FILE *from) {
    int c = '\n';
    LB_clear(self);
    int w, h;
    fscanf(from, "%d %d", &w, &h);
    LB_initialize(self, w, h);
    for(int y=0; y<self->height; y+=1) {
        while(c == '\n') c = fgetc(from);
        for(int x=0; x<self->width; x+=1) {
            LB_set(self, x, y, c == '.' ? 0 : 1);
            c = fgetc(from);
        }
    }
}
LifeBoard *LB_import(FILE *from) {
    LifeBoard *ans = calloc(1, sizeof(LifeBoard));
    LB_load(ans, from);
    return ans;
}

