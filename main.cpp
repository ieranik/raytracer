/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */
#include <windows.h>
#include <glut.h>
#endif

//#include <FILE.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "bitmap_image.hpp"

#include<GL/glut.h>

#define pi 2*acos(0)
#define dis 4.0
#define ang .04
#define eps 0.0000001

double cameraHeight;
double cameraAngle;
double angle;

FILE* fin;

int hor,ver,nd;

struct point
{
	double x,y,z;
};

struct color
{
	double cr,cg,cb;
};

typedef pair<double,bool> pdb;

class vec
{
public:
	double x,y,z;
	vec(){x=0.0;y=0.0;z=0.0;}
	vec(double xx,double yy,double zz){x=xx;y=yy;z=zz;}
	void setq(double xx,double yy,double zz){x=xx;y=yy;z=zz;}
	void norm()
	{
		double len=sqrt(x*x+y*y+z*z);
		x=x/len;
		y=y/len;
		z=z/len;
	}
	vec cp(vec v)
	{
		return vec(y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x);
	}
	double dp(vec v)
	{
		return x*v.x+y*v.y+z*v.z;
	}
	vec add(vec v)
	{
		return vec(v.x+x,v.y+y,v.z+z);
	}
	vec sub(vec v)
	{
		return vec(-v.x+x,-v.y+y,-v.z+z);
	}
	vec scale(double s)
	{
		return vec(s*x,s*y,s*z);
	}
	struct point add(struct point p)
	{
		struct point ret;
		ret.x=x+p.x;
		ret.y=y+p.y;
		ret.z=z+p.z;
		return ret;

	}
};

class ray
{
public:
    double orgx,orgy,orgz;
    double dirx,diry,dirz;
    ray()
    {
        orgx=0.0;orgy=0.0;orgz=0.0;
        dirx=0.0;diry=0.0;dirz=0.0;
    }
    ray(double ox,double oy,double oz,double dx,double dy,double dz)
    {
        orgx=ox;orgy=oy;orgz=oz;
        dirx=dx;diry=dy;dirz=dz;
    }
    void norm()
	{
		double len=sqrt(dirx*dirx+diry*diry+dirz*dirz);
		dirx=dirx/len;
		diry=diry/len;
		dirz=dirz/len;
	}
	vec org(){return vec(orgx,orgy,orgz);}
	vec dir(){return vec(dirx,diry,dirz);}
};

typedef pair<vec,ray> pvr;

vec ptov(struct point p)
{
    vec v(0.0,0.0,0.0);
    v.x=p.x;
    v.y=p.y;
    v.z=p.z;
    return v;
}

