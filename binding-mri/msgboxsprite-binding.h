#ifndef MSGBOXSPRITEBINDING_H
#define MSGBOXSPRITEBINDING_H

#include "bitmap.h"
#include "viewport.h"
#include "sharedstate.h"
#include "binding-util.h"
#include "binding-types.h"
#include "sceneelement-binding.h"

/* 'Children' are disposables that are disposed together
 * with their parent. Currently this is only used by Viewport in RGSS1. */
inline void disposableAddChild(VALUE disp, VALUE child)
{
  VALUE children = rb_iv_get(disp, "children");
  if (RB_NIL_P(children)) {
    children = rb_ary_new();
    rb_iv_set(disp, "children", children);
  } // Assumes children are never removed until destruction
  rb_ary_push(children, child);
}

inline void disposableDisposeChildren(VALUE disp)
{
  VALUE children = rb_iv_get(disp, "children");
  if (RB_NIL_P(children)) return;
  ID dispFun = rb_intern("_HC_dispose_alias");
  for (long i = 0; i < RARRAY_LEN(children); ++i)
    rb_funcall(rb_ary_entry(children, i), dispFun, 0);
}

template<class C>
static VALUE disposableDispose(VALUE self)
{
  C *d = getPrivateData<C>(self);
  if (!d) return Qnil;
  if (d->isDisposed()) return Qnil;
  if (rgssVer < 2) disposableDisposeChildren(self);
  if (d->getContents()) d->getContents()->dispose();
  if (d->getCloseIcon()) d->getCloseIcon()->dispose();
  if (d->getBarBitmap()) d->getBarBitmap()->dispose();
  d->dispose();
  return Qnil;
}

template<class C>
static VALUE disposableIsDisposed(VALUE self)
{
  C *d = getPrivateData<C>(self);
  if (!d) return Qtrue;
  return d->isDisposed() ? Qtrue : Qfalse;
}

template<class C>
static void disposableBindingInit(VALUE klass)
{
  rb_define_method(klass, "dispose", RUBY_METHOD_FUNC(disposableDispose<C>), 0);
  rb_define_method(klass, "disposed?", RUBY_METHOD_FUNC(disposableIsDisposed<C>), 0);
  if (rgssVer < 2)
    rb_define_alias(klass, "_HC_dispose_alias", "dispose");
}

template<class C>
inline void
checkDisposed(VALUE self)
{
  if (disposableIsDisposed<C>(self) == Qtrue)
    raiseDisposedAccess(self);
}

#endif