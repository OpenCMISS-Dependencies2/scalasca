
#include <string.h>

#include <string>
#include <vector>
#include <map>

#include <otf2/otf2.h>

//@ class, which holds all definitions
class Otf2Definitions
{
public:
    Otf2Definitions( OTF2_Reader* reader );
    ~Otf2Definitions();

    typedef std::map< OTF2_StringRef, std::string > StringT;
    typedef StringT::iterator       StringIT;
    typedef StringT::const_iterator StringCIT;
    typedef StringT::value_type     StringV;
    StringT     m_Strings;
    //@ There are definitions, which don't have an Id. for example the TimeRange
    //@ collect them all into an vector
    @otf2 for def in defs|global_defs|not_referable_defs|not_supplement_defs:
    struct @@def.name@@
    {
        @otf2 for attr in def.attributes:
        @otf2  if attr.type == 'OTF2_StringRef':
        std::string @@attr.name@@;
        @otf2  elif attr is array_attr:
        @otf2   for array_attr in attr.array_attributes:
        std::vector< @@array_attr.type@@ > @@array_attr.name@@;
        @otf2   endfor
        @otf2  else:
        @@attr.type@@ @@attr.name@@;
        @otf2  endif
        @otf2 endfor
    };
    typedef std::vector< @@def.name@@* > @@def.name@@T;
    typedef @@def.name@@T::iterator       @@def.name@@IT;
    typedef @@def.name@@T::const_iterator @@def.name@@CIT;
    typedef @@def.name@@T::value_type     @@def.name@@V;
    @otf2  if def is not supplement_def:
    @@def.name@@T m_@@def.name|pluralize@@;
    @otf2  endif

    @otf2 endfor
    //@ supplement definitions to super definitions.
    @otf2 for def in defs|global_defs|supplement_defs:
    struct @@def.name@@
    {
        @otf2 for attr in def.attributes[1:]:
        @otf2  if attr.type == 'OTF2_StringRef':
        std::string @@attr.name@@;
        @otf2  elif attr is array_attr:
        @otf2   for array_attr in attr.array_attributes:
        std::vector< @@array_attr.type@@ > @@array_attr.name@@;
        @otf2   endfor
        @otf2  else:
        @@attr.type@@ @@attr.name@@;
        @otf2  endif
        @otf2 endfor
    };
    typedef std::vector< @@def.name@@* > @@def.name@@T;
    typedef @@def.name@@T::iterator       @@def.name@@IT;
    typedef @@def.name@@T::const_iterator @@def.name@@CIT;
    typedef @@def.name@@T::value_type     @@def.name@@V;

    @otf2 endfor
    @otf2 for def in defs|global_defs|shared_defs|first_born_defs|referable_defs|exclude(String):
    enum @@def.ref_name@@Type
    {
        @otf2 for sibling in def.siblings:
        @@sibling.upper@@,
        @otf2 endfor
    };
    struct @@def.ref_name@@
    {
        @@def.ref_name@@Type @@def.ref_lname@@Type;
        @otf2 for supp_def in def.supplements:
        @@supp_def.name@@T @@supp_def.lname|pluralize@@;
        @otf2 endfor
    };
    typedef std::map< @@def.self_attr.type@@, @@def.ref_name@@* > @@def.ref_name@@T;
    typedef @@def.ref_name@@T::iterator       @@def.ref_name@@IT;
    typedef @@def.ref_name@@T::const_iterator @@def.ref_name@@CIT;
    typedef @@def.ref_name@@T::value_type     @@def.ref_name@@V;
    @@def.ref_name@@T m_@@def.ref_name|pluralize@@;

