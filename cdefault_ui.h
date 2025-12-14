#ifndef CDEFAULT_UI_H_
#define CDEFAULT_UI_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO: text wrapping
// TODO: more std widgets
// TODO: builtin render & text measure if asked, using cdefault renderer & font libs
// TODO: dirty flag on widgets, only recalculate size & pos of self and children if dirty is true

#define UIID_IMPL__(file, line, str) file ":" #line ":" str
#define UIID_IMPL_(file, line, str)  UIID_IMPL__(file, line, str)
#define UIID_IMPL(str)               Str8Hash(Str8Lit(UIID_IMPL_(__FILE__, __LINE__, str)))

#define UIID()      UIID_IMPL("")
#define UIID_STR(s) UIID_IMPL(s)
#define UIID_INT(i) UIID() + i

// NOTE: this struct is how you can respond to different events for each widget.
typedef struct UiInteraction UiInteraction;
struct UiInteraction {
  B8  hovered;
  B8  clicked;
  B8  right_clicked;
  B8  dragged;
  B8  open;
  U32 selected;
};

// NOTE: create & pass this struct to modify the colors, etc. of various UI components.
typedef struct UiStyle UiStyle;
struct UiStyle {
  F32 anim_max_time;
  F32 border_size;
  V3  border_color;
  V3  window_color;
  V3  button_default_color;
  V3  button_hovered_color;
  V3  button_clicked_color;
  V3  text_color;
};
static UiStyle ui_default_style;

// NOTE: this library does not render widgets by default, rather it gives you this, a linked list of
// commands, for you to do it yourself (so that it can be divorced of a particular rendering backend).
typedef struct UiDrawCommand UiDrawCommand;
struct UiDrawCommand {
  UiDrawCommand* prev;
  UiDrawCommand* next;

  enum {
    UiDrawCommand_Rect,
    UiDrawCommand_Text,
  } type;

  union {
    struct {
      V2 center;
      V2 size;
      V3 color;
    } rect;
    struct {
      V2 pos;
      String8 string;
      V3 color;
    } text;
  };
};

// NOTE: callbacks necessary to measure and place text.
typedef void UiTextMeasure_Fn(String8 text, V2* size);
typedef void UiFontGetAttributes_Fn(F32* descent);

// NOTE: initialize / deinitialize the framework. UiInit must be called before any other function.
void UiInit(UiTextMeasure_Fn* ui_text_measure_fn, UiFontGetAttributes_Fn ui_font_get_attributes_fn);
void UiDeinit();

// NOTE: pass mouse data into the UI framework. this is required for UiInteractions to contain expected data.
void UiPointerStateUpdate(V2 mouse_pos, B32 is_lmb_down, B32 is_rmb_down);
// NOTE: get / update the style (colors, etc.) of UI widgets. ui_default_style (static / global) is used as the default.
UiStyle* UiStyleGet();

// NOTE: begin and end calls for determining UI layout position, sizes, etc.
// UiEnd returns a doubly-linked list of instructions for how to render the UI (which can be processed in-order).
void UiBegin(F32 dt_s);
UiDrawCommand* UiEnd();

// NOTE: UI Widget functions, invoke these within UiBegin and UiEnd to express the contents and layout of your UI.
// NOTE: For widgets that allow a size to be specified, you may specify a dimension of 0 to fit to that widget's contents along the given axis.

// NOTE: Windows. Primarily how you specify the location of a container / group of UI widgets.
// Windows cannot be defined as children of other windows.
UiInteraction UiWindowBegin(U32 id, String8 title, V2 pos, V2 size);
void UiWindowEnd();
// NOTE: Popups are specialized windows that can be defined within other windows. They are always rendered on top of everything else.
UiInteraction UiPopupBegin(U32 id, V2 pos, V2 size);
void UiPopupEnd();

// NOTE: Panels. Useful for adding padding between UI widgets within some parent container.
UiInteraction UiPanelVerticalBegin(U32 id, V2 edge_pad, F32 child_pad);
UiInteraction UiPanelHorizontalBegin(U32 id, V2 edge_pad, F32 child_pad);
void UiPanelEnd();

// NOTE: draw a line across the direction axis of the container it's placed in.
UiInteraction UiSeparator(U32 id);

UiInteraction UiButton(U32 id, String8 text, V2 size);
UiInteraction UiButtonToggle(U32 id, String8 text, V2 size, B32* toggled);
UiInteraction UiButtonRadio(U32 id, String8* options, U32 options_size, V2 button_size, V2 child_gap);

UiInteraction UiComboBox(U32 id, String8* options, U32 options_size);

UiInteraction UiText(U32 id, String8 text);

#endif // CDEFAULT_UI_H_

