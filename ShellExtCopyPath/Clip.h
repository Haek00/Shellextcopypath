// Clip Library
// Copyright (c) 2015-2016 David Capello
// https://github.com/dacap/clip

#pragma once

namespace clip {

	class lock {
	public:
		// You can give your current HWND as the "native_window_handle."
		// Windows clipboard functions use this handle to open/close
		// (lock/unlock) the clipboard. From the MSDN documentation we
		// need this handler so SetClipboardData() doesn't fail after a
		// EmptyClipboard() call. Anyway it looks to work just fine if we
		// call OpenClipboard() with a null HWND.
		explicit lock(void *native_window_handle = nullptr);
		~lock();

		// Returns true if we've locked the clipboard successfully in
		// lock() constructor.
		bool locked() const;

		// Clears the clipboard content. If you don't clear the content,
		// previous clipboard content (in unknown formats) could persist
		// after the unlock.
		bool clear();

		// Returns true if the clipboard can be converted to the given
		// format.
		bool set_data(const wchar_t *buf, size_t len);

	private:
		class impl;
		impl *p;
	};

	// Clears the clipboard content.
	bool clear();

	// ======================================================================
	// Error handling
	// ======================================================================

	enum class ErrorCode {
		CannotLock,
		ImageNotSupported,
	};

	typedef void (*error_handler)(ErrorCode code);

	void set_error_handler(error_handler f);
	error_handler get_error_handler();

	// ======================================================================
	// Text
	// ======================================================================

	// High-level API to put/get UTF8 text in/from the clipboard. These
	// functions returns false in case of error.
	bool set_text(const std::wstring &value);

} // namespace clip
