#ifndef CDEFAULT_UI_H_
#define CDEFAULT_UI_H_

#include "cdefault_std.h"
#include "cdefault_math.h"
#include "cdefault_io.h"

// TODO: for widget caching, use a hash map instead of referencing the old frame's tree
// TODO: text wrapping
// TODO: more std widgets
// TODO: builtin render & text measure if asked, using cdefault renderer & font libs
// TODO: dirty flag on widgets? only recalculate size & pos of self and children if dirty is true

// NOTE: Basic immediate-mode UI library, a-la Dear ImGui, Clay, nuklear, etc. Allows you to define UIs in a procedural,
// easy to compose directly in C way. Mainly intended for quick debug views to investigate / modify data in realtime.
// Supports features like dynamic layout schemes (fit, grow, exact), various widgets (text, button, graphs, etc.),
// window scrolling & resizing, on hover & on click animations, etc. Designed to be decoupled from most cdefault modules.
//
// Example:
#if 0
UiBegin(dt_s);
  if (UiWindowFloatingBegin(UIID(), Str8Lit("my window"), V2Assign(10, 10), V2Assign(0, 0), true).open) {
    UiPanelHorizontalBegin(UIID(), V2_ZEROES, 0);
      UiText(UIID(), Str8Lit("my big beautiful button:"), V2_ZEROES);
      UiGrow(UIID());
      if (UiButton(UIID(), Str8Lit("click me"), V2Assign(200, 200)).clicked) {
        LOG_INFO("the button was clicked!");
      }
    UiPanelEnd();
  UiWindowEnd();
UiDrawCommand* commands = UiEnd();
#endif

// NOTE: The UI system keeps track of widget state, for position, bounds checking, animations, etc. To do
// this between frames, some sort of identifier is required to uniquely identify each widget. These macros
// can be used to generate these ids.
#define UIID()      (UIID_IMPL("")) // Default option.
#define UIID_STR(s) (UIID_IMPL(s))  // If you want to name your widgets, this is useful.
#define UIID_INT(i) (UIID() + i)    // Useful if defining widgets in reusable function, or in a loop.

// NOTE: This struct is how you can respond to different events for each widget.
typedef struct UiInteraction UiInteraction;
struct UiInteraction {
  B8  is_new;
  B8  hovered;
  B8  clicked;
  B8  right_clicked;
  B8  dragged;
  B8  open;
  U32 selected;
};

// NOTE: Create & pass this struct to modify the colors, etc. of various UI components.
typedef struct UiStyle UiStyle;
struct UiStyle {
  F32 anim_max_time; // NOTE: set to 0 to disable animations.
  F32 border_size;
  V3  border_color;
  V3  background_color;
  V3  text_color;
  V3  graph_color;
  V3  widget_color;
  V3  widget_color_bright;
  V3  widget_color_active;
};

// NOTE: this library does not render widgets by default, rather it gives you this, a linked list of
// commands, for you to do it yourself (so that it can be divorced of a particular rendering backend).
typedef struct UiDrawCommand UiDrawCommand;
struct UiDrawCommand {
  UiDrawCommand* prev;
  UiDrawCommand* next;

  enum {
    UiDrawCommand_Line,
    UiDrawCommand_Rect,
    UiDrawCommand_Text,
    UiDrawCommand_ScissorEnable,
    UiDrawCommand_ScissorDisable,
  } type;

  union {
    struct {
      V2 start;
      V2 end;
      V3 color;
    } line;
    struct {
      V2 center;
      V2 size;
      V3 color;
    } rect;
    struct {
      V2 pos;
      V3 color;
      String8 string;
    } text;
    struct {
      V2 min;
      V2 max;
    } scissor_enable;
  };
};

// NOTE: Callbacks necessary to measure and place text.
typedef void UiFontMeasureText_Fn(void* user_data, String8 text, V2* size);
typedef void UiFontGetAttributes_Fn(void* user_data, F32* descent);

// NOTE: Initialize / deinitialize the framework.
void UiInit(); // NOTE: must be called before any other function.
void UiDeinit();

// NOTE: Query / update internal UI framework state.
void UiSetPointerState(V2 mouse_pos, F32 mouse_scroll, B32 is_lmb_down, B32 is_rmb_down); // NOTE: pass mouse data into the UI framework. this is required for UiInteractions to contain expected data.
void UiSetFontUserData(void* user_data); // NOTE: sets data that is transparently passed to the font callbacks later.
void UiSetFontCallbacks(UiFontMeasureText_Fn* ui_font_measure_text_fn, UiFontGetAttributes_Fn* ui_font_get_attributes_fn); // NOTE: required for text to be placed & measured properly.
UiStyle* UiGetStyle(); // NOTE: get / update the style (colors, etc.) of UI widgets.
B32 UiContainsMouse(); // NOTE: e.g. to determine if a click should potentially be delegated to the UI system or some other system.

// NOTE: begin and end calls for determining UI layout position, sizes, etc.
void UiBegin(F32 dt_s); // NOTE: Must be called before any widgets are placed. Delta time is passed for animations.
UiDrawCommand* UiEnd(); // NOTE: returns a doubly-linked list of instructions for how to render the UI (which can be processed in-order).

// NOTE: Widgets with a size attribute can be set to 0 for default behavior (fit or grow depending on the widget).

// NOTE: Windows. Primarily how you specify the location of a container / group of UI widgets. Initial size and position is specified & may be updated by the user (tracked internally).
// NOTE: Windows cannot be defined as children of other windows.
UiInteraction UiWindowFixedBegin(U32 id, String8 title, V2 pos, V2 size, B32 resizable);
UiInteraction UiWindowFloatingBegin(U32 id, String8 title, V2 pos, V2 size, B32 resizable);
void UiWindowEnd();
// NOTE: Popups are specialized windows that can be defined within other windows. They are lighter weight than regular windows, pos & size state is not propagated between frames.
// They are always rendered on top of everything else.
UiInteraction UiPopupBegin(U32 id, V2 pos, V2 size, B32 visible);
void UiPopupEnd();

typedef enum UiPanelDirection UiPanelDirection;
enum UiPanelDirection {
  UiPanelDirection_Vertical,
  UiPanelDirection_Horizontal,
};

typedef enum UiPanelSize UiPanelSize;
enum UiPanelSize {
  UiPanelSize_Fit,
  UiPanelSize_Grow,
};

// NOTE: Panels. Useful for adding padding between UI widgets within some parent container.
UiInteraction UiPanelBegin(U32 id, UiPanelDirection direction, UiPanelSize main_axis_size, UiPanelSize cross_axis_size, B32 bordered, B32 scrollable, V2 pad, F32 child_gap);
UiInteraction UiPanelVerticalBegin(U32 id, V2 pad, F32 child_gap);
UiInteraction UiPanelHorizontalBegin(U32 id, V2 pad, F32 child_gap);
UiInteraction UiPanelVerticalBorderedBegin(U32 id, V2 pad, F32 child_gap);
UiInteraction UiPanelHorizontalBorderedBegin(U32 id, V2 pad, F32 child_gap);
void UiPanelEnd();

// NOTE: Layout / visual modifiers without a clickable component.
UiInteraction UiSeparator(U32 id);
UiInteraction UiPad(U32 id, F32 size);
UiInteraction UiGrow(U32 id);

// NOTE: Widgets. Must be defined within at least a top level window container.
// NOTE: A size of 0 defaults to a grow scheme.
UiInteraction UiButton(U32 id, String8 text, V2 size);
UiInteraction UiButtonToggle(U32 id, String8 text, B32* toggled, V2 size);
UiInteraction UiButtonRadio(U32 id, String8* options, U32 options_size, V2 button_size, V2 child_gap);
UiInteraction UiComboBox(U32 id, String8* options, U32 options_size, V2 size, F32 child_gap);
UiInteraction UiText(U32 id, String8 text, V2 size);
UiInteraction UiSliderF32(U32 id, F32* value, F32 min, F32 max, F32 resolution, V2 size);
UiInteraction UiSliderU32(U32 id, U32* value, U32 min, U32 max, F32 resolution, V2 size);
UiInteraction UiSliderS32(U32 id, S32* value, S32 min, S32 max, F32 resolution, V2 size);
UiInteraction UiPlotLines(U32 id, F32* values, U32 values_count, V2 size);
UiInteraction UiPlotBar(U32 id, F32* values, U32 values_count, V2 size);

#define UIID_IMPL__(file, line, str) file ":" #line ":" str
#define UIID_IMPL_(file, line, str)  UIID_IMPL__(file, line, str)
#define UIID_IMPL(str)               Str8Hash(Str8Lit(UIID_IMPL_(__FILE__, __LINE__, str)))

#endif // CDEFAULT_UI_H_

#ifdef CDEFAULT_UI_IMPLEMENTATION
#undef CDEFAULT_UI_IMPLEMENTATION

// https://www.rfleury.com/p/ui-series-table-of-contents
// https://www.youtube.com/watch?v=by9lQvpvMIc
// https://github.com/ocornut/imgui

typedef enum UiWidgetOptions UiWidgetOptions;
enum UiWidgetOptions {
  // NOTE: exactly one of these must be specified for container widgets.
  UiWidgetOptions_DirectionHorizontal  = BIT(0),
  UiWidgetOptions_DirectionVertical    = BIT(1),

  // NOTE: exactly one of these must be specified always.
  UiWidgetOptions_SizingHorizFixed     = BIT(2),
  UiWidgetOptions_SizingHorizGrow      = BIT(3),
  UiWidgetOptions_SizingHorizFit       = BIT(4),

  // NOTE: exactly one of these must be specified always.
  UiWidgetOptions_SizingVertFixed      = BIT(5),
  UiWidgetOptions_SizingVertGrow       = BIT(6),
  UiWidgetOptions_SizingVertFit        = BIT(7),

  // TODO: instead of having attributes for align, imply them with Grow?
  // NOTE: exactly one of these must be specified for container widgets.
  UiWidgetOptions_AlignHorizStart      = BIT(8),
  UiWidgetOptions_AlignHorizCenter     = BIT(9),
  UiWidgetOptions_AlignHorizEnd        = BIT(10),

  // NOTE: exactly one of these must be specified for container widgets.
  UiWidgetOptions_AlignVertStart       = BIT(11),
  UiWidgetOptions_AlignVertCenter      = BIT(12),
  UiWidgetOptions_AlignVertEnd         = BIT(13),

  // NOTE: rendering options.
  UiWidgetOptions_RenderBorder         = BIT(14),
  UiWidgetOptions_RenderRect           = BIT(15),
  UiWidgetOptions_RenderText           = BIT(16),
  UiWidgetOptions_RenderGraphLine      = BIT(17),
  UiWidgetOptions_RenderGraphBar       = BIT(18),

  // NOTE: optional sizing options.
  UiWidgetOptions_Resizable            = BIT(19),
  UiWidgetOptions_Scrollable           = BIT(20),
};

#define UiWidgetOptions_SizingFixed (UiWidgetOptions_SizingHorizFixed | UiWidgetOptions_SizingVertFixed)
#define UiWidgetOptions_SizingGrow  (UiWidgetOptions_SizingHorizGrow  | UiWidgetOptions_SizingVertGrow)
#define UiWidgetOptions_SizingFit   (UiWidgetOptions_SizingHorizFit   | UiWidgetOptions_SizingVertFit)
#define UiWidgetOptions_AlignStart  (UiWidgetOptions_AlignHorizStart  | UiWidgetOptions_AlignVertStart)
#define UiWidgetOptions_AlignCenter (UiWidgetOptions_AlignHorizCenter | UiWidgetOptions_AlignVertCenter)
#define UiWidgetOptions_AlignEnd    (UiWidgetOptions_AlignHorizEnd    | UiWidgetOptions_AlignVertEnd)

#define UI_WIDGET_MAX_NUM_CHILDREN 64
#define UI_TEXT_MEASUREMENT_CACHE_CASHOUT_SIZE MB(1)

// NOTE: max number of children any single widget may have
typedef struct UiWidget UiWidget;
struct UiWidget {
  U32 id;
  UiWidget* cached;
  UiWidget* parent;
  UiWidget* children_head;
  UiWidget* children_tail;
  U32       children_size;
  UiWidget* sibling_next;
  UiWidget* sibling_prev;

  UiWidgetOptions options;
  U32 priority;

  V2  pos;
  V2  size;
  V2  fit_size;
  V2  pref_size;
  F32 scroll_offset;

  V2  pad;
  F32 child_gap;

  B32 open;
  U32 selected;
  B32 hovered;

  String8 text;
  F32 slider_value;
  F32* graph_values;
  U32 graph_values_count;

  V3  color;
  V3  target_color;
  B32 is_animating;
  F32 anim_time;
  B32 animation_lock;
};

typedef struct UiCachedTextMeasurement UiCachedTextMeasurement;
struct UiCachedTextMeasurement {
  U32 str8_hash;
  V2 size;
  UiCachedTextMeasurement* next;
};

typedef struct UiContext UiContext;
struct UiContext {
  B32 is_initialized;
  Arena* command_arena;
  Arena* prev_widget_arena;
  Arena* widget_arena;
  Arena* str8_hash_arena;

  U32 deepest_hover_id;
  U32 drag_id;
  UiStyle style;
  UiWidget* root;
  UiWidget* prev_root;
  UiWidget* widget_free_list;
  UiWidget* current_widget;

  void* font_user_data;
  UiFontMeasureText_Fn* ui_font_measure_text_fn;
  UiFontGetAttributes_Fn* ui_font_get_attributes_fn;
  UiCachedTextMeasurement* cached_text_measurements[256];

  F32 dt_s;
  V2 mouse_pos;
  V2 mouse_d_pos;
  F32 mouse_scroll;
  B32 is_lmb_just_pressed;
  B32 is_lmb_down;
  B32 is_rmb_just_pressed;
  B32 is_rmb_down;
};
struct UiContext _ui_context;

static UiContext* UiGetContext() {
  DEBUG_ASSERT(_ui_context.is_initialized);
  return &_ui_context;
}

static void UiFontTextMeasurementHashMapClear() {
  UiContext* c = UiGetContext();
  ArenaClear(c->str8_hash_arena);
  MEMORY_ZERO_STATIC_ARRAY(c->cached_text_measurements);
}

static void UiFontMeasureText(String8 str, V2* size) {
  UiContext* c = UiGetContext();
  if (c->ui_font_measure_text_fn == NULL) {
    *size = V2_ZEROES;
    return;
  }

  // NOTE: string measurements are very frequent and can take a long time. many strings don't change between frames,
  // so we store them in a hashmap to speed up future lookups.
  U32 str_hash = Str8Hash(str);
  U32 idx = str_hash % STATIC_ARRAY_SIZE(c->cached_text_measurements);
  UiCachedTextMeasurement* node = NULL;
  for (node = c->cached_text_measurements[idx]; node != NULL; node = node->next) {
    if (node->str8_hash == str_hash) { break; }
  }

  if (node != NULL) {
    *size = node->size;
  } else {
    // TODO: could do e.g. LRU but this is probably fine.
    if (ArenaPos(c->str8_hash_arena) > UI_TEXT_MEASUREMENT_CACHE_CASHOUT_SIZE) {
      LOG_WARN("[UI] UI text measurement cache size exceeds threshold, clearing.");
      UiFontTextMeasurementHashMapClear();
    }

    c->ui_font_measure_text_fn(c->font_user_data, str, size);
    node = ARENA_PUSH_STRUCT(c->str8_hash_arena, UiCachedTextMeasurement);
    node->str8_hash = str_hash;
    node->size = *size;
    node->next = c->cached_text_measurements[idx];
    c->cached_text_measurements[idx] = node;
  }
}

static void UiFontGetAttributes(F32* descent) {
  UiContext* c = UiGetContext();
  if (c->ui_font_get_attributes_fn == NULL) {
    *descent = 0;
    return;
  }
  c->ui_font_get_attributes_fn(c->font_user_data, descent);
}

static S32 UiWidgetComparePriorityAsc(void* a, void* b) {
  UiWidget* a_widget = *((UiWidget**) a);
  UiWidget* b_widget = *((UiWidget**) b);
  return (S32) a_widget->priority - (S32) b_widget->priority;
}

static void UiWidgetApplySizeDefaultFit(UiWidgetOptions* options, V2 size) {
  if (size.x == 0) { *options |= UiWidgetOptions_SizingHorizFit;   }
  else             { *options |= UiWidgetOptions_SizingHorizFixed; }
  if (size.y == 0) { *options |= UiWidgetOptions_SizingVertFit;    }
  else             { *options |= UiWidgetOptions_SizingVertFixed;  }
}

static void UiWidgetApplySizeDefaultGrow(UiWidgetOptions* options, V2 size) {
  if (size.x == 0) { *options |= UiWidgetOptions_SizingHorizGrow;  }
  else             { *options |= UiWidgetOptions_SizingHorizFixed; }
  if (size.y == 0) { *options |= UiWidgetOptions_SizingVertGrow;   }
  else             { *options |= UiWidgetOptions_SizingVertFixed;  }
}

static UiWidgetOptions UiWidgetGetDirOpt(UiWidget* widget) {
  UiWidgetOptions result = widget->options & (UiWidgetOptions_DirectionHorizontal | UiWidgetOptions_DirectionVertical);
  // NOTE: if the widget has children, a direction must be defined for it.
  DEBUG_ASSERT(!(widget->children_size > 0 && result == 0));
  return result;
}

static UiWidgetOptions UiWidgetGetSizeOpt(UiWidget* widget, UiWidgetOptions direction) {
  UiWidgetOptions result;
  switch (direction) {
    case UiWidgetOptions_DirectionHorizontal: { result = widget->options & (UiWidgetOptions_SizingHorizFixed | UiWidgetOptions_SizingHorizGrow | UiWidgetOptions_SizingHorizFit); } break;
    case UiWidgetOptions_DirectionVertical:   { result = widget->options & (UiWidgetOptions_SizingVertFixed  | UiWidgetOptions_SizingVertGrow  | UiWidgetOptions_SizingVertFit);  } break;
    default: { UNREACHABLE(); return 0; }
  }
  // NOTE: all widgets must have size options defined for each axis.
  DEBUG_ASSERT(result != 0);
  return result;
}

static UiWidgetOptions UiWidgetGetAlignOpt(UiWidget* widget, UiWidgetOptions direction) {
  UiWidgetOptions result;
  switch (direction) {
    case UiWidgetOptions_DirectionHorizontal: { result = widget->options & (UiWidgetOptions_AlignHorizStart | UiWidgetOptions_AlignHorizCenter | UiWidgetOptions_AlignHorizEnd); } break;
    case UiWidgetOptions_DirectionVertical:   { result = widget->options & (UiWidgetOptions_AlignVertStart  | UiWidgetOptions_AlignVertCenter  | UiWidgetOptions_AlignVertEnd);  } break;
    default: { UNREACHABLE(); return 0; }
  }
  // NOTE: if the widget has children, an alignment must be defined for it
  DEBUG_ASSERT(!(widget->children_size > 0 && result == 0));
  return result;
}

static F32* UiWidgetGetDim(V2* v2, UiWidgetOptions direction) {
  switch (direction) {
    case UiWidgetOptions_DirectionHorizontal: { return &v2->x; }
    case UiWidgetOptions_DirectionVertical:   { return &v2->y; }
    default: { UNREACHABLE(); return NULL; }
  }
}

static void UiWidgetUpdatePos(F32* pos, F32 delta, UiWidgetOptions direction) {
  // NOTE: pos should increase in x direction and decrease in y direction.
  // (assuming that the origin is at the bottom left of the screen!)
  switch (direction) {
    case UiWidgetOptions_DirectionHorizontal: { *pos += delta; } break;
    case UiWidgetOptions_DirectionVertical:   { *pos -= delta; } break;
    default: { UNREACHABLE(); } break;
  }
}

static UiWidget* UiWidgetAllocate() {
  UiContext* c = UiGetContext();
  UiWidget* widget = ARENA_PUSH_STRUCT(c->widget_arena, UiWidget);
  MEMORY_ZERO_STRUCT(widget);
  return widget;
}

static void UiWidgetAddChild(UiWidget* parent, UiWidget* child) {
  DEBUG_ASSERT(parent->children_size < UI_WIDGET_MAX_NUM_CHILDREN);
  DLL_PUSH_BACK(parent->children_head, parent->children_tail, child, sibling_prev, sibling_next);
  parent->children_size++;
  child->parent = parent;
}

static void UiWidgetRemoveParent(UiWidget* child) {
  UiWidget* parent = child->parent;
  DLL_REMOVE(parent->children_head, parent->children_tail, child, sibling_prev, sibling_next);
  parent->children_size--;
  child->parent = NULL;
}

// TODO: better cache?
static UiWidget* UiWidgetFind(UiWidget* widget, U32 id) {
  if (widget == NULL)   { return NULL; }
  if (widget->id == id) { return widget; }
  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    UiWidget* maybe_hit = UiWidgetFind(child, id);
    if (maybe_hit != NULL) { return maybe_hit; }
  }
  return NULL;
}