#ifdef CDEFAULT_UI_IMPLEMENTATION
#undef CDEFAULT_UI_IMPLEMENTATION

typedef enum UiWidgetOptions UiWidgetOptions;
enum UiWidgetOptions {
  // NOTE: exactly one of these must be specified for container widgets.
  UiWidgetOptions_DirectionHorizontal = BIT(0),
  UiWidgetOptions_DirectionVertical   = BIT(1),

  // NOTE: exactly one of these must be specified always.
  UiWidgetOptions_SizingHorizFixed    = BIT(2),
  UiWidgetOptions_SizingHorizGrow     = BIT(3),
  UiWidgetOptions_SizingHorizFit      = BIT(4),

  // NOTE: exactly one of these must be specified always.
  UiWidgetOptions_SizingVertFixed     = BIT(5),
  UiWidgetOptions_SizingVertGrow      = BIT(6),
  UiWidgetOptions_SizingVertFit       = BIT(7),

  // NOTE: exactly one of these must be specified for container widgets.
  UiWidgetOptions_AlignHorizStart     = BIT(8),
  UiWidgetOptions_AlignHorizCenter    = BIT(9),
  UiWidgetOptions_AlignHorizEnd       = BIT(10),

  // NOTE: exactly one of these must be specified for container widgets.
  UiWidgetOptions_AlignVertStart      = BIT(11),
  UiWidgetOptions_AlignVertCenter     = BIT(12),
  UiWidgetOptions_AlignVertEnd        = BIT(13),

  // NOTE: optional flags.
  UiWidgetOptions_RenderBorder        = BIT(14),
  UiWidgetOptions_RenderRect          = BIT(15),
  UiWidgetOptions_RenderText          = BIT(16),
};

#define UiWidgetOptions_SizingFixed (UiWidgetOptions_SizingHorizFixed | UiWidgetOptions_SizingVertFixed)
#define UiWidgetOptions_SizingGrow  (UiWidgetOptions_SizingHorizGrow  | UiWidgetOptions_SizingVertGrow)
#define UiWidgetOptions_SizingFit   (UiWidgetOptions_SizingHorizFit   | UiWidgetOptions_SizingVertFit)
#define UiWidgetOptions_AlignStart  (UiWidgetOptions_AlignHorizStart  | UiWidgetOptions_AlignVertStart)
#define UiWidgetOptions_AlignCenter (UiWidgetOptions_AlignHorizCenter | UiWidgetOptions_AlignVertCenter)
#define UiWidgetOptions_AlignEnd    (UiWidgetOptions_AlignHorizEnd    | UiWidgetOptions_AlignVertEnd)

// NOTE: max number of children any single widget may have
#define UI_WIDGET_MAX_CHILDREN 64
typedef struct UiWidget UiWidget;
struct UiWidget {
  U32 id;
  UiWidget* prev;
  UiWidget* parent;
  UiWidget* children[UI_WIDGET_MAX_CHILDREN];
  U32 children_size;

  // NOTE: widget configuration
  UiWidgetOptions options;
  String8 text;
  V2  pad;
  F32 child_gap;
  V2  pref_size;
  V3  text_color;
  V3  rect_color;
  V3  border_color;
  F32 border_size;

  // NOTE: calculated by framework.
  U32 priority; // NOTE: for overlapping window precedence. in range (0, num_windows)
  V2  pos;
  V2  fit_size;
  V2  size;
  B32 is_animating;
  V3  target_rect_color;
  F32 anim_time;
  B32 animation_lock;
  B32 open;
  U32 selected;
};

typedef struct UiContext UiContext;
struct UiContext {
  B32 is_initialized;
  Arena* command_arena;
  Arena* prev_widget_arena;
  Arena* widget_arena;
  Arena* temp_arena;

  U32 hot_id;
  U32 active_id;
  UiStyle style;
  UiWidget* root;
  UiWidget* prev_root;
  UiWidget* widget_free_list;
  UiWidget* current_widget;
  UiTextMeasure_Fn* ui_text_measure_fn;
  UiFontGetAttributes_Fn* ui_font_get_attributes_fn;

  F32 dt_s;
  V2 mouse_pos;
  V2 mouse_d_pos;
  B32 is_lmb_just_pressed;
  B32 is_lmb_down;
  B32 is_rmb_just_pressed;
  B32 is_rmb_down;
};
struct UiContext _ui_context;

static S32 UiWidgetComparePriorityAsc(void* a, void* b) {
  UiWidget* a_widget = *((UiWidget**) a);
  UiWidget* b_widget = *((UiWidget**) b);
  return (S32) a_widget->priority - (S32) b_widget->priority;
}

