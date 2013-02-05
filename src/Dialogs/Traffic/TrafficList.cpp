/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2013 The XCSoar Project
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

#include "TrafficDialogs.hpp"
#include "Dialogs/WidgetDialog.hpp"
#include "Widget/ListWidget.hpp"
#include "Widget/TwoWidgets.hpp"
#include "Widget/RowFormWidget.hpp"
#include "Screen/Canvas.hpp"
#include "Screen/Layout.hpp"
#include "Form/DataField/Prefix.hpp"
#include "Form/DataField/Listener.hpp"
#include "FLARM/FlarmNet.hpp"
#include "FLARM/FlarmNetRecord.hpp"
#include "FLARM/FlarmDetails.hpp"
#include "FLARM/FlarmId.hpp"
#include "Util/StaticString.hpp"
#include "Language/Language.hpp"
#include "UIGlobals.hpp"
#include "Look/DialogLook.hpp"

enum Controls {
  CALLSIGN,
};

enum Buttons {
  DETAILS,
};

class TrafficListWidget : public ListWidget, public DataFieldListener,
                          public ActionListener {
  struct Item {
    FlarmId id;

    /**
     * Were the attributes below already lazy-loaded from the
     * database?  We can't use nullptr for this, because both will be
     * nullptr after a failed lookup.
     */
    bool loaded;

    const FlarmNetRecord *record;
    const TCHAR *callsign;

    explicit Item(FlarmId _id):id(_id), loaded(false) {
      assert(id.IsDefined());
    }

    void Load() {
      assert(id.IsDefined());

      record = FlarmNet::FindRecordById(id);
      callsign = FlarmDetails::LookupCallsign(id);

      loaded = true;
    }

    void AutoLoad() {
      if (!loaded)
        Load();
    }
  };

  ActionListener *action_listener;

  const RowFormWidget *filter_widget;

  std::vector<Item> items;

public:
  TrafficListWidget(ActionListener &_action_listener,
                    const FlarmId *array, size_t count)
    :action_listener(&_action_listener), filter_widget(nullptr) {
    items.reserve(count);

    for (unsigned i = 0; i < count; ++i)
      items.emplace_back(array[i]);
  }

  TrafficListWidget(const RowFormWidget &_filter_widget)
    :action_listener(nullptr), filter_widget(&_filter_widget) {
  }

  FlarmId GetCursorId() const {
    return items.empty()
      ? FlarmId::Undefined()
      : items[GetList().GetCursorIndex()].id;
  }

  void UpdateList();

  /* virtual methods from class Widget */

  virtual void Prepare(ContainerWindow &parent,
                       const PixelRect &rc) override;
  virtual void Unprepare() override {
    DeleteWindow();
  }

  /* virtual methods from ListItemRenderer */
  virtual void OnPaintItem(Canvas &canvas, const PixelRect rc,
                           unsigned idx) override;

  /* virtual methods from ListCursorHandler */
  virtual bool CanActivateItem(unsigned index) const override {
    return true;
  }

  virtual void OnActivateItem(unsigned index) override;

  /* virtual methods from DataFieldListener */
  virtual void OnModified(DataField &df) override {
    UpdateList();
  }

  /* virtual methods from ActionListener */
  virtual void OnAction(int id) override;
};

class TrafficFilterWidget : public RowFormWidget {
  DataFieldListener *listener;

public:
  TrafficFilterWidget(const DialogLook &look)
    :RowFormWidget(look, true) {}

  void SetListener(DataFieldListener *_listener) {
    listener = _listener;
  }

  virtual void Prepare(ContainerWindow &parent,
                       const PixelRect &rc) override {
    PrefixDataField *callsign_df = new PrefixDataField();
    callsign_df->SetListener(listener);
    Add(_("Competition ID"), nullptr, callsign_df);
  }
};

class TrafficListButtons : public RowFormWidget {
  ActionListener &dialog;
  ActionListener *list;

public:
  TrafficListButtons(const DialogLook &look, ActionListener &_dialog)
    :RowFormWidget(look), dialog(_dialog) {}

  void SetList(ActionListener *_list) {
    list = _list;
  }

  virtual void Prepare(ContainerWindow &parent,
                       const PixelRect &rc) override {
    AddButton(_("Details"), *list, DETAILS);
    AddButton(_("Cancel"), dialog, mrCancel);
  }
};

gcc_pure
static UPixelScalar
GetRowHeight(const DialogLook &look)
{
  return look.list.font_bold->GetHeight() + Layout::Scale(6)
    + look.small_font->GetHeight();
}

