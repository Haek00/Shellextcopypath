// Clip Library
// Copyright (c) 2015-2016 David Capello
// https://github.com/dacap/clip

#pragma once

namespace clip {

	class lock::impl {
	public:
		explicit impl(void *hwnd);
		~impl();

		bool locked() const;
		bool clear() const;
		bool set_data(const wchar_t *buf, size_t len);

	private:
		bool _locked;
	};

} // namespace clip
