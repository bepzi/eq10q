link_libraries(m)

add_library(eqwin STATIC
        mainwidget.cpp bandctl.cpp eqparams.cpp knob2.cpp
        vuwidget.cpp eqwindow.cpp filter.cpp setwidgetcolors.cpp
        bodeplot.cpp button.cpp toggle_button.cpp abbutton.cpp sidechainbox.cpp)

add_library(dynamicwin STATIC mainwidget.cpp vuwidget.cpp knob2.cpp dynamicswindow.cpp setwidgetcolors.cpp
        button.cpp toggle_button.cpp dynplot.cpp sidechainbox.cpp)

add_library(bassupwin STATIC mainwidget.cpp knob2.cpp bassupwindow.cpp)

add_library(midsidewin STATIC mainwidget.cpp vuwidget.cpp knob2.cpp midsidewindow.cpp
        setwidgetcolors.cpp button.cpp toggle_button.cpp sidechainbox.cpp)