static void UiWidgetUpdateAnimation(UiWidget* widget) {
  UiContext* c = UiGetContext();
  if (!widget->is_animating) { return; }
  F32 anim_max_time = c->style.anim_max_time;
  widget->anim_time += c->dt_s;
  if (widget->anim_time >= anim_max_time) {
    widget->color          = widget->target_color;
    widget->is_animating   = false;
    widget->animation_lock = false;
  } else {
    V3Lerp(&widget->color, &widget->color, &widget->target_color, widget->anim_time / anim_max_time);
  }
}

static void UiWidgetAnimateTo(UiWidget* widget, V3 color, B32 lock) {
  if (widget->animation_lock || V3Eq(&widget->target_color, &color)) { return; }
  widget->target_color = color;
  widget->anim_time = 0;
  widget->is_animating = true;
  widget->animation_lock = lock;
}

static void UiWidgetMaybeAnimate(UiWidget* widget, UiInteraction* interaction) {
  UiContext* c = UiGetContext();
  if (interaction->dragged || interaction->clicked) {
    UiWidgetAnimateTo(widget, c->style.widget_color_active, true);
  } else if (interaction->hovered) {
    UiWidgetAnimateTo(widget, c->style.widget_color_bright, false);
  } else {
    UiWidgetAnimateTo(widget, c->style.widget_color, false);
  }
}

