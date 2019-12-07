#  EQ10Q Parametric Equalizer (LV2 Plugin)

EQ10Q is an audio plugin over the LV2 standard (http://lv2plug.in)
implementing a powerful and flexible parametric equalizer with
diferent filter types like peaking, HPF, LPF, Shelving and Notch with
IIR algorithms and a nice GUI displaying the equalization curve.

At the moment we have implemented second order biquadratic filters
(Peaking, Low Shelving, High Shelving and Notch), and configurable
order (1, 2, 3, 4) HPF and LPF filters, all with IIR algorithms, and a
GUI written with Gtkmm (http://www.gtkmm.org) to control all the
parameters and display the curve using [plotmm](http://plotmm.sourceforge.net). 
The plugin uses the LV2 standard and is written in C for the audio DSP and 
C++ for the GUI.

The original EQ10Q plugin is hosted at http://eq10q.sourceforge.net/, 
and its developer is Pere Ràfols Soler (sapista2@gmail.com). 

## Installation

EQ10Q is built with CMake and additionally requires 
[the LV2 headers](http://lv2plug.in/), [gtkmm-2.4](https://www.gtkmm.org/en/), 
and [FFTW3](http://fftw.org/).

To install EQ10Q:

``` bash
mkdir build/
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ..
sudo make install
```

To run EQ10Q you need an LV2 host, such as [Ardour](http://ardour.org).

## Uninstallation

Remove the EQ10Q directory, by default `/usr/lib/lv2/sapistaEQv2.lv2`

## Copyright

```
Copyright (C) 2009 by Pere Ràfols Soler
sapista2@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
```
