#include "TinyXMLSettings.h"
#include <iostream>

TinyXMLSettings::TinyXMLSettings() :
    modified(false)
{
    abort();//todo to home folder
    document.LoadFile("");
    whereIs=document.RootElement();
    if(whereIs==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", unable to link the base root" << std::endl;
        abort();
    }
}

TinyXMLSettings::TinyXMLSettings(const std::string &file) :
    modified(false)
{
    this->file=file;
    const auto loadOkay = document.CATCHCHALLENGER_XMLDOCUMENTLOAD(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(file));
    if(!CATCHCHALLENGER_XMLDOCUMENTRETURNISLOADED(loadOkay))
    {
        modified=true;
        const auto &errorId=document.CATCHCHALLENGER_XMLERRORID();
        #ifdef CATCHCHALLENGER_XLMPARSER_TINYXML1
        if(errorId==2 /*NOT 12! Error document empty. but can be just corrupted!*/ && errno==2)
        #elif defined(CATCHCHALLENGER_XLMPARSER_TINYXML2)
        if(errorId==tinyxml2::XML_ERROR_FILE_NOT_FOUND)
        #endif
        {
            #ifdef CATCHCHALLENGER_XLMPARSER_TINYXML1
            CATCHCHALLENGER_XMLELEMENT * root = new CATCHCHALLENGER_XMLELEMENT("configuration");
            document.LinkEndChild(root);
            #elif defined(CATCHCHALLENGER_XLMPARSER_TINYXML2)
            tinyxml2::XMLNode * pRoot = document.NewElement("configuration");
            document.InsertFirstChild(pRoot);
            #endif
        }
        else
        {
            const CATCHCHALLENGER_XMLDOCUMENT * const documentBis=&document;
            std::cerr << file+", "+CATCHCHALLENGER_XMLDOCUMENTERROR(documentBis) << std::endl;
            abort();
        }
    }
    whereIs=document.RootElement();
    if(whereIs==NULL)
    {
        std::cerr << "Unable to open the file: " << file << ", unable to link the base root" << std::endl;
        abort();
    }
}

TinyXMLSettings::~TinyXMLSettings()
{
    sync();
}

void TinyXMLSettings::beginGroup(const std::string &group)
{
    CATCHCHALLENGER_XMLELEMENT * item = whereIs->FirstChildElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(group));
    if(item==NULL)
    {
        #ifdef CATCHCHALLENGER_XLMPARSER_TINYXML1
        CATCHCHALLENGER_XMLELEMENT item(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(group));
        whereIs->InsertEndChild(item);
        whereIs=whereIs->FirstChildElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(group));
        #elif defined(CATCHCHALLENGER_XLMPARSER_TINYXML2)
        tinyxml2::XMLNode * item = document.NewElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(group));
        whereIs->InsertFirstChild(item);
        whereIs=whereIs->FirstChildElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(group));
        #endif
    }
    else
        whereIs=item;
}

void TinyXMLSettings::endGroup()
{
    if(whereIs==document.RootElement())
        return;
    CATCHCHALLENGER_XMLELEMENT * item = static_cast<CATCHCHALLENGER_XMLELEMENT *>(whereIs->Parent());
    if(item!=NULL)
        whereIs=item;
}

std::string TinyXMLSettings::value(const std::string &var, const std::string &defaultValue)
{
    CATCHCHALLENGER_XMLELEMENT * item = whereIs->FirstChildElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(var));
    if(item==NULL)
        return defaultValue;
    else
    {
        if(item->Attribute("value")==NULL)
            return defaultValue;
        else
            return item->Attribute("value");
    }
}

bool TinyXMLSettings::contains(const std::string &var)
{
    return whereIs->FirstChildElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(var))!=NULL;
}

void TinyXMLSettings::setValue(const std::string &var,const std::string &value)
{
    CATCHCHALLENGER_XMLELEMENT * item = whereIs->FirstChildElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(var));
    if(item==NULL)
    {
        modified=true;
        #ifdef CATCHCHALLENGER_XLMPARSER_TINYXML1
        CATCHCHALLENGER_XMLELEMENT item(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(var));
        item.SetAttribute("value",CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(value));
        whereIs->InsertEndChild(item);
        #elif defined(CATCHCHALLENGER_XLMPARSER_TINYXML2)
        tinyxml2::XMLElement * item = document.NewElement(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(var));
        item->SetAttribute("value",CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(value));
        whereIs->InsertFirstChild(item);
        #endif

    }
    else
    {
        if(item->Attribute("value")==value)
            return;
        modified=true;
        item->SetAttribute("value",CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(value));
    }
}

void TinyXMLSettings::setValue(const std::string &var,const int &value)
{
    setValue(var,std::to_string(value));
}

void TinyXMLSettings::setValue(const std::string &var,const double &value)
{
    setValue(var,std::to_string(value));
}

void TinyXMLSettings::setValue(const std::string &var,const bool &value)
{
    if(value)
        setValue(var,std::string("true"));
    else
        setValue(var,std::string("false"));
}

void TinyXMLSettings::setValue(const std::string &var,const char * const value)
{
    setValue(var,std::string(value));
}

void TinyXMLSettings::sync()
{
    if(!modified)
        return;
    const auto &returnVar=document.SaveFile(CATCHCHALLENGER_XMLSTDSTRING_TONATIVESTRING(file));
    #ifdef CATCHCHALLENGER_XLMPARSER_TINYXML1
    if(!returnVar)
    {
        std::cerr << "Unable to save the file: " << file << ", errno: " << std::to_string(errno) << std::endl;
        abort();
    }
    #elif defined(CATCHCHALLENGER_XLMPARSER_TINYXML2)
    if(returnVar!=tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Unable to save the file: " << file << ", returnVar: " << std::to_string(returnVar) << std::endl;
        abort();
    }
    #endif
    modified=false;
}
