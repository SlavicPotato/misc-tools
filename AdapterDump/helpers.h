#pragma once

#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <vector>

#ifdef UNICODE
#define STD_STRING      std::wstring
#define STD_OFSTREAM    std::wofstream
#define STD_OSSTREAM    std::wostringstream
#define STD_ISSTREAM    std::wistringstream
#define STD_SSTREAM     std::wstringstream
#define STD_TOSTR       std::to_wstring

#define VSPRINTF        _vsnwprintf_s
#define SPRINTF         _snwprintf_s
#define STRCMP          _wcsicmp
#define FSOPEN          _wfsopen
#define MKDIR           _wmkdir
#define FPUTS           fputws
#else
#define STD_STRING      std::string
#define STD_OFSTREAM    std::ofstream
#define STD_OSSTREAM    std::ostringstream
#define STD_ISSTREAM    std::istringstream
#define STD_SSTREAM     std::stringstream
#define STD_TOSTR       std::to_string

#define VSPRINTF        _vsnprintf_s
#define SPRINTF         _snprintf_s
#define STRCMP          _stricmp
#define FSOPEN          _fsopen
#define MKDIR           _mkdir
#define FPUTS           fputs
#endif

namespace StrHelpers
{
    using namespace std;

    typedef wstring_convert<codecvt_utf8<wchar_t>, wchar_t> wsconv_t;
    inline static wsconv_t strconverter;

    __inline string ToString(const wstring& wstr)
    {
        return strconverter.to_bytes(wstr);
    }

    __inline wstring ToWString(const string& str)
    {
        return strconverter.from_bytes(str);
    }

    __inline void
        SplitString(const wstring& s, wchar_t delim, vector<wstring>& elems)
    {
        wistringstream ss(s);
        wstring item;

        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }

    template <typename T>
    __inline void
        SplitString(const wstring& s, wchar_t delim, vector<T>& elems)
    {
        vector<wstring> tmp;
        SplitString(s, delim, tmp);

        if (tmp.size())
        {
            T iv;
            wstringstream oss;
            for (const auto& e : tmp) {
                oss << e;
                oss >> iv;
                oss.clear();
                elems.push_back(iv);
            }
        }
    }

    __inline void
        SplitString(const string& s, char delim, vector<string>& elems)
    {
        istringstream ss(s);
        string item;

        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }

    template <typename T>
    __inline void
        SplitString(const string& s, char delim, vector<T>& elems)
    {
        vector<string> tmp;
        SplitString(s, delim, tmp);

        if (tmp.size())
        {
            T iv;
            stringstream oss;
            for (const auto& e : tmp) {
                oss << e;
                oss >> iv;
                oss.clear();
                elems.push_back(iv);
            }
        }
    }

#ifdef UNICODE
    __inline wstring ToNative(const string& str)
    {
        return strconverter.from_bytes(str);
    }

    __inline const wstring& ToNative(const wstring& str)
    {
        return str;
    }

    __inline string StrToStr(const wstring& str)
    {
        return strconverter.to_bytes(str);
    }

#else
    __inline string ToNative(const wstring& wstr)
    {
        return strconverter.to_bytes(wstr);
    }

    __inline const string& ToNative(string& str)
    {
        return str;
    }

    __inline const string& StrToStr(const string& str)
    {
        return str;
    }


#endif

}

namespace FileHelpers
{
    using namespace std;

    __inline string GetPathFileNameA(const string& in)
    {
        string s(in);

        auto pos = s.rfind('\\');
        if (pos != string::npos) {
            return s.substr(pos + 1, string::npos);
        }

        pos = s.rfind('/');
        if (pos != string::npos) {
            return s.substr(pos + 1, string::npos);
        }

        return s;
    }

    __inline wstring GetPathFileNameW(const wstring& in)
    {
        wstring s(in);

        auto pos = s.rfind(L'\\');
        if (pos != wstring::npos) {
            return s.substr(pos + 1, wstring::npos);
        }

        pos = s.rfind(L'/');
        if (pos != wstring::npos) {
            return s.substr(pos + 1, wstring::npos);
        }

        return s;
    }
}

namespace Misc
{
    template <typename T>
    constexpr typename std::underlying_type<T>::type Underlying(T e) noexcept {
        return static_cast<typename std::underlying_type<T>::type>(e);
    }
}

namespace Exceptions
{
    class hexception :
        public std::exception
    {
    public:
        hexception(LONG hr, int line, const char *filename) :
            std::exception(),
            _hres(hr),
            _lineno(line),
            _filename(filename)
        {
            _msg = std::system_category().message(hr);
        }

        virtual LONG hresult() const
        {
            return _hres;
        }

        virtual int line() const
        {
            return _lineno;
        }

        virtual const char* file() const
        {
            return _filename;
        }

        _NODISCARD virtual char const* what() const override
        {
            return _msg.size() > 0 ? _msg.c_str() : "Unknown exception";
        }
    private:
        LONG _hres;
        std::string _msg;
        int _lineno;
        const char* _filename;
    };
}

#define ThrowIfFailed(r) { auto hr = (r); if (FAILED(hr)) { throw Exceptions::hexception(hr, __LINE__, __FILE__); }}
#define ThrowIfFailed2(r) { auto hr = (r); if (hr != ERROR_SUCCESS) { throw Exceptions::hexception(hr, __LINE__, __FILE__); }}

template<class T, class...> struct is_any_of : std::false_type {};
template<class T, class Head, class... Tail>
struct is_any_of<T, Head, Tail...> : std::conditional_t<
    std::is_same<T, Head>::value,
    std::true_type,
    is_any_of<T, Tail...>>
{};
