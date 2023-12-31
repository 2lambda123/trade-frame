/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

//#include "StdAfx.h"

#include <iostream>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "CurlGetMktSymbols.h"

/*
Header:  HTTP/1.1 200 OK
Header:  Content-Length: 6824968
Header:  Content-Type: application/x-zip-compressed
Header:  Last-Modified: Sun, 14 Oct 2012 11:06:31 GMT
Header:  Accept-Ranges: bytes
Header:  ETag: "23f414f7fba9cd1:1432"
Header:  Server: Microsoft-IIS/6.0
Header:  X-Powered-By: ASP.NET
Header:  Date: Sun, 14 Oct 2012 23:39:13 GMT
Header:  */

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

static boost::regex rxHttpStatus( "^HTTP/1\\.[0-9] ([0-9]+) ([A-Z]+)" );

CurlGetMktSymbols::CurlGetMktSymbols(void)
  : m_hCurl( 0 ), m_bSizeFound( false ), m_buf( 0 ), m_offset( 0 ), m_size( 0 )
{
  curl_global_init(CURL_GLOBAL_ALL);

  m_hCurl = curl_easy_init();
  curl_easy_setopt(m_hCurl, CURLOPT_URL, "http://www.dtniq.com/product/mktsymbols_v2.zip");
  curl_easy_setopt(m_hCurl, CURLOPT_WRITEFUNCTION, &CurlGetMktSymbols::WriteMemoryCallback);
  curl_easy_setopt(m_hCurl, CURLOPT_WRITEDATA, this);
  curl_easy_setopt(m_hCurl, CURLOPT_HEADERFUNCTION, &CurlGetMktSymbols::HeaderFunctionCallback);
  curl_easy_setopt(m_hCurl, CURLOPT_HEADERDATA, this);
  //curl_easy_setopt(m_hCurl, CURLOPT_PROGRESSFUNCTION, &CurlGetMktSymbols::ProgressFunctionCallback);
  curl_easy_setopt(m_hCurl, CURLOPT_XFERINFOFUNCTION, &CurlGetMktSymbols::ProgressFunctionCallback);
  curl_easy_setopt(m_hCurl, CURLOPT_PROGRESSDATA, this);
  curl_easy_setopt(m_hCurl, CURLOPT_NOPROGRESS, 0);
  curl_easy_setopt(m_hCurl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  CURLcode code;
  code = curl_easy_perform(m_hCurl);
  curl_easy_setopt(m_hCurl, CURLOPT_NOPROGRESS, 1);
  curl_easy_cleanup(m_hCurl);
}

CurlGetMktSymbols::~CurlGetMktSymbols(void) {
  curl_global_cleanup();
  delete[] m_buf;
  m_buf = 0;
}

size_t CurlGetMktSymbols::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void* self) {
  CurlGetMktSymbols* p = reinterpret_cast<CurlGetMktSymbols*>( self );
  if ( !p->m_bSizeFound ) {
    std::cerr << "problems" << std::endl;
    throw std::runtime_error( "CurlGetMktSymbols::WriteMemoryCallback 1" );
  }
  size_t cnt = size * nmemb;
  size_t whatwehave = p->m_offset - p->m_buf;
  size_t whatsleft = p->m_size - whatwehave;
  if ( cnt > whatsleft ) {
    std::cerr << "problems" << std::endl;
    throw std::runtime_error( "CurlGetMktSymbols::WriteMemoryCallback 2" );
    cnt = whatsleft;
  }
  memcpy( p->m_offset, contents, cnt );
  p->m_offset += cnt;

  if ( p->m_size == ( p->m_offset - p->m_buf ) ) {
    std::cout << "done" << std::endl;
  }

  return cnt;
}

static boost::regex rxContentLength( "[Cc]ontent-[Ll]ength[ ]*:[ ]*([0-9]+)" );

size_t CurlGetMktSymbols::HeaderFunctionCallback(void* ptr, size_t size, size_t nmemb, void* self) {
  CurlGetMktSymbols* p = reinterpret_cast<CurlGetMktSymbols*>( self );
  if ( 0 != nmemb ) {
    char* pChar( reinterpret_cast<char*>( ptr ) );
    std::string s( pChar, size * nmemb );
    std::cout << "Header:  " << s;
    boost::cmatch what;
    bool bResult = boost::regex_search( s.c_str(), what, rxContentLength, boost::match_continuous  );
    if ( !bResult ) {
      // no errors as we are waiting for the header we need
      // flag is evaluated elsewhere
    }
    else {
      p->m_size = boost::lexical_cast<size_t>( what[1].str() );
      p->m_bSizeFound = true;
      p->m_offset = p->m_buf = new char[ p->m_size ];
    }
  }
  return size * nmemb;
}

// uses curl_xferinfo_callback introduced in 7.32.0
int CurlGetMktSymbols::ProgressFunctionCallback( void *self, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow ) {
  CurlGetMktSymbols* p = reinterpret_cast<CurlGetMktSymbols*>( self );
  std::cout
    << dltotal << ","
    << dlnow << ","
    << dltotal - dlnow << ","
    << ultotal << ","
    << ulnow
    << std::endl;
  return 0;  // non zero will cause transfer abort
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
