#ifndef VARIANT_H
#define VARIANT_H
/*!

    \author B. J. Hill
    \date __DATE__
    License:  GNU LESSER GENERAL PUBLIC LICENSE 2.1
    (c)  Micro Research Limited 2010 -
*/
#include "wx/wxprec.h"
#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif
#include <map>
#include <boost/any.hpp>
#include <wx/datstrm.h>
#include <wx/xml/xml.h>
//
// JSON support
#include <json/json.h>
//
// These classes and functions enable casting between strings and variants
// These are used to read and write property trees to/from XML & JSON
//
// encode into XML
void setXml(wxXmlNode *, boost::any &);
void getXml(wxXmlNode *, boost::any &);
//
void setJson(Json::Value &,boost::any &);
void getJson(Json::Value &,boost::any &);
//
wxDataInputStream & operator  >> (wxDataInputStream &is, boost::any &a);
wxDataOutputStream & operator << (wxDataOutputStream &os, const boost::any &a);
// Streams
wxDataInputStream & operator  >> (wxDataInputStream &, std::string &);
wxDataOutputStream & operator << (wxDataOutputStream &, const std::string &);

/*!
 * \brief operator >>
 */
inline wxDataInputStream & operator  >> (wxDataInputStream &is, bool &f)
{
    char c = is.Read8();
    f = c != 0;
    return is;
}

/*!
 * \brief operator <<
 */
inline wxDataOutputStream & operator << (wxDataOutputStream &os, char c)
{
    os.Write8(c);
    return os;
}

/*!
 * \brief operator >>
 */
inline wxDataInputStream & operator  >> (wxDataInputStream &is, char &c)
{
    c = is.Read8();
    return is;
}

/*!
 * \brief operator <<
 */
inline wxDataOutputStream & operator << (wxDataOutputStream &os, bool f)
{
    os.Write8(f?1:0);
    return os;
}




//
// Basic types we need
//
wxDataInputStream & operator  >> (wxDataInputStream &, wxDateTime &);
wxDataOutputStream & operator << (wxDataOutputStream &, const wxDateTime &);
wxDataInputStream & operator  >> (wxDataInputStream &, wxTimeSpan &);
wxDataOutputStream & operator << (wxDataOutputStream &, const wxTimeSpan &);
//
//
// to stream variants in native form needs a map of stream objects to type name
//
/*!
 * \brief The VariantStreamerBase class
 * Variant handler mapping and handler classes
 */
class VariantStreamerBase
{
    std::string _id; //!< class idenitfier string
protected:
    static std::map<std::string, VariantStreamerBase *> _map; //!< portable name translation
    static std::map<std::string, VariantStreamerBase *> _typemap; //!< name returned by typeid().name
public:
    // self register
    /*!
     * \brief VariantStreamerBase
     * \param s variant name
     */
    VariantStreamerBase(const std::string &s) : _id(s) { _map[s] = this;}
    /*!
     * \brief ~VariantStreamerBase
     */
    virtual ~VariantStreamerBase() { _map.erase(_id); }
    /*!
     * \brief id
     * \return variant name
     */
    virtual const char * id() const { return _id.c_str();}
    /*!
     * \brief read
     * Read from stream
     * \return true on success
     */
    virtual bool read(wxDataInputStream &,   boost::any &) { return false;}
    /*!
     * \brief write
     * Write to stream
     * \return true on success
     */
    virtual bool write(wxDataOutputStream &, const boost::any &) { return false;}
    // Used for XML encoding
    /*!
     * \brief toString
     * \param a any value
     * \return string form
     */
    virtual std::string toString(const boost::any &a) { return boost::any_cast<std::string>(a);}
    /*!
     * \brief fromString
     * \param s string to convert
     * \return  any value
     */
    virtual boost::any fromString(const std::string &s) { return boost::any(s); }

    /*!
     * \brief toJson
     * Convert to JSON from Any
     * \param v JSON value
     * \param a any value
     */
    virtual void toJson(Json::Value &v, const boost::any &a)
    {
        v = boost::any_cast<std::string>(a);
    } // setJson should handle base types

    /*!
     * \brief fromJson
     * Convert from JSON to any
     * \param v JSON value
     * \param a Any value
     */
    virtual void fromJson(Json::Value &v, boost::any &a)
    {
        a = v.asString();
    } // getJson should handle base types
    //
    /*!
     * \brief find
     * Look up the handler
     * \param s handler name
     * \return pointer to handler or null
     */
    static VariantStreamerBase *find(const std::string &s){ return _map[s];}
    /*!
     * \brief findByTypename
     * Look up by application's type
     * \param s typename
     * \return pointer to handler or null
     */
    static VariantStreamerBase *findByTypename(const std::string &s){ return _typemap[s];}
    //
    /*!
     * \brief anyToString
     * \param a any value
     * \return string form
     */
    static std::string anyToString(const boost::any &a)
    {
        VariantStreamerBase *v = findByTypename(a.type().name());
        if(v)
        {
            return v->toString(a);
        }
        std::string uk = "(Unknown:" + std::string(a.type().name()) + ")";
        return uk;
    }
    /*!
     * \brief anyFromString
     * \param s string
     * \param t type
     * \return any value as type
     */
    static boost::any anyFromString(const std::string &s, const std::string &t)
    {
        VariantStreamerBase *v = find(t);
        if(v)
        {
            return v->fromString(s);
        }
        return boost::any(std::string("Unknown"));
    }

    /*!
     * \brief anyToJson
     * Convert Any to JSON
     * \param val JSON value
     * \param a any value
     */
    static void anyToJson(Json::Value &val, const boost::any &a)
    {
        VariantStreamerBase *v = findByTypename(a.type().name());
        if(v)
        {
            v->toJson(val,a);
        }
    }
    /*!
     * \brief anyFromJson
     * \param val JSON value
     * \param t type name
     * \return  any value
     */
    static boost::any anyFromJson(Json::Value &val, const std::string &t)
    {
        VariantStreamerBase *v = find(t);
        if(v)
        {
            boost::any a;
            v->fromJson(val,a);
            return a;
        }
        return boost::any(std::string("Unknown"));
    }
};

// generic temlate
template <typename T>
/*!
 * \brief The VariantStreamer class
 */
class VariantStreamer : public VariantStreamerBase
{
public:
    /*!
     * \brief VariantStreamer
     * \param s portable name
     */
    VariantStreamer(const std::string &s) : VariantStreamerBase(s)
    {
        _typemap[typeid(T).name()] = this; // internal name which may be mangled
    }
    /*!
     * \brief read
     * \param is stream to read on
     * \param a any value
     * \return true on success
     */
    bool read(wxDataInputStream &is, boost::any &a)
    {
        try
        {
            T v;
            is >> v;
            a = boost::any(v);
            return true;
        }
        catch(...)
        {

        }
        return false;
    }
    /*!
     * \brief write
     * \param os output stream
     * \param a any value
     * \return true on success
     */
    bool write(wxDataOutputStream &os, const boost::any &a)
    {
        try
        {
            os << id() << boost::any_cast<T>(a);
            return true;
        }
        catch(...)
        {

        }
        return false;
    }

    /*!
     * \brief fromString
     * Convert string to any
     * \param s input string
     * \return any value
     */
    virtual boost::any fromString(const std::string &s)
    {
        T v = boost::any_cast<T>(boost::any(s)); // go from string to type
        return boost::any(v);
    }

};





#endif // VARIANT_H
