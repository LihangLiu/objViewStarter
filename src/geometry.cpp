#include "geometry.hpp"

#define BUFFER_OFFSET(_i) (reinterpret_cast<char *>(NULL) + (_i))

/******************************************************************************/

Mesh2D::Mesh2D(vec2 xy_min_, vec2 xy_max_, ivec2 steps_)
    : xy_min(xy_min_)
    , xy_max(xy_max_)
    , steps(steps_)
    , vbo(0)
    , valid(false)
    , num_ndxs(0)
    , ndx(NULL)
{
    makeAabb();
}

Mesh2D::~Mesh2D()
{
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    delete ndx;
}

void Mesh2D::validate()
{
    if (vbo && valid) {
        return;  // Already valid.
    }

    vec2 wh = xy_max - xy_min;
    vec2 delta = wh / vec2(steps);

    int num_vtxs = (steps.x+1)*(steps.y+1);
    assert(sizeof(vec2) == 2*sizeof(GLfloat));
    vec2 *vtx = new vec2[num_vtxs];

    num_ndxs = 2*(steps.x+1)*steps.y;
    ndx = new GLuint[num_ndxs];

    int v = 0,
        n = 0,
        i;
    for (i=0; i<steps.y; i++) {
        int bottom = i*(steps.x+1);
        int top = bottom+(steps.x+1);
        for (int j=0; j<steps.x; j++) {
            vec2 xy = xy_min + delta * vec2(j,i);
            vtx[v++] = xy;
            ndx[n++] = bottom++;
            ndx[n++] = top++;
        }
        // Maximum x edge
        vec2 xy = vec2(xy_max.x, xy_min.y + delta.y*i);
        vtx[v++] = xy;
        ndx[n++] = bottom++;
        ndx[n++] = top++;
    }
    { // Topmost row
        for (int j=0; j<steps.x; j++) {
            vec2 xy = vec2(xy_min.x + delta.x*j, xy_max.y);
            vtx[v++] = xy;
        }
        // Topmost maximum x edge
        vec2 xy = xy_max;
        vtx[v++] = xy;
    }
    assert(v == num_vtxs);
    assert(n == num_ndxs);

    if (!vbo) {
        glGenBuffers(1, &vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, num_vtxs*2*sizeof(GLfloat), vtx, GL_STATIC_DRAW);

    valid = true;

    delete vtx;
}

void Mesh2D::draw()
{
    validate();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), BUFFER_OFFSET(0));
    const GLuint *p = ndx;
    const GLsizei ndxs_per_strip =  2*(steps.x+1);
    for (int i=0; i<steps.y; i++) {
        glDrawElements(GL_TRIANGLE_STRIP, ndxs_per_strip, GL_UNSIGNED_INT, p);
        p += ndxs_per_strip;
    }
}

void Mesh2D::makeAabb()
{
    aabb.lbn = vec3(xy_min, 0);
    aabb.rtf = vec3(xy_max, 0);
}

/******************************************************************************/

ObjMesh::ObjMesh(tinyobj::shape_t s)
    : shape(s)
    , index(0)
    , name(0)
    , vao(0)
    , vbo(0)
    , ebo(0)
    , valid(false)
    , num_ndxs(0)
{
    setUpVBO();
}

ObjMesh::~ObjMesh()
{
    if (vbo) {
        glDeleteBuffers(1, &vbo);
    }
    if (ebo) {
        glDeleteBuffers(1, &ebo);
    }
}

