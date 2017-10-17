#pragma once

#include <windows.h>
#include <wrl.h>
#include <wrl/module.h>
#include <objbase.h>
#include <ShObjIdl.h>
#include <crtdbg.h>
#include <stdio.h>
#include <atlbase.h>
#include <string>
#include <exception>
#include <ktmw32.h>

#pragma comment(lib, "ktmw32")
#pragma comment(lib, "runtimeobject")

// Debug macros

#define ASSERT _ASSERTE

#ifdef _DEBUG
#define VERIFY ASSERT
#else
#define VERIFY(expression) (expression)
#endif

#ifdef _DEBUG

inline auto Trace(wchar_t const *format, ...) -> void
{
	va_list args;
	va_start(args, format);

	wchar_t buffer [256];

	ASSERT(-1 != _vsnwprintf_s(buffer,
		_countof(buffer) -1,
		format,
		args));

	va_end(args);

	OutputDebugString(buffer);
}

struct Tracer {
	char const *m_filename;
	unsigned m_line;

	Tracer(char const *filename, unsigned const line) :
		m_filename {filename},
		m_line {line} { }

	template <typename... Args>
	auto operator()(wchar_t const *format, Args ... args) const -> void
	{
		wchar_t buffer [256];

		auto count = swprintf_s(buffer,
			L"%S(%d): ",
			m_filename,
			m_line);

		ASSERT(-1 != count);

		ASSERT(-1 != _snwprintf_s(buffer + count,
			_countof(buffer) - count,
			_countof(buffer) - count - 1,
			format,
			args...));

		OutputDebugString(buffer);
	}
};

#endif

#ifdef _DEBUG
#define TRACE Tracer("", __LINE__)
#else
#define TRACE __noop
#endif


// convert c++ exceptions to HRESULT

#define COM_EXCEPTION_GUARD_BEGIN       try {

#define COM_EXCEPTION_GUARD_END         } catch(const CAtlException & ex) {  \
                                            return static_cast<HRESULT>(ex); \
                                        } catch(const std::bad_alloc & ) {   \
                                            return E_OUTOFMEMORY;            \
                                        } catch(const std::exception & ) {   \
                                            return E_FAIL;                   \
                                        }
