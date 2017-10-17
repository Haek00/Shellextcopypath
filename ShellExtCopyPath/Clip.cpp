#include "Precompiled.h"
#include "Clip.h"
#include "ClipWin.h"

namespace clip {

namespace {

void default_error_handler(ErrorCode code) {
  static const char* err[] = {
    "Cannot lock clipboard",
    "Image format is not supported"
  };
  throw std::runtime_error(err[static_cast<int>(code)]);
}

} // anonymous namespace

error_handler g_error_handler = default_error_handler;

lock::lock(void* native_window_handle)
  : p(new impl(native_window_handle)) {
}

lock::~lock() {
  delete p;
}

bool lock::locked() const {
  return p->locked();
}

bool lock::clear() {
  return p->clear();
}

bool lock::set_data(const wchar_t* buf, size_t length) {
  return p->set_data(buf, length);
}


bool clear() {
  lock l;
  if (l.locked())
    return l.clear();
  else
    return false;
}

bool set_text(const std::wstring& value) {
  lock l;
  if (l.locked()) {
    l.clear();
    return l.set_data(value.c_str(), value.size());
  }
  else
    return false;
}


void set_error_handler(error_handler handler) {
  g_error_handler = handler;
}

error_handler get_error_handler() {
  return g_error_handler;
}

} // namespace clip

