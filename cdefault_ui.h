#ifndef CDEFAULT_UI_H_
#define CDEFAULT_UI_H_

#include "cdefault_std.h"
#include "cdefault_math.h"

// TODO: text wrapping
// TODO: more std widgets
// TODO: builtin render & text measure if asked, using cdefault renderer & font libs

#define UIID_IMPL__(file, line, str) file ":" #line ":" str
#define UIID_IMPL_(file, line, str)  UIID_IMPL__(file, line, str)
#define UIID_IMPL(str)               Str8Hash(Str8Lit(UIID_IMPL_(__FILE__, __LINE__, str)))

#define UIID_STR(s) UIID_IMPL(s)
#define UIID_INT(i) UIID_STR(#i)
#define UIID()      UIID_STR("")

typedef enum UiWidgetOptions UiWidgetOptions;
enum UiWidgetOptions {
  // NOTE: exactly one of these must be specified.
  UiWidgetOptions_DirectionHorizontal = BIT(0),
  UiWidgetOptions_DirectionVertical   = BIT(1),

  // NOTE: exactly one of these must be specified.
  UiWidgetOptions_SizingHorizFixed    = BIT(2),
  UiWidgetOptions_SizingHorizGrow     = BIT(3),
  UiWidgetOptions_SizingHorizFit      = BIT(4),

  // NOTE: exactly one of these must be specified.
  UiWidgetOptions_SizingVertFixed     = BIT(5),
  UiWidgetOptions_SizingVertGrow      = BIT(6),
  UiWidgetOptions_SizingVertFit       = BIT(7),

  // NOTE: optional flags.
  UiWidgetOptions_IsInvisible         = BIT(8),
};

#define UiWidgetOptions_SizingFixed UiWidgetOptions_SizingHorizFixed | UiWidgetOptions_SizingVertFixed
#define UiWidgetOptions_SizingGrow  UiWidgetOptions_SizingHorizGrow | UiWidgetOptions_SizingVertGrow
#define UiWidgetOptions_SizingFit   UiWidgetOptions_SizingHorizFit | UiWidgetOptions_SizingVertFit

// NOTE: can pass 0 to relevant UI functions, defaults will automatically be used.
#define UI_WINDOW_DEFAULT_OPTS UiWidgetOptions_DirectionVertical | UiWidgetOptions_SizingFit
#define UI_PANEL_DEFAULT_OPTS  UiWidgetOptions_DirectionVertical | UiWidgetOptions_SizingGrow | UiWidgetOptions_IsInvisible
#define UI_BUTTON_DEFAULT_OPTS UiWidgetOptions_DirectionVertical | UiWidgetOptions_SizingFixed
#define UI_LABEL_DEFAULT_OPTS  UiWidgetOptions_DirectionVertical | UiWidgetOptions_SizingGrow

// NOTE: this struct is how you can respond to different events for each widget.
typedef struct UiInteraction UiInteraction;
struct UiInteraction {
  B32 hovered;
  B32 clicked;
  B32 right_clicked;
  B32 dragged;
};