    @otf2 endfor
    //@ normal definitions have an id, as the first attribute
    //@ special case the String def
    @otf2 for def in defs|global_defs|referable_defs|exclude(String):
    struct @@def.name@@
    @otf2 if def is shared_def:
        : public @@def.ref_name@@
    @otf2 endif
    {
        @otf2 for attr in def.attributes[1:]:
        @otf2  if attr.type == 'OTF2_StringRef':
        std::string @@attr.name@@;
        @otf2  elif attr is array_attr:
        @otf2   for array_attr in attr.array_attributes:
        std::vector< @@array_attr.type@@ > @@array_attr.name@@;
        @otf2   endfor
        @otf2  else:
        @@attr.type@@ @@attr.name@@;
        @otf2  endif
        @otf2 endfor
        @otf2 if def is not shared_def:
        @otf2  for supp_def in def.supplements:
        @@supp_def.name@@T @@supp_def.lname|pluralize@@;
        @otf2  endfor
        @otf2 endif
    };
    @otf2 if def is not shared_def:
    typedef std::map< @@def.self_attr.type@@, @@def.name@@* > @@def.name@@T;
    typedef @@def.name@@T::iterator       @@def.name@@IT;
    typedef @@def.name@@T::const_iterator @@def.name@@CIT;
    typedef @@def.name@@T::value_type     @@def.name@@V;
    @@def.name@@T m_@@def.name|pluralize@@;
    @otf2 endif

    @otf2 endfor
    @otf2 for def in defs|global_defs:
    OTF2_CallbackCode
    HandleDef@@def.name@@( @@def.funcargs(leading_comma=False)@@ );

    @otf2 endfor
private:
    OTF2_Reader* m_Reader;
};

OTF2_CallbackCode
Otf2Definitions::HandleDefString( OTF2_StringRef stringID,
                                  const char*    string )
{
    m_Strings[ stringID ] = std::string( string );

    return OTF2_CALLBACK_SUCCESS;
}

//@ callbacks for defs without id
@otf2 for def in defs|global_defs|not_referable_defs|not_supplement_defs:
OTF2_CallbackCode
Otf2Definitions::HandleDef@@def.name@@( @@def.funcargs(leading_comma=False)@@ )
{
    @@def.name@@& new_@@def.lower@@ = *( new @@def.name@@ );

    @otf2 for attr in def.attributes:
    @otf2  if attr.type == 'OTF2_StringRef':
    new_@@def.lower@@.@@attr.name@@ = m_Strings[ @@attr.name@@ ];
    @otf2  elif attr is array_attr:
    @otf2   for array_attr in attr.array_attributes:
    std::vector< @@array_attr.type@@ > @@array_attr.name@@_vector( @@array_attr.name@@, @@array_attr.name@@ + @@attr.name@@ );
    new_@@def.lower@@.@@array_attr.name@@ = @@array_attr.name@@_vector;
    @otf2   endfor
    @otf2  else:
    new_@@def.lower@@.@@attr.name@@ = @@attr.name@@;
    @otf2  endif
    @otf2 endfor

    m_@@def.name|pluralize@@.push_back( &new_@@def.lower@@ );

    return OTF2_CALLBACK_SUCCESS;
}

@otf2 endfor
@otf2 for def in defs|global_defs|supplement_defs:
OTF2_CallbackCode
Otf2Definitions::HandleDef@@def.name@@( @@def.funcargs(leading_comma=False)@@ )
{
    @otf2 if def.parent_def is shared_def:
    @@def.parent_def.ref_name@@IT parent_it = m_@@def.parent_def.ref_name|pluralize@@.find( @@def.attributes[0].name@@ );
    @otf2 else:
    @@def.parent_def.name@@IT parent_it = m_@@def.parent_def.name|pluralize@@.find( @@def.attributes[0].name@@ );
    @otf2 endif
    @@def.name@@& new_@@def.lower@@ = *( new @@def.name@@ );

    @otf2 for attr in def.attributes[1:]:
    @otf2  if attr is array_attr:
    @otf2   for array_attr in attr.array_attributes:
    std::vector< @@array_attr.type@@ > @@array_attr.name@@_vector( @@array_attr.name@@, @@array_attr.name@@ + @@attr.name@@ );
    new_@@def.lower@@.@@array_attr.name@@ = @@array_attr.name@@_vector;
    @otf2   endfor
    @otf2  else:
    new_@@def.lower@@.@@attr.name@@ = @@attr.name@@;
    @otf2  endif
    @otf2 endfor

    @otf2 if def.parent_def is shared_def:
    parent_it->second->@@def.lname|pluralize@@.push_back( &new_@@def.lower@@ );
    @otf2 else:
    parent_it->second->@@def.lname|pluralize@@.push_back( &new_@@def.lower@@ );
    @otf2 endif

    return OTF2_CALLBACK_SUCCESS;
}

