#pragma once
namespace sf {
#if 1  // def  //_DEBUG
  inline void dout(wchar_t* file,int line,boost::wformat& fmt)
  {
    OutputDebugStringW((boost::wformat(L"FILE:%s LINE:%d MESSAGE:%s \n") % file % line % fmt.str()).str().data());
  }
  inline void dout(wchar_t* file,int line,std::wstring& str)
  {
    OutputDebugStringW((boost::wformat(L"FILE:%s LINE:%d MESSAGE:%s \n") % file % line % str).str().data());
  }
  inline void dout(wchar_t* file,int line,wchar_t* str)
  {
    OutputDebugStringW((boost::wformat(L"FILE:%s LINE:%d MESSAGE:%s \n") % file % line % str).str().data());
  }
  inline void dout(wchar_t* file,int line,HRESULT hr)
  {
	  wchar_t mes[1024];
	  DWORD result = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,(LPWSTR)mes,ARRAYSIZE(mes),0);
	  if(result != 0){
      OutputDebugStringW((boost::wformat(L"#### Error File:%s Line:%d Message:%s\n") % file % line % mes).str().data());
    }
  }

  #define DOUT(s) sf::dout(__FILEW__,__LINE__,s);

#else
  #define DOUT(s)
  #define dout(s) 
#endif
}