static UiInteraction UiWidgetGetInteraction(UiWidget* widget) {
  UiContext* c = UiGetContext();

  UiInteraction result;
  MEMORY_ZERO_STRUCT(&result);
  result.is_new        = widget->cached == NULL;
  result.hovered       = widget->cached != NULL && widget->cached->hovered;
  result.clicked       = result.hovered && c->is_lmb_just_pressed;
  result.right_clicked = result.hovered && c->is_rmb_just_pressed;
  result.dragged       = c->drag_id == widget->id;

  // NOTE: tracked manually / separately.
  result.open     = false;
  result.selected = 0;

  return result;
}

static void UiInteractionMerge(UiInteraction* dest, UiInteraction src) {
  dest->hovered       = dest->hovered || src.hovered;
  dest->clicked       = dest->clicked || src.clicked;
  dest->right_clicked = dest->right_clicked || src.right_clicked;
  dest->dragged       = dest->dragged || src.dragged;
  dest->open          = dest->open || src.open;
  dest->selected      = (src.selected != 0) ? src.selected : dest->selected;
}

static UiWidget* UiWidgetBegin(U32 id, UiWidgetOptions options) {
  UiContext* c = UiGetContext();

  UiWidget* new_widget = UiWidgetAllocate();
  new_widget->id       = id;
  new_widget->options  = options;
  UiWidgetAddChild(c->current_widget, new_widget);

  // NOTE: sync data from last frame
  new_widget->cached = UiWidgetFind(c->prev_root, id);;
  if (new_widget->cached != NULL) {
    new_widget->scroll_offset  = new_widget->cached->scroll_offset;
    new_widget->slider_value   = new_widget->cached->slider_value;
    new_widget->priority       = new_widget->cached->priority;
    new_widget->pos            = new_widget->cached->pos;
    new_widget->open           = new_widget->cached->open;
    new_widget->selected       = new_widget->cached->selected;
    new_widget->is_animating   = new_widget->cached->is_animating;
    new_widget->color          = new_widget->cached->color;
    new_widget->target_color   = new_widget->cached->target_color;
    new_widget->anim_time      = new_widget->cached->anim_time;
    new_widget->animation_lock = new_widget->cached->animation_lock;
    UiWidgetUpdateAnimation(new_widget);
  } else {
    new_widget->color = c->style.widget_color;
  }

  c->current_widget = new_widget;
  return c->current_widget;
}

static void UiWidgetEnd() {
  UiContext* c = UiGetContext();
  UiWidget* widget = c->current_widget;
  c->current_widget = widget->parent;

  // NOTE: only windows can be resizable.
  if (widget->options & UiWidgetOptions_Resizable) { DEBUG_ASSERT(widget->parent == c->root); }
}

static void UiWidgetsSize(UiWidget* widget, UiWidgetOptions direction) {
  // NOTE: size starting at the bottom and moving up.
  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    UiWidgetsSize(child, direction);
  }

  UiWidget* parent = widget->parent;
  if (parent == NULL) { return; }

  // NOTE: widget->fit_size is updated when the children go (at the start of this fn). so, just finalize.
  F32* widget_fit_size_dim = UiWidgetGetDim(&widget->fit_size, direction);
  if (UiWidgetGetDirOpt(widget) == direction) { *widget_fit_size_dim += (widget->children_size - 1) * widget->child_gap; }
  F32 widget_pad_dim = *UiWidgetGetDim(&widget->pad, direction);
  *widget_fit_size_dim += widget_pad_dim * 2;

  // NOTE: determine final size from fit & min size.
  F32 widget_pref_size_dim = *UiWidgetGetDim(&widget->pref_size, direction);
  F32* widget_size_dim = UiWidgetGetDim(&widget->size, direction);
  UiWidgetOptions widget_size_opt = UiWidgetGetSizeOpt(widget, direction);
  if (widget_size_opt & UiWidgetOptions_SizingFit)        { *widget_size_dim = *widget_fit_size_dim; }
  else if (widget_size_opt & UiWidgetOptions_SizingGrow)  { *widget_size_dim = MAX(*widget_fit_size_dim, widget_pref_size_dim); }
  else if (widget_size_opt & UiWidgetOptions_SizingFixed) { *widget_size_dim = widget_pref_size_dim; }
  else { UNREACHABLE(); }

  // NOTE: now, apply final size to parent
  F32* parent_fit_size_dim  = UiWidgetGetDim(&parent->fit_size, direction);
  if (UiWidgetGetDirOpt(parent) == direction) { *parent_fit_size_dim += *widget_size_dim; }
  else { *parent_fit_size_dim = MAX(*parent_fit_size_dim, *widget_size_dim); }
}

