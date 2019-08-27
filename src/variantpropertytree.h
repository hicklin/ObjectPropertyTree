#ifndef VARIANTPROPERTYTREE_H
#define VARIANTPROPERTYTREE_H

/*!

    \author B. J. Hill
    \date __DATE__
    License:  GNU LESSER GENERAL PUBLIC LICENSE 2.1
    (c)  Micro Research Limited 2010 -
*/

#include "propertytree.h"
#include <wx/variant.h>
#include <wx/any.h>
#include <typeinfo>
#include <wx/datstrm.h>
#include <string>
#include "trace.hpp"
#include <boost/any.hpp>
#include "variant.h"
#include <wx/sstream.h>
#include <functional>
#include <memory>
#include <iostream>

namespace logair {

    template <typename T>
    /*!
     * \brief stringToNumber
     * \param Text
     * \return
     */
    inline T stringToNumber ( const std::string &Text )//Text not by const reference so that the function can be used with a
    {
        if(!Text.empty())
        {
            //character array as argument
            std::stringstream ss(Text);
            T result;
            return ss >> result ? result : 0;
         }
        return 0;
    }

    template <typename T>
    /*!
     * \brief isType
     * \param a
     * \return true if the type stored in the any matches
     */
    inline bool isType(boost::any &a)
    {
        return a.type().hash_code() == typeid(T).hash_code();
    }

    /*!
     * \brief stringToBool
     * \param s string true or false
     * \return bool value of the string
     */
    bool stringToBool(const std::string &s);

    typedef std::vector<std::string> StringList;
    /*!
        \brief VariantPath
        Parsed path to a value in a tree
    */
    typedef NodePath<std::string> VariantPath;
    /*!
        \brief The VariantTree class - Stores generic objects as wxAny

    */
    typedef PropertyTree<std::string, boost::any>::PropertyNode VariantNode;
    /*!
           \brief VariantNodeType
           Returns the type of the value as text
           \param n  node
           \return RTTI name of the any type
    */
    inline const std::string VariantNodeType(VariantNode *n) {
        return n->data().type().name(); // get the type of the data at a node
    }

    /*!
        \brief The VariantTree class
        Property Tree of any values
    */
    class VariantTree : public PropertyTree<std::string, boost::any> {
        public:
            /*!
                \brief VariantPropertyTree
            */
            VariantTree() {}
            /*!
             * \brief ~VariantTree
             */
            virtual ~VariantTree() {}
            /*!
                \brief load
                \param file filename to load property tree from as XML
                \return true on success
            */
            bool load(const std::string &file); // load from file
            /*!
                \brief save
                \param file name of file to save property tree data as XML to
                \return true on success
            */
            bool save(const std::string &file); // save to file
            //
            template <typename P, typename T>
            /*!
                \brief setPointer
                Saves pointer as a referenced counter shared_ptr
                \param path path to item in tree
                \param p pointer to store
            */
            void setPointer(P path, T *p) {
                auto sp = std::shared_ptr<T>(p);
                setValue(path, sp);
            }

            template <typename T, typename P>
            /*!
                \brief getPointer
                \param path path to item in tree
                \return
            */
            T *getPointer(P path) {
                T *ret = nullptr;
                VariantNode *n = node(path);
                if(n)
                {
                    return getPointer<T>(n);
                }
                return ret;
            }

            template <typename T>
            /*!
             * \brief getPointer
             * \param n node
             * \return pointer or null
             */
            T *getPointer(VariantNode *n) {
                T *ret = nullptr;
                if(n)
                {
                    try {
                        boost::any &a = n->data();
                        if (!a.empty() && isType<std::shared_ptr<T> >(a)) {
                            std::shared_ptr<T> *s = boost::any_cast<std::shared_ptr<T> >(&a);
                            ret = s->get();
                        }
                    }
                    catch (...) {

                    }
                }
                return ret; // return pointer to object not reference ot shared_ptr
            }

            //
            template <typename P, typename T>
            /*!
                \brief setValue
                \param path path to object
                \param v value to store
            */
            void setValue(P path, const T &v) {
                boost::any a(v);
                set(path, a);
            }


