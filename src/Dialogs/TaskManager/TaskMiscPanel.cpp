/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "TaskMiscPanel.hpp"
#include "TaskActionsPanel.hpp"
#include "TaskListPanel.hpp"

TaskMiscPanel::TaskMiscPanel(TabBarControl &_tab_bar,
                             OrderedTask **_active_task, bool *_task_modified)
{
  TaskActionsPanel *actions_panel =
    new TaskActionsPanel(*this, _tab_bar, _active_task, _task_modified);
  Add(actions_panel);

  TaskListPanel *list_panel =
    new TaskListPanel(&_tab_bar, _active_task, _task_modified);
  Add(list_panel);

  actions_panel->SetListPanel(list_panel);
}

void
TaskMiscPanel::SetTaskView(WndOwnerDrawFrame *_task_view)
{
  TaskActionsPanel &actions_panel = (TaskActionsPanel &)GetWidget(0);
  actions_panel.SetTaskView(_task_view);

  TaskListPanel &list_panel = (TaskListPanel &)GetWidget(1);
  list_panel.SetTaskView(_task_view);
}

void
TaskMiscPanel::ReClick()
{
  if (GetCurrentIndex() > 0)
    SetCurrent(0);
  else
    PagerWidget::ReClick();
}

void
TaskMiscPanel::Show(const PixelRect &rc)
{
  SetCurrent(0);
  PagerWidget::Show(rc);
}