// NOTE: create & pass this struct to modify the colors, etc. of various UI components.
typedef struct UiStyle UiStyle;
struct UiStyle {
  F32 anim_max_time;
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

// NOTE: callback necessary to measure and place text.
typedef V2 UiMeasureText_Fn(String8 text);

// NOTE: initialize / deinitialize the framework. UiInit must be called before any other function.
void UiInit(UiMeasureText_Fn* ui_measure_text_fn);
void UiDeinit();

// NOTE: pass mouse data into the UI framework. this is required for UiInteractions to contain expected data.
void UiPointerStateUpdate(V2 pos, B32 is_lmb_down, B32 is_rmb_down);
// NOTE: get / update the style (colors, etc.) of UI widgets. ui_default_style (static / global) is used as the default.
UiStyle* UiStyleGet();

// NOTE: begin and end calls for determining UI layout position, sizes, etc.
// UiEnd returns a doubly-linked list of instructions for how to render the UI (which can be processed in-order).
void UiBegin(F32 dt_s);
UiDrawCommand* UiEnd();

// NOTE: Windows. Primarily how you specify the location of a container / group of UI widgets.
UiInteraction UiWindowFitBegin(U32 id, V2 pos, U32 options);
UiInteraction UiWindowSizedBegin(U32 id, V2 pos, V2 size, U32 options);
void UiWindowEnd();

// NOTE: Panels. Useful for adding padding between UI widgets within some parent container.
UiInteraction UiPanelVerticalBegin(U32 id, V2 edge_pad, F32 child_pad, U32 options);
UiInteraction UiPanelHorizontalBegin(U32 id, V2 edge_pad, F32 child_pad, U32 options);
void UiPanelEnd();

UiInteraction UiButton(U32 id, V2 size, U32 options);

UiInteraction UiLabel(U32 id, String8 text, U32 options);

#endif // CDEFAULT_UI_H_

#ifdef CDEFAULT_UI_IMPLEMENTATION
#undef CDEFAULT_UI_IMPLEMENTATION

typedef enum UiWidgetSizing UiWidgetSizing;
enum UiWidgetSizing {
  UiWidgetSizing_Fit,
  UiWidgetSizing_Grow,
  UiWidgetSizing_Fixed,
};

typedef enum UiWidgetDirection UiWidgetDirection;
enum UiWidgetDirection {
  UiWidgetDirection_Vertical,
  UiWidgetDirection_Horizontal,
};

// NOTE: max number of children any single widget may have
#define UI_WIDGET_MAX_CHILDREN 64
typedef struct UiWidget UiWidget;
struct UiWidget {
  U32 id;
  UiWidget* parent;
  UiWidget* children[UI_WIDGET_MAX_CHILDREN];
  UiWidget* prev;
  U32 children_size;

  // NOTE: widget configuration
  UiWidgetSizing sizing[2];
  UiWidgetDirection direction;
  String8 text;
  V2  pad;
  F32 child_gap;
  V2  pref_size;
  B32 is_invisible;
  V3  color;

  // NOTE: calculated by framework.
  V2  pos;
  V2  fit_size;
  V2  size;
  B32 is_animating;
  V3  target_color;
  F32 anim_time;
  B32 animation_lock;
};

typedef struct UiContext UiContext;
struct UiContext {
  B32 is_initialized;
  Arena* command_arena;
  Arena* prev_widget_arena;
  Arena* widget_arena;
  Arena* temp_arena;

  UiStyle style;
  UiWidget* root;
  UiWidget* prev_root;
  UiWidget* widget_free_list;
  UiWidget* current_widget;
  UiMeasureText_Fn* ui_measure_text_fn;