void
TrafficListWidget::UpdateList()
{
  assert(filter_widget != nullptr);

  items.clear();

  const TCHAR *callsign = filter_widget->GetValueString(CALLSIGN);
  if (!StringIsEmpty(callsign)) {
    FlarmId ids[30];
    unsigned count = FlarmDetails::FindIdsByCallSign(callsign, ids, 30);

    for (unsigned i = 0; i < count; ++i)
      items.emplace_back(ids[i]);
  }

  GetList().SetLength(items.size());
}

void
TrafficListWidget::Prepare(ContainerWindow &parent,
                           const PixelRect &rc)
{
  const DialogLook &look = UIGlobals::GetDialogLook();
  ListControl &list = CreateList(parent, look, rc,
                                 GetRowHeight(look));

  if (filter_widget != nullptr)
    UpdateList();
  else
    list.SetLength(items.size());
}

void
TrafficListWidget::OnPaintItem(Canvas &canvas, const PixelRect rc,
                               unsigned index)
{
  assert(index < items.size());
  Item &item = items[index];
  assert(item.id.IsDefined());

  item.AutoLoad();

  const FlarmNetRecord *record = item.record;
  const TCHAR *callsign = item.callsign;

  const DialogLook &look = UIGlobals::GetDialogLook();
  const Font &name_font = *look.list.font_bold;
  const Font &small_font = *look.small_font;

  TCHAR tmp_id[10];
  item.id.Format(tmp_id);

  canvas.Select(name_font);

  StaticString<256> tmp;
  if (record != NULL)
    tmp.Format(_T("%s - %s - %s"),
               callsign, record->registration.c_str(), tmp_id);
  else if (callsign != NULL)
    tmp.Format(_T("%s - %s"), callsign, tmp_id);
  else
    tmp.Format(_T("%s"), tmp_id);

  canvas.DrawText(rc.left + Layout::FastScale(2),
                  rc.top + Layout::FastScale(2), tmp);

  if (record != NULL) {
    tmp.clear();

    if (!record->pilot.empty())
      tmp = record->pilot.c_str();

    if (!record->plane_type.empty()) {
      if (!tmp.empty())
        tmp.append(_T(" - "));

      tmp.append(record->plane_type);
    }

    if (!record->airfield.empty()) {
      if (!tmp.empty())
        tmp.append(_T(" - "));

      tmp.append(record->airfield);
    }

    if (!tmp.empty()) {
      canvas.Select(small_font);
      canvas.DrawText(rc.left + Layout::FastScale(2),
                      rc.top + name_font.GetHeight() + Layout::FastScale(4),
                      tmp);
    }
  }
}

void
TrafficListWidget::OnActivateItem(unsigned index)
{
  if (action_listener != nullptr)
    action_listener->OnAction(mrOK);
  else
    dlgFlarmTrafficDetailsShowModal(GetCursorId());
}

void
TrafficListWidget::OnAction(int id)
{
  switch (Buttons(id)) {
  case DETAILS:
    dlgFlarmTrafficDetailsShowModal(GetCursorId());
    break;
  }
}

void
TrafficListDialog()
{
  const DialogLook &look = UIGlobals::GetDialogLook();
  WidgetDialog dialog(look);

  TrafficFilterWidget *filter_widget = new TrafficFilterWidget(look);

  TrafficListButtons *buttons_widget = new TrafficListButtons(look, dialog);

  TwoWidgets *left_widget =
    new TwoWidgets(filter_widget, buttons_widget, true);

  TrafficListWidget *const list_widget =
    new TrafficListWidget(*filter_widget);

  filter_widget->SetListener(list_widget);
  buttons_widget->SetList(list_widget);

  TwoWidgets *widget = new TwoWidgets(left_widget, list_widget, false);

  dialog.CreateFull(UIGlobals::GetMainWindow(), _("Traffic"), widget);
  dialog.ShowModal();
}

FlarmId
PickFlarmTraffic(const TCHAR *title, FlarmId array[], unsigned count)
{
  assert(count > 0);

  WidgetDialog dialog(UIGlobals::GetDialogLook());

  TrafficListWidget *const list_widget =
    new TrafficListWidget(dialog, array, count);

  Widget *widget = list_widget;

  dialog.CreateFull(UIGlobals::GetMainWindow(), title, widget);
  dialog.AddButton(_("Select"), mrOK);
  dialog.AddButton(_("Cancel"), mrCancel);

  return dialog.ShowModal() == mrOK
    ? list_widget->GetCursorId()
    : FlarmId::Undefined();
}