#include<fstream>
#include<iostream>
#include<map>
#include<vector>
#include<set>
#include<cmath>
#include<memory.h>
#include<vector>
#include<stdlib.h>
#include<cstring>
#include "point.h"
#include "face.h"
#include "edge.h"
using namespace std;

double percent = 0.1;//输出百分比
int fnum = 1;//记录面数
int targetFaceNum = 0;//需要输出的面数
int RealPoint[230000];//辅助输出的数组
int cnt = 1;

vector<point> Point;
vector<face> Face;

void Prepare(char* filename);
void SetOutput(char* filename);
void Shrink();
void setEdgeva(int u, int v, int ef);
void CalcQ(int u);
double CalcDelta(int v1, int v2);
void CalcKp(int ff);

set<edge> Edge;

int main(int argc, char* argv[])
{
    point zeropoint;
    Point.push_back(zeropoint);
    face zeroface;
    Face.push_back(zeroface);//Face和Point的零号元素不使用
    cout<<"Preparing"<<endl;
    percent = atof(argv[3]);
    Prepare(argv[1]);

    targetFaceNum = fnum*percent;

    cout<<"Shrink"<<endl;

    while(fnum > targetFaceNum)
        Shrink();

    // int realfacenum = 0;
    // for(int i = 1; i < Face.size(); i++)
    //     if(Face[i].exist)
    //         realfacenum++;

    cout<<"Write obj"<<endl;

    SetOutput(argv[2]);

    return 0;
}

void Shrink()
{
    edge e = *(Edge.begin());
    int u = e.u;
    int v = e.v;
    if(!Point[u].exist || !Point[v].exist)
    {
        Edge.erase(Edge.begin());
        return;
    }

    int f1 = e.f1;
    int f2 = e.f2;
    Face[f1].exist = false;
    Face[f2].exist = false;
    fnum -= 2;

    set<int>::iterator it;
    for(it = Point[u].neighborFace.begin(); it != Point[u].neighborFace.end(); it++)
    {//把u周围的面改成v周围
        int k = *(it);
        if(Face[k].exist)
        {
            if(Face[k].p[0] == u)
                Face[k].p[0] = v;
            else if(Face[k].p[1] == u)
                Face[k].p[1] = v;
            else if(Face[k].p[2] == u)
                Face[k].p[2] = v;
            Point[v].addFace(k);
        }
    }

    setEdgeva(u,v,f1);
    setEdgeva(u,v,f2);
    Point[u].exist = false;

    for(it = Point[u].neighborPoint.begin(); it != Point[u].neighborPoint.end(); it++)
    {//删除u的连边，添加v的连边
        int q = *(it);
        if(Point[q].exist)
        {
            edge qu(q,u);
            qu.Delta = CalcDelta(q,u);
            set<edge>::iterator itr = Edge.find(qu);
            if(itr != Edge.end())
            {
                edge tmp = *(itr);
                Edge.erase(itr);

                if (!(tmp.f1 == f1 || tmp.f1 == f2 || tmp.f2 == f1 || tmp.f2 == f2))
                {
                    edge qv(q,v);
                    qv.Delta = CalcDelta(q,v);
                    qv.f1 = tmp.f1;
                    qv.f2 = tmp.f2;
                    Edge.insert(qv);
                    Point[v].addPoint(q);
                    Point[q].addPoint(v);
                }
            }
        }
    }

    //将v移至中点
    point mid;
    mid.x = (Point[u].x + Point[v].x) / 2;
    mid.y = (Point[u].y + Point[v].y) / 2;
    mid.z = (Point[u].z + Point[v].z) / 2;

    Point[0] = Point[v];//用Point[0]记录新的v点
    Point[0].setpos(mid.x, mid.y, mid.z);
    CalcQ(0);

    for(it = Point[v].neighborPoint.begin(); it != Point[v].neighborPoint.end(); it++)
    {
        int q = *(it);
        if(Point[q].exist)
        {
            edge oldqv(q,v);
            oldqv.Delta = CalcDelta(q,v);
            set<edge>::iterator itr = Edge.find(oldqv);
            if(itr == Edge.end())
            {
               continue;
            }
            edge tmp = *(itr);
            Edge.erase(itr);
            tmp.Delta = CalcDelta(q,0);

            Edge.insert(tmp);
        }
    }
    Point[v] = Point[0];

//    set<int>::iterator sit;
//    for(sit=Point[v].neighborFace.begin() ; sit!=Point[v].neighborFace.end() ; sit++)
//    {
//        int tmp = *(sit);
//        if(Face[tmp].exist)
//            CalcKp(tmp);
//    }

//    for(sit=Point[v].neighborPoint.begin() ; sit!=Point[v].neighborPoint.end() ; sit++)
//    {
//        int tmp = *(sit);
//        if(Point[tmp].exist)
//            CalcQ(tmp);
//    }

}