static void UiWidgetsGrow(UiWidget* widget, UiWidgetOptions direction) {
  // NOTE: collect all children that can grow. they need to share the growth delta.
  // TODO: cache this on the widget?
  UiWidget* growable_children[UI_WIDGET_MAX_NUM_CHILDREN];
  U32 growable_children_size = 0;
  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    if (UiWidgetGetSizeOpt(child, direction) & UiWidgetOptions_SizingGrow) {
      DEBUG_ASSERT(growable_children_size < STATIC_ARRAY_SIZE(growable_children));
      growable_children[growable_children_size++] = child;
    }
  }

  if (growable_children_size > 0) {
    F32 widget_pad_dim_2 = *UiWidgetGetDim(&widget->pad, direction) * 2;
    F32 widget_size_dim  = *UiWidgetGetDim(&widget->size, direction) - widget_pad_dim_2;
    if (UiWidgetGetDirOpt(widget) == direction) {
      // NOTE: share the main-axis dim with all growable children
      F32 widget_fit_size_dim = *UiWidgetGetDim(&widget->fit_size, direction) - widget_pad_dim_2;
      F32 delta               = widget_size_dim - widget_fit_size_dim;

      // NOTE: there is space to share, grow
      while (delta > 1e-5) {
        // NOTE: find the 2 smallest sets of widgets.
        F32 smallest_size_dim = *UiWidgetGetDim(&growable_children[0]->size, direction);
        F32 second_smallest_size_dim = F32_MAX;
        for (U32 i = 1; i < growable_children_size; i++) {
          UiWidget* child = growable_children[i];
          F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
          if (child_size_dim < smallest_size_dim) {
            second_smallest_size_dim = smallest_size_dim;
            smallest_size_dim = child_size_dim;
          } else if (child_size_dim < second_smallest_size_dim) {
            second_smallest_size_dim = child_size_dim;
          }
        }
        if (second_smallest_size_dim == F32_MAX) { second_smallest_size_dim = smallest_size_dim; }

        // NOTE: grow the smallest widgets so they're as big as the second smallest
        F32 size_delta = MIN(second_smallest_size_dim - smallest_size_dim, delta);
        if (F32ApproxEq(size_delta, 0)) { size_delta = delta / growable_children_size; }
        for (U32 i = 0; i < growable_children_size; i++) {
          UiWidget* child = growable_children[i];
          F32* child_size_dim = UiWidgetGetDim(&child->size, direction);
          if (F32ApproxEq(*child_size_dim, smallest_size_dim)) {
            *child_size_dim += size_delta;
            delta -= size_delta;
          }
        }
      }

      // NOTE: there is not enough space, shrink
      // NOTE: there is an added complexity for shrinking, we want to prevent shrinking widgets below their fit size
      while (delta < -1e-5) {
        // NOTE: find the 2 largest sets of widgets.
        F32 largest_size_dim = F32_MIN;
        F32 second_largest_size_dim = F32_MIN;
        F32 max_shrink_delta = F32_MAX;
        for (U32 i = 0; i < growable_children_size; i++) {
          UiWidget* child = growable_children[i];

          F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
          F32 child_fit_size_dim = *UiWidgetGetDim(&child->size, direction);
          // NOTE: don't shrink elements that are already at their fit size
          if (child_size_dim == child_fit_size_dim) { continue; }

          if (child_size_dim >= largest_size_dim) {
            second_largest_size_dim = largest_size_dim;
            largest_size_dim = child_size_dim;
            max_shrink_delta = MIN(max_shrink_delta, child_size_dim - child_fit_size_dim);
          } else if (child_size_dim > second_largest_size_dim) {
            second_largest_size_dim = child_size_dim;
          }
        }
        // NOTE: no widgets have room to shrink, give up
        if (largest_size_dim == F32_MIN)        { break; }
        if (second_largest_size_dim == F32_MIN) { second_largest_size_dim = largest_size_dim; }
        DEBUG_ASSERT(max_shrink_delta > 0 && max_shrink_delta != F32_MAX);

        // NOTE: collect count widgets of target size that we actually can shrink
        U32 target_shrink_widgets_size = 0;
        for (U32 i = 0; i < growable_children_size; i++) {
          UiWidget* child = growable_children[i];
          F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
          F32 child_fit_size_dim = *UiWidgetGetDim(&child->size, direction);
          if (child_size_dim == largest_size_dim && child_size_dim > child_fit_size_dim) {
            target_shrink_widgets_size++;
          }
        }

        // NOTE: shrink the largest widgets so they're as big as the second largest
        F32 size_delta = MIN(largest_size_dim - second_largest_size_dim, -delta);
        // NOTE: but only so far as the maximum shrink distance for widgets of that size
        size_delta = MIN(size_delta, max_shrink_delta);
        if (F32ApproxEq(size_delta, 0)) { size_delta = -delta / target_shrink_widgets_size; }

        for (U32 i = 0; i < growable_children_size; i++) {
          UiWidget* child = growable_children[i];
          F32* child_size_dim = UiWidgetGetDim(&child->size, direction);
          if (F32ApproxEq(*child_size_dim, largest_size_dim)) {
            *child_size_dim -= size_delta;
            delta += size_delta;
          }
        }
      }
    } else {
      // NOTE: fill up the cross-axis dim
      for (U32 i = 0; i < growable_children_size; i++) {
        UiWidget* child = growable_children[i];
        F32* child_size_dim = UiWidgetGetDim(&child->size, direction);
        *child_size_dim = widget_size_dim;
      }
    }
  }

  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    UiWidgetsGrow(child, direction);
  }
}

// TODO: this only works for single level scrollables, need to update it to work for nested scrollables
static void UiWidgetsClampScrollOffset(UiWidget* widget) {
  UiContext* c = UiGetContext();
  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    UiWidgetsClampScrollOffset(child);
  }
  if (!(widget->options & UiWidgetOptions_Scrollable)) { return; }
  DEBUG_ASSERT(widget->parent != c->root);

  F32 scroll_room = MAX(widget->parent->fit_size.y - widget->parent->size.y, 0);
  widget->scroll_offset = CLAMP(-scroll_room, widget->scroll_offset, 0);
}

static void UiWidgetsPosition(UiWidget* widget, UiWidgetOptions direction) {
  UiWidgetOptions widget_direction_dim = UiWidgetGetDirOpt(widget);
  UiWidgetOptions widget_align_dim     = UiWidgetGetAlignOpt(widget, direction);
  F32 widget_size_dim = *UiWidgetGetDim(&widget->size, direction);
  F32 widget_pad_dim  = *UiWidgetGetDim(&widget->pad, direction);
  F32 widget_pos_dim  = *UiWidgetGetDim(&widget->pos, direction);

  F32 pos = widget_pos_dim;

  // NOTE: apply any scroll delta
  if (direction == UiWidgetOptions_DirectionVertical) {
    UiWidgetUpdatePos(&pos, widget->scroll_offset, UiWidgetOptions_DirectionVertical);
  }

  // NOTE: apply alignment along the widget's direction
  // need to determine final child content size in order to determine placement
  if (widget_direction_dim == direction) {
    if (widget_align_dim & UiWidgetOptions_AlignStart) {
      UiWidgetUpdatePos(&pos, widget_pad_dim, direction);
    } else {
      F32 content_size_dim = 0;
      for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
        F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
        content_size_dim += child_size_dim;
        if (child->sibling_next != NULL) { content_size_dim += widget->child_gap; }
      }
      if (widget_align_dim & UiWidgetOptions_AlignCenter)   { UiWidgetUpdatePos(&pos, (widget_size_dim / 2.0f) - (content_size_dim / 2.0f), direction); }
      else if (widget_align_dim & UiWidgetOptions_AlignEnd) { UiWidgetUpdatePos(&pos, widget_size_dim - content_size_dim - widget_pad_dim, direction);  }
      else { DEBUG_ASSERT(widget->children_size == 0); }
    }
  }

  // NOTE: position this nodes children relative to itself
  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    F32 child_pos = pos;

    // NOTE: apply alignment across the widget's direction
    // placement only concerns the child widget
    if (widget_direction_dim != direction) {
      if (widget_align_dim & UiWidgetOptions_AlignStart) {
        UiWidgetUpdatePos(&pos, widget_pad_dim, direction);
      } else {
        F32 content_size_dim = *UiWidgetGetDim(&child->size, direction);
        if (widget_align_dim & UiWidgetOptions_AlignCenter)   { UiWidgetUpdatePos(&child_pos, (widget_size_dim / 2.0f) - (content_size_dim / 2.0f), direction); }
        else if (widget_align_dim & UiWidgetOptions_AlignEnd) { UiWidgetUpdatePos(&child_pos, widget_size_dim - content_size_dim - widget_pad_dim, direction);  }
        else { DEBUG_ASSERT(widget->children_size == 0); }
      }
    }

    // NOTE: place children widgets
    *UiWidgetGetDim(&child->pos, direction) = child_pos;
    UiWidgetsPosition(child, direction);

    if (widget_direction_dim == direction) {
      F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
      F32 pos_delta = child_size_dim + widget->child_gap;
      UiWidgetUpdatePos(&pos, pos_delta, direction);
    }
  }
}

