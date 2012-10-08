/***************************************************************************
 *   Copyright (C) 2009 by Pere Ràfols Soler                               *
 *   sapista2@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "pixmapcombo.h"
#include <cstring>

#include "setwidgetcolors.h"


PixMapCombo::PixMapCombo(const char *bundlePath)
:m_bundlePath(bundlePath)
{
  //Create the tree model
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  set_model(m_refTreeModel);
  
  Glib::RefPtr<Gdk::Pixbuf> m_refTopixbuf;
  Gtk::TreeModel::Row row;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_LPF1));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_LPF2));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_LPF3));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_LPF4));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_HPF1));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_HPF2));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_HPF3));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_HPF4));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;

  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_LOSHELF));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_HISHELF));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_PEAK));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;
  
  //Enganxar una imatge al pixbuf
  m_refTopixbuf= Gdk::Pixbuf::create_from_file(m_bundlePath +"/" + std::string(RUTA_NOTCH));
  
  //Fill the ComboBox Tree Model
  row = *(m_refTreeModel->append());
  row[m_Columns.m_col_pix] = m_refTopixbuf;  
  
   //add to combo
   pack_start(m_Columns.m_col_pix);
   
   //Setup style
   ///TODO SetWidgetColors myColors; Remve this and also the include
   /*
   get_toplevel()->set_style(myColors.getPlainButtonStyle());
   get_toplevel()->modify_bg(Gtk::STATE_INSENSITIVE, Gdk::Color::Color("#FF00F7"));
   get_toplevel()->modify_bg(Gtk::STATE_ACTIVE, Gdk::Color::Color("#FF00F7"));
   get_toplevel()->modify_bg(Gtk::STATE_NORMAL, Gdk::Color::Color("#FF00F7"));
   get_toplevel()->modify_bg(Gtk::STATE_PRELIGHT, Gdk::Color::Color("#FF00F7"));
   get_toplevel()->modify_bg(Gtk::STATE_SELECTED, Gdk::Color::Color("#FF00F7"));*/
   
   
   Glib::RefPtr<Gtk::Style> comboStyle = get_style();
   comboStyle->set_bg(Gtk::STATE_INSENSITIVE, Gdk::Color::Color("#FF00F7"));
   comboStyle->set_bg(Gtk::STATE_ACTIVE, Gdk::Color::Color("#FF00F7"));
   comboStyle->set_bg(Gtk::STATE_NORMAL, Gdk::Color::Color("#FF00F7"));
   comboStyle->set_bg(Gtk::STATE_PRELIGHT, Gdk::Color::Color("#FF00F7"));
   comboStyle->set_bg(Gtk::STATE_SELECTED, Gdk::Color::Color("#FF00F7"));
   set_style(comboStyle);
   
   /*
   modify_fg(Gtk::STATE_INSENSITIVE, Gdk::Color::Color("#FF00F7"));
   modify_fg(Gtk::STATE_ACTIVE, Gdk::Color::Color("#FF00F7"));
   modify_fg(Gtk::STATE_NORMAL, Gdk::Color::Color("#FF00F7"));
   modify_fg(Gtk::STATE_PRELIGHT, Gdk::Color::Color("#FF00F7"));
   modify_fg(Gtk::STATE_SELECTED, Gdk::Color::Color("#FF00F7"));
   */
      
   show_all_children();
}

PixMapCombo::~PixMapCombo(){

}