class object
{
public:
    int type;
    double posx,posy,posz;
    double hlr;
    double colr,colg,colb;
    double al,dl,sl,rl;
    double sh;
    object(int t,double px,double py,double pz,double h,double cr,double cg,double cb,double all,double dll,double sll,double rll,double shh)
    {
        type=t;
        posx=px;posy=py;posz=pz;
        hlr=h;
        colr=cr;colg=cg;colb=cb;
        al=all;dl=dll;sl=sll;rl=rll;
        sh=shh;
    }
    vec pos(){return vec(posx,posy,posz);}
    pvr reflect(ray r,struct point org,struct point dst)
    {
        pvr ret;
        bool inside;
        if(type==1)
        {
            vec ro=r.org().add(pos().scale(-1.0));
            vec a=r.dir();
            inside=(hlr*hlr-ro.dp(ro)>0);

            vec n=ptov(dst);
            n=n.sub(pos());
            n.norm();
            if(inside=true)n=n.scale(-1.0);
            ret.first=n;

            vec r=a.sub(n.scale(2*a.dp(n)));
            vec nor=ptov(dst).add(r.scale(0.1));
            ray rr(nor.x,nor.y,nor.z,r.x,r.y,r.z);
            rr.norm();
            ret.second=rr;

            return ret;

        }
        else if(type==2)
        {
            vec n(0.0,0.0,0.0);
            if(fabs(posx-dst.x)<eps)n.setq(-1.0,0.0,0.0);
            else if(fabs(posx+hlr-dst.x)<eps)n.setq(1.0,0.0,0.0);
            else if(fabs(posy-dst.y)<eps)n.setq(0.0,-1.0,0.0);
            else if(fabs(posy+hlr-dst.y)<eps)n.setq(0.0,1.0,0.0);
            else if(fabs(posz-dst.z)<eps)n.setq(0.0,0.0,-1.0);
            else if(fabs(posz+hlr-dst.z)<eps)n.setq(0.0,0.0,1.0);

            inside=(org.x>posx&&org.x<posx+hlr&&org.y>posy&&org.y<posy+hlr&&org.z>posz&&org.z<posz+hlr);
            if(inside==true)n=n.scale(-1.0);
            ret.first=n;

            vec a=r.dir();
            vec r=a.sub(n.scale(2*a.dp(n)));
            vec nor=ptov(dst).add(r.scale(0.1));
            ray rr(nor.x,nor.y,nor.z,r.x,r.y,r.z);
            rr.norm();
            ret.second=rr;

            return ret;
        }
        else if(type==3)
        {
            vec n(0.0,0.0,0.0);
            if(org.z<0)n.setq(0.0,0.0,-1.0);
            else n.setq(0.0,0.0,1.0);
            ret.first=n;

            vec a=r.dir();
            vec r=a.sub(n.scale(2*a.dp(n)));
            vec nor=ptov(dst).add(r.scale(0.1));
            ray rr(nor.x,nor.y,nor.z,r.x,r.y,r.z);
            rr.norm();
            ret.second=rr;

            return ret;
        }

    }
    pdb intersect(ray r)
    {
        if(type==1)
        {
            vec ro=r.org().add(this->pos().scale(-1.0));
            vec rd=r.dir();
            bool inside=(hlr*hlr-ro.dp(ro)>0);
            double tp=rd.dp(ro.scale(-1));
            if(inside==false&&tp<0)return pdb(-1,false);
            double d=ro.dp(ro)-tp*tp;
            if(d>hlr*hlr)return pdb(-1,false);
            double t1=sqrt(hlr*hlr-d);
            if(inside==false)return pdb(tp-t1,true);
            else return pdb(tp+t1,true);
        }
        else if(type==2)
        {
            double t=100000.0;
            double t1;
            bool b;
            struct point pp;
            pp.x=r.orgx;
            pp.y=r.orgy;
            pp.z=r.orgz;

            b=false;
            if(r.dirz!=0)
            {
                t1=-(-posz+r.orgz)/(r.dirz);
                struct point p=r.dir().scale(t1).add(pp);
                if(p.x>=posx&&p.x<=posx+hlr&&p.y>=posy&&p.y<=posy+hlr)b=true;
            }
            if(b==true&&t1>0&&t1<t)t=t1;

            b=false;
            if(r.dirz!=0)
            {
                t1=-(-posz-hlr+r.orgz)/(r.dirz);
                struct point p=r.dir().scale(t1).add(pp);
                if(p.x>=posx&&p.x<=posx+hlr&&p.y>=posy&&p.y<=posy+hlr)b=true;
            }
            if(b==true&&t1>0&&t1<t)t=t1;



            b=false;
            if(r.diry!=0)
            {
                t1=-(-posy+r.orgy)/(r.diry);
                struct point p=r.dir().scale(t1).add(pp);
                if(p.x>=posx&&p.x<=posx+hlr&&p.z>=posz&&p.z<=posz+hlr)b=true;
            }
            if(b==true&&t1>0&&t1<t)t=t1;

            b=false;
            if(r.diry!=0)
            {
                t1=-(-posy-hlr+r.orgy)/(r.diry);
                struct point p=r.dir().scale(t1).add(pp);
                if(p.x>=posx&&p.x<=posx+hlr&&p.z>=posz&&p.z<=posz+hlr)b=true;
            }
            if(b==true&&t1>0&&t1<t)t=t1;



            b=false;
            if(r.dirx!=0)
            {
                t1=-(-posx+r.orgx)/(r.dirx);
                struct point p=r.dir().scale(t1).add(pp);
                if(p.y>=posy&&p.y<=posy+hlr&&p.z>=posz&&p.z<=posz+hlr)b=true;
            }
            if(b==true&&t1>0&&t1<t)t=t1;

            b=false;
            if(r.dirx!=0)
            {
                t1=-(-posx-hlr+r.orgx)/(r.dirx);
                struct point p=r.dir().scale(t1).add(pp);
                if(p.y>=posy&&p.y<=posy+hlr&&p.z>=posz&&p.z<=posz+hlr)b=true;
            }
            if(b==true&&t1>0&&t1<t)t=t1;

            if(t>99999.0)return pdb(-1,false);
            else return pdb(t,true);

        }
        else if(type==3)
        {
            double t1;
            bool b=false;
            struct point pp;
            pp.x=r.orgx;
            pp.y=r.orgy;
            pp.z=r.orgz;

            if(r.dirz!=0)
            {
                t1=-(r.orgz)/(r.dirz);
                struct point p=r.dir().scale(t1).add(pp);
                b=true;

            }
            if(b==true&&t1>0)return pdb(t1,true);
            else return pdb(-1,false);

        }
    }
    struct color getcol(struct point p)
    {
        struct color c;
        if(type==1||type==2)
        {
            c.cr=colr;
            c.cg=colg;
            c.cb=colb;
            return c;
        }
        else
        {
            if((int((p.x+10000.0)/20)+int((p.y+10000.0)/20))%2==0)
            {
                c.cr=0.0;
                c.cg=0.0;
                c.cb=0.0;
                return c;
            }
            else
            {
                c.cr=1.0;
                c.cg=1.0;
                c.cb=1.0;
                return c;
            }


        }
    };

};

