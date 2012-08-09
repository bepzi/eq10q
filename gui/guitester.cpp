#include "guitester.h"
#include "guiconstants.h"
#include <gtkmm/main.h>
#include <iostream>
#include <cmath>

//Build
//g++ -g guitester.cpp eqbutton.cpp ctlbutton.cpp pixmapcombo.cpp bandctl.cpp gainctl.cpp eqparams.cpp faderwidget.cpp vuwidget.cpp ../eqwindow.cpp ../dsp/filter.c -o test `pkg-config gtkmm-2.4 slv2 --libs --cflags`


HelloWorld::HelloWorld()
{

  m_EqWin = Gtk::manage(new EqMainWindow(2, 10,"http://eq10q.sourceforge.net/eq/eq10qs"));
  add(*m_EqWin);
  m_EqWin->show();
  
  //Signals connections
  m_EqWin->signal_Bypass_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BypassChanged));
  m_EqWin->signal_InputGain_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_InputGainChanged));
  m_EqWin->signal_OutputGain_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_OutputGainChanged));
  
  m_EqWin->signal_BandGain_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandGainChanged));
  m_EqWin->signal_BandFreq_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandFreqChanged));
  m_EqWin->signal_BandQ_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandQChanged));
  m_EqWin->signal_BandType_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandTypeChanged));
  m_EqWin->signal_BandEnabled_Changed().connect(sigc::mem_fun(*this, &HelloWorld::on_BandEnabledChanged));
}

HelloWorld::~HelloWorld()
{
 delete m_EqWin;
}

//Slots signal handlers
void HelloWorld::on_BypassChanged(bool bypass)
{
  std::cout<<"Bypass = "<<bypass<<std::endl;
}

void HelloWorld::on_InputGainChanged(float gain)
{
  std::cout<<"In Gain = "<<gain<<std::endl;
}

void HelloWorld::on_OutputGainChanged(float gain)
{
  std::cout<<"Out Gain = "<<gain<<std::endl;
}

void HelloWorld::on_BandGainChanged(int band, float gain)
{
  std::cout<<"Band "<<band<<" Gain = "<<gain<<std::endl;
}

void HelloWorld::on_BandFreqChanged(int band, float freq)
{
  std::cout<<"Band "<<band<<" Freq = "<<freq<<std::endl;
}

void HelloWorld::on_BandQChanged(int band, float q)
{
  std::cout<<"Band "<<band<<" Q = "<<q<<std::endl; 
}

void HelloWorld::on_BandTypeChanged(int band, int type)
{
  std::cout<<"Band "<<band<<" Type = "<<type<<std::endl;  
}

void HelloWorld::on_BandEnabledChanged(int band, bool enabled)
{
  std::cout<<"Band "<<band<<" Enabled = "<<enabled<<std::endl;  
}

int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  HelloWorld helloworld;
  //Shows the window and returns when it is closed.
  Gtk::Main::run(helloworld);

  return 0;
}