static void UiWidgetOptionsApplySizeConfig(UiWidgetOptions* options, V2 size) {
  // NOTE: If a dim is specified as 0 along an axis, fit to child contents along that axis.
  if (size.x == 0) { *options |= UiWidgetOptions_SizingHorizFit;   }
  else             { *options |= UiWidgetOptions_SizingHorizFixed; }
  if (size.y == 0) { *options |= UiWidgetOptions_SizingVertFit;    }
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
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidget* widget = ARENA_PUSH_STRUCT(c->widget_arena, UiWidget);
  MEMORY_ZERO_STRUCT(widget);
  return widget;
}

// TODO: better cache?
static UiWidget* UiWidgetFind(UiWidget* widget, U32 id) {
  if (widget == NULL) { return NULL; }
  if (widget->id == id) { return widget; }
  for (U32 i = 0; i < widget->children_size; i++) {
    UiWidget* maybe_hit = UiWidgetFind(widget->children[i], id);
    if (maybe_hit != NULL) { return maybe_hit; }
  }
  return NULL;
}

static void UiWidgetUpdateAnimation(UiWidget* widget) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (!widget->is_animating) { return; }
  F32 anim_max_time = c->style.anim_max_time;
  widget->anim_time += c->dt_s;
  if (widget->anim_time > anim_max_time) {
    widget->rect_color     = widget->target_rect_color;
    widget->is_animating   = false;
    widget->animation_lock = false;
  } else {
    V3Lerp(&widget->rect_color, &widget->rect_color, &widget->target_rect_color, MIN(widget->anim_time / anim_max_time, 1));
  }
}

static void UiWidgetAnimateTo(UiWidget* widget, V3 color, B32 lock) {
  if (widget->animation_lock || V3Eq(&widget->target_rect_color, &color)) { return; }
  widget->target_rect_color = color;
  widget->anim_time = 0;
  widget->is_animating = true;
  widget->animation_lock = lock;
}

static UiInteraction UiWidgetGetInteraction(UiWidget* widget) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);

  // NOTE: determine if the mouse is hovering, etc. by checking the mouse state against the given widget's state last frame.
  // we cannot evaluate the mouse state against the current state, as we don't calculate the final position and size of the
  // widget until later, after UiEnd() is called.
  UiInteraction result;
  MEMORY_ZERO_STRUCT(&result);
  if (widget->prev == NULL) { return result; }

  result.hovered       = c->hot_id == widget->id;
  result.clicked       = result.hovered && c->is_lmb_just_pressed;
  result.right_clicked = result.hovered && c->is_rmb_just_pressed;
  result.dragged       = c->active_id == widget->id;
  result.open          = false;

  return result;
}

static void UiInteractionMerge(UiInteraction* dest, UiInteraction src) {
  dest->hovered       = dest->hovered || src.hovered;
  dest->clicked       = dest->clicked || src.clicked;
  dest->right_clicked = dest->right_clicked || src.right_clicked;
  dest->dragged       = dest->dragged || src.dragged;
  dest->open          = dest->open || src.open;
}

static B32 UiWidgetOptionsValidate(UiWidgetOptions options) {
#define UI_CAST(x) (x?1:0)
  B32 dir_horiz = options & UiWidgetOptions_DirectionHorizontal;
  B32 dir_vert  = options & UiWidgetOptions_DirectionVertical;
  U32 dir_count = UI_CAST(dir_horiz) + UI_CAST(dir_vert);
  DEBUG_ASSERT(dir_count == 0 || dir_count == 1);

  B32 size_horiz_fixed = options & UiWidgetOptions_SizingHorizFixed;
  B32 size_horiz_grow  = options & UiWidgetOptions_SizingHorizGrow;
  B32 size_horiz_fit   = options & UiWidgetOptions_SizingHorizFit;
  U32 size_horiz_count = UI_CAST(size_horiz_fixed) + UI_CAST(size_horiz_grow) + UI_CAST(size_horiz_fit);
  DEBUG_ASSERT(size_horiz_count == 1);

  B32 size_vert_fixed = options & UiWidgetOptions_SizingVertFixed;
  B32 size_vert_grow  = options & UiWidgetOptions_SizingVertGrow;
  B32 size_vert_fit   = options & UiWidgetOptions_SizingVertFit;
  U32 size_vert_count = UI_CAST(size_vert_fixed) + UI_CAST(size_vert_grow) + UI_CAST(size_vert_fit);
  DEBUG_ASSERT(size_vert_count == 1);

  return true;
#undef UI_CAST
}

