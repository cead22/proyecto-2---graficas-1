///////////////////////////
// INICIO DE TRIMESH
//////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "trimesh/include/TriMesh.h"
#include "trimesh/include/XForm.h"
#include "trimesh/include/GLCamera.h"
#include "trimesh/include/ICP.h"
#include <GL/glut.h>

using std::string;
using std::vector;


// Globals
vector<TriMesh *> meshes;
vector<xform> xforms;
vector<bool> visible;
vector<string> xffilenames;

TriMesh::BSphere global_bsph;
xform global_xf;
GLCamera camera;

int current_mesh = -1;

bool draw_edges = false;
bool draw_2side = false;
bool draw_shiny = true;
bool draw_lit = true;
bool draw_falsecolor = false;
bool draw_index = false;
bool white_bg = false;


// Signal a redraw
void need_redraw()
{
        glutPostRedisplay();
}


// Clear the screen
void cls()
{
        glDisable(GL_DITHER);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_NORMALIZE);
        glDisable(GL_LIGHTING);
        glDisable(GL_NORMALIZE);
        glDisable(GL_COLOR_MATERIAL);
        /*if (white_bg)
                glClearColor(1, 1, 1, 0);
        else
                glClearColor(0.08, 0.08, 0.08, 0);*/
        //glClearDepth(1);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


// Set up lights and materials
void setup_lighting(int id)
{
        Color c(1.0f);
        if (draw_falsecolor)
                c = Color::hsv(-3.88 * id, 0.6 + 0.2 * sin(0.42 * id), 1.0);
        glColor3fv(c);

        if (!draw_lit || meshes[id]->normals.empty()) {
                glDisable(GL_LIGHTING);
                return;
        }

        GLfloat mat_specular[4] = { 0.18, 0.18, 0.18, 0.18 };
        if (!draw_shiny) {
                mat_specular[0] = mat_specular[1] =
                mat_specular[2] = mat_specular[3] = 0.0f;
        }
        GLfloat mat_shininess[] = { 64 };
        GLfloat global_ambient[] = { 0.02, 0.02, 0.05, 0.05 };
        GLfloat light0_ambient[] = { 0, 0, 0, 0 };
        GLfloat light0_diffuse[] = { 0.85, 0.85, 0.8, 0.85 };
        if (current_mesh >= 0 && id != current_mesh) {
                light0_diffuse[0] *= 0.5f;
                light0_diffuse[1] *= 0.5f;
                light0_diffuse[2] *= 0.5f;
        }
        GLfloat light1_diffuse[] = { -0.01, -0.01, -0.03, -0.03 };
        GLfloat light0_specular[] = { 0.85, 0.85, 0.85, 0.85 };
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, draw_2side);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_NORMALIZE);
}


// Draw triangle strips.  They are stored as length followed by values.
void draw_tstrips(const TriMesh *themesh)
{
        static bool use_glArrayElement = false;
        static bool tested_renderer = false;
        if (!tested_renderer) {
                use_glArrayElement = !!strstr(
                        (const char *) glGetString(GL_RENDERER), "Intel");
                tested_renderer = true;
        }

        const int *t = &themesh->tstrips[0];
        const int *end = t + themesh->tstrips.size();
        if (use_glArrayElement) {
                while (likely(t < end)) {
                        glBegin(GL_TRIANGLE_STRIP);
                        int striplen = *t++;
                        for (int i = 0; i < striplen; i++)
                                glArrayElement(*t++);
                        glEnd();
                }
        } else {
                while (likely(t < end)) {
                        int striplen = *t++;
                        glDrawElements(GL_TRIANGLE_STRIP, striplen, GL_UNSIGNED_INT, t);
                        t += striplen;
                }
        }
}


