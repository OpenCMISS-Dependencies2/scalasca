/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "Pattern.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <sstream>
#include <vector>

#include "Helper.h"
#include "IndentStream.h"

using namespace std;


// --- Global variables -----------------------------------------------------

extern map< string, Pattern* > id2pattern;
extern vector< Pattern* >      pattern;


// --- Function prototypes --------------------------------------------------

extern void
yyerror(const string& message);


// --- Utility functions ----------------------------------------------------

namespace
{
struct fo_tolower
    : public std::unary_function< int, int >
{
    int
    operator()(int x) const
    {
        return std::tolower(x);
    }
};


string
lowercase(const string& str)
{
    string result(str);

    transform(str.begin(), str.end(), result.begin(), fo_tolower());

    return result;
}


string
wrappedCharConstant(string    text,
                    const int width,
                    const int indent)
{
    string result("\"");

    while (text.length() > width)
    {
        string::size_type pos = text.rfind(' ', width);

        result += text.substr(0, pos);
        if (pos != string::npos)
        {
            result += "\"\n";
            result += string(indent, ' ') + '"';
        }
        text.erase(0, pos);
    }

    return (result + text + '"');
}
}    // unnamed namespace


/*--- Adding callbacks ----------------------------------------------------*/

bool
Pattern::add_callback(const std::string& group,
                      const string&      event,
                      const string&      code)
{
    std::string cbname(event + "_" + group);

    map< string, string >::iterator it = m_callbacks.lower_bound(cbname);
    if (  (it != m_callbacks.end())
       && !(m_callbacks.key_comp()(cbname, it->first)))
    {
        return false;
    }

    m_callbacks.insert(it, make_pair(cbname, code));
    m_callbackgroups[group].push_back(event);

    return true;
}


/*--- Checking validity ---------------------------------------------------*/

missing_t
Pattern::is_valid() const
{
    if (m_name.empty())
    {
        return DEF_NAME;
    }
    if (m_class.empty())
    {
        return DEF_CLASS;
    }
    if (m_info.empty())
    {
        return DEF_INFO;
    }
    if (m_unit.empty())
    {
        return DEF_UNIT;
    }
    if (m_mode.empty())
    {
        return DEF_MODE;
    }

    return NOTHING;
}


/*--- Writing output ------------------------------------------------------*/