static void UiMaybeUpdateHot(UiWidget* widget, UiWidget** active_window, UiWidget** deepest_scrollable) {
  UiContext* c = UiGetContext();

  // NOTE: correct coords, pos is top left.
  V2 m   = c->mouse_pos;
  V2 min = widget->pos;
  V2 max = widget->pos;
  min.y -= widget->size.y;
  max.x += widget->size.x;
  if (min.x <= m.x && m.x <= max.x && min.y <= m.y && m.y <= max.y) {
    widget->hovered = true;
    c->deepest_hover_id = widget->id;
    for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
      UiMaybeUpdateHot(child, active_window, deepest_scrollable);
    }

    // NOTE: find the deepmost scrollable from bottom up, don't overwrite if already found.
    if (*deepest_scrollable == NULL && (widget->options & UiWidgetOptions_Scrollable)) { *deepest_scrollable = widget; }
    // NOTE: the window will always be the topmost widget, so clobber when going back up.
    *active_window = widget;
  }
}

static UiDrawCommand* UiAllocateCommand(UiDrawCommand** head, UiDrawCommand** tail) {
  UiContext* c = UiGetContext();
  UiDrawCommand* command = ARENA_PUSH_STRUCT(c->command_arena, UiDrawCommand);
  DLL_PUSH_BACK(*head, *tail, command, prev, next);
  return command;
}

static void UiEnqueueDrawCommands(UiWidget* widget, UiDrawCommand** head, UiDrawCommand** tail, V2 scissor_min, V2 scissor_max) {
  UiContext* c = UiGetContext();
  UiDrawCommand* command;

  B32 scissor = widget->children_size > 0;
  if (scissor) {
    scissor_min.x = MAX(widget->pos.x, scissor_min.x);
    scissor_min.y = MAX(widget->pos.y - widget->size.y, scissor_min.y);
    scissor_max.x = MIN(widget->pos.x + widget->size.x, scissor_max.x);
    scissor_max.y = MIN(widget->pos.y, scissor_max.y);;
    if (scissor_min.x >= scissor_max.x || scissor_min.y >= scissor_max.y) { return; }

    command = UiAllocateCommand(head, tail);
    command->type = UiDrawCommand_ScissorEnable;
    command->scissor_enable.min = scissor_min;
    command->scissor_enable.max = scissor_max;
  }

  if (widget->options & UiWidgetOptions_RenderBorder && c->style.border_size > 0) {
    command = UiAllocateCommand(head, tail);
    command->type = UiDrawCommand_Rect;
    command->rect.center.x = widget->pos.x + widget->size.x / 2.0f;
    command->rect.center.y = widget->pos.y - widget->size.y / 2.0f;
    command->rect.size     = widget->size;
    command->rect.color    = c->style.border_color;
  }

  if (widget->options & UiWidgetOptions_RenderRect) {
    command = UiAllocateCommand(head, tail);
    command->type = UiDrawCommand_Rect;
    command->rect.center.x = widget->pos.x + widget->size.x / 2.0f;
    command->rect.center.y = widget->pos.y - widget->size.y / 2.0f;
    command->rect.size     = widget->size;
    command->rect.color    = widget->color;
    if (widget->options & UiWidgetOptions_RenderBorder) {
      V2 border_size = V2Assign(c->style.border_size * 2, c->style.border_size * 2);
      V2SubV2(&command->rect.size, &command->rect.size, &border_size);
    }
  }

  if (widget->options & UiWidgetOptions_RenderGraphLine && widget->graph_values_count > 0) {
    F32 y_min = F32_MAX;
    F32 y_max = F32_MIN;
    for (U32 i = 0; i < widget->graph_values_count; i++) {
      y_min = MIN(y_min, widget->graph_values[i]);
      y_max = MAX(y_max, widget->graph_values[i]);
    }
    F32 x_step = (widget->size.x - (c->style.border_size * 2) - 2) / (widget->graph_values_count - 1);
    F32 y_step = (widget->size.y - (c->style.border_size * 2) - 2) / (y_max - y_min);
    F32 x_pos = widget->pos.x + c->style.border_size + 1;
    F32 y_pos = widget->pos.y - widget->size.y + c->style.border_size + 1;

    for (U32 i = 0; i < widget->graph_values_count - 1; i++) {
      F32 curr_value = widget->graph_values[i]     - y_min;
      F32 next_value = widget->graph_values[i + 1] - y_min;

      command = UiAllocateCommand(head, tail);
      command->type = UiDrawCommand_Line;
      command->line.start.x = x_pos;
      command->line.start.y = y_pos + (curr_value * y_step);
      command->line.end.x   = x_pos + x_step;
      command->line.end.y   = y_pos + (next_value * y_step);
      command->line.color   = c->style.graph_color;

      x_pos += x_step;
    }
  }

  if (widget->options & UiWidgetOptions_RenderGraphBar && widget->graph_values_count > 0) {
    F32 y_min = F32_MAX;
    F32 y_max = F32_MIN;
    for (U32 i = 0; i < widget->graph_values_count; i++) {
      y_min = MIN(y_min, widget->graph_values[i]);
      y_max = MAX(y_max, widget->graph_values[i]);
    }
    F32 x_step = (widget->size.x - (c->style.border_size * 2) - 2) / widget->graph_values_count;
    F32 y_step = (widget->size.y - (c->style.border_size * 2) - 2) / (y_max - y_min);
    F32 x_pos = widget->pos.x + c->style.border_size + 1;
    F32 y_pos = widget->pos.y - widget->size.y + c->style.border_size + 1;

    for (U32 i = 0; i < widget->graph_values_count; i++) {
      F32 curr_value = widget->graph_values[i] - y_min;

      command = UiAllocateCommand(head, tail);
      command->type = UiDrawCommand_Rect;
      command->rect.center.x = x_pos + (x_step / 2.0f);
      command->rect.center.y = y_pos + (curr_value * y_step / 2.0f);
      command->rect.size.x   = x_step;
      command->rect.size.y   = curr_value * y_step;
      command->rect.color    = c->style.graph_color;

      x_pos += x_step;
    }
  }

  if (widget->options & UiWidgetOptions_RenderText) {
    F32 font_descent;
    UiFontGetAttributes(&font_descent);

    command = UiAllocateCommand(head, tail);
    command->type = UiDrawCommand_Text;
    command->text.string = widget->text;
    command->text.pos = widget->pos;
    command->text.pos.y -= (widget->size.y - font_descent);
    command->text.color = c->style.text_color;
  }

  for (UiWidget* child = widget->children_head; child != NULL; child = child->sibling_next) {
    UiEnqueueDrawCommands(child, head, tail, scissor_min, scissor_max);

    // NOTE: reapply scissor as child widget may have modified it.
    if (scissor) {
      command = UiAllocateCommand(head, tail);
      command->type = UiDrawCommand_ScissorEnable;
      command->scissor_enable.min = scissor_min;
      command->scissor_enable.max = scissor_max;
    }
  }

  if (scissor) {
    command = UiAllocateCommand(head, tail);
    command->type = UiDrawCommand_ScissorDisable;
  }
}