void ObjMesh::setUpVBO()
{
    if (!vao || !valid) {
        assert((shape.mesh.indices.size() % 3) == 0);
        assert((shape.mesh.positions.size() % 3) == 0);
        if (!shape.name.empty()) name = shape.name.c_str();
        if (!shape.mesh.material_ids.empty()) {
            index = shape.mesh.material_ids[0];
            for (size_t i = 0; i < shape.mesh.material_ids.size(); i++)
                assert(index == shape.mesh.material_ids[i]);
        } else index = 0;
        if (index < 0) index = 0;

        bool patched = false;

        num_ndxs = shape.mesh.indices.size();
        GLuint* ndx = new GLuint[num_ndxs];

        for (size_t i = 0; i < num_ndxs; i++)
            ndx[i] = shape.mesh.indices[i];

        size_t num_vtxs = shape.mesh.positions.size() / 3;
        Vtx* vtx = new Vtx[num_vtxs];

        for (size_t i = 0; i < num_vtxs; i++) {
            vtx[i].xyz.x = shape.mesh.positions[3 * i];
            if (vtx[i].xyz.x < aabb.lbn.x) aabb.lbn.x = vtx[i].xyz.x;
            if (vtx[i].xyz.x > aabb.rtf.x) aabb.rtf.x = vtx[i].xyz.x;
            vtx[i].xyz.y = shape.mesh.positions[3 * i + 1];
            if (vtx[i].xyz.y < aabb.lbn.y) aabb.lbn.y = vtx[i].xyz.y;
            if (vtx[i].xyz.y > aabb.rtf.y) aabb.rtf.y = vtx[i].xyz.y;
            vtx[i].xyz.z = shape.mesh.positions[3 * i + 2];
            if (vtx[i].xyz.z < aabb.lbn.z) aabb.lbn.z = vtx[i].xyz.z;
            if (vtx[i].xyz.z > aabb.rtf.z) aabb.rtf.z = vtx[i].xyz.z;
        }

        Vtx* pvtx;

        if (!shape.mesh.normals.empty()) {
            assert(shape.mesh.normals.size() == shape.mesh.positions.size());
            for (size_t i = 0; i < num_vtxs; i++) {
                vtx[i].n.x = shape.mesh.normals[3 * i];
                vtx[i].n.y = shape.mesh.normals[3 * i + 1];
                vtx[i].n.z = shape.mesh.normals[3 * i + 2];
            }
        } else
        /*
        {
            vtx[0].n = normalize(cross(vtx[1].xyz - vtx[0].xyz, vtx[2].xyz - vtx[0].xyz));
            for (size_t i = 1; i < num_vtxs; i++) vtx[i].n = vtx[i - 1].n;
        }

        for (size_t i = 0; i < num_vtxs; i++) {
            vtx[i].st.x = vtx[i].xyz.x;
            vtx[i].st.y = vtx[i].xyz.y;
        }
        */
        {
            patched = true;
            pvtx = new Vtx[num_ndxs];
            for (GLuint i = 0; i < num_ndxs; i++) {
                pvtx[i] = vtx[ndx[i]];
                ndx[i] = i;
            }
            for (size_t i = 0; i < num_ndxs; i += 3) {
                pvtx[i].n = normalize(cross(pvtx[i + 1].xyz - pvtx[i].xyz, pvtx[i + 2].xyz - pvtx[i].xyz));
                pvtx[i + 2].n = pvtx[i + 1].n = pvtx[i].n;
            }
        }
        if (!patched) {
            if (!shape.mesh.texcoords.empty()) {
                //assert((shape.mesh.texcoords.size() / 2) == num_vtxs);
                for (size_t i = 0; i < num_vtxs; i++) {
                    if (2 * i < shape.mesh.texcoords.size() - 1) {
                    vtx[i].st.x = shape.mesh.texcoords[2 * i];
                    vtx[i].st.y = shape.mesh.texcoords[2 * i + 1];
                    }
                }
            } else {
                for (size_t i = 0; i < num_vtxs; i++) {
                    vtx[i].st.x = vtx[i].xyz.x;
                    vtx[i].st.y = vtx[i].xyz.y;
                }
            }
        } else {
            for (size_t i = 0; i < num_ndxs; i++) {
                pvtx[i].st.x = pvtx[i].xyz.x;
                pvtx[i].st.y = pvtx[i].xyz.y;
            }
            delete[] vtx;
            vtx = pvtx;
            num_vtxs = num_ndxs;
        }
/*
        if (!shape.name.empty()) printf("name = %s\n", name);
        printf("index = %d\n", index);
        printf("vtx = \n");
        for (size_t i = 0; i < num_vtxs; i++)
            printf("p[%d] = (%f, %f, %f)\n, n[%d] = (%f, %f, %f)\n, st[%d] = (%f, %f)\n\n",
                   i, vtx[i].xyz.x, vtx[i].xyz.y, vtx[i].xyz.z,
                   i, vtx[i].n.x, vtx[i].n.y, vtx[i].n.z,
                   i, vtx[i].st.x, vtx[i].st.y);
        printf("ndx = ");
        for (size_t i = 0; i < num_ndxs; i++) {
            if (i % 3 == 0) printf("\n");
            printf("%d ", ndx[i]);
        }
        printf("\n");
        printf("size of vtx = %d\n", num_vtxs * sizeof(vtx));
        printf("size of ndx = %d\n", num_ndxs * sizeof(ndx));
*/
        //if (!vao) glGenVertexArrays(1, &vao);
        //glBindVertexArray(vao);
            if (!vbo) glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, num_vtxs * sizeof(Vtx), vtx, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), BUFFER_OFFSET(0));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vtx), BUFFER_OFFSET(sizeof(vec3)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vtx), BUFFER_OFFSET(2 * sizeof(vec3)));
            if (!ebo) glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_ndxs * sizeof(GLuint), ndx, GL_STATIC_DRAW);
       // glBindVertexArray(0);

        valid = true;

        delete[] vtx;
        delete[] ndx;
    }
}

void ObjMesh::draw()
{
    setUpVBO();
    //glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, (GLsizei)num_ndxs, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, (GLsizei)num_ndxs);
    //glBindVertexArray(0);
}

void ObjMesh::makeAabb()
{
    setUpVBO();
}