int nob,nls;
vector<object> objects;


vec rotate1(vec v1,vec v2,double angle)
{
	vec t1=v1.scale(cos(angle));
	vec t2=v2.scale(sin(angle));
	t1=t1.add(t2);
	t1.norm();
	return t1;
}

vec rotate2(vec v1,vec v2,double angle)
{
	vec t1=v1.scale(-sin(angle));
	vec t2=v2.scale(cos(angle));
	t1=t1.add(t2);
	t1.norm();
	return t1;
}



struct point cp;
vec l(1,0,0),u(1,0,0),r(1,0,0);
vec t1(0,0,1);
vec t2(0,0,1);





void keyboardListener(unsigned char key, int x,int y){

	switch(key){

		case '1':

			t1=rotate1(r,l,ang);

			t2=rotate2(r,l,ang);
			r=t1;
			l=t2;
			break;

		case '2':

			t1=rotate1(r,l,-ang);

			t2=rotate2(r,l,-ang);
			r=t1;
			l=t2;
			break;


		case '3':

			t1=rotate1(l,u,ang);

			t2=rotate2(l,u,ang);
			l=t1;
			u=t2;
			break;

		case '4':

			t1=rotate1(l,u,-ang);

			t2=rotate2(l,u,-ang);
			l=t1;
			u=t2;
			break;

		case '5':

			t1=rotate1(u,r,ang);

			t2=rotate2(u,r,ang);
			u=t1;
			r=t2;
			break;

		case '6':

			t1=rotate1(u,r,-ang);

			t2=rotate2(u,r,-ang);
			u=t1;
			r=t2;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cp.x=l.scale(-dis).add(cp).x;
			cp.y=l.scale(-dis).add(cp).y;
			cp.z=l.scale(-dis).add(cp).z;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cp.x=l.scale(dis).add(cp).x;
			cp.y=l.scale(dis).add(cp).y;
			cp.z=l.scale(dis).add(cp).z;
			break;

		case GLUT_KEY_RIGHT:
			cp.x=r.scale(dis).add(cp).x;
			cp.y=r.scale(dis).add(cp).y;
			cp.z=r.scale(dis).add(cp).z;
			break;
		case GLUT_KEY_LEFT:
			cp.x=r.scale(-dis).add(cp).x;
			cp.y=r.scale(-dis).add(cp).y;
			cp.z=r.scale(-dis).add(cp).z;
			break;

		case GLUT_KEY_PAGE_UP:
			cp.x=u.scale(dis).add(cp).x;
			cp.y=u.scale(dis).add(cp).y;
			cp.z=u.scale(dis).add(cp).z;
			break;
		case GLUT_KEY_PAGE_DOWN:
			cp.x=u.scale(-dis).add(cp).x;
			cp.y=u.scale(-dis).add(cp).y;
			cp.z=u.scale(-dis).add(cp).z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}

struct ls
{
    double px,py,pz;
};

vector<ls> lss;

double dist(double x1,double y1,double z1,double x2,double y2,double z2)
{
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
}

bool doesint(point p,int id)
{
    ray r(p.x,p.y,p.z,lss[id].px,lss[id].py,lss[id].pz);
    r.norm();
    vec tmp=r.org().add(r.dir().scale(0.1));
    r.orgx=tmp.x;
    r.orgy=tmp.y;
    r.orgz=tmp.z;
    double t=dist(p.x,p.y,p.z,lss[id].px,lss[id].py,lss[id].pz);
    int i;
    pdb ret;
    for(i=0;i<nob;i++)
    {
        ret=objects[i].intersect(r);
        if(ret.second==true&&ret.first<t)
        {
            //fprintf(fout," %d\n",i);
            return true;
        }
    }
    return false;
}

struct color addcol(struct color c1,struct color c2)
{
    struct color ret;
    ret.cr=c1.cr+c2.cr;
    ret.cg=c1.cg+c2.cg;
    ret.cb=c1.cb+c2.cb;

    if(ret.cr>1.0)ret.cr=1.0;
    if(ret.cg>1.0)ret.cg=1.0;
    if(ret.cb>1.0)ret.cb=1.0;

    return ret;
};

struct color scalecol(double d,struct color c)
{
    struct color ret;
    ret.cr=d*c.cr;
    ret.cg=d*c.cg;
    ret.cb=d*c.cb;

    if(ret.cr>1.0)ret.cr=1.0;
    if(ret.cg>1.0)ret.cg=1.0;
    if(ret.cb>1.0)ret.cb=1.0;

    return ret;
};

struct color clampcol(struct color c)
{
    if(c.cr>1.0)c.cr=1.0;
    if(c.cg>1.0)c.cg=1.0;
    if(c.cb>1.0)c.cb=1.0;
    return c;
};

struct color detcol(ray r,int depth)
{
    int k,id;
    struct color c;
    double t=100000.0;
    for(k=0;k<nob;k++)
    {
        pdb tmp=objects[k].intersect(r);
        if(tmp.second==false)continue;
        if(tmp.first<t)
        {
            t=tmp.first;
            id=k;
        }
    }
    if(t>99999.0)
    {
        c.cr=0.0;
        c.cg=0.0;
        c.cb=0.0;
        return c;
    }
    else
    {
        struct point org,dst;
        org.x=r.orgx;
        org.y=r.orgy;
        org.z=r.orgz;
        dst=r.dir().scale(t).add(org);


        struct color ac=objects[id].getcol(dst);
        pvr vr=objects[id].reflect(r,org,dst);
        vec an=vr.first;
        an.norm();
        ray arr=vr.second;
        arr.norm();
        struct color ret;
        ret.cr=0.0;
        ret.cg=0.0;
        ret.cb=0.0;

        ret=addcol(ret,scalecol(objects[id].al,ac));
        //fprintf(fout," %lf %lf %lf 1\n",ret.cr,ret.cg,ret.cb);

        int i;
        double f=(1.0-objects[id].al-objects[id].rl)/(double)nls;
        for(i=0;i<nls;i++)
        {
            vec cur(lss[i].px-dst.x,lss[i].py-dst.y,lss[i].pz-dst.z);
            cur.norm();
            if(doesint(dst,i)==false)
            {
                double tfd=max(0.0,cur.dp(an));
                tfd*=objects[id].dl/(objects[id].dl+objects[id].sl);
                double tfs=max(0.0,cur.dp(arr.dir()));
                tfs=pow(tfs,objects[id].sh);

                //if(id==1&&tfs>0.85)printf("ok");

                tfs*=objects[id].sl/(objects[id].dl+objects[id].sl);
                ret=addcol(ret,scalecol(f*(tfd+tfs),ac));
                //fprintf(fout," %lf %lf %lf 2\n",ret.cr,ret.cg,ret.cb);

            }

        }
        if(depth==1)return clampcol(scalecol(1.5,scalecol(1.0/(1.0-objects[id].rl),ret)));
        else return clampcol(scalecol(1.5,addcol(ret,scalecol(objects[id].rl,detcol(arr,depth-1)))));

    }
};


void create_image()
{
    bitmap_image image(hor,ver);

    image.set_all_channels(255,255,255);

    image_drawer draw(image);


    int i,j,k;
    vec orig(cp.x,cp.y,cp.z);
    struct point oo=l.scale(10).add(cp);
    for(i=0;i<hor;i++)
    {
        for(j=0;j<ver;j++)
        {
            double x=-5.7735+double(i)*11.547/double(hor);
            double y=5.7735-double(j)*11.547/double(ver);
            vec dest(oo.x,oo.y,oo.z);
            dest=dest.add(r.scale(x));
            dest=dest.add(u.scale(y));
            vec dir=dest.add(orig.scale(-1));
            ray r(orig.x,orig.y,orig.z,dir.x,dir.y,dir.z);
            r.norm();
            struct color c=detcol(r,nd);
            draw.pen_color(int(c.cr*255),int(c.cg*255),int(c.cb*255));
            draw.plot_pixel(i,j);
        }
    }
    //fclose(fout);

    image.save_image("output.bmp");

}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				create_image();
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

void drawCube(double l)
{
    glBegin(GL_QUADS);{
		glVertex3f( 0, 0, 0);
		glVertex3f( 0, l, 0);
		glVertex3f( l, l, 0);
		glVertex3f( l, 0, 0);
	}glEnd();

    glBegin(GL_QUADS);{
		glVertex3f( 0, 0, l);
		glVertex3f( 0, l, l);
		glVertex3f( l, l, l);
		glVertex3f( l, 0, l);
	}glEnd();

    glBegin(GL_QUADS);{
		glVertex3f( 0, 0, 0);
		glVertex3f( 0, l, 0);
		glVertex3f( 0, l, l);
		glVertex3f( 0, 0, l);
	}glEnd();

	glBegin(GL_QUADS);{
		glVertex3f( l, 0, 0);
		glVertex3f( l, l, 0);
		glVertex3f( l, l, l);
		glVertex3f( l, 0, l);
	}glEnd();

	glBegin(GL_QUADS);{
		glVertex3f( 0, 0, 0);
		glVertex3f( l, 0, 0);
		glVertex3f( l, 0, l);
		glVertex3f( 0, 0, l);
	}glEnd();

	glBegin(GL_QUADS);{
		glVertex3f( 0, l, 0);
		glVertex3f( l, l, 0);
		glVertex3f( l, l, l);
		glVertex3f( 0, l, l);
	}glEnd();
}

void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

//	now give three info
//	1. where is the camera (viewer)?
//	2. where is the camera looking?
//	3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);


	//gluLookAt(100*cos(cameraAngle), 100*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,-1,150,	0,0,0,	0,0,1);


	struct point tmp=l.add(cp);
	gluLookAt(cp.x,cp.y,cp.z, tmp.x,tmp.y,tmp.z, u.x,u.y,u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	int i;
	for(i=0;i<nob;i++)
    {
        if(objects[i].type==1)
        {
            glPushMatrix();
            {
                glColor3f(objects[i].colr,objects[i].colg,objects[i].colb);
                glTranslatef(objects[i].posx,objects[i].posy,objects[i].posz);
                glutSolidSphere(objects[i].hlr,20,20);
            }
            glPopMatrix();
        }
        else if(objects[i].type==2)
        {
            glPushMatrix();
            {
                glColor3f(objects[i].colr,objects[i].colg,objects[i].colb);
                glTranslatef(objects[i].posx,objects[i].posy,objects[i].posz);
                drawCube(objects[i].hlr);
            }
            glPopMatrix();
        }
        else if(objects[i].type==3)
        {
            int j,k;
            for(j=-30;j<30;j++)
            {
                for(k=-30;k<30;k++)
                {
                    if((j+k)%2==0)glColor3f(0.0,0.0,0.0);
                    else glColor3f(1.0,1.0,1.0);
                    glBegin(GL_QUADS);{
                        glVertex3f( j*20, k*20, 0);
                        glVertex3f( j*20+20, k*20, 0);
                        glVertex3f( j*20+20, k*20+20, 0);
                        glVertex3f( j*20, k*20+20, 0);
                    }glEnd();


                }
            }
        }

    }


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate(){
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){

	//codes for initialization
	cameraHeight=100.0;
	cameraAngle=1.0;
	cp.x=100;
	cp.y=100;
	cp.z=0;
	l.x=-1;
	l.y=-1;
	l.z=0;
	l.norm();
	u.x=0;
	u.y=0;
	u.z=1;
	u.norm();
	r=l.cp(u);
	r.norm();


	fin=fopen("description.txt","r");
	//fout=fopen("log.txt","w");


	if(fin==NULL)
	{
		printf("Error opening file");
		return;
	}

    fscanf(fin," %d",&nd);

	fscanf(fin," %d",&hor);
	ver=hor;

	fscanf(fin," %d",&nob);

	int i;
	for(i=0;i<nob;i++)
    {
        int t;
        double px,py,pz,h,cr,cg,cb,al,dl,sl,rl,sh;
        fscanf(fin," %d",&t);
        fscanf(fin," %lf",&px);
        fscanf(fin," %lf",&py);
        fscanf(fin," %lf",&pz);
        fscanf(fin," %lf",&h);
        fscanf(fin," %lf",&cr);
        fscanf(fin," %lf",&cg);
        fscanf(fin," %lf",&cb);
        fscanf(fin," %lf",&al);
        fscanf(fin," %lf",&dl);
        fscanf(fin," %lf",&sl);
        fscanf(fin," %lf",&rl);
        fscanf(fin," %lf",&sh);

        objects.push_back(object(t,px,py,pz,h,cr,cg,cb,al,dl,sl,rl,sh));
    }
    nob++;
    objects.push_back(object(3,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.2,0.35,0.25,0.2,10));

    fscanf(fin," %d",&nls);


	for(i=0;i<nls;i++)
    {
        struct ls l;
        fscanf(fin," %lf",&l.px);
        fscanf(fin," %lf",&l.py);
        fscanf(fin," %lf",&l.pz);
        lss.push_back(l);
        //printf("%d",i);
    }





	fclose(fin);


	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(60,	1,	1,	10000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
