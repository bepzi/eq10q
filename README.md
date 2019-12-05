#  EQ10Q LV2 Plugin

EQ10Q is an audio plugin over the LV2 standard (http://lv2plug.in)
implementing a powerful and flexible parametric equalizer with
diferent filter types like peaking, HPF, LPF, Shelving and Notch with
IIR algorithms and a nice GUI displaying the equalization curve.

At the moment we have implemented second order biquadratic filters
(Peaking, Low Shelving, High Shelving and Notch), and configurable
order (1, 2, 3, 4) HPF and LPF filters, all with IIR algorithms, and a
GUI written with Gtkmm (http://www.gtkmm.org) to control all the
parameters and display the curve using plotmm
(http://plotmm.sourceforge.net).  The plugin uses the LV2 standard and
is written in C for the audio DSP and C++ for the GUI.

EQ10Q is hosted at http://eq10q.sourceforge.net/ Main developer: Pere
Ràfols Soler (sapista2@gmail.com)

## Installation

Before installing EQ10Q you must resolve all required libraries:
  - G++ compiler
  - cmake
  - pkg-config
  - LV2 Headers
  - Gtkmm >= 2.4
  - FFTW3

To install EQ10Q in your system open a terminal, go to EQ10Q top level
folder:

``` bash
mkdir build/
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release ..
sudo make install
```

To run EQ10Q you need a LV2 host, such as Ardour: http://ardour.org

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