            //
            template <typename T>
            /*!
                \brief setValue
                \param path path to parent as VariantPath
                \param c child name
                \param v value to store
            */
            void setValue(logair::VariantPath &path, const std::string &c,  const T &v) {
                if (!c.empty()) {
                    path.push_back(c);
                    boost::any a(v);
                    set(path, a);
                    path.pop_back();
                }
            }
            /*!
                \brief sync
                Synchronise(copy) one tree to another optimially
                \param tree tree to sync from
            */
            void sync(VariantTree &tree);
            //
            template<typename P>
            /*!
                \brief getAsWxString
                Get a value as a wxString. Useful for working with property sheets
                \param path path to item
                \return value as wxString
            */
            wxString getAsWxString(P path) {
                try {
                    boost::any &a = get(path);
                    if (!a.empty()) {
                        std::string s = VariantStreamerBase::anyToString(a); // intelligent conversion
                        return wxString(s);
                    }
                }
                catch (...) {

                }
                return wxString("");
            }


            template<typename P>
            /*!
                \brief getAsWxString
                \param path Path to item
                \return value as a std::string.
            */
            std::string getAsString(P path) {
                try {
                    ReadLock l(mutex());
                    boost::any &a = get(path);
                    if (!a.empty()) {
                        std::string s = VariantStreamerBase::anyToString(a); // intelligent conversion
                        return s;
                    }
                }
                catch (...) {

                }
                return std::string("");
            }



            /*!
                \brief getAsWxString
                \param path Path to parent
                \param s child name
                \return value as an std::string
            */
            std::string getAsString(logair::VariantPath &path, const std::string &s) {
                try {
                    ReadLock l(mutex());
                    path.push_back(s);
                    boost::any &a = get(path);
                    path.pop_back();
                    if (!a.empty()) {
                        std::string s = VariantStreamerBase::anyToString(a); // intelligent conversion
                        return s;
                    }
                }
                catch (...) {

                }
                return std::string("");
            }




            template <typename T, typename P>
            /*!
                \brief getValue
                \param path Path to item
                \return value of requested type or empty if item value is not of requested type
            */
            T getValue(P path) {
                try {
                    ReadLock l(mutex());
                    auto *n = root().find(path);
                    if (n) {
                        boost::any &a = n->data();
                        if (!a.empty()) {
                            return  boost::any_cast<T>(a);
                        }
                    }
                }
                catch (...) {

                }
                return T();
            }

            template <typename T>
            /*!
             * \brief getValue
             * \param p Path to parent of item as a VariantPath
             * \param c child name
             * \return value cast to requested type or empty value if invalid
             */
            T getValue(logair::VariantPath &p, const std::string &c) {
                if (!c.empty()) {
                    try {
                        ReadLock l(mutex());
                        p.push_back(c);
                        auto *n = root().find(p);
                        p.pop_back();
                        if (n) {
                            boost::any &a = n->data();
                            if (!a.empty() && isType<T>(a)) {
                                return  boost::any_cast<T>(a);
                            }
                        }
                    }
                    catch (...) {

                    }
                }
                return T();
            }



            template <typename P>
            /*!
                \brief list
                List children into a wxArrayString. Useful for UI
                \param path path to item
                \param l receives list of children
            */
            int list(const P &path, wxArrayString  &l) {
                l.Clear();
                auto i =  node(path);
                if (i) {
                    for (auto j = i->children().begin(); j != i->children().end(); j++) {
                        l.Add(wxString(j->first));
                    }
                }
                return l.Count();
            }

            //
            // Generate JSON
            //
            /*!
                \brief toJson
                \param n node (and its children) to stream as JSON.
                \param v receives the JSON values
            */
            static void toJson(VariantNode *n, Json::Value &v);

            /*!
                \brief fromJson
                \param n node to stream JSON records to. Previous children are deleted
                \param v source JSON value
            */
            static void fromJson(VariantNode *n, Json::Value &v);

            /*!
                \brief toJson
                Write the whole tree to JSON
                \param v recieves tree as a JSON document
            */
            void toJson(Json::Value &v) {
                // whole tree
                toJson(rootNode(), v);
            }

            /*!
                \brief fromJson
                \param v reads tree (after clearing) from the JSON document (value)
            */
            void fromJson(Json::Value &v) {
                // whole tree
                clear();
                fromJson(rootNode(), v);
            }
            //
            // Generate XML
            //
            /*!
                \brief toXml
                \param n node to stream as XML
                \param parentNode node to add XML values
            */
            static void toXml(VariantNode *n, wxXmlNode *parentNode);
            //
            // load from XML
            //
            /*!
                \brief fromXml
                \param n node (and its children) to receive XML data
                \param x source of XML data
            */
            static void fromXml(VariantNode *n, wxXmlNode *x);


            /*!
                \brief toXml
                \param d document to recieve XML data
            */
            void toXml(wxXmlDocument &d) {
                wxXmlNode *r = new wxXmlNode(wxXML_ELEMENT_NODE, "root");
                d.SetRoot(r);
                toXml(rootNode(), r);
            }