static UiWidget* UiWidgetBegin(U32 id, UiWidgetOptions options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  DEBUG_ASSERT(c->current_widget->children_size < UI_WIDGET_MAX_CHILDREN);

  UiWidget* new_widget = UiWidgetAllocate();
  new_widget->id       = id;
  new_widget->parent   = c->current_widget;

  DEBUG_ASSERT(UiWidgetOptionsValidate(options));
  new_widget->options      = options;
  new_widget->text_color   = c->style.text_color;
  new_widget->border_color = c->style.border_color;
  new_widget->border_size  = c->style.border_size;

  // NOTE: sync data from last frame
  new_widget->prev = UiWidgetFind(c->prev_root, id);
  if (new_widget->prev != NULL) {
    new_widget->priority          = new_widget->prev->priority;
    new_widget->pos               = new_widget->prev->pos;
    new_widget->open              = new_widget->prev->open;
    new_widget->selected          = new_widget->prev->selected;
    new_widget->is_animating      = new_widget->prev->is_animating;
    new_widget->rect_color        = new_widget->prev->rect_color;
    new_widget->target_rect_color = new_widget->prev->target_rect_color;
    new_widget->anim_time         = new_widget->prev->anim_time;
    new_widget->animation_lock    = new_widget->prev->animation_lock;
    UiWidgetUpdateAnimation(new_widget);
  }

  c->current_widget->children[c->current_widget->children_size++] = new_widget;
  c->current_widget = new_widget;
  return c->current_widget;
}

static void UiWidgetEnd() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  c->current_widget = c->current_widget->parent;
}