void
Pattern::write_impl(FILE* fp,
                    int&  indent) const
{
    if (skip_impl())
    {
        return;
    }

    // *INDENT-OFF*
    const struct patterntype
    {
        const char* type;
        const char* begin;
        const char* end;
        const char* baseclass;
    }
    patterntypes[] = {
        { "Generic", 0, 0, "Pattern" },
        { "MPI",
          "#if defined(_MPI)\n",
          "#endif    // _MPI\n",
          "MpiPattern"
        },
        { "MPIDEP",
          "#if defined(_MPI)\n",
          "#endif    // _MPI\n",
          "Pattern",
        },
        { "MPI_RMA",
          "#if defined(_MPI) && defined(HAS_MPI2_1SIDED)\n",
          "#endif    // _MPI && HAS_MPI2_1SIDED\n",
          "MpiPattern",
        },
        { "MPI_RMADEP",
          "#if defined(_MPI) && defined(HAS_MPI2_1SIDED)\n",
          "#endif    // _MPI && HAS_MPI2_1SIDED\n",
          "Pattern",
        },
        { "OMP",
          "#if defined(_OPENMP)\n",
          "#endif    // _OPENMP\n",
          "OmpPattern",
        },
        { "OMPDEP",
          "#if defined(_OPENMP)\n",
          "#endif    // _OPENMP\n",
          "Pattern",
        },
        { "PTHREAD",
          "#if defined(_OPENMP)\n",
          "#endif    // _OPENMP\n",
          "PthreadPattern",
        },
        { "ARMCI",
          "#if defined(HAS_ARMCI)\n",
          "#endif    // HAS_ARMCI\n",
          "ArmciPattern"
        },
        { "ARMCIDEP",
          "#if defined(HAS_ARMCI)\n",
          "#endif    // HAS_ARMCI\n",
          "Pattern"
        },

        { 0, 0, 0, 0 }
    };
    // *INDENT-ON*


    const struct patterntype* pt;

    for (pt = patterntypes; pt->type && (m_type != pt->type); ++pt)
    {
    }

    /***** Header *****/

    /* #ifdef */
    if (pt->begin)
    {
        IndentStream(fp, indent)
            << pt->begin;
        indent++;
    }

    IndentStream(fp, indent)
        << separatorComment(indent, C_STYLE) << '\n'
        << "/**\n"
        << " *  @class " << m_class << '\n'
        << " *  @brief " << get_docname() << " metric.\n"
        << " *\n"
        << " *  Implements the \"" << get_docname() << "\" metric.\n"
        << " */\n"
        << separatorComment(indent, C_STYLE) << '\n'
        << '\n';

    IndentStream(fp, indent)
        << "class " << m_class << '\n'
        << "    : public " << pt->baseclass << '\n'
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "public:\n";
    indent++;

    /***** Destructor *****/

    if (!m_cleanup.empty())
    {
        IndentStream(fp, indent)
            << "/// @name Constructors & destructor\n"
            << "/// @{\n"
            << '\n'
            << "virtual\n"
            << '~' << m_class << "()\n"
            << "{\n";
        indent++;

        IndentStream(fp, indent)
            << m_cleanup << '\n';

        indent--;
        IndentStream(fp, indent)
            << "}\n"
            << '\n'
            << "/// @}\n";
    }

    /***** reg_cb() *****/

    if (!m_callbacks.empty())
    {
        IndentStream(fp, indent)
            << "/// @name Registering callbacks\n"
            << "/// @{\n"
            << '\n'
            << "virtual void\n"
            << "reg_cb(CallbackManagerMap& cbmanagers);\n"
            << '\n'
            << "/// @}\n";
    }

    /***** get_id() *****/

    IndentStream(fp, indent)
        << "/// @name Retrieving pattern information\n"
        << "/// @{\n"
        << '\n'
        << "virtual long\n"
        << "get_id() const\n"
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "return PAT_" << m_id << ";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** get_parent() *****/

    IndentStream(fp, indent)
        << "virtual long\n"
        << "get_parent() const\n"
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "return PAT_" << m_parent << ";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** get_name() *****/

    IndentStream(fp, indent)
        << "virtual string\n"
        << "get_name() const\n"
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "return " << wrappedCharConstant(m_name, 52, 7) << ";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** get_unique_name() *****/

    IndentStream(fp, indent)
        << "virtual string\n"
        << "get_unique_name() const\n"
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "return \"" << lowercase(m_id) << "\";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** get_descr() *****/

    IndentStream(fp, indent)
        << "virtual string\n"
        << "get_descr() const\n"
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "return " << wrappedCharConstant(m_info, 52, 7) << ";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** get_unit() *****/

    IndentStream(fp, indent)
        << "virtual string\n"
        << "get_unit() const\n"
        << "{\n";
    indent++;

    IndentStream(fp, indent)
        << "return \"" << m_unit << "\";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** get_mode() *****/

    IndentStream(fp, indent)
        << "virtual CubeMetricType\n"
        << "get_mode() const\n"
        << "{\n";
    indent++;

    string mode;
    if (m_mode == "inclusive")
    {
        mode = "CUBE_METRIC_INCLUSIVE";
    }
    else if (m_mode == "exclusive")
    {
        mode = "CUBE_METRIC_EXCLUSIVE";
    }
    IndentStream(fp, indent)
        << "return " << mode << ";\n";

    indent--;
    IndentStream(fp, indent)
        << "}\n"
        << '\n';

    /***** is_hidden() *****/

    if (m_hidden)
    {
        IndentStream(fp, indent)
            << "virtual bool\n"
            << "is_hidden() const\n"
            << "{\n";
        indent++;

        IndentStream(fp, indent)
            << "return true;\n";

        indent--;
        IndentStream(fp, indent)
            << "}\n"
            << '\n';
    }

    IndentStream(fp, indent)
        << "/// @}\n";

    /***** Callback methods *****/

    if (!m_callbacks.empty())
    {
        IndentStream(fp, indent)
            << "/// @name Callback methods\n"
            << "/// @{\n"
            << '\n';

        map< string, string >::const_iterator it = m_callbacks.begin();
        while (it != m_callbacks.end())
        {
            // Parameter indent: callback name + "_cb("
            string paramIndent(it->first.length() + 4, ' ');

            IndentStream(fp, indent)
                << "void\n" << lowercase(it->first) << "_cb("
                << "const CallbackManager& cbmanager,\n"
                << paramIndent
                << "int                    user_event,\n"
                << paramIndent
                << "const Event&           event,\n"
                << paramIndent
                << "CallbackData*          cdata);\n"
                << '\n';
            ++it;
        }

        IndentStream(fp, indent)
            << "/// @}\n";
    }

    /***** end of public stuff *****/

    indent--;

    /***** init() *****/

    if (!m_init.empty())
    {
        IndentStream(fp, indent)
            << '\n'
            << '\n'
            << "protected:\n";
        indent++;

        IndentStream(fp, indent)
            << "/// @name Pattern management\n"
            << "/// @{\n"
            << '\n'
            << "virtual void\n"
            << "init()\n"
            << "{\n";
        indent++;

        IndentStream(fp, indent)
            << m_init << '\n';

        indent--;
        IndentStream(fp, indent)
            << "}\n"
            << '\n'
            << "/// @}\n";

        indent--;
    }

    /***** Data *****/

    if (!m_data.empty())
    {
        IndentStream(fp, indent)
            << '\n'
            << '\n'
            << "private:\n";
        indent++;

        IndentStream(fp, indent)
            << m_data << '\n';

        indent--;
    }

    /***** Footer *****/

    indent--;
    IndentStream(fp, indent)
        << "};\n"
        << '\n'
        << '\n';

    /***** Static class data *****/

    if (!m_staticinit.empty())
    {
        IndentStream(fp, indent)
            << separatorComment(indent, CPP_STYLE, "Static class data") << '\n'
            << '\n'
            << m_staticinit << '\n'
            << '\n'
            << '\n';
    }

    /***** Callback method registration & implementation *****/

    if (!m_callbacks.empty())
    {
        IndentStream(fp, indent)
            << separatorComment(indent, CPP_STYLE, "Registering callbacks") << '\n'
            << '\n'
            << "void\n"
            << m_class << "::" << "reg_cb(CallbackManagerMap& cbmanagers)\n"
            << "{\n";
        indent++;

        IndentStream(fp, indent)
            << "init();\n"
            << '\n'
            << "CallbackManagerMap::iterator    it;\n"
            << "CallbackManagerMap::mapped_type cbmanager;\n";

        for (map< string, vector< string > >::const_iterator git = m_callbackgroups.begin();
             git != m_callbackgroups.end();
             ++git)
        {
            string phase;
            if (git->first.empty())
            {
                phase = "main";
            }
            else
            {
                phase = git->first;
            }

            IndentStream(fp, indent)
                << '\n'
                << "// Register " << phase << " callbacks\n"
                << "it = cbmanagers.find(\"" << lowercase(git->first) << "\");\n"
                << "assert(it != cbmanagers.end());\n"
                << '\n'
                << "cbmanager = it->second;\n";

            for (vector< string >::const_iterator cit = git->second.begin();
                 cit != git->second.end();
                 ++cit)
            {
                IndentStream(fp, indent)
                    << "cbmanager->register_callback(\n"
                    << "    " << (*cit) << ",\n"
                    << "    PEARL_create_callback(\n"
                    << "        this,\n"
                    << "        &"
                    << m_class
                    << "::"
                    << lowercase(*cit)
                    << '_'
                    << lowercase(git->first)
                    << "_cb));\n";
            }
        }

        indent--;
        IndentStream(fp, indent)
            << "}\n"
            << '\n'
            << '\n';

        IndentStream(fp, indent)
            << separatorComment(indent, CPP_STYLE, "Callback methods") << '\n'
            << '\n';
        map< string, string >::const_iterator it = m_callbacks.begin();
        while (it != m_callbacks.end())
        {
            // Parameter indent: class name + "::" + callback name + "_cb("
            string paramIndent(m_class.length() + it->first.length() + 6, ' ');

            IndentStream(fp, indent)
                << "void\n"
                << m_class << "::" << lowercase(it->first) << "_cb("
                << "const CallbackManager& cbmanager,\n"
                << paramIndent
                << "int                    user_event,\n"
                << paramIndent
                << "const Event&           event,\n"
                << paramIndent
                << "CallbackData*          cdata)\n"
                << "{\n";
            indent++;

            // Heuristic to check whether the data object is used
            // Not bullet proof, but should work reasonably well...
            if (it->second.find("data") != string::npos)
            {
                IndentStream(fp, indent)
                    << "CbData* data = static_cast< CbData* >(cdata);\n"
                    << '\n';
            }
            IndentStream(fp, indent)
                << it->second << '\n';

            indent--;
            IndentStream(fp, indent)
                << "}\n";

            ++it;
            if (  (it != m_callbacks.end())
               || !pt->end)
            {
                IndentStream(fp, indent)
                    << '\n'
                    << '\n';
            }
        }
    }

    /* #endif */
    if (pt->end)
    {
        indent--;
        IndentStream(fp, indent)
            << pt->end
            << '\n'
            << '\n';
    }
}


