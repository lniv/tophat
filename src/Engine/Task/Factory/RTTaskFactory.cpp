/* Copyright_License {

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

#include "RTTaskFactory.hpp"
#include "TaskFactoryConstraints.hpp"
#include "Util/Macros.hpp"

static constexpr TaskFactoryConstraints rt_constraints = {
  true,
  false,
  false,
  false,
  false,
  2, 13,
};

static constexpr TaskPointFactoryType rt_start_types[] = {
  TaskPointFactoryType::START_LINE,
  TaskPointFactoryType::START_CYLINDER,
  TaskPointFactoryType::START_SECTOR,
  TaskPointFactoryType::START_BGA,
};

static constexpr TaskPointFactoryType rt_im_types[] = {
  TaskPointFactoryType::AST_CYLINDER,
  TaskPointFactoryType::KEYHOLE_SECTOR,
  TaskPointFactoryType::BGAFIXEDCOURSE_SECTOR,
  TaskPointFactoryType::BGAENHANCEDOPTION_SECTOR,
  TaskPointFactoryType::FAI_SECTOR,
};

static constexpr TaskPointFactoryType rt_finish_types[] = {
  TaskPointFactoryType::FINISH_LINE,
  TaskPointFactoryType::FINISH_CYLINDER,
  TaskPointFactoryType::FINISH_SECTOR,
};

/**
 * US flavors of the valid point types
 */
static constexpr TaskPointFactoryType rt_start_types_us[] = {
  TaskPointFactoryType::START_CYLINDER,
};

static constexpr TaskPointFactoryType rt_im_types_us[] = {
  TaskPointFactoryType::AST_CYLINDER,
};

static constexpr TaskPointFactoryType rt_finish_types_us[] = {
  TaskPointFactoryType::FINISH_LINE,
  TaskPointFactoryType::FINISH_CYLINDER,
};

RTTaskFactory::RTTaskFactory(OrderedTask& _task,
                               const TaskBehaviour &tb)
  :AbstractTaskFactory(rt_constraints, _task, tb,
                       LegalPointConstArray(rt_start_types,
                                            ARRAY_SIZE(rt_start_types)),
                       LegalPointConstArray(rt_im_types,
                                            ARRAY_SIZE(rt_im_types)),
                       LegalPointConstArray(rt_finish_types,
                                            ARRAY_SIZE(rt_finish_types)))
{
}

RTTaskFactory::RTTaskFactory(OrderedTask &_task, const TaskBehaviour &_behaviour,
                             const LegalPointConstArray _start_types,
                             const LegalPointConstArray _intermediate_types,
                             const LegalPointConstArray _finish_types)
  :AbstractTaskFactory(rt_constraints, _task, _behaviour,
                       _start_types,
                       _intermediate_types,
                       _finish_types)
{
}


bool 
RTTaskFactory::Validate()
{
  bool valid = AbstractTaskFactory::Validate();

  return valid;
}

TaskPointFactoryType
RTTaskFactory::GetMutatedPointType(const OrderedTaskPoint &tp) const
{
  const TaskPointFactoryType oldtype = GetType(tp);
  TaskPointFactoryType newtype = oldtype;

  switch (oldtype) {
  case TaskPointFactoryType::START_SECTOR:
  case TaskPointFactoryType::START_LINE:
  case TaskPointFactoryType::START_CYLINDER:
  case TaskPointFactoryType::START_BGA:
    break;

  case TaskPointFactoryType::KEYHOLE_SECTOR:
  case TaskPointFactoryType::BGAFIXEDCOURSE_SECTOR:
  case TaskPointFactoryType::BGAENHANCEDOPTION_SECTOR:
  case TaskPointFactoryType::FAI_SECTOR:
  case TaskPointFactoryType::AST_CYLINDER:
    break;

  case TaskPointFactoryType::FINISH_SECTOR:
  case TaskPointFactoryType::FINISH_LINE:
  case TaskPointFactoryType::FINISH_CYLINDER:
    break;

  case TaskPointFactoryType::AAT_SEGMENT:
  case TaskPointFactoryType::AAT_ANNULAR_SECTOR:
  case TaskPointFactoryType::MAT_CYLINDER:
  case TaskPointFactoryType::AAT_CYLINDER:
    newtype = TaskPointFactoryType::AST_CYLINDER;
    break;
  }

  return newtype;
}

RTTaskFactoryUs::RTTaskFactoryUs(OrderedTask& _task, const TaskBehaviour &tb)
  :RTTaskFactory(_task, tb,
                 LegalPointConstArray(rt_start_types_us,
                                      ARRAY_SIZE(rt_start_types_us)),
                 LegalPointConstArray(rt_im_types_us,
                                      ARRAY_SIZE(rt_im_types_us)),
                 LegalPointConstArray(rt_finish_types_us,
                                      ARRAY_SIZE(rt_finish_types_us)))
{
}

TaskPointFactoryType
RTTaskFactoryUs::GetMutatedPointType(const OrderedTaskPoint &tp) const
{
  const TaskPointFactoryType oldtype = GetType(tp);
  TaskPointFactoryType newtype = oldtype;

  switch (oldtype) {
  case TaskPointFactoryType::START_CYLINDER:
    break;

  case TaskPointFactoryType::START_LINE:
  case TaskPointFactoryType::START_SECTOR:
  case TaskPointFactoryType::START_BGA:
    newtype = TaskPointFactoryType::START_CYLINDER;
    break;

  case TaskPointFactoryType::AST_CYLINDER:
    break;

  case TaskPointFactoryType::KEYHOLE_SECTOR:
  case TaskPointFactoryType::BGAFIXEDCOURSE_SECTOR:
  case TaskPointFactoryType::BGAENHANCEDOPTION_SECTOR:
  case TaskPointFactoryType::FAI_SECTOR:
  newtype = TaskPointFactoryType::AST_CYLINDER;
    break;

  case TaskPointFactoryType::FINISH_CYLINDER:
  case TaskPointFactoryType::FINISH_LINE:
    break;

  case TaskPointFactoryType::FINISH_SECTOR:
    newtype = TaskPointFactoryType::FINISH_CYLINDER;
    break;

  case TaskPointFactoryType::AAT_SEGMENT:
  case TaskPointFactoryType::AAT_ANNULAR_SECTOR:
  case TaskPointFactoryType::MAT_CYLINDER:
  case TaskPointFactoryType::AAT_CYLINDER:
    newtype = TaskPointFactoryType::AST_CYLINDER;
    break;
  }

  return newtype;
}
