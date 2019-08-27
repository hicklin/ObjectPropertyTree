#include "variant.h"
#include <wx/log.h>
#include "trace.hpp"
//
std::map<std::string, VariantStreamerBase *> VariantStreamerBase::_map; // portable name
std::map<std::string, VariantStreamerBase *> VariantStreamerBase::_typemap; // name returned by typeid().name
//
// This is the object's type
#define JSON_OBJECT_TYPE "__TYPE__"
/*
 *  setXml
 */
void setXml(wxXmlNode *n, boost::any &a)
{
    VariantStreamerBase * p = VariantStreamerBase::findByTypename(a.type().name());
    if(p)
    {
        n->AddAttribute("Type",wxString(p->id())); // note the type
        wxString v(p->toString(a));
        (void)new wxXmlNode (n, wxXML_TEXT_NODE,"Value", v);
    }
    else
    {
        if(!a.empty())
        {
            n->AddAttribute("Type","string"); // note the type
            wxString v(boost::any_cast<std::string>(a));
            (void) new wxXmlNode (n, wxXML_TEXT_NODE,"Value", v);
        }
    }
}

/*
 *  getXml
 */
void getXml(wxXmlNode *n, boost::any &a)
{
    std::string t = n->GetAttribute("Type").ToStdString();
    VariantStreamerBase * p = VariantStreamerBase::find(t);
    std::string s = n->GetNodeContent().ToStdString();
    if(p)
    {
        a = p->fromString(s); // cast to the correct type
    }
    else
    {
        a = boost::any(false); // default to bool instead of empty
    }
}


/*
 *  operator >>
 */
wxDataInputStream & operator  >> (wxDataInputStream &is, boost::any &a)
{
    std::string t;
    is >> t;
    VariantStreamerBase * p = VariantStreamerBase::find(t);
    if(p)
    {
        p->read(is,a);
    }
    else
    {
        wxLogError("Unknown Variant Type  %s",t.c_str());
    }
    return is;
}

/*
 *  operator <<
 */
wxDataOutputStream & operator << (wxDataOutputStream &os, const boost::any &a)
{
    VariantStreamerBase * p = VariantStreamerBase::findByTypename(a.type().name());
    if(p)
    {
        p->write(os,a);
    }
    else
    {
        wxLogError("Unknown Variant Type  %s",a.type().name());
    }

    return os;
}


/*
 *  operator >>
 */
wxDataInputStream & operator  >> (wxDataInputStream &is, std::string &s)
{
    wxString v;
    is >> v;
    s = v.ToStdString();
    return is;
}

/*
 *  operator <<
 */
wxDataOutputStream & operator << (wxDataOutputStream &os, const std::string &s)
{
    wxString v(s);
    os << v;
    return os;
}

/*
 *  operator >>
 */
wxDataInputStream & operator  >> (wxDataInputStream &is, wxDateTime &d)
{
    wxString s;
    is >> s;
    d.ParseISOCombined(s);
    return is;
}

/*
 *  operator <<
 */
wxDataOutputStream & operator << (wxDataOutputStream &os, const wxDateTime &d)
{
    os << d.FormatISOCombined();
    return os;
}

/*
 *  operator >>
 */
wxDataInputStream & operator  >> (wxDataInputStream &is, wxTimeSpan &t)
{
    wxLongLong l;
    is >> l;
    t = wxTimeSpan::Seconds(l);
    return is;
}

/*
 *  operator <<
 */
wxDataOutputStream & operator << (wxDataOutputStream &os, const wxTimeSpan &t)
{
    os << t.GetSeconds();
    return os;
}

//
// Define the variant handlers as static instances of classes
//
static VariantStreamer<char> _Char("char") ;
//
static class BoolStreamer : public VariantStreamer<bool>
{
public:
    BoolStreamer() : VariantStreamer("bool") {}
    std::string toString(const boost::any &a)
    {
        bool d = boost::any_cast<bool>(a);
        return d?"true":"false";
    }

    boost::any fromString(const std::string &s)
    {
        bool f = (s == "true");
        return boost::any(f);
    }
    void toJson(Json::Value &v, const boost::any &a)
    {
        // default JSON handling
        v = boost::any_cast<bool>(boost::any(a));
    }

} _Bool;
//
static class IntStreamer : public VariantStreamer<int>
{
public:
    IntStreamer() : VariantStreamer("int") {}
    std::string toString(const boost::any &a)
    {
        return std::to_string(boost::any_cast<int>(a));
    }

    boost::any fromString(const std::string &s)
    {
        int i = std::stoi(s);
        return boost::any(i);
    }
    void toJson(Json::Value &v, const boost::any &a)
    {
        // default JSON handling
        v = boost::any_cast<int>(boost::any(a));
    }

} _Int;

static class UnsignedStreamer : public VariantStreamer<unsigned>
{
public:
    UnsignedStreamer() : VariantStreamer("unsigned") {}
    std::string toString(const boost::any &a)
    {
        return std::to_string(boost::any_cast<unsigned>(a));
    }

    boost::any fromString(const std::string &s)
    {
        unsigned i = std::stoul(s);
        return boost::any(i);
    }
    void toJson(Json::Value &v, const boost::any &a)
    {
        // default JSON handling
        v = boost::any_cast<unsigned int>(boost::any(a));
    }

} _Unsigned;