void UiInit() {
  UiContext* c = &_ui_context;
  if (c->is_initialized) { return; }

  c->widget_arena      = ArenaAllocate();
  c->prev_widget_arena = ArenaAllocate();
  c->command_arena     = ArenaAllocate();
  c->str8_hash_arena   = ArenaAllocate();
  c->is_initialized    = true;

  UiStyle* default_style = UiGetStyle();
  default_style->anim_max_time       = 0.07f;
  default_style->border_size         = 1.0f;
  default_style->border_color        = V3_MOONFLY_DARK_GRAY;
  default_style->background_color    = V3_MOONFLY_BLACK;
  default_style->text_color          = V3_MOONFLY_WHITE;
  default_style->graph_color         = V3_MOONFLY_WHITE;
  default_style->widget_color        = V3_MOONFLY_BLUE;
  default_style->widget_color_bright = V3_MOONFLY_LIGHT_BLUE;
  default_style->widget_color_active = V3_MOONFLY_LIGHT_RED;
}

void UiDeinit() {
  UiContext* c = &_ui_context;
  if (!c->is_initialized) { return; }

  UiFontTextMeasurementHashMapClear();
  ArenaRelease(c->widget_arena);
  ArenaRelease(c->prev_widget_arena);
  ArenaRelease(c->command_arena);
  ArenaRelease(c->str8_hash_arena);
  c->is_initialized = false;
}

void UiSetPointerState(V2 mouse_pos, F32 mouse_scroll, B32 is_lmb_down, B32 is_rmb_down) {
  UiContext* c = UiGetContext();
  V2SubV2(&c->mouse_d_pos, &mouse_pos, &c->mouse_pos);
  c->mouse_pos = mouse_pos;
  c->mouse_scroll = mouse_scroll;
  c->is_lmb_just_pressed = !c->is_lmb_down && is_lmb_down;
  c->is_rmb_just_pressed = !c->is_rmb_down && is_rmb_down;
  c->is_lmb_down = is_lmb_down;
  c->is_rmb_down = is_rmb_down;
}

void UiSetFontUserData(void* user_data) {
  UiContext* c = UiGetContext();
  UiFontTextMeasurementHashMapClear();
  c->font_user_data = user_data;
}

void UiSetFontCallbacks(UiFontMeasureText_Fn* ui_font_measure_text_fn, UiFontGetAttributes_Fn* ui_font_get_attributes_fn) {
  UiContext* c = UiGetContext();
  UiFontTextMeasurementHashMapClear();
  c->ui_font_measure_text_fn   = ui_font_measure_text_fn;
  c->ui_font_get_attributes_fn = ui_font_get_attributes_fn;
}

UiStyle* UiGetStyle() {
  UiContext* c = UiGetContext();
  return &c->style;
}

B32 UiContainsMouse() {
  UiContext* c = UiGetContext();
  return c->deepest_hover_id != 0 || c->drag_id != 0;
}

void UiBegin(F32 dt_s) {
  UiContext* c = UiGetContext();
  DEBUG_ASSERT(c->root == NULL);
  DEBUG_ASSERT(c->current_widget == NULL);
  c->root = UiWidgetAllocate();
  c->root->options  = ~0; // NOTE: don't trip flag assertions on the root / null widget.
  c->current_widget = c->root;
  c->dt_s = dt_s;
}

UiDrawCommand* UiEnd() {
  UiContext* c = UiGetContext();
  DEBUG_ASSERT(c->current_widget == c->root);

  // NOTE: order windows by priority / draw precedence
  // could sort inside the DLL, but the number of windows on screen will never be large enough to matter.
  UiWidget* windows[UI_WIDGET_MAX_NUM_CHILDREN];
  U32 windows_size = 0;
  for (UiWidget* window = c->root->children_head; window != NULL; window = window->sibling_next) {
    windows[windows_size++] = window;
  }
  SORT(UiWidget*, windows, windows_size, UiWidgetComparePriorityAsc);

  // NOTE: perform passes over all windows (to position, size, enqueue draw cmds)
  c->deepest_hover_id = 0;
  ArenaClear(c->command_arena);
  UiDrawCommand* commands_head = NULL;
  UiDrawCommand* commands_tail = NULL;
  UiWidget* hot_root_widget    = NULL;
  UiWidget* deepest_scrollable = NULL;
  for (U32 i = 0; i < windows_size; i++) {
    UiWidget* window = windows[i];
    window->priority = i; // NOTE: fix priority so in range (0, num windows)
    UiWidgetsSize(window, UiWidgetOptions_DirectionHorizontal);
    UiWidgetsSize(window, UiWidgetOptions_DirectionVertical);
    UiWidgetsGrow(window, UiWidgetOptions_DirectionHorizontal);
    UiWidgetsGrow(window, UiWidgetOptions_DirectionVertical);
    UiWidgetsClampScrollOffset(window); // NOTE: do every frame to respond to arbitrary resize events
    UiWidgetsPosition(window, UiWidgetOptions_DirectionHorizontal);
    UiWidgetsPosition(window, UiWidgetOptions_DirectionVertical);
    UiEnqueueDrawCommands(window, &commands_head, &commands_tail, V2Assign(F32_MIN, F32_MIN), V2Assign(F32_MAX, F32_MAX));

    UiWidget* current_hot_root_widget    = NULL;
    UiWidget* current_deepest_scrollable = NULL;
    UiMaybeUpdateHot(window, &current_hot_root_widget, &current_deepest_scrollable);
    // NOTE: since windows are sorted by priority, clobbering ensures that only the highest prio window wins here.
    if (current_hot_root_widget != NULL)    { hot_root_widget = current_hot_root_widget;       }
    if (current_deepest_scrollable != NULL) { deepest_scrollable = current_deepest_scrollable; }
  }

  // NOTE: update active widget if just clicked
  if (c->is_lmb_just_pressed && c->drag_id == 0 && hot_root_widget != NULL) {
    c->drag_id = c->deepest_hover_id;
    // NOTE: boost priority of widget's window, to affect next frame draw order / precedence
    hot_root_widget->priority = c->root->children_size;
  } else if (!c->is_lmb_down) {
    c->drag_id = 0;
  }

  // NOTE: update scroll
  if (deepest_scrollable != NULL && c->mouse_scroll != 0) {
    deepest_scrollable->scroll_offset += c->mouse_scroll * 10;
  }

  // NOTE: move current widget tree to prev for next frame's lookups
  ArenaClear(c->prev_widget_arena);
  SWAP(Arena*, c->prev_widget_arena, c->widget_arena);
  c->prev_root = c->root;
  c->root = NULL;
  c->current_widget = NULL;

  return commands_head;
}

UiInteraction UiWindowFixedBegin(U32 id, String8 title, V2 pos, V2 size, B32 resizable) {
  UiContext* c = UiGetContext();
  DEBUG_ASSERT(c->current_widget == c->root);
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignStart | UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect;
  if (resizable) { options |= UiWidgetOptions_Resizable; }
  UiWidgetApplySizeDefaultFit(&options, size);
  UiWidget* window = UiWidgetBegin(id, options);
  if (window->cached == NULL) {
    window->pos = pos;
    window->open = true;
    window->priority = S32_MAX;
  }
  window->pref_size = size;
  window->color = c->style.background_color;
  UiInteraction result = UiWidgetGetInteraction(window);
  if (title.size > 0) {
    B32 open_old = window->open;
    UiInteractionMerge(&result, UiPanelBegin(UIID_INT(id), UiPanelDirection_Horizontal, UiPanelSize_Grow, UiPanelSize_Fit, false, false, V2Assign(5, 5), 0));
      UiInteractionMerge(&result, UiText(UIID_INT(id), title, V2_ZEROES));
      UiInteractionMerge(&result, UiGrow(UIID_INT(id)));
      UiInteractionMerge(&result, UiButtonToggle(UIID_INT(id), Str8Lit(""), &window->open, V2Assign(20, 20)));
    UiPanelEnd();
    UiInteractionMerge(&result, UiSeparator(UIID_INT(id)));

    if (!window->open)            { window->options &= ~UiWidgetOptions_Resizable; }
    // NOTE: trigger an automatic size pass if open has been toggled
    if (window->open != open_old) { window->options &= ~UiWidgetOptions_Resizable; }
  }

  UiPanelBegin(UIID_INT(id), UiPanelDirection_Vertical, UiPanelSize_Fit, UiPanelSize_Grow, false, true, V2_ZEROES, 0);
  result.open = window->open;
  return result;
}

UiInteraction UiWindowFloatingBegin(U32 id, String8 title, V2 pos, V2 size, B32 resizable) {
  UiContext* c = UiGetContext();
  UiInteraction result = UiWindowFixedBegin(id, title, pos, size, resizable);
  UiWidget* window = UiWidgetFind(c->root, id);
  if (result.dragged) { V2AddV2(&window->pos, &window->pos, &c->mouse_d_pos); }
  return result;
}

