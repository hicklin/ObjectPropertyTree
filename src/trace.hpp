#ifndef TRACE_HPP
#define TRACE_HPP
/*!
 *
 *  Author:  B. J. Hill
 *  (c)  Air Monitors Limited 2017 -
 */

//
// wxLog does all the work
//
#include <wx/log.h>
//
// Trace function/file/line
//
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define TRACE_AT()
#define LINEAT __FUNCTION__,__LINE__
#define FILELINE(reason)  TRACE_AT  reason
//
#ifdef TRACE_ON
#define TRC(s) std::cerr << __FUNCTION__ << __LINE__ << s;
#define TRACE_ENTRY wxLog::LogText( __FUNCTION__ " Entry");
#define TRACE_EXIT wxLog::LogText( __FUNCTION__ " Exit");
#else
#define TRC(s)
#define TRACE_ENTRY
#define TRACE_EXIT
#endif

//
#endif // TRACE_HPP