static class DoubleStreamer : public VariantStreamer<double>
{
public:
    DoubleStreamer() : VariantStreamer("double") {}
    std::string toString(const boost::any &a)
    {
        return std::to_string(boost::any_cast<double>(a));
    }

    boost::any fromString(const std::string &s)
    {
        double i = std::stod(s);
        return boost::any(i);
    }
    void toJson(Json::Value &v, const boost::any &a)
    {
        // default JSON handling
        v = boost::any_cast<double>(boost::any(a));
    }

} _Double;

static class LongLongStreamer : public VariantStreamer<long long>
{
public:
    LongLongStreamer() : VariantStreamer("longlong") {}
    std::string toString(const boost::any &a)
    {
        return std::to_string(boost::any_cast<long long>(a));
    }

    boost::any fromString(const std::string &s)
    {
        long long  i = std::stoll(s);
        return boost::any(i);
    }
    virtual void toJson(Json::Value &v, const boost::any &a)
    {
        // default JSON handling
        v = Json::Int64(boost::any_cast<long long>(boost::any(a)));
    }
} _LongLong;

static class UnsignedLongLongStreamer : public VariantStreamer<unsigned long long>
{
public:
    UnsignedLongLongStreamer() : VariantStreamer("unsignedlonglong") {}
    std::string toString(const boost::any &a)
    {
        return std::to_string(boost::any_cast<unsigned long long>(a));
    }

    boost::any fromString(const std::string &s)
    {
        unsigned long long i = std::stoull(s);
        return boost::any(i);
    }
    void toJson(Json::Value &v, const boost::any &a)
    {
        // default JSON handling
        v = Json::UInt64(boost::any_cast<unsigned long long >(boost::any(a)));
    }

} _UnsignedLongLong;

static VariantStreamer<std::string> _String("string");
//
// need to define to and from string functions for XML encoding
static class DateTimeStreamer : public VariantStreamer<wxDateTime>
{
public:
    DateTimeStreamer() : VariantStreamer("DateTime")
    {

    }

    std::string toString(const boost::any &a)
    {
        wxDateTime d = boost::any_cast<wxDateTime>(a);
        return d.FormatISOCombined().ToStdString();
    }

    boost::any fromString(const std::string &s)
    {
        wxDateTime d;
        wxString w(s);
        d.ParseISOCombined(w);
        return boost::any(d);
    }

    void toJson(Json::Value &v, const boost::any &a)
    {
        wxDateTime d = boost::any_cast<wxDateTime>(a);
        v[JSON_OBJECT_TYPE] = id(); // set the object type
        v["Date"] = d.FormatISOCombined().ToStdString();
    }

    void fromJson(Json::Value &v, boost::any &a)
    {
        wxDateTime d;
        wxString w(v.asString());
        d.ParseISOCombined(w);
        a = boost::any(d);
    }

} _dateTime;


static class TimeSpanStreamer : public VariantStreamer<wxTimeSpan>
{
public:
   TimeSpanStreamer() : VariantStreamer("TimeSpan") {}

   std::string toString(const boost::any &a)
   {
       wxTimeSpan t = boost::any_cast<wxTimeSpan>(a);
       return std::to_string(t.GetSeconds().ToLong());
   }

   boost::any fromString(const std::string &s)
   {
       wxLongLong l = std::stol(s);
       wxTimeSpan t = wxTimeSpan::Seconds(l);
       return boost::any(t);
   }

   void toJson(Json::Value &v, const boost::any &a)
   {
       wxTimeSpan t = boost::any_cast<wxTimeSpan>(a);
       v[JSON_OBJECT_TYPE] = id(); // set the object type
       v["Time"] = int(t.GetSeconds().ToLong());
   }

   void fromJson(Json::Value &v, boost::any &a)
   {
       a = boost::any(wxTimeSpan::Seconds(v.get("Time",0).asUInt()));
   }


} _timeSpan;
//

/*
 *  setJson
 */
void setJson(Json::Value &v,boost::any &a)
{
    VariantStreamerBase::anyToJson(v, a);
}

/*
 *  getJson
 */
void getJson(Json::Value &v,boost::any &a)
{
    // Get FROM JSON any any
    switch(v.type())
    {
        case Json::nullValue:     ///< 'null' value
        a = std::string("");
        break;
        case Json::intValue:      ///< signed integer value
        a = v.asInt();
        break;
        case Json::uintValue:     ///< unsigned integer value
        a = v.asUInt();
        break;
        case Json::realValue:     ///< double value
        a = v.asDouble();
        break;
        case Json::stringValue:   ///< UTF-8 string value
        a = v.asString();
        break;
        case Json::booleanValue:  ///< bool value
        a = v.asBool();
        break;
        case Json::arrayValue:    ///< array value (ordered list)
        break;
        case Json::objectValue:   ///< object value (collection of name/value pairs).
        {
            // Workout object type
            a = VariantStreamerBase::anyFromJson(v, v.get(JSON_OBJECT_TYPE,"").asString());
        }
        break;
    default:
        break;
    }

}




