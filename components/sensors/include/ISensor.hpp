#pragma once


class ISensor 
{
private:

public:
    virtual ~ISensor() = default ;

    virtual bool turnOn() = 0 ;
    virtual bool turnOff() = 0 ;
    virtual const void* getSample()const  = 0 ;
    virtual bool stopListening_IT()  = 0   ;

    virtual bool listen_IT() = 0   ;


};