static void UiSizeWidgets(UiWidget* widget, UiWidgetOptions direction) {
  // NOTE: size starting at the bottom and moving up.
  for (U32 i = 0; i < widget->children_size; i++) {
    UiSizeWidgets(widget->children[i], direction);
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
  if      (widget_size_opt & UiWidgetOptions_SizingFit)   { *widget_size_dim = *widget_fit_size_dim;                            }
  else if (widget_size_opt & UiWidgetOptions_SizingGrow)  { *widget_size_dim = MAX(*widget_fit_size_dim, widget_pref_size_dim); }
  else if (widget_size_opt & UiWidgetOptions_SizingFixed) { *widget_size_dim = widget_pref_size_dim;                            }
  else { UNREACHABLE(); }

  // NOTE: now, apply final size to parent
  F32* parent_fit_size_dim  = UiWidgetGetDim(&parent->fit_size, direction);
  if (UiWidgetGetDirOpt(parent) == direction) { *parent_fit_size_dim += *widget_size_dim; }
  else { *parent_fit_size_dim = MAX(*parent_fit_size_dim, *widget_size_dim); }
}

static void UiGrowWidgets(UiWidget* widget, UiWidgetOptions direction) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);

  // NOTE: collect all children that can grow. they need to share the growth delta.
  // TODO: cache this on the widget?
  UiWidget* growable_children[UI_WIDGET_MAX_CHILDREN];
  U32 growable_children_size = 0;
  for (U32 i = 0; i < widget->children_size; i++) {
    UiWidget* child = widget->children[i];
    if (UiWidgetGetSizeOpt(child, direction) & UiWidgetOptions_SizingGrow) {
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
      // TODO: merge with shrink logic
      while (delta > 0) {
        // NOTE: find the 2 smallest sets of widgets.
        F32 smallest_size_dim = *UiWidgetGetDim(&growable_children[0]->size, direction);
        F32 second_smallest_size_dim = 0;
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
        // NOTE: nothing more we can grow, give up.
        if (smallest_size_dim == F32_MAX) { break; }

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
      // TODO: merge with grow logic
      while (delta < 0) {
        // NOTE: find the 2 largest sets of widgets.
        F32 largest_size_dim = *UiWidgetGetDim(&growable_children[0]->size, direction);
        F32 second_largest_size_dim = 0;
        for (U32 i = 1; i < growable_children_size; i++) {
          UiWidget* child = growable_children[i];
          F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
          if (child_size_dim > largest_size_dim) {
            second_largest_size_dim = largest_size_dim;
            largest_size_dim = child_size_dim;
          } else if (child_size_dim > second_largest_size_dim) {
            second_largest_size_dim = child_size_dim;
          }
        }
        // NOTE: nothing more we can shrink, give up.
        if (F32ApproxEq(largest_size_dim, 0)) { break; }

        // NOTE: shrink the largest widgets so they're as big as the second largest
        F32 size_delta = MIN(largest_size_dim - second_largest_size_dim, -delta);
        if (F32ApproxEq(size_delta, 0)) { size_delta = -delta / growable_children_size; }
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

  for (U32 i = 0; i < widget->children_size; i++) {
    UiGrowWidgets(widget->children[i], direction);
  }
}

static void UiPositionWidgets(UiWidget* widget, F32 pos, UiWidgetOptions direction) {
  F32* widget_pos_dim = UiWidgetGetDim(&widget->pos, direction);
  *widget_pos_dim = pos;

  F32 widget_pad_dim = *UiWidgetGetDim(&widget->pad, direction);
  UiWidgetUpdatePos(&pos, widget_pad_dim, direction);

  // NOTE: calculate final (including growth) size of contents
  F32 content_size_dim = 0;
  for (U32 i = 0; i < widget->children_size; i++) {
    UiWidget* child = widget->children[i];
    F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
    if (UiWidgetGetDirOpt(widget) == direction) {
      content_size_dim += child_size_dim;
      if (i != widget->children_size - 1) { content_size_dim += widget->child_gap; }
    } else {
      content_size_dim = MAX(content_size_dim, child_size_dim);
    }
  }
  content_size_dim += 2 * widget_pad_dim;

  // NOTE: determine alignment offset by moving child content relative to the empty space between
  // the widget's determined size and the calculated size of its contents
  UiWidgetOptions align = UiWidgetGetAlignOpt(widget, direction);
  if (!(align & UiWidgetOptions_AlignStart)) {
    F32 widget_size_dim = *UiWidgetGetDim(&widget->size, direction);
    if (align & UiWidgetOptions_AlignCenter)   { UiWidgetUpdatePos(&pos, (widget_size_dim / 2.0f) - (content_size_dim / 2.0f), direction); }
    else if (align & UiWidgetOptions_AlignEnd) { UiWidgetUpdatePos(&pos, widget_size_dim - content_size_dim, direction);                   }
    else { DEBUG_ASSERT(widget->children_size == 0); }
  }

  // NOTE: position this nodes children relative to itself
  for (U32 i = 0; i < widget->children_size; i++) {
    UiWidget* child = widget->children[i];
    UiPositionWidgets(child, pos, direction);
    if (UiWidgetGetDirOpt(widget) == direction) {
      F32 child_size_dim = *UiWidgetGetDim(&child->size, direction);
      F32 pos_delta = child_size_dim + widget->child_gap;
      UiWidgetUpdatePos(&pos, pos_delta, direction);
    }
  }
}

// TODO: can these be merged
static void UiPositionWidgetsRoot(UiWidget* widget, UiWidgetOptions direction) {
  F32* widget_pos_dim = UiWidgetGetDim(&widget->pos, direction);
  UiPositionWidgets(widget, *widget_pos_dim, direction);
}

static B32 UiMaybeUpdateHot(UiWidget* widget) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);

  // NOTE: correct coords, pos is top left.
  V2 m   = c->mouse_pos;
  V2 min = widget->pos;
  V2 max = widget->pos;
  min.y -= widget->size.y;
  max.x += widget->size.x;
  if (min.x <= m.x && m.x <= max.x &&
      min.y <= m.y && m.y <= max.y) {
    c->hot_id = widget->id;
    // NOTE: only need to check children if we're within the bounds of the current widget.
    for (U32 i = 0; i < widget->children_size; i++) {
      UiMaybeUpdateHot(widget->children[i]);
    }
    return true;
  }
  return false;
}

static void UiEnqueueDrawCommands(UiWidget* widget, UiDrawCommand** head, UiDrawCommand** tail) {
  UiContext* c = &_ui_context;
  UiDrawCommand* command;

  if (widget->options & UiWidgetOptions_RenderBorder && widget->border_size > 0) {
    command = ARENA_PUSH_STRUCT(c->command_arena, UiDrawCommand);
    DLL_PUSH_BACK(*head, *tail, command, prev, next);
    command->type = UiDrawCommand_Rect;
    command->rect.center.x = widget->pos.x + widget->size.x / 2.0f;
    command->rect.center.y = widget->pos.y - widget->size.y / 2.0f;
    command->rect.size     = widget->size;
    command->rect.color    = widget->border_color;
  }

  if (widget->options & UiWidgetOptions_RenderRect) {
    command = ARENA_PUSH_STRUCT(c->command_arena, UiDrawCommand);
    DLL_PUSH_BACK(*head, *tail, command, prev, next);
    command->type = UiDrawCommand_Rect;
    command->rect.center.x = widget->pos.x + widget->size.x / 2.0f;
    command->rect.center.y = widget->pos.y - widget->size.y / 2.0f;
    command->rect.size     = widget->size;
    command->rect.size.x   -= widget->border_size * 2;
    command->rect.size.y   -= widget->border_size * 2;
    command->rect.color    = widget->rect_color;
  }

  if (widget->options & UiWidgetOptions_RenderText) {
    F32 font_descent;
    c->ui_font_get_attributes_fn(&font_descent);
    command = ARENA_PUSH_STRUCT(c->command_arena, UiDrawCommand);
    DLL_PUSH_BACK(*head, *tail, command, prev, next);
    command->type = UiDrawCommand_Text;
    command->text.string = widget->text;
    command->text.pos = widget->pos;
    command->text.pos.y -= (widget->size.y - font_descent);
    command->text.color = widget->text_color;
  }

  for (U32 i = 0; i < widget->children_size; i++) {
    UiEnqueueDrawCommands(widget->children[i], head, tail);
  }
}

void UiInit(UiTextMeasure_Fn* ui_text_measure_fn, UiFontGetAttributes_Fn ui_font_get_attributes_fn) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(!c->is_initialized);
  c->widget_arena = ArenaAllocate();
  c->prev_widget_arena = ArenaAllocate();
  c->command_arena = ArenaAllocate();
  c->temp_arena = ArenaAllocate();
  c->ui_text_measure_fn = ui_text_measure_fn;
  c->ui_font_get_attributes_fn = ui_font_get_attributes_fn;
  c->is_initialized = true;

  ui_default_style.anim_max_time        = 0.07f;
  ui_default_style.border_size          = 1.0f;
  ui_default_style.border_color         = V3_MOONFLY_DARK_GRAY;
  ui_default_style.window_color         = V3_MOONFLY_BLACK;
  ui_default_style.button_default_color = V3_MOONFLY_BLUE;
  ui_default_style.button_hovered_color = V3_MOONFLY_LIGHT_BLUE;
  ui_default_style.button_clicked_color = V3_MOONFLY_LIGHT_RED;
  ui_default_style.text_color           = V3_MOONFLY_WHITE;
  *UiStyleGet() = ui_default_style;
}