@otf2 endfor
//@ callbacks for defs with id
@otf2 for def in defs|global_defs|referable_defs|exclude(String):
OTF2_CallbackCode
Otf2Definitions::HandleDef@@def.name@@( @@def.funcargs(leading_comma=False)@@ )
{
    @@def.name@@& new_@@def.lower@@ = *( new @@def.name@@ );

    @otf2 if def is shared_def:
    new_@@def.lower@@.@@def.ref_lname@@Type = @@def.upper@@;

    @otf2 endif
    @otf2 for attr in def.attributes[1:]:
    @otf2  if attr.type == 'OTF2_StringRef':
    new_@@def.lower@@.@@attr.name@@ = m_Strings[ @@attr.name@@ ];
    @otf2  elif attr is array_attr:
    @otf2   for array_attr in attr.array_attributes:
    std::vector< @@array_attr.type@@ > @@array_attr.name@@_vector( @@array_attr.name@@, @@array_attr.name@@ + @@attr.name@@ );
    new_@@def.lower@@.@@array_attr.name@@ = @@array_attr.name@@_vector;
    @otf2   endfor
    @otf2  else:
    new_@@def.lower@@.@@attr.name@@ = @@attr.name@@;
    @otf2  endif
    @otf2 endfor

    @otf2 if def is shared_def:
    m_@@def.ref_name|pluralize@@[ @@def.self_attr.name@@ ] = &new_@@def.lower@@;
    @otf2 else:
    m_@@def.name|pluralize@@[ @@def.self_attr.name@@ ] = &new_@@def.lower@@;
    @otf2 endif

    return OTF2_CALLBACK_SUCCESS;
}

@otf2 endfor

extern "C" {

@otf2 for def in defs|global_defs:

static OTF2_CallbackCode
@@def.lower@@_forward_cb( void* userData @@def.funcargs()@@ )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDef@@def.name@@( @@def.callargs(leading_comma=False)@@ );
}

@otf2 endfor

} // extern "C"

Otf2Definitions::Otf2Definitions( OTF2_Reader* reader ) :
    m_Reader( reader )
{
    OTF2_GlobalDefReader* def_reader = OTF2_Reader_GetGlobalDefReader( m_Reader );

    OTF2_GlobalDefReaderCallbacks* def_callbacks = OTF2_GlobalDefReaderCallbacks_New();

    @otf2 for def in defs|global_defs:
    OTF2_GlobalDefReaderCallbacks_Set@@def.name@@Callback( def_callbacks, @@def.lower@@_forward_cb );
    @otf2 endfor

    OTF2_Reader_RegisterGlobalDefCallbacks( m_Reader, def_reader, def_callbacks, this );

    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllGlobalDefinitions( m_Reader, def_reader, &definitions_read );

    OTF2_GlobalDefReaderCallbacks_Delete( def_callbacks );

    OTF2_Reader_CloseGlobalDefReader( m_Reader,
                                      def_reader );
}

Otf2Definitions::~Otf2Definitions()
{
    @otf2 for def in defs|global_defs|not_referable_defs|not_supplement_defs:
    for ( @@def.name@@IT it = m_@@def.name|pluralize@@.begin();
          it != m_@@def.name|pluralize@@.end();
          it++ )
    {
        delete *it;
    }

    @otf2 endfor
    @otf2 for def in defs|global_defs|referable_defs|first_born_defs|exclude(String):
    for ( @@def.ref_name@@IT it = m_@@def.ref_name|pluralize@@.begin();
          it != m_@@def.ref_name|pluralize@@.end();
          it++ )
    {
        @otf2 for supp_def in def.supplements:
        for ( @@supp_def.name@@IT sit = it->second->@@supp_def.lname|pluralize@@.begin();
              sit != it->second->@@supp_def.lname|pluralize@@.end();
              sit++ )
        {
            delete *sit;
        }
        @otf2 endfor
        delete it->second;
    }

    @otf2 endfor
}