void setEdgeva(int u, int v, int ef)
{
    int a = 0;
    for(int j = 0; j < 3; j++)
    {
        if(Face[ef].p[j] != u && Face[ef].p[j] != v)
        {
            a = Face[ef].p[j];
            break;
        }
    }
    edge v_a(v, a), u_a(u, a);
    v_a.Delta = CalcDelta(v,a);
    u_a.Delta = CalcDelta(u,a);
    set<edge>::iterator itv = Edge.find(v_a);
    set<edge>::iterator itu = Edge.find(u_a);
    if(itv != Edge.end())
    {
        edge tmp1 = *(itv);
        Edge.erase(itv);
        int F1 = tmp1.f1;
        if(F1 == ef)
            F1 = tmp1.f2;

        edge tmp2 = *(itu);
        int F2 = tmp2.f1;
        if(F2 == ef)
            F2 = tmp2.f2;
        v_a.f1 = F1;
        v_a.f2 = F2;
        Edge.insert(v_a);
    }
}

void Prepare(char* filename)
{
    ifstream fin(filename);
    while(!fin.eof())
    {
        char ch;
        fin >> ch;
        if (ch == 'v')
        {
            point tmpp;
            fin >> tmpp.x >> tmpp.y >> tmpp.z;
            Point.push_back(tmpp);
        }
        else if (ch == 'f')
        {
            face tmpf;
            fin >> tmpf.p[0] >> tmpf.p[1] >> tmpf.p[2];
            Face.push_back(tmpf);
            if (Face[fnum].p[0] == 0) break;
            fnum++;
        }
        else
        {
            char buf[1000];
            fin.getline(buf, sizeof(buf));
        }
    }
    fin.close();

    for(int k = 1; k < Face.size(); k++)
    {
        for(int j = 0; j <= 2; j++)
        {
            int u = Face[k].p[j], v;
            if (j == 2) v = Face[k].p[0];
            else v = Face[k].p[j + 1];
            Point[u].addPoint(v);
            Point[v].addPoint(u);
            Point[u].addFace(k);
        }
        CalcKp(k);
    }

    for(int i = 1; i < Point.size(); i++)
        CalcQ(i);

    for(int k = 1; k < Face.size(); k++)
    {
        for(int j = 0; j <= 2; j++)
        {
            int u = Face[k].p[j], v;
            if(j == 2)
                v = Face[k].p[0];
            else
                v = Face[k].p[j + 1];
            edge e0(u, v);
            e0.Delta = CalcDelta(u,v);
            set<edge>::iterator ite = Edge.find(e0);
            if(ite == Edge.end())
            {
                e0.f1 = k;
                Edge.insert(e0);
            }
            else
            {
                edge tmp = *(ite);
                e0.f1 = tmp.f1;
                e0.f2 = k;
                Edge.erase(ite);
                Edge.insert(e0);
            }
        }
    }
}

void SetOutput(char* filename)
{
    ofstream fout(filename, ios::app);
    for(int i = 1; i < Point.size(); i++)
    {
        if(Point[i].exist)
        {
            fout << "v " << Point[i].x << ' ' << Point[i].y << ' ' << Point[i].z << endl;
            RealPoint[i] = cnt;
            cnt++;
        }
    }
    for(int k = 1; k < Face.size(); k++)
        if(Face[k].exist)
            fout << "f " << RealPoint[Face[k].p[0]] << ' ' << RealPoint[Face[k].p[1]] << ' ' << RealPoint[Face[k].p[2]] << endl;

    fout.close();
}

void CalcKp(int ff)
{
    int p0 = Face[ff].p[0];
    int p1 = Face[ff].p[1];
    int p2 = Face[ff].p[2];
    double x1 = Point[p0].x;
    double x2 = Point[p1].x;
    double x3 = Point[p2].x;
    double y1 = Point[p0].y;
    double y2 = Point[p1].y;
    double y3 = Point[p2].y;
    double z1 = Point[p0].z;
    double z2 = Point[p1].z;
    double z3 = Point[p2].z;
    double abcd[4];
    double a = y1*z2 - y1*z3 - y2*z1 + y2*z3 + y3*z1 - y3*z2;
    double b = -x1*z2 + x1*z3 + x2*z1 - x2*z3 - x3*z1 + x3*z2;
    double c = x1*y2 - x1*y3 - x2*y1 + x2*y3 + x3*y1 - x3*y2;
    double t = sqrt(a*a + b*b + c*c);
    a /= t;
    b /= t;
    c /= t;
    double d = - (a*x1 + b*y1 + c*z1);
    abcd[0] = a;
    abcd[1] = b;
    abcd[2] = c;
    abcd[3] = d;

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            Face[ff].Kp[i][j] = abcd[i]*abcd[j];


}

void CalcQ(int u)
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            Point[u].Q[i][j] = 0;
    set<int>::iterator it;
    for(it = Point[u].neighborFace.begin(); it != Point[u].neighborFace.end(); it++)
    {
        int k = *(it);
        if(Face[k].exist)
            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 4; j++)
                    Point[u].Q[i][j] += Face[k].Kp[i][j];
    }
}

double CalcDelta(int v1, int v2)
{
    double Q12[4][4];

    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
        {
            Q12[i][j] = Point[v1].Q[i][j] + Point[v2].Q[i][j];
        }
    double V[4];
    V[0] = (Point[v1].x + Point[v2].x)/2;
    V[1] = (Point[v1].y + Point[v2].y)/2;
    V[2] = (Point[v1].z + Point[v2].z)/2;
    V[3] = 1;

    double temp[4];
    for(int i=0;i<4;i++)
        temp[i] = 0;
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            temp[i] += V[j]*Q12[j][i];

    double ans = 0;
    for(int i=0;i<4;i++)
        ans += temp[i]*V[i];

    return ans;
}