void UiDeinit() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  ArenaRelease(c->widget_arena);
  ArenaRelease(c->prev_widget_arena);
  ArenaRelease(c->command_arena);
  ArenaRelease(c->temp_arena);
  c->is_initialized = false;
}

void UiPointerStateUpdate(V2 mouse_pos, B32 is_lmb_down, B32 is_rmb_down) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  V2SubV2(&c->mouse_d_pos, &mouse_pos, &c->mouse_pos);
  c->mouse_pos = mouse_pos;
  c->is_lmb_just_pressed = !c->is_lmb_down && is_lmb_down;
  c->is_rmb_just_pressed = !c->is_rmb_down && is_rmb_down;
  c->is_lmb_down = is_lmb_down;
  c->is_rmb_down = is_rmb_down;
}

UiStyle* UiStyleGet() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  return &c->style;
}

void UiBegin(F32 dt_s) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  DEBUG_ASSERT(c->root == NULL);
  DEBUG_ASSERT(c->current_widget == NULL);
  c->root = UiWidgetAllocate();
  c->root->options  = ~0; // NOTE: don't trip flag assertions on the root / null widget.
  c->current_widget = c->root;
  c->dt_s = dt_s;
}

UiDrawCommand* UiEnd() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  DEBUG_ASSERT(c->current_widget == c->root);

  // NOTE: order windows by priority / draw precedence
  SORT(UiWidget*, c->root->children, c->root->children_size, UiWidgetComparePriorityAsc);

  // NOTE: perform passes over all windows (to position, size, enqueue draw cmds)
  c->hot_id = 0;
  ArenaClear(c->command_arena);
  UiDrawCommand* commands_head = NULL;
  UiDrawCommand* commands_tail = NULL;
  UiWidget* hot_root_widget    = NULL;
  for (U32 i = 0; i < c->root->children_size; i++) {
    UiWidget* root = c->root->children[i];
    root->priority = i; // NOTE: fix priority so in range (0, num windows)
    UiSizeWidgets(root, UiWidgetOptions_DirectionHorizontal);
    UiSizeWidgets(root, UiWidgetOptions_DirectionVertical);
    UiGrowWidgets(root, UiWidgetOptions_DirectionHorizontal);
    UiGrowWidgets(root, UiWidgetOptions_DirectionVertical);
    UiPositionWidgetsRoot(root, UiWidgetOptions_DirectionHorizontal);
    UiPositionWidgetsRoot(root, UiWidgetOptions_DirectionVertical);
    if (UiMaybeUpdateHot(root)) { hot_root_widget = root; }
    UiEnqueueDrawCommands(root, &commands_head, &commands_tail);
  }

  // NOTE: update active widget if just clicked
  if (c->is_lmb_just_pressed && c->active_id == 0 && hot_root_widget != NULL) {
    c->active_id = c->hot_id;
    // NOTE: boost priority of widget's window, to affect next frame draw order / precedence
    hot_root_widget->priority = c->root->children_size;
  } else if (!c->is_lmb_down) {
    c->active_id = 0;
  }

  // NOTE: move current widget tree to prev for next frame's lookups
  ArenaClear(c->prev_widget_arena);
  SWAP(Arena*, c->prev_widget_arena, c->widget_arena);
  c->prev_root = c->root;
  c->root = NULL;
  c->current_widget = NULL;

  return commands_head;
}