void
Pattern::write_html(FILE* fp,
                    bool  isFirst)
{
    if (m_nodocs)
    {
        return;
    }

    process_html(m_descr);
    process_html(m_diag);

    fprintf(fp, "\n");
    if (!isFirst)
    {
        fprintf(fp, "<hr width=\"75%%\" align=\"center\">\n\n");
    }
    fprintf(fp, "<a name=\"%s\"><h3>%s</h3></a>\n",
            lowercase(m_id).c_str(), get_docname().c_str());
    if (m_hidden)
    {
        fprintf(fp, "(only available after "
                    "<a href=\"#remapping_info\">remapping</a>)\n");
    }
    fprintf(fp, "<dl>\n");

    /* Description */
    fprintf(fp, "<dt><b>Description:</b></dt>\n");
    if (m_descr.empty())
    {
        if (m_info.empty())
        {
            fprintf(fp, "<dd>No description available.</dd>\n");
        }
        else
        {
            fprintf(fp, "<dd>\n%s\n</dd>\n", m_info.c_str());
        }
    }
    else
    {
        fprintf(fp, "<dd>\n%s\n</dd>\n", m_descr.c_str());
    }

    /* Unit */
    fprintf(fp, "<dt><b>Unit:</b></dt>\n");
    if (m_unit == "sec")
    {
        fprintf(fp, "<dd>Seconds</dd>\n");
    }
    else if (m_unit == "occ")
    {
        fprintf(fp, "<dd>Counts</dd>\n");
    }
    else if (m_unit == "bytes")
    {
        fprintf(fp, "<dd>Bytes</dd>\n");
    }
    else
    {
        fprintf(stderr, "Unknown unit of measurement!");
        exit(1);
    }

    /* Diagnosis */
    if (!m_diag.empty())
    {
        fprintf(fp, "<dt><b>Diagnosis:</b></dt>\n");
        fprintf(fp, "<dd>\n%s\n</dd>\n", m_diag.c_str());
    }

    /* Parent */
    string                                  parent_name;
    map< string, Pattern* >::const_iterator it = id2pattern.find(m_parent);
    if (it == id2pattern.end())
    {
        parent_name = "None";
    }
    else
    {
        parent_name = "<a href=\"#" + lowercase(it->second->get_id()) + "\">"
                      + it->second->get_docname() + "</a>";
    }
    fprintf(fp, "<dt><b>Parent metric:</b></dt>\n"
                "<dd>%s</dd>\n", parent_name.c_str());

    /* Children */
    fprintf(fp, "<dt><b>Sub-metrics:</b></dt>\n<dd>\n");
    bool                         hasChildren = false;
    vector< Pattern* >::iterator pat         = pattern.begin();
    while (pat != pattern.end())
    {
        if ((*pat)->get_parent() == m_id)
        {
            if (hasChildren)
            {
                fprintf(fp, "<br/>\n");
            }
            fprintf(fp, "    <a href=\"#%s\">%s</a>",
                    lowercase((*pat)->get_id()).c_str(),
                    (*pat)->get_docname().c_str());
            hasChildren = true;
        }
        ++pat;
    }
    if (!hasChildren)
    {
        fprintf(fp, "    None");
    }
    fprintf(fp, "\n</dd>\n");

    fprintf(fp, "</dl>\n");
}


