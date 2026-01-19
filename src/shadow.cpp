#include "shadow.h"


shadow::shadow()
{
}



void shadow::build_shadow_volume(myMesh* mesh, TVec3<float> light_pos, TVec3<float> pos )
{
	if( shadowDL != -1 )
			glDeleteLists( shadowDL, 0 );
	shadowDL = glGenLists(1);
    glNewList( shadowDL, GL_COMPILE );
	{
		glDisable( GL_LIGHTING );
		glNormal3f(0.0f, 1.0f, 0.0f ); 
        glBegin( GL_QUADS );
        {
            glColor3f( 0.0f, 0.0f, 0.0f );

            TVec3<float> vExtended;

            //
            // For each vertex of the shadow casting object, find the edge 
            // that it helps define and extrude a quad out from that edge.
            //
            for( int i = 0; i < mesh->numVertices(); ++i )
            {
                // Define the edge we're currently working on extruding...
                int e0 = i;
                int e1 = i+1;

                // If the edge's second vertex is out of array range, 
                // place it back at 0
                if( e1 >= mesh->numVertices() )
                    e1 = 0;

                // v0 of our extruded quad will simply use the edge's first 
                // vertex or e0.
                glVertex3fv( mesh->getVertex(e0) );

                // v1 of our quad is created by taking the edge's first 
                // vertex and extending it out by some amount.
                extendVertex( vExtended, light_pos, mesh->getVertex(e0));

                glVertex3fv( vExtended );

                // v2 of our quad is created by taking the edge's second 
                // vertex and extending it out by some amount.
                extendVertex( vExtended, light_pos, mesh->getVertex(e1));
                glVertex3fv( vExtended );

                // v3 of our extruded quad will simply use the edge's second 
                // vertex or e1.
                glVertex3fv( mesh->getVertex(e1) );
            }
        }
        glEnd();

        glEnable( GL_LIGHTING );
    }
    glEndList();
	
}

void shadow::render_shadow()
{
	
	glDisable(GL_LIGHTING);
	glDepthFunc( GL_EQUAL );
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glStencilFunc( GL_ALWAYS, 0, ~0 );
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0f, 0.0f);
	
	
	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCallList( shadowDL );

	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCallList( shadowDL );

	/*glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);*/
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glDepthMask(GL_TRUE);
	

	glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
	draw_shadow();
	
	glDisable(GL_STENCIL_TEST);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	

	glDepthMask( GL_TRUE );
    glDepthFunc( GL_LEQUAL );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    glCullFace( GL_BACK );
    glDisable( GL_CULL_FACE );
	glEnable(GL_LIGHTING);
		
	//glCallList( shadowDL );

}

void shadow::draw_shadow()
{
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 1);
		glVertex2i(1, 1);
		glVertex2i(1, 0);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


void shadow::extendVertex( TVec3<float> &newVert, TVec3<float> lightPos, TVec3<float> vert)
{
    TVec3<float> lightDir;

    // Create a vector that points from the light's position to the original vertex.
    lightDir[0] = vert[0] - lightPos[0];
    lightDir[1] = vert[1] - lightPos[1];
    lightDir[2] = vert[2] - lightPos[2];

    // Then use that vector to extend the original vertex out to a new position.
    // The distance to extend or extrude the new vector is specified by t.
    //newVert = lightPos + lightDir * 2.0f;
	newVert[0] = lightPos[0] + lightDir[0] * 1000000.0f;
    newVert[1] = lightPos[1] + lightDir[1] * 1000000.0f;
    newVert[2] = lightPos[2] + lightDir[2] * 1000000.0f;
}

void shadow::sDraw(myMesh* mesh, TVec3<float> light_pos, TVec3<float> pos)
{
	glPushMatrix();
	glScalef(1.0f, 1.0f, 0.1f);
	float dist=distance(pos[0], pos[2], light_pos[0], light_pos[2]);
	
	float angle=getAngle(light_pos[0], light_pos[2], pos[0], pos[2]);
	glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(0.0f, angle, 0.0f, 1.0f);
	
	glTranslatef(pos[0], pos[1]+1.0f, pos[2]);
	mesh->draw();
	glPopMatrix();
}


float shadow::getAngle(float s_x, float s_z, float d_x, float d_z )
{
	float x, z, dist;
	float angle;
	x=( d_x - s_x);
	z=( d_z - s_z);
	angle=atan( z / x )*180.0f/M_PI;
	dist=sqrt(x*x+z*z);
	x/=dist;
	z/=dist;
				
	if(x<0.0)//quads 3 and 4 need to flip
		angle=angle+180.0f;
	return angle;
}

float shadow::distance(float pointA_X, float pointA_Z, float pointB_X, float pointB_Z )
{
	float x_comp, y_comp;
	x_comp=pointA_X-pointB_X;
	y_comp=pointA_Z-pointB_Z;
	return sqrt(x_comp*x_comp + y_comp*y_comp);
}