UiInteraction UiWindowBegin(U32 id, String8 title, V2 pos, V2 size) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  DEBUG_ASSERT(c->current_widget == c->root);
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignCenter | UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect;
  UiWidgetOptionsApplySizeConfig(&options, size);
  UiWidget* window = UiWidgetBegin(id, options);
  if (window->prev == NULL) {
    window->pos = pos;
    window->open = true;
    if (c->prev_root != NULL) { window->priority = c->prev_root->children_size; }
    else                      { window->priority = c->root->children_size;      }
  }
  window->rect_color = c->style.window_color;
  window->pref_size = size;
  UiInteraction result = UiWidgetGetInteraction(window);
  if (title.size > 0) {
    UiInteractionMerge(&result, UiPanelHorizontalBegin(UIID_INT(id), V2Assign(5, 5), 0));
      UiInteractionMerge(&result, UiText(UIID_INT(id), title));
        UiInteractionMerge(&result, UiPanelHorizontalBegin(UIID_INT(id), V2Assign(5, 0), 0));
        UiPanelEnd();
      UiInteractionMerge(&result, UiButtonToggle(UIID_INT(id), Str8Lit(""), V2Assign(20, 20), &window->open));
    UiPanelEnd();
    UiInteractionMerge(&result, UiSeparator(UIID_INT(id)));
  }
  if (result.dragged) { V2AddV2(&window->pos, &window->pos, &c->mouse_d_pos); }
  result.open = window->open;
  return result;
}

void UiWindowEnd() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetEnd();
  DEBUG_ASSERT(c->current_widget == c->root);
}

UiInteraction UiPopupBegin(U32 id, V2 pos, V2 size) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignCenter | UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect;
  UiWidgetOptionsApplySizeConfig(&options, size);
  UiWidget* popup = UiWidgetBegin(id, options);
  popup->pos = pos;
  popup->rect_color = c->style.window_color;
  popup->pref_size = size;
  popup->priority = S32_MAX;
  return UiWidgetGetInteraction(popup);
}

void UiPopupEnd() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidget* popup = c->current_widget;
  UiWidgetEnd();
  UiWidget* parent = popup->parent;
  if (parent != c->root) {
    // NOTE: popups are windows, move them to be children of the root widget.
    B32 removed_popup_from_non_root_parent = false;
    for (U32 i = 0; i < parent->children_size; i++) {
      if (parent->children[i] == popup) {
        DA_SHIFT_REMOVE_EX(parent->children, parent->children_size, i);
        removed_popup_from_non_root_parent = true;
        break;
      }
    }
    DEBUG_ASSERT(removed_popup_from_non_root_parent);
    popup->parent = c->root;
    c->root->children[c->root->children_size++] = popup;
  }
}

UiInteraction UiPanelVerticalBegin(U32 id, V2 pad, F32 child_gap) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignCenter | UiWidgetOptions_SizingHorizFit | UiWidgetOptions_SizingVertGrow;
  UiWidget* panel = UiWidgetBegin(id, options);
  panel->pad = pad;
  panel->child_gap = child_gap;
  return UiWidgetGetInteraction(panel);
}

UiInteraction UiPanelHorizontalBegin(U32 id, V2 pad, F32 child_gap) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_DirectionHorizontal | UiWidgetOptions_AlignCenter | UiWidgetOptions_SizingHorizGrow | UiWidgetOptions_SizingVertFit;
  UiWidget* panel = UiWidgetBegin(id, options);
  panel->pad = pad;
  panel->child_gap = child_gap;
  return UiWidgetGetInteraction(panel);
}

void UiPanelEnd() {
  UiWidgetEnd();
}

UiInteraction UiSeparator(U32 id) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect;
  UiWidgetOptions parent_dir = UiWidgetGetDirOpt(c->current_widget);
  if      (parent_dir == UiWidgetOptions_DirectionVertical)   { options |= UiWidgetOptions_SizingHorizGrow  | UiWidgetOptions_SizingVertFixed; }
  else if (parent_dir == UiWidgetOptions_DirectionHorizontal) { options |= UiWidgetOptions_SizingHorizFixed | UiWidgetOptions_SizingVertGrow;  }
  else { UNREACHABLE(); }
  UiWidget* separator = UiWidgetBegin(id, options);
  separator->rect_color = c->style.border_color;
  separator->pref_size = V2Assign(c->style.border_size, c->style.border_size);
  UiWidgetEnd();
  return UiWidgetGetInteraction(separator);
}