/*--- Private methods -----------------------------------------------------*/

void
Pattern::process_html(string& text)
{
    /* Remove trailing spaces */
    string::reverse_iterator it = text.rbegin();
    while ((it != text.rend()) && (*it == ' '))
    {
        ++it;
    }
    text.erase(it.base(), text.end());

    /* Keyword substitution: Cross-references */
    string::size_type spos = text.find("@ref(");
    while (spos != string::npos)
    {
        /* Search for closing brace */
        string::size_type epos = text.find(")", spos);
        if (epos == string::npos)
        {
            yyerror("Description error: \")\" missing.");
        }

        /* Extract & validate ID */
        string                            id(text, spos + 5, epos - (spos + 5));
        map< string, Pattern* >::iterator pat = id2pattern.find(id);
        if (pat == id2pattern.end())
        {
            ostringstream msg;
            msg << "Description error: Unknown pattern name (" << id << ").";
            yyerror(msg.str().c_str());
        }

        /* Insert anchor */
        text.replace(spos, epos - spos + 1,
                     "<a href=\"#" + lowercase(pat->second->get_id()) + "\">"
                     + pat->second->get_docname() + "</a>");

        /* Search for new reference */
        spos = text.find("@ref(");
    }

    /* Keyword substitution: Images */
    spos = text.find("@img(");
    while (spos != string::npos)
    {
        /* Search for closing brace */
        string::size_type epos = text.find(")", spos);
        if (epos == string::npos)
        {
            yyerror("Description error: \")\" missing.");
        }

        /* Extract image name */
        string id(text, spos + 5, epos - (spos + 5));

        /* Insert image reference */
        text.replace(spos, epos - spos + 1,
                     "<br>\n"
                     "<div align=\"center\">\n"
                     "<img src=\""
                     + id
                     + "\" alt=\""
                     + m_name
                     + " Example\">\n"
                       "</div>\n"
                       "<br>\n");

        /* Search for new reference */
        spos = text.find("@img(");
    }
}