/*void shadow::dMap()
{
	GLint viewport[4];
	GLfloat lightPos[4];

	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glGetIntegerv(GL_VIEWPORT, viewport);

	glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(lightFovy,  lightAspect, lightNearPlane, lightFarPlane);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	glLoadIdentity();
	gluLookAt(lightPos[0], lightPos[1], lightPos[2],
		lookAt[0], lookAt[1], lookAt[2],
		up[0], up[1], up[2]);
	drawObjects();
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 
		SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0);

	glViewport(viewport[0], viewport[1],
		viewport[2], viewport[3]);

	GLfloat tmpMatrix(16);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.5, 0.5, 0.0);
	glScalef(0.5, 0.5, 1.0);
	gluPerspective(lightFovy, lightAspect,
		lightNearPlane, lightFarPlane);
	gluLookAt(lightPos[0], lightPos[1], lightPos[2],
		lookAt[0], lookAt[1], lookAt[2],
		up[0], up[1], up[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, tmpMatrix);
	glPopMatrix();

	transposeMatrix(tmpMatrix);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexGenfv(GL_S, GL_OBJECT_PLANE, &tmpMatrix[0]);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, &tmpMatrix[4]);
	glTexGenfv(GL_R, GL_OBJECT_PLANE, &tmpMatrix[8]);
	glTexGenfv(GL_Q, GL_OBJECT_PLANE, &tmpMatrix[12]);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_GEN_Q);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);;
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glEnable(GL_TEXTURE_2D);

}
*/


void shadow::buildShadowMatrix(GLfloat lightPos[4])
{
    float dotp;
	TVec3<float> v0, v1, v2;
	v0[0]=0.0f;
	v0[1]=3.0f;
	v0[2]=0.0f;
	v1=v0;
	v1[0]=-1000.0f;
	v2=v1;
	v2[2]=1000.0f;
	findPlane(v0, v1, v2);

    // Calculate the dot-product between the plane and the light's position
    dotp = plane[0] * lightPos[0] + 
           plane[1] * lightPos[1] + 
           plane[1] * lightPos[2] + 
           plane[3] * lightPos[3];

    // First column
    shadowMatrix[0]  = dotp - lightPos[0] * plane[0];
    shadowMatrix[4]  = 0.0f - lightPos[0] * plane[1];
    shadowMatrix[8]  = 0.0f - lightPos[0] * plane[2];
    shadowMatrix[12] = 0.0f - lightPos[0] * plane[3];

    // Second column
    shadowMatrix[1]  = 0.0f - lightPos[1] * plane[0];
    shadowMatrix[5]  = dotp - lightPos[1] * plane[1];
    shadowMatrix[9]  = 0.0f - lightPos[1] * plane[2];
    shadowMatrix[13] = 0.0f - lightPos[1] * plane[3];

    // Third column
    shadowMatrix[2]  = 0.0f - lightPos[2] * plane[0];
    shadowMatrix[6]  = 0.0f - lightPos[2] * plane[1];
    shadowMatrix[10] = dotp - lightPos[2] * plane[2];
    shadowMatrix[14] = 0.0f - lightPos[2] * plane[3];

    // Fourth column
    shadowMatrix[3]  = 0.0f - lightPos[3] * plane[0];
    shadowMatrix[7]  = 0.0f - lightPos[3] * plane[1];
    shadowMatrix[11] = 0.0f - lightPos[3] * plane[2];
    shadowMatrix[15] = dotp - lightPos[3] * plane[3];
}

// Name: findPlane()
// Desc: find the plane equation given 3 points
//-----------------------------------------------------------------------------
void shadow::findPlane( TVec3<float> v0, TVec3<float> v1, TVec3<float> v2  )
{
    TVec3<float> vec0, vec1;

    // Need 2 vectors to find cross product
    vec0[0] = v1[0] - v0[0];
    vec0[1] = v1[1] - v0[1];
    vec0[2] = v1[2] - v0[2];

    vec1[0] = v2[0] - v0[0];
    vec1[1] = v2[1] - v0[1];
    vec1[2] = v2[2] - v0[2];

    // Find cross product to get A, B, and C of plane equation
    plane[0] =   vec0[1] * vec1[2] - vec0[2] * vec1[1];
    plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
    plane[2] =   vec0[0] * vec1[1] - vec0[1] * vec1[0];

    plane[3] = -(plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}

void shadow::projectShadow(TVec3<float> pos, myMesh* mesh)//, TVec3<float> angle)
{
	glDisable(GL_DEPTH_TEST);
    //glDisable(GL_LIGHTING);

	//glLoadIdentity();
    glColor3f(0.0f, 0.0f, 0.0f); // Shadow's color
    glPushMatrix();
    {
		glScalef(1.f, 0.f, 1.f);
        glMultMatrixf((const GLfloat*)shadowMatrix);

        glTranslatef( pos[0], pos[1], pos[2] );
        //glRotatef( -g_fSpinY_R, 1.0f, 0.0f, 0.0f );
        //glRotatef( -g_fSpinX_R, 0.0f, 1.0f, 0.0f );
		mesh->draw();
        
    }
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
}