UiInteraction UiButton(U32 id, String8 text, V2 size) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignCenter | UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect;
  UiWidgetOptionsApplySizeConfig(&options, size);
  UiWidget* button = UiWidgetBegin(id, options);
  if (button->prev == NULL) { button->rect_color = c->style.button_default_color; }
  button->pref_size = size;
  UiInteraction interaction = UiWidgetGetInteraction(button);
  UiInteractionMerge(&interaction, UiText(UIID_INT(id), text));
  if (interaction.clicked)      { UiWidgetAnimateTo(button, c->style.button_clicked_color, true); }
  else if (interaction.hovered) { UiWidgetAnimateTo(button, c->style.button_hovered_color, false); }
  else                          { UiWidgetAnimateTo(button, c->style.button_default_color, false); }
  UiWidgetEnd();
  return interaction;
}

UiInteraction UiButtonToggle(U32 id, String8 text, V2 size, B32* toggled) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_AlignCenter | UiWidgetOptions_RenderBorder | UiWidgetOptions_RenderRect;
  UiWidgetOptionsApplySizeConfig(&options, size);
  UiWidget* button = UiWidgetBegin(id, options);
  if (button->prev == NULL) { button->rect_color = c->style.button_default_color; }
  button->pref_size = size;
  UiInteraction interaction = UiWidgetGetInteraction(button);
  UiInteractionMerge(&interaction, UiText(UIID_INT(id), text));
  if (interaction.clicked)      { *toggled = !*toggled; }
  if (*toggled)                 { UiWidgetAnimateTo(button, c->style.button_clicked_color, true); }
  else if (interaction.hovered) { UiWidgetAnimateTo(button, c->style.button_hovered_color, false); }
  else                          { UiWidgetAnimateTo(button, c->style.button_default_color, false); }
  UiWidgetEnd();
  return interaction;
}

UiInteraction UiButtonRadio(U32 id, String8* options, U32 options_size, V2 button_size, V2 child_gap) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions widget_options = UiWidgetOptions_DirectionVertical | UiWidgetOptions_SizingFit | UiWidgetOptions_AlignStart;
  UiWidget* radio_group = UiWidgetBegin(id, widget_options);
  radio_group->child_gap = child_gap.y;
  UiInteraction result;
  MEMORY_ZERO_STRUCT(&result);
  for (U32 i = 0; i < options_size; i++) {
    B32 selected = radio_group->selected == i;
    UiInteractionMerge(&result, UiPanelHorizontalBegin(UIID_INT(id + i), V2_ZEROES, child_gap.x));
    UiInteractionMerge(&result, UiButtonToggle(UIID_INT(id + i), Str8Lit(""), button_size, &selected));
    UiInteractionMerge(&result, UiText(UIID_INT(id + i), options[i]));
    UiPanelEnd();
    if (selected) { radio_group->selected = i; }
  }
  UiWidgetEnd();
  result.selected = radio_group->selected;
  return result;
}

UiInteraction UiComboBox(U32 id, String8* options, U32 options_size) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions widget_options = UiWidgetOptions_DirectionHorizontal | UiWidgetOptions_SizingFit | UiWidgetOptions_AlignStart;
  UiWidget* combo_box = UiWidgetBegin(id, widget_options);
  UiInteraction result = UiWidgetGetInteraction(combo_box);
  UiInteractionMerge(&result, UiText(UIID_INT(id), options[combo_box->selected]));
  UiInteraction open_button = UiButton(UIID_INT(id), Str8Lit(""), V2Assign(20, 20));
  UiInteractionMerge(&result, open_button);
  if (open_button.clicked) { combo_box->open = true; }
  if (combo_box->open) {
    V2 popup_pos, popup_size;
    popup_pos.x  = combo_box->prev->pos.x;
    popup_pos.y  = combo_box->prev->pos.y - combo_box->prev->size.y;
    popup_size.x = combo_box->prev->size.x;
    popup_size.y = 0;
    UiInteractionMerge(&result, UiPopupBegin(UIID_INT(id), popup_pos, popup_size));
    for (U32 i = 0; i < options_size; i++) {
      UiInteraction button = UiButton(UIID_INT(id + i), options[i], V2_ZEROES);
      UiInteractionMerge(&result, button);
      if (button.clicked) {
        combo_box->selected = i;
        combo_box->open = false;
        break;
      }
    }
    UiPopupEnd();
  }
  if (!result.hovered) { combo_box->open = false; }
  UiWidgetEnd();
  return result;
}

UiInteraction UiText(U32 id, String8 text) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  UiWidgetOptions options = UiWidgetOptions_SizingFit | UiWidgetOptions_RenderText;
  UiWidget* text_widget = UiWidgetBegin(id, options);
  text_widget->text = Str8Copy(c->widget_arena, text);
  c->ui_text_measure_fn(text_widget->text, &text_widget->fit_size);
  UiWidgetEnd();
  return UiWidgetGetInteraction(text_widget);
}

#endif // CDEFAULT_UI_IMPLEMENTATION