void UiWindowEnd() {
  UiContext* c = UiGetContext();
  UiPanelEnd();
  UiWidget* window = c->current_widget;
  UiWidgetEnd();
  DEBUG_ASSERT(c->current_widget == c->root);

  if ((window->options & UiWidgetOptions_Resizable) && window->cached != NULL) {
    window->options &= ~(UiWidgetOptions_SizingFixed | UiWidgetOptions_SizingGrow | UiWidgetOptions_SizingFit);
    window->options |= UiWidgetOptions_SizingFixed;
    V2 size = window->cached->size;

    V2 resize_pos;
    resize_pos.x = window->cached->pos.x + window->cached->size.x;
    resize_pos.y = window->cached->pos.y - window->cached->size.y;
    U32 popup_id = UIID_INT(window->id);
    if (UiPopupBegin(popup_id, resize_pos, V2Assign(20, 20), false).dragged) {
      window->priority = S32_MAX;
      V2 d_size = V2Assign(c->mouse_d_pos.x, -c->mouse_d_pos.y);
      V2AddV2(&size, &size, &d_size);
    }
    UiPopupEnd();
    UiWidget* popup = UiWidgetFind(c->root, popup_id);
    popup->priority = window->priority;

    window->pref_size.x = MAX(size.x, 5);
    window->pref_size.y = MAX(size.y, 5);
  }
}

UiInteraction UiPopupBegin(U32 id, V2 pos, V2 size, B32 visible) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignCenter;
  if (visible) { options |= (UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect); }
  UiWidgetApplySizeDefaultFit(&options, size);
  UiWidget* popup = UiWidgetBegin(id, options);
  popup->pos = pos;
  popup->color = c->style.background_color;
  popup->pref_size = size;
  popup->priority = S32_MAX;
  return UiWidgetGetInteraction(popup);
}

void UiPopupEnd() {
  UiContext* c = UiGetContext();
  UiWidget* popup = c->current_widget;
  UiWidgetEnd();
  UiWidget* parent = popup->parent;
  // NOTE: popups are windows, and windows should be children of the root widget
  if (parent != c->root) {
    UiWidgetRemoveParent(popup);
    UiWidgetAddChild(c->root, popup);
  }
}

UiInteraction UiPanelBegin(U32 id, UiPanelDirection direction, UiPanelSize main_axis_size, UiPanelSize cross_axis_size, B32 bordered, B32 scrollable, V2 pad, F32 child_gap) {
  U32 options = 0;
  switch (direction) {
    case UiPanelDirection_Vertical: {
      options |= UiWidgetOptions_DirectionVertical;
      switch (main_axis_size) {
        case UiPanelSize_Grow: { options |= UiWidgetOptions_SizingVertGrow;  } break;
        case UiPanelSize_Fit:  { options |= UiWidgetOptions_SizingVertFit;   } break;
        default: UNREACHABLE();
      }
      switch (cross_axis_size) {
        case UiPanelSize_Grow: { options |= UiWidgetOptions_SizingHorizGrow; } break;
        case UiPanelSize_Fit:  { options |= UiWidgetOptions_SizingHorizFit;  } break;
        default: UNREACHABLE();
      }
    } break;
    case UiPanelDirection_Horizontal: {
      options |= UiWidgetOptions_DirectionHorizontal;
      switch (main_axis_size) {
        case UiPanelSize_Grow: { options |= UiWidgetOptions_SizingHorizGrow; } break;
        case UiPanelSize_Fit:  { options |= UiWidgetOptions_SizingHorizFit;  } break;
        default: UNREACHABLE();
      }
      switch (cross_axis_size) {
        case UiPanelSize_Grow: { options |= UiWidgetOptions_SizingVertGrow;  } break;
        case UiPanelSize_Fit:  { options |= UiWidgetOptions_SizingVertFit;   } break;
        default: UNREACHABLE();
      }
    } break;
    default: UNREACHABLE();
  }
  if (bordered)   { options |= (UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect); }
  if (scrollable) { options |= UiWidgetOptions_Scrollable; }
  options |= UiWidgetOptions_AlignCenter;

  UiContext* c = UiGetContext();
  UiWidget* panel = UiWidgetBegin(id, options);
  panel->pad = pad;
  panel->child_gap = child_gap;
  panel->color = c->style.background_color;
  return UiWidgetGetInteraction(panel);
}

UiInteraction UiPanelVerticalBegin(U32 id, V2 pad, F32 child_gap) {
  return UiPanelBegin(id, UiPanelDirection_Vertical, UiPanelSize_Grow, UiPanelSize_Grow, false, false, pad, child_gap);
}

UiInteraction UiPanelHorizontalBegin(U32 id, V2 pad, F32 child_gap) {
  return UiPanelBegin(id, UiPanelDirection_Horizontal, UiPanelSize_Grow, UiPanelSize_Grow, false, false, pad, child_gap);
}

UiInteraction UiPanelVerticalBorderedBegin(U32 id, V2 pad, F32 child_gap) {
  return UiPanelBegin(id, UiPanelDirection_Vertical, UiPanelSize_Grow, UiPanelSize_Grow, true, false, pad, child_gap);
}

UiInteraction UiPanelHorizontalBorderedBegin(U32 id, V2 pad, F32 child_gap) {
  return UiPanelBegin(id, UiPanelDirection_Horizontal, UiPanelSize_Grow, UiPanelSize_Grow, true, false, pad, child_gap);
}

void UiPanelEnd() {
  UiWidgetEnd();
}

UiInteraction UiSeparator(U32 id) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_RenderBorder;
  switch (UiWidgetGetDirOpt(c->current_widget)) {
    case UiWidgetOptions_DirectionVertical:   { options |= (UiWidgetOptions_SizingHorizGrow  | UiWidgetOptions_SizingVertFixed); } break;
    case UiWidgetOptions_DirectionHorizontal: { options |= (UiWidgetOptions_SizingHorizFixed | UiWidgetOptions_SizingVertGrow);  } break;
    default: UNREACHABLE();
  }
  UiWidget* separator = UiWidgetBegin(id, options);
  separator->color = c->style.border_color;
  separator->pref_size = V2Assign(c->style.border_size, c->style.border_size);
  UiWidgetEnd();
  return UiWidgetGetInteraction(separator);
}

UiInteraction UiPad(U32 id, F32 size) {
  UiContext* c = UiGetContext();
  UiWidgetOptions parent_dir = UiWidgetGetDirOpt(c->current_widget);
  UiWidgetOptions options = parent_dir | UiWidgetOptions_SizingFit | UiWidgetOptions_AlignCenter;
  UiWidget* pad = UiWidgetBegin(id, options);
  if (parent_dir == UiWidgetOptions_DirectionVertical) { pad->fit_size.y = size; }
  else { pad->fit_size.x = size; }
  UiWidgetEnd();
  return UiWidgetGetInteraction(pad);
}

UiInteraction UiGrow(U32 id) {
  UiContext* c = UiGetContext();
  UiWidgetOptions parent_dir = UiWidgetGetDirOpt(c->current_widget);
  UiInteraction result;
  if (parent_dir == UiWidgetOptions_DirectionVertical) { result = UiPanelVerticalBegin(id, V2_ZEROES, 0); }
  else { result = UiPanelHorizontalBegin(id, V2_ZEROES, 0); }
  UiPanelEnd();
  return result;
}

UiInteraction UiButton(U32 id, String8 text, V2 size) {
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical |
                            UiWidgetOptions_AlignCenter |
                            UiWidgetOptions_RenderBorder |
                            UiWidgetOptions_RenderRect;
  UiWidgetApplySizeDefaultGrow(&options, size);
  UiWidget* button = UiWidgetBegin(id, options);
  button->pref_size = size;
  UiInteraction interaction = UiWidgetGetInteraction(button);
  if (text.size > 0) { UiInteractionMerge(&interaction, UiText(UIID_INT(id), text, V2_ZEROES)); }
  UiWidgetMaybeAnimate(button, &interaction);
  UiWidgetEnd();
  return interaction;
}

UiInteraction UiButtonToggle(U32 id, String8 text, B32* toggled, V2 size) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical |
                            UiWidgetOptions_AlignCenter |
                            UiWidgetOptions_RenderBorder |
                            UiWidgetOptions_RenderRect;
  UiWidgetApplySizeDefaultGrow(&options, size);
  UiWidget* button = UiWidgetBegin(id, options);
  button->pref_size = size;
  UiInteraction interaction = UiWidgetGetInteraction(button);
  UiInteractionMerge(&interaction, UiText(UIID_INT(id), text, V2_ZEROES));
  if (interaction.clicked) { *toggled = !*toggled; }

  UiWidgetMaybeAnimate(button, &interaction);
  if (*toggled) { UiWidgetAnimateTo(button, c->style.widget_color_active, true);  }

  UiWidgetEnd();
  return interaction;
}

