/* gtkmm example Copyright (C) 2002 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "treeview_withpopup.h"
#include <iostream>

TreeView_WithPopup::TreeView_WithPopup()
{
  dbInstance = Db::getInstance();
  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  set_model(m_refTreeModel);

  //Add the TreeView's view columns:
  append_column("ID", m_Columns.m_col_id);
  append_column("Name", m_Columns.m_col_name);

  //Fill popup menu:

  auto item = Gtk::manage(new Gtk::MenuItem("_Remove", true));
  item->signal_activate().connect(
      sigc::mem_fun(*this, &TreeView_WithPopup::on_menu_file_popup_generic));
  m_Menu_Popup.append(*item);

  m_Menu_Popup.accelerate(*this);
  m_Menu_Popup.show_all(); //Show all menu items when the menu pops up
  update_tree_view();
}

TreeView_WithPopup::~TreeView_WithPopup()
{
}

bool TreeView_WithPopup::on_button_press_event(GdkEventButton *button_event)
{
  bool return_value = false;

  //Call base class, to allow normal handling,
  //such as allowing the row to be selected by the right-click:
  return_value = TreeView::on_button_press_event(button_event);

  //Then do our custom stuff:
  if ((button_event->type == GDK_BUTTON_PRESS) && (button_event->button == 3))
  {
    m_Menu_Popup.popup_at_pointer((GdkEvent *)button_event);

    // Menu::popup_at_pointer() is new in gtkmm 3.22.
    // If you have an older revision, try this:
    //m_Menu_Popup.popup(button_event->button, button_event->time);
  }

  return return_value;
}

void TreeView_WithPopup::on_menu_file_popup_generic()
{

  auto refSelection = get_selection();
  if (refSelection)
  {
    Gtk::TreeModel::iterator iter = refSelection->get_selected();
    if (iter)
    {
      int id = (*iter)[m_Columns.m_col_id];
      Db::getInstance()->delete_selected_city(id);
      update_tree_view();
    }
  }
}

void TreeView_WithPopup::update_tree_view()
{
  Gtk::TreeModel::Row row;
  m_refTreeModel->clear();
  std::map<std::string, std::string> selectedCities =
      dbInstance->select_picked_cities_from_db(
          "Selected_Cities", "", "");
  for (const auto &pairs : selectedCities)
  {
    row = *(m_refTreeModel->append());
    row[m_Columns.m_col_id] = std::stoi(pairs.first);
    row[m_Columns.m_col_name] = pairs.second;
  }
}
