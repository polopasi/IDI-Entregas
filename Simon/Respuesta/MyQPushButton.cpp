#include "MyQPushButton.h"
#include <iostream>


MyQPushButton::MyQPushButton (QWidget* parent) : QPushButton (parent)
{

}

void MyQPushButton::cambiaColor (int color)
{
    std::string s = "background-color: rgb(" + std::to_string((color & 0xff0000)>>16) + ","
                                             + std::to_string((color & 0x00ff00)>>8) + ","
                                             + std::to_string(color & 0x0000ff) + ","
                                             + ")";
    this->setStyleSheet(QString::fromStdString(s));
}

void MyQPushButton::desactivar () 
{
    this->setEnabled (false);
}


void MyQPushButton::activar () 
{
    this->setEnabled (true);
}