            /*!
                \brief fromXml
                \param d document that is source of XML data
            */
            void fromXml(wxXmlDocument &d) {
                clear();
                fromXml(rootNode(), d.GetRoot());
            }
            //

            /*!
                \brief printNode
                \param os stream to write
                \param n    node to (and its children) to report
                \param level indentation level
            */
            void printNode(std::ostream &os, VariantNode *n, int level = 0);

            /*!
                \brief dump the property tree
                \param os stream to write on
            */
            void dump(std::ostream &os = std::cerr) {
                printNode(os, rootNode());
            }

            //
            // These functions allow Selena (LUA) to interface to the tree - no templates
            //
            /*!
                \brief getConfigBool
                \param address path to item as string
                \return value as bool
            */
            bool getBool(std::string address) {
                return getValue<bool>(address);
            }
            /*!
                \brief getConfigInt
                \param address path to item as string
                \return value as int
            */
            int getInt(std::string address) {
                return getValue<int>(address);

            }
            /*!
                \brief getConfigDouble
                \param address path to item as string
                \return value as double
            */
            double getDouble(std::string address) {
                return getValue<double>(address);
            }
            /*!
                \brief getConfigString
                \param address path to item as string
                \return value as string
            */
            std::string getString(std::string address) {
                return getValue<std::string>(address);

            }

            /*!
                \brief setConfigBool
                \param address path to item as string
                \param v value as bool
            */
            void setBool(std::string address, bool v) {
                setValue(address, v);
            }

            /*!
                \brief setInt
                \param address path as string
                \param v value as int
            */
            void setInt(std::string address, int v) {
                setValue(address, v);
            }

            /*!
                \brief setDouble
                \param address path  as string
                \param v value as double
            */
            void setDouble(std::string address, double v) {
                setValue(address, v);
            }

            /*!
                \brief setString
                \param address path  as string
                \param v value as string
            */
            void setString(std::string address, bool v) {
                setValue(address, v);
            }

            /*!
                \brief treeRemove
                Remove item
                \param address path  as string
            */
            void treeRemove(std::string address) {
                remove(address);
            }


            template <typename T>
            /*!
             * \brief iterateReferenceObjects
             * Iterate objects only of type T
             * \param in node to visit
             * \param func lambda to use as visitor
             */
            void iterateReferenceObjects(VariantNode *in, std::function<void (T *)> func) {

                if (in) {
                    boost::any &a = in->data();
                    if(!a.empty())
                    {
                        try
                        {
                            if(isType<T>(a))
                            {
                                std::shared_ptr<T> *s =
                                        boost::any_cast< std::shared_ptr<T>  >(&a);
                                func(s->get());
                            }
                        }
                        catch(...)
                        {

                        }
                    }
                    //
                    for (auto i = in->children().begin(); i != in->children().end(); i++) {
                        if (i->second) {
                            iterateReferenceObjects(i->second, func);
                        }
                    }
                }
            }

    };

    /*!
        \brief stringToXml
        \param s    XML document in string form
        \param doc  receives parsed XML document
        \return true on success
    */
    inline bool stringToXml(const std::string &s, wxXmlDocument &doc) {
        wxString in(s);
        wxStringInputStream is(in);
        return doc.Load(is);
    }

    /*!
        \brief xmlToString
        \param doc Document to convert to string
        \param s receives XML document as string
        \return true on success
    */
    inline bool xmlToString(wxXmlDocument &doc, std::string &s) {
        wxString out;
        wxStringOutputStream os(&out);
        bool ret = doc.Save(os);
        s = out.ToStdString();
        return ret;
    }

    /*!
        \brief stringToJson
        \param s string to convert to JSON value
        \param v JSON value
        \return true on success
    */
    inline bool stringToJson(const std::string &s, Json::Value &v) {
        if (!s.empty()) {
            try {
                std::istringstream is;
                is.str(s);
                v.clear();
                is >> v;
                return true;
            }
            catch (...) {

            }
        }
        return false;
    }

    /*!
        \brief jsonToString
        \param v JSON value to convert to string
        \param s recieves JSON value as string
        \return true on success
    */
    inline bool jsonToString(Json::Value &v, std::string &s) {
        try {
            std::ostringstream os;
            os << v << std::endl;
            s = os.str();
            return true;
        }
        catch (...) {

        }
        return false;
    }

#define XML_MIN_SIZE (20)

    // interval times to integer
    /*!
     * \brief stringTimeToInt
     * \param s time in string form
     * \return time as integer
     */
    time_t stringTimeToInt(const std::string &s);

}

#endif // VARIANTPROPERTYTREE_H
