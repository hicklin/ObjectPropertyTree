#include "variantpropertytree.h"
#include <wx/datstrm.h>
#include <wx/file.h>
#include <wx/wfstream.h>
#include <wx/log.h>
#include "json/json.h"
//
static logair::VariantNode _dummyNode;

/*
 *  logair::stringToBool
 */
bool logair::stringToBool(const std::string &s)
{
    static const char *trueStr[] = {"True","true","1"};
    for(int i = 0; i < 3; i++)
        if(s == trueStr[i]) return true;
    return false;
}

/*
 *  logair::VariantPropertyTree::load
 */
bool logair::VariantTree::load(const std::string &file) // load from file
{
    wxXmlDocument doc;
    if(doc.Load(file))
    {
        fromXml(doc);
        return true;
    }
    return false;
}

/*
 *  logair::VariantPropertyTree::save
 */
bool logair::VariantTree::save(const std::string &file) // save to file
{
    wxXmlDocument doc;
    toXml(doc);
    return doc.Save(file);
}

/*
 *  logair::VariantPropertyTree::sync
 */
void logair::VariantTree::sync(logair::VariantTree &/*tree*/)
{

}

/*
 *  logair::VariantTree::printNode
 */
void logair::VariantTree::printNode(std::ostream &os, VariantNode *n, int level)
{
    if(n)
    {
        std::string indent(level,' ');
        os << indent << n->name();
        VariantStreamerBase *v = VariantStreamerBase::findByTypename(n->data().type().name());
        if(v)
        {
            os << ":" <<  v->id();
        }
        else
        {
           os << ":" <<  n->data().type().name();
        }

        if(n->data().empty())
        {
           os << ": EMPTY";
        }
        else
        {
           os << " : " << VariantStreamerBase::anyToString(n->data());
        }
        os << std::endl;
        if(n->children().size() > 0)
        {
            level++;
            for(auto i = n->children().begin(); i != n->children().end(); i++)
            {
                printNode(os,i->second,level); // recurse
            }
        }
    }
}

/*
 *  logair::VariantTree::fromJson
 */
void logair::VariantTree::fromJson(VariantNode *n, Json::Value &v)
{
   if(n && !v.isNull())
   {
       getJson(v,n->data());
       if(v.isObject())
       {
           Json::Value::Members m = v.getMemberNames();
           for(int i = 0; i < int(m.size()); i++)
           {
               Json::Value &cv =  v[m[i]];
               if(!cv.isNull()) // can only recurse objects
               {
                   VariantNode *ch = new VariantNode(m[i],n);
                   fromJson(ch,cv);
                   n->addChild(ch);
               }
           }
       }
   }
}

/*
 *  logair::VariantTree::toJson
 */
void logair::VariantTree::toJson(VariantNode *n, Json::Value &v)
{
   if(n)
   {
       setJson(v,n->data()); // set the JSON data
       if(n->children().size() > 0)
       {
           for(auto i = n->children().begin(); i != n->children().end(); i++)
           {
               toJson(i->second,v[i->first]);
           }
       }
   }
}

/*
*  toXml
*/
void logair::VariantTree::toXml(VariantNode *n, wxXmlNode *parentNode)
{
  if(n)
  {
      setXml(parentNode,n->data()); // set the XML data
      if(n->children().size() > 0)
      {
          for(auto i = n->children().begin(); i != n->children().end(); i++)
          {
              wxXmlNode *x = new wxXmlNode(parentNode,wxXML_ELEMENT_NODE,"NODE");
              x->AddAttribute("Name",wxString(i->first));
              toXml(i->second,x);
          }
      }
  }
}

//
// load from XML
//
/*
*  fromXml
*/
void logair::VariantTree::fromXml(VariantNode *n, wxXmlNode *x)
{
  if(n)
  {
      n->clear();
      getXml(x,n->data());
      wxXmlNode *c = x->GetChildren();
      while(c != nullptr)
      {
          if(c->GetType() == wxXML_ELEMENT_NODE) // only want element nodes
          {
              std::string cn = c->GetAttribute("Name").ToStdString();
              if(!cn.empty()) // skip empty nodes
              {
                  VariantNode *ch = new VariantNode(cn,n);
                  fromXml(ch,c); // recurse
                  n->addChild(ch); // add to tree
              }
          }
          c = c->GetNext();
      }
  }
}

/*
 *  logair::stringTimeToInt
 */
time_t  logair::stringTimeToInt(const std::string &s) {
    time_t ret = 0;
    boost::char_separator<char> sep(":");
    tokenizer tokens(s, sep);
    std::vector<std::string> l;
    for (auto i = tokens.begin(); i != tokens.end(); i++) {
        l.push_back(*i);
    }

    switch (l.size()) {
        case 1:
            ret = time_t(std::stoi(l[0]));
            break;
        case 2:
            ret = time_t(std::stoi(l[1]) * 60) + time_t(std::stoi(l[0]));
            break;
        case 3:
            ret = time_t(std::stoi(l[0]) * 3600) + time_t(std::stoi(l[1]) * 60) + time_t(std::stoi(l[2])) ;
            break;
        default:
            break;
    }
    return ret;
}