  F32 dt_s;
  V2 mouse_pos;
  B32 is_lmb_just_pressed;
  B32 is_lmb_down;
  B32 is_rmb_just_pressed;
  B32 is_rmb_down;
};
struct UiContext _ui_context;

static void UiWidgetUpdatePos(F32* pos, F32 delta, UiWidgetDirection direction) {
  // NOTE: pos should increase in x direction and decrease in y direction.
  // (assuming that the origin is at the bottom left of the screen!)
  switch (direction) {
    case UiWidgetDirection_Horizontal: { *pos += delta; } break;
    case UiWidgetDirection_Vertical:   { *pos -= delta; } break;
    default: UNREACHABLE();
  }
}

static F32* UiWidgetGetDim(V2* v2, UiWidgetDirection direction) {
  switch (direction) {
    case UiWidgetDirection_Horizontal: { return &v2->x; }
    case UiWidgetDirection_Vertical:   { return &v2->y; }
    default: UNREACHABLE(); return NULL;
  }
}

static UiWidgetSizing UiWidgetGetSizing(UiWidget* widget, UiWidgetDirection direction) {
  switch (direction) {
    case UiWidgetDirection_Horizontal: { return widget->sizing[0]; }
    case UiWidgetDirection_Vertical:   { return widget->sizing[1]; }
    default: UNREACHABLE(); return 0;
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
    widget->color = widget->target_color;
    widget->is_animating   = false;
    widget->animation_lock = false;
  } else {
    V3Lerp(&widget->color, &widget->color, &widget->target_color, MIN(widget->anim_time / anim_max_time, 1));
  }
}

static void UiWidgetAnimateTo(UiWidget* widget, V3 color, B32 lock) {
  if (V3Eq(&widget->target_color, &color) || widget->animation_lock) { return; }
  widget->target_color = color;
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

  // NOTE: correct coords, pos is top left.
  V2 min, max;
  min.x = widget->prev->pos.x;
  min.y = widget->prev->pos.y - widget->prev->size.y;
  max.x = widget->prev->pos.x + widget->prev->size.x;
  max.y = widget->prev->pos.y;

  B32 is_hovering = min.x <= c->mouse_pos.x && c->mouse_pos.x <= max.x &&
                    min.y <= c->mouse_pos.y && c->mouse_pos.y <= max.y;
  result.hovered = is_hovering;
  result.clicked = is_hovering && c->is_lmb_just_pressed;
  result.right_clicked = is_hovering && c->is_lmb_just_pressed;

  return result;
}

static B32 UiWidgetOptionsValidate(U32 options) {
#define UI_CAST(x) (x?1:0)
  B32 dir_horiz = options & UiWidgetOptions_DirectionHorizontal;
  B32 dir_vert  = options & UiWidgetOptions_DirectionVertical;
  DEBUG_ASSERT(UI_CAST(dir_horiz) + UI_CAST(dir_vert) == 1);

  B32 size_horiz_fixed = options & UiWidgetOptions_SizingHorizFixed;
  B32 size_horiz_grow  = options & UiWidgetOptions_SizingHorizGrow;
  B32 size_horiz_fit   = options & UiWidgetOptions_SizingHorizFit;
  DEBUG_ASSERT(UI_CAST(size_horiz_fixed) + UI_CAST(size_horiz_grow) + UI_CAST(size_horiz_fit) == 1);

  B32 size_vert_fixed = options & UiWidgetOptions_SizingVertFixed;
  B32 size_vert_grow  = options & UiWidgetOptions_SizingVertGrow;
  B32 size_vert_fit   = options & UiWidgetOptions_SizingVertFit;
  DEBUG_ASSERT(UI_CAST(size_vert_fixed) + UI_CAST(size_vert_grow) + UI_CAST(size_vert_fit) == 1);

  return true;
#undef UI_CAST
}

static UiWidget* UiWidgetBegin(U32 id, V3 color, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  DEBUG_ASSERT(c->current_widget->children_size < UI_WIDGET_MAX_CHILDREN);

  UiWidget* new_widget = UiWidgetAllocate();
  new_widget->id     = id;
  new_widget->parent = c->current_widget;

  DEBUG_ASSERT(UiWidgetOptionsValidate(options));
  if      (options & UiWidgetOptions_DirectionHorizontal) { new_widget->direction = UiWidgetDirection_Horizontal; }
  else if (options & UiWidgetOptions_DirectionVertical)   { new_widget->direction = UiWidgetDirection_Vertical;   }
  if      (options & UiWidgetOptions_SizingHorizFixed)    { new_widget->sizing[0] = UiWidgetSizing_Fixed;         }
  else if (options & UiWidgetOptions_SizingHorizGrow)     { new_widget->sizing[0] = UiWidgetSizing_Grow;          }
  else if (options & UiWidgetOptions_SizingHorizFit)      { new_widget->sizing[0] = UiWidgetSizing_Fit;           }
  if      (options & UiWidgetOptions_SizingVertFixed)     { new_widget->sizing[1] = UiWidgetSizing_Fixed;         }
  else if (options & UiWidgetOptions_SizingVertGrow)      { new_widget->sizing[1] = UiWidgetSizing_Grow;          }
  else if (options & UiWidgetOptions_SizingVertFit)       { new_widget->sizing[1] = UiWidgetSizing_Fit;           }
  if      (options & UiWidgetOptions_IsInvisible)         { new_widget->is_invisible = true;                      }

  // NOTE: sync data from last frame
  new_widget->prev = UiWidgetFind(c->prev_root, id);
  if (new_widget->prev != NULL) {
    new_widget->is_animating   = new_widget->prev->is_animating;
    new_widget->color          = new_widget->prev->color;
    new_widget->target_color   = new_widget->prev->target_color;
    new_widget->anim_time      = new_widget->prev->anim_time;
    new_widget->animation_lock = new_widget->prev->animation_lock;
    UiWidgetUpdateAnimation(new_widget);
  } else {
    new_widget->color        = color;
    new_widget->target_color = color;
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

static void UiSizeWidgets(UiWidget* widget, UiWidgetDirection direction) {
  // NOTE: size starting at the bottom and moving up.
  for (U32 i = 0; i < widget->children_size; i++) {
    UiSizeWidgets(widget->children[i], direction);
  }

  UiWidget* parent = widget->parent;
  if (parent == NULL) { return; }

  // NOTE: widget->fit_size is updated when the children go (at the start of this fn). so, just finalize.
  F32* widget_fit_size_dim = UiWidgetGetDim(&widget->fit_size, direction);
  if (widget->direction == direction) { *widget_fit_size_dim += MAX(widget->children_size - 1, 0) * widget->child_gap; }
  F32 widget_pad_dim = *UiWidgetGetDim(&widget->pad, direction);
  *widget_fit_size_dim += widget_pad_dim * 2;

  // NOTE: determine final size from fit & min size.
  F32 widget_pref_size_dim = *UiWidgetGetDim(&widget->pref_size, direction);
  F32* widget_size_dim = UiWidgetGetDim(&widget->size, direction);
  switch (UiWidgetGetSizing(widget, direction)) {
    case UiWidgetSizing_Fit:   { *widget_size_dim = *widget_fit_size_dim;                            } break;
    case UiWidgetSizing_Grow:  { *widget_size_dim = MAX(*widget_fit_size_dim, widget_pref_size_dim); } break;
    case UiWidgetSizing_Fixed: { *widget_size_dim = widget_pref_size_dim;                            } break;
  }

  // NOTE: now, apply final size to parent
  F32* parent_fit_size_dim  = UiWidgetGetDim(&parent->fit_size, direction);
  if (parent->direction == direction) { *parent_fit_size_dim += *widget_size_dim; }
  else { *parent_fit_size_dim = MAX(*parent_fit_size_dim, *widget_size_dim); }
}

static void UiGrowWidgets(UiWidget* widget, UiWidgetDirection direction) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);

  UiWidget* growable_children[UI_WIDGET_MAX_CHILDREN];
  U32 growable_children_size = 0;
  for (U32 i = 0; i < widget->children_size; i++) {
    UiWidget* child = widget->children[i];
    if (UiWidgetGetSizing(child, direction) == UiWidgetSizing_Grow) {
      growable_children[growable_children_size++] = child;
    }
  }
  if (growable_children_size > 0) {
    F32 widget_pad_dim_2 = *UiWidgetGetDim(&widget->pad, direction) * 2;
    F32 widget_size_dim  = *UiWidgetGetDim(&widget->size, direction) - widget_pad_dim_2;
    if (widget->direction == direction) {
      // NOTE: share the main-axis dim with all growable children
      F32 widget_fit_size_dim = *UiWidgetGetDim(&widget->fit_size, direction) - widget_pad_dim_2;
      F32 delta               = widget_size_dim - widget_fit_size_dim;

      // NOTE: there is space to share, grow
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
        F32 size_delta = MIN(smallest_size_dim - second_smallest_size_dim, delta);
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

static void UiPositionWidgets(UiWidget* widget, F32 pos, UiWidgetDirection direction) {
  F32* widget_pos_dim = UiWidgetGetDim(&widget->pos, direction);
  *widget_pos_dim = pos;

  F32* widget_pad_dim = UiWidgetGetDim(&widget->pad, direction);
  UiWidgetUpdatePos(&pos, *widget_pad_dim, direction);

  for (U32 i = 0; i < widget->children_size; i++) {
    UiWidget* child = widget->children[i];
    UiPositionWidgets(child, pos, direction);
    F32* child_size_dim = UiWidgetGetDim(&child->size, direction);
    F32 pos_delta = *child_size_dim + widget->child_gap;
    if (widget->direction == direction) { UiWidgetUpdatePos(&pos, pos_delta, direction); }
  }
}

static void UiPositionWidgetsRoot(UiWidget* widget, UiWidgetDirection direction) {
  F32* widget_pos_dim = UiWidgetGetDim(&widget->pos, direction);
  UiPositionWidgets(widget, *widget_pos_dim, direction);
}

static void UiEnqueueDrawCommands(UiWidget* widget, UiDrawCommand** head, UiDrawCommand** tail) {
  UiContext* c = &_ui_context;
  if (!widget->is_invisible) {
    if (widget->text.size == 0) {
      V2 widget_half_size;
      V2DivF32(&widget_half_size, &widget->size, 2.0f);

      UiDrawCommand* command = ARENA_PUSH_STRUCT(c->command_arena, UiDrawCommand);
      DLL_PUSH_BACK(*head, *tail, command, prev, next);
      command->type = UiDrawCommand_Rect;
      command->rect.center.x = widget->pos.x + widget->size.x / 2.0f;
      command->rect.center.y = widget->pos.y - widget->size.y / 2.0f;
      command->rect.size = widget->size;
      command->rect.color = widget->color;
    } else {
      UiDrawCommand* command = ARENA_PUSH_STRUCT(c->command_arena, UiDrawCommand);
      DLL_PUSH_BACK(*head, *tail, command, prev, next);
      command->type = UiDrawCommand_Text;
      command->text.string = widget->text;
      command->text.pos = widget->pos;
      command->text.color = widget->color;
    }
  }
  for (U32 i = 0; i < widget->children_size; i++) {
    UiEnqueueDrawCommands(widget->children[i], head, tail);
  }
}

void UiInit(UiMeasureText_Fn* ui_measure_text_fn) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(!c->is_initialized);
  c->widget_arena = ArenaAllocate();
  c->prev_widget_arena = ArenaAllocate();
  c->command_arena = ArenaAllocate();
  c->temp_arena = ArenaAllocate();
  c->ui_measure_text_fn = ui_measure_text_fn;
  c->is_initialized = true;

  ui_default_style.anim_max_time        = 0.07f;
  ui_default_style.window_color         = V3FromHex(0x1c1c1c);
  ui_default_style.button_default_color = V3FromHex(0x80a0ff);
  ui_default_style.button_hovered_color = V3FromHex(0xc6c6c6);
  ui_default_style.button_clicked_color = V3_RED;
  ui_default_style.text_color           = V3FromHex(0xc6c6c6);
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

void UiPointerStateUpdate(V2 pos, B32 is_lmb_down, B32 is_rmb_down) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  c->mouse_pos = pos;
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
  c->current_widget = c->root;
  c->dt_s = dt_s;
}

UiDrawCommand* UiEnd() {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  DEBUG_ASSERT(c->current_widget == c->root);

  ArenaClear(c->command_arena);
  UiDrawCommand* commands_head = NULL;
  UiDrawCommand* commands_tail = NULL;
  for (U32 i = 0; i < c->root->children_size; i++) {
    UiWidget* root = c->root->children[i];
    UiSizeWidgets(root, UiWidgetDirection_Vertical);
    UiSizeWidgets(root, UiWidgetDirection_Horizontal);
    UiGrowWidgets(root, UiWidgetDirection_Vertical);
    UiGrowWidgets(root, UiWidgetDirection_Horizontal);
    UiPositionWidgetsRoot(root, UiWidgetDirection_Vertical);
    UiPositionWidgetsRoot(root, UiWidgetDirection_Horizontal);
    UiEnqueueDrawCommands(root, &commands_head, &commands_tail);
  }

  ArenaClear(c->prev_widget_arena);
  SWAP(Arena*, c->prev_widget_arena, c->widget_arena);
  c->prev_root = c->root;
  c->root = NULL;
  c->current_widget = NULL;

  return commands_head;
}

UiInteraction UiWindowFitBegin(U32 id, V2 pos, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (options == 0) { options = UI_WINDOW_DEFAULT_OPTS; }
  options &= ~(UiWidgetOptions_SizingFixed | UiWidgetOptions_SizingGrow);
  options |= UiWidgetOptions_SizingFit;
  UiWidget* window = UiWidgetBegin(id, c->style.window_color, options);
  window->pos = pos;
  return UiWidgetGetInteraction(window);
}

UiInteraction UiWindowSizedBegin(U32 id, V2 pos, V2 size, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (options == 0) { options = UI_WINDOW_DEFAULT_OPTS; }
  options &= ~(UiWidgetOptions_SizingGrow | UiWidgetOptions_SizingFit);
  options |= UiWidgetOptions_SizingFixed;
  UiWidget* window = UiWidgetBegin(id, c->style.window_color, options);
  window->pos = pos;
  window->pref_size = size;
  return UiWidgetGetInteraction(window);
}

void UiWindowEnd() {
  UiWidgetEnd();
}

UiInteraction UiPanelVerticalBegin(U32 id, V2 pad, F32 child_gap, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (options == 0) { options = UI_PANEL_DEFAULT_OPTS; }
  options &= ~UiWidgetOptions_DirectionHorizontal;
  options |= UiWidgetOptions_DirectionVertical;
  UiWidget* panel = UiWidgetBegin(id, V3_BLACK, options);
  panel->pad = pad;
  panel->child_gap = child_gap;
  return UiWidgetGetInteraction(panel);
}

UiInteraction UiPanelHorizontalBegin(U32 id, V2 pad, F32 child_gap, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (options == 0) { options = UI_PANEL_DEFAULT_OPTS; }
  options &= ~UiWidgetOptions_DirectionVertical;
  options |= UiWidgetOptions_DirectionHorizontal;
  UiWidget* panel = UiWidgetBegin(id, V3_BLACK, options);
  panel->pad = pad;
  panel->child_gap = child_gap;
  return UiWidgetGetInteraction(panel);
}

void UiPanelEnd() {
  UiWidgetEnd();
}

UiInteraction UiButton(U32 id, V2 size, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (options == 0) { options = UI_BUTTON_DEFAULT_OPTS; }
  UiWidget* button = UiWidgetBegin(id, c->style.button_default_color, options);
  button->pref_size = size;
  UiInteraction interaction = UiWidgetGetInteraction(button);
  if (interaction.clicked)      { UiWidgetAnimateTo(button, c->style.button_clicked_color, true); }
  else if (interaction.hovered) { UiWidgetAnimateTo(button, c->style.button_hovered_color, false); }
  else                          { UiWidgetAnimateTo(button, c->style.button_default_color, false); }
  UiWidgetEnd();
  return interaction;
}

UiInteraction UiLabel(U32 id, String8 text, U32 options) {
  UiContext* c = &_ui_context;
  DEBUG_ASSERT(c->is_initialized);
  if (options == 0) { options = UI_LABEL_DEFAULT_OPTS; }
  UiWidget* label = UiWidgetBegin(id, c->style.text_color, options);
  label->text = Str8Copy(c->widget_arena, text);
  if (c->ui_measure_text_fn != NULL) { label->pref_size = c->ui_measure_text_fn(label->text); }
  UiWidgetEnd();
  return UiWidgetGetInteraction(label);
}

#endif // CDEFAULT_UI_IMPLEMENTATION