UiInteraction UiButtonRadio(U32 id, String8* options, U32 options_size, V2 button_size, V2 child_gap) {
  UiWidgetOptions widget_options = UiWidgetOptions_DirectionVertical |
                                   UiWidgetOptions_SizingFit |
                                   UiWidgetOptions_AlignStart;
  UiWidget* radio_group = UiWidgetBegin(id, widget_options);
  radio_group->child_gap = child_gap.y;
  UiInteraction result;
  MEMORY_ZERO_STRUCT(&result);
  for (U32 i = 0; i < options_size; i++) {
    B32 selected = radio_group->selected == i;
    UiInteraction option_interaction = UiPanelHorizontalBegin(UIID_INT(id + i), V2_ZEROES, child_gap.x);
    UiInteractionMerge(&option_interaction, UiButtonToggle(UIID_INT(id + i), Str8Lit(""), &selected, button_size));
    UiInteractionMerge(&option_interaction, UiGrow(UIID_INT(id + i)));
    UiInteractionMerge(&option_interaction, UiText(UIID_INT(id + i), options[i], V2_ZEROES));
    UiPanelEnd();
    if (option_interaction.clicked) { radio_group->selected = i; }
    UiInteractionMerge(&result, option_interaction);
  }
  UiWidgetEnd();
  result.selected = radio_group->selected;
  return result;
}

UiInteraction UiComboBox(U32 id, String8* options, U32 options_size, V2 size, F32 child_gap) {
  UiContext* c = UiGetContext();
  UiWidgetOptions widget_options = UiWidgetOptions_DirectionHorizontal |
                                   UiWidgetOptions_AlignCenter |
                                   UiWidgetOptions_RenderBorder |
                                   UiWidgetOptions_RenderRect;
  UiWidgetApplySizeDefaultGrow(&widget_options, size);
  UiWidget* combo_box = UiWidgetBegin(id, widget_options);
  combo_box->pref_size = size;
  combo_box->color = c->style.background_color;
  UiInteraction result = UiWidgetGetInteraction(combo_box);

  // NOTE: ensure the open button is square by basing off y dim.
  F32 button_size = 0;
  if (combo_box->cached != NULL) { button_size = combo_box->cached->size.y; }

  UiInteractionMerge(&result, UiPanelHorizontalBegin(UIID_INT(id), V2Assign(5, 5), 5));
  U32 text_id = UIID_INT(id);
  UiInteractionMerge(&result, UiText(text_id, options[combo_box->selected], V2_ZEROES));
  UiInteractionMerge(&result, UiGrow(UIID_INT(id)));
  UiPanelEnd();
  UiInteractionMerge(&result, UiButton(UIID_INT(id), Str8Lit(""), V2Assign(button_size, 0)));

  // NOTE: ensure consistent sizing by setting the text widget's size to the largest option's size.
  V2 max_option_size = V2_ZEROES;
  for (U32 i = 0; i < options_size; i++) {
    String8 option = options[i];
    V2 option_size;
    UiFontMeasureText(option, &option_size);
    max_option_size.x = MAX(max_option_size.x, option_size.x);
    max_option_size.y = MAX(max_option_size.y, option_size.y);
  }
  UiWidget* text_widget = UiWidgetFind(c->root, text_id);
  text_widget->fit_size = max_option_size;

  if (result.clicked) { combo_box->open = !combo_box->open; }
  if (combo_box->open) {
    V2 popup_pos, popup_size;
    popup_pos.x  = combo_box->cached->pos.x;
    popup_pos.y  = combo_box->cached->pos.y - combo_box->cached->size.y + c->style.border_size;
    popup_size.x = combo_box->cached->size.x;
    popup_size.y = 0;
    UiInteractionMerge(&result, UiPopupBegin(UIID_INT(id), popup_pos, popup_size, true));
    UiInteractionMerge(&result, UiPanelVerticalBegin(UIID(), V2Assign(child_gap, child_gap), child_gap));
    for (U32 i = 0; i < options_size; i++) {
      UiInteraction option_interaction = UiPanelHorizontalBegin(UIID_INT(id + i), V2_ZEROES, 0);
      UiInteractionMerge(&option_interaction, UiText(UIID_INT(id + i), options[i], V2_ZEROES));
      UiPanelEnd();
      UiInteractionMerge(&result, option_interaction);
      if (option_interaction.clicked) {
        combo_box->selected = i;
        combo_box->open = false;
        break;
      }
      if (i != options_size - 1) { UiInteractionMerge(&result, UiSeparator(UIID_INT(id + i))); }
    }
    UiPanelEnd();
    UiPopupEnd();
  }
  if (!result.hovered) { combo_box->open = false; }
  UiWidgetEnd();
  return result;
}

UiInteraction UiText(U32 id, String8 text, V2 size) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_RenderText;
  UiWidgetApplySizeDefaultFit(&options, size);
  UiWidget* text_widget = UiWidgetBegin(id, options);
  text_widget->text = Str8Copy(c->widget_arena, text);
  text_widget->pref_size = size;
  UiFontMeasureText(text_widget->text, &text_widget->fit_size);
  UiWidgetEnd();
  return UiWidgetGetInteraction(text_widget);
}

static UiWidget* UiSliderBegin(U32 id, V2 size, F32 resolution, String8 label, UiInteraction* interaction) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical |
                            UiWidgetOptions_AlignCenter |
                            UiWidgetOptions_RenderBorder |
                            UiWidgetOptions_RenderRect;
  UiWidgetApplySizeDefaultGrow(&options, size);
  UiWidget* slider = UiWidgetBegin(id, options);
  slider->pref_size = size;
  *interaction = UiWidgetGetInteraction(slider);
  UiInteractionMerge(interaction, UiText(UIID_INT(id), label, V2_ZEROES));
  if (interaction->dragged) {
    if      (c->mouse_d_pos.x > 0) { slider->slider_value += resolution; }
    else if (c->mouse_d_pos.x < 0) { slider->slider_value -= resolution; }
  }
  UiWidgetMaybeAnimate(slider, interaction);
  return slider;
}

UiInteraction UiSliderF32(U32 id, F32* value, F32 min, F32 max, F32 resolution, V2 size) {
  UiContext* c = UiGetContext();
  UiInteraction result;
  UiWidget* slider = UiSliderBegin(id, size, resolution, Str8Format(c->widget_arena, "%0.2f", *value), &result);
  slider->slider_value = CLAMP(min, slider->slider_value, max);
  *value = slider->slider_value;
  UiWidgetEnd();
  return result;
}

UiInteraction UiSliderU32(U32 id, U32* value, U32 min, U32 max, F32 resolution, V2 size) {
  UiContext* c = UiGetContext();
  UiInteraction result;
  UiWidget* slider = UiSliderBegin(id, size, resolution, Str8Format(c->widget_arena, "%d", *value), &result);
  slider->slider_value = CLAMP(min, slider->slider_value, max);
  *value = slider->slider_value;
  UiWidgetEnd();
  return result;
}

UiInteraction UiSliderS32(U32 id, S32* value, S32 min, S32 max, F32 resolution, V2 size) {
  UiContext* c = UiGetContext();
  UiInteraction result;
  UiWidget* slider = UiSliderBegin(id, size, resolution, Str8Format(c->widget_arena, "%d", *value), &result);
  slider->slider_value = CLAMP(min, slider->slider_value, max);
  *value = slider->slider_value;
  UiWidgetEnd();
  return result;
}

UiInteraction UiPlotLines(U32 id, F32* values, U32 values_count, V2 size) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical |
                            UiWidgetOptions_AlignCenter |
                            UiWidgetOptions_RenderBorder |
                            UiWidgetOptions_RenderRect |
                            UiWidgetOptions_RenderGraphLine;
  UiWidgetApplySizeDefaultGrow(&options, size);
  UiWidget* graph = UiWidgetBegin(id, options);
  graph->pref_size = size;
  graph->color = c->style.background_color;
  graph->graph_values = ARENA_PUSH_ARRAY(c->widget_arena, F32, values_count);
  graph->graph_values_count = values_count;
  MEMORY_MOVE_ARRAY(graph->graph_values, values, values_count);
  UiWidgetEnd();
  return UiWidgetGetInteraction(graph);
}

UiInteraction UiPlotBar(U32 id, F32* values, U32 values_count, V2 size) {
  UiContext* c = UiGetContext();
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical |
                            UiWidgetOptions_AlignCenter |
                            UiWidgetOptions_RenderBorder |
                            UiWidgetOptions_RenderRect |
                            UiWidgetOptions_RenderGraphBar;
  UiWidgetApplySizeDefaultGrow(&options, size);
  UiWidget* graph = UiWidgetBegin(id, options);
  graph->pref_size = size;
  graph->color = c->style.background_color;
  graph->graph_values = ARENA_PUSH_ARRAY(c->widget_arena, F32, values_count);
  graph->graph_values_count = values_count;
  MEMORY_MOVE_ARRAY(graph->graph_values, values, values_count);
  UiWidgetEnd();
  return UiWidgetGetInteraction(graph);
}

#endif // CDEFAULT_UI_IMPLEMENTATION
