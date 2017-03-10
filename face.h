#ifndef FACE_H
#define FACE_H


class face
{
public:
    int p[3];
    double a,b,c,d;
    bool exist;
    double Kp[4][4];
    face();
    ~face();


};

#endif // FACE_H