// Draw the mesh
void draw_mesh(int i)
{
        const TriMesh *themesh = meshes[i];

        glPushMatrix();
        glMultMatrixd(xforms[i]);

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);

        if (draw_2side) {
                glDisable(GL_CULL_FACE);
        } else {
                glCullFace(GL_BACK);
                glEnable(GL_CULL_FACE);
        }

        // Vertices
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT,
                        sizeof(themesh->vertices[0]),
                        &themesh->vertices[0][0]);

        // Normals
        if (!themesh->normals.empty() && !draw_index) {
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT,
                                sizeof(themesh->normals[0]),
                                &themesh->normals[0][0]);
        } else {
                glDisableClientState(GL_NORMAL_ARRAY);
        }

        // Colors
        if (!themesh->colors.empty() && !draw_falsecolor && !draw_index) {
                glEnableClientState(GL_COLOR_ARRAY);
                glColorPointer(3, GL_FLOAT,
                               sizeof(themesh->colors[0]),
                               &themesh->colors[0][0]);
        } else {
                glDisableClientState(GL_COLOR_ARRAY);
        }

        // Main drawing pass
        if (themesh->tstrips.empty()) {
                // No triangles - draw as points
                glPointSize(1);
                glDrawArrays(GL_POINTS, 0, themesh->vertices.size());
                glPopMatrix();
                return;
        }

        if (draw_edges) {
                glPolygonOffset(10.0f, 10.0f);
                glEnable(GL_POLYGON_OFFSET_FILL);
        }

        draw_tstrips(themesh);
        glDisable(GL_POLYGON_OFFSET_FILL);

        // Edge drawing pass
        if (draw_edges) {
                glPolygonMode(GL_FRONT, GL_LINE);
                glDisableClientState(GL_COLOR_ARRAY);
                glDisable(GL_COLOR_MATERIAL);
                GLfloat global_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
                GLfloat light0_diffuse[] = { 0.8, 0.8, 0.8, 0.0 };
                GLfloat light1_diffuse[] = { -0.2, -0.2, -0.2, 0.0 };
                GLfloat light0_specular[] = { 0.0f, 0.0f, 0.0f, 0.0f };
                glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
                glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
                GLfloat mat_diffuse[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);
                glColor3f(0, 0, 1); // Used iff unlit
                draw_tstrips(themesh);
                glPolygonMode(GL_FRONT, GL_FILL);
        }

        glPopMatrix();
}
// Update global bounding sphere.
void update_bsph()
{
        point boxmin(1e38, 1e38, 1e38);
        point boxmax(-1e38, -1e38, -1e38);
        bool some_vis = false;
        for (int i = 0; i < (int)meshes.size(); i++) {
                if (!visible[i])
                        continue;
                some_vis = true;
                point c = xforms[i] * meshes[i]->bsphere.center;
                float r = meshes[i]->bsphere.r;
                for (int j = 0; j < 3; j++) {
                        boxmin[j] = std::min(boxmin[j], c[j]-r);
                        boxmax[j] = std::max(boxmax[j], c[j]+r);
                }
        }
        if (!some_vis)
                return;
        point &gc = global_bsph.center;
        float &gr = global_bsph.r;
        gc = 0.5f * (boxmin + boxmax);
        gr = 0.0f;
        for (int i = 0; i < (int)meshes.size(); i++) {
                if (!visible[i])
                        continue;
                point c = xforms[i] * meshes[i]->bsphere.center;
                float r = meshes[i]->bsphere.r;
                gr = std::max(gr, dist(c, gc) + r);
        }
}


// Set the view...
void resetview()
{
        printf("VIEW\n");
        //camera.stopspin();
        for (int i = 0; i < (int)meshes.size(); i++)
                if (!xforms[i].read(xffilenames[i]))
                        xforms[i] = xform();

        update_bsph();
        global_xf = xform::trans(0, 0, -5.0f * global_bsph.r) *
                    xform::trans(-global_bsph.center);

        // Special case for 1 mesh
        if (meshes.size() == 1 && xforms[0].read(xffilenames[0])) {
                global_xf = xforms[0];
                xforms[0] = xform();
                update_bsph();
        }
}

void usage(const char *myname)
{
        fprintf(stderr, "Usage: %s infile...\n", myname);
        exit(1);
}

/////////////////////////
// FIN DE TRIMESH
////////////////////////