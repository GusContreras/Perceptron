#include "perceptron.h"
#include "point.h"
#include <QDebug>
#include <QtMath>

const double euler = 2.71828182845;

Perceptron::Perceptron()
{
    w1 = Gen.bounded(1.0) - 1.0;
    w2 = Gen.bounded(1.0) - 1.0;
    wb = Gen.bounded(1.0) - 1.0;
}

int Perceptron::functionZ(double x1, double x2)
{
    double test = (w1*x1) + (w2*x2) - wb;
    if(test > 0.0){
        return 1;
    }
    return 0;

}

void Perceptron::rand(){
    w1 = Gen.bounded(1.0) - 1.0;
    w2 = Gen.bounded(1.0) - 1.0;
    wb = Gen.bounded(1.0) - 1.0;
}

void Perceptron::update(Point i,double factor,int error){
    w1 = w1 + (i.x * factor * error);
    w2 = w2 + (i.y * factor * error);
    wb = wb + (-1 * factor * error);
    qDebug() << w1  << " " << w2 << " " << wb;
}

void Perceptron::updateA(Point i, double factor, double error, double s)
{
    s = s * (1 - s);
    w1 = w1 + (i.x * factor * error * s);
    w2 = w2 + (i.y * factor * error * s);
    wb = wb + ( -1 * factor * error * s);
}

double Perceptron::functionS(double x1, double x2)
{
    double z = (w1*x1) + (w2*x2) - wb;
    return (1.0/(1.0 + qPow(euler,z*(-1.0))));
}
