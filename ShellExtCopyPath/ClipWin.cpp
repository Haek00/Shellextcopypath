#include "Precompiled.h"
#include "Clip.h"
#include "ClipWin.h"

namespace clip {

	namespace {

		class Hglobal {
		public:
			Hglobal() : _handle(nullptr) { }

			explicit Hglobal(HGLOBAL handle) : _handle(handle) { }

			explicit Hglobal(size_t len) : _handle(GlobalAlloc(GHND, len)) { }

			~Hglobal()
			{
				if (_handle)
					GlobalFree(_handle);
			}

			void release()
			{
				_handle = nullptr;
			}

			operator HGLOBAL()
			{
				return _handle;
			}

		private:
			HGLOBAL _handle;
		};

	}

	lock::impl::impl(void *hwnd) : _locked(false)
	{
		for (int i = 0; i < 5; ++i) {
			if (OpenClipboard(static_cast<HWND>(hwnd))) {
				_locked = true;
				break;
			}
			Sleep(20);
		}

		if (!_locked) {
			error_handler e = get_error_handler();
			if (e)
				e(ErrorCode::CannotLock);
		}
	}

	lock::impl::~impl()
	{
		if (_locked)
			CloseClipboard();
	}

	bool lock::impl::locked() const
	{
		return _locked;
	}

	bool lock::impl::clear() const
	{
		return EmptyClipboard() ? true : false;
	}

	bool lock::impl::set_data(const wchar_t *buf, size_t len)
	{
		bool result = false;

		Hglobal hglobal((len + 1) * sizeof(wchar_t));
		if (hglobal) {
			wchar_t *dst = static_cast<wchar_t*>(GlobalLock(hglobal));
			if (dst) {
				memcpy(dst, buf, len * sizeof(wchar_t));
				dst[len] = 0;
				GlobalUnlock(hglobal);
				result = SetClipboardData(CF_UNICODETEXT, hglobal) ? true : false;
				if (result)
					hglobal.release();
			}
		}

		return result;
	}

} // namespace clip
