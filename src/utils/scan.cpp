/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/* SCALASCA Collection & Analysis Nexus:
 * Parses commandline to identify MPI launcher and launch options which are
 * augmented for measurement collection and adjusted for trace analysis.
 * Uses the target MPI executable and number of processes as default experiment
 * title, unless explicitly specified on commandline or via configuation.
 * Verifies experiment configuration prior to (optional) execution.
 * Spawns measurement collection and (when appropriate) trace analysis.
 * Manages output from measurement and analysis and archives it in experiment.
 */


#include <config.h>

#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <UTILS_CStr.h>

#include "MultiRunConfig.h"
#include "epk_archive.h"
#include "epk_conf.h"

#ifdef _SX
    #include <algorithm>
    #include <vector>
#endif    // _SX
#if HAVE(GETOPT_H)
    #include <getopt.h>
#endif    // HAVE(GETOPT_H)

using namespace std;


#if !HAVE(GETOPT_H)
    const int no_argument       = 0;
    const int required_argument = 1;
    const int optional_argument = 2;
    struct option
    {
        const char* name;
        int         has_arg;
        int*        flag;
        int         val;
    };


    extern char* optarg;
    extern int   optind, opterr, optopt;
#endif    // !HAVE(GETOPT_H)
#if !HAVE(DECL_GETOPT_LONG_ONLY)
    extern "C" int
    getopt_long_only(int                  __argc,
                     char* const*         __argv,
                     const char*          __shortopts,
                     const struct option* __longopts,
                     int*                 __longind);
#endif

#ifdef _SX
    /* additional libiberty prototypes required on NEC SX */
    extern "C" int
    setenv(const char* name,
           const char* value,
           int         overwrite);

    extern "C" int
    unsetenv(const char* name);
#endif

const string SCAN          = "S=C=A=N: ";
string       expt_prefix   = "scorep_";
const string expt_tracedir = "/traces";
const string spc           = " ";

unsigned verbose  = 0;
bool     cobalt   = false;
bool     dual_mic = false;    // only used with ibrun.symm "-2"

enum cnmodes
{
    MODE_DEF, MODE_CO, MODE_SN, MODE_VN, MODE_SMP, MODE_DUAL, MODE_QUAD
};


static const char* modes[7] = {
    "", "co", "sn", "vn", "smp", "dual", "quad"
};

enum launch_opt
{
    NO_ARG, NON_OPT, NNODES, NPROCS, NTHRDS, MPICFG, MPICWD, MPIENV,
    TARGET, T_ARGS, T_PATH, CNMODE, SWITCH_D, CPUCMD, MICCMD, IGNORE
};


struct launch_options_t
{
    char*       target;
    char*       t_args;
    char*       target2;
    char*       t_args2;
    char*       t_path;
    const char* cnmode;
    char*       nprocs;
    char*       nthrds;
    char*       mpicwd;
    string      mpienv;
    string      other;
} launch_options = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "", ""
};

int
parse_launch(string launcher,
             bool   measure,
             int    argc,
             char*  argv[])
{
    static int modeopt = MODE_DEF;    // only used for yod

    // *INDENT-OFF*
    static struct option unified_options[] = {
        { "n",                 required_argument, 0,        NPROCS   },
      #if HAVE_PLATFORM_BGL || HAVE_PLATFORM_BGP || HAVE_PLATFORM_BGQ
        { "p",                 required_argument, 0,        CNMODE   },    // ranks-per-node
      #else
        { "p",                 required_argument, 0,        IGNORE   },    // partition
      #endif
        { "np",                required_argument, 0,        NPROCS   },
        { "sz",                required_argument, 0,        NPROCS   },    // yod
        { "size",              required_argument, 0,        NPROCS   },    // yod
        { "proccount",         required_argument, 0,        NPROCS   },    // Cobalt
        // XXXX -nodes is an alias for -np using BG mpirun
        { "nn",                required_argument, 0,        NNODES   },    // NECSX
        { "nodes",             required_argument, 0,        NNODES   },    // SLURM,POE
        { "ntasks-per-socket", required_argument, 0,        IGNORE   },    // SLURM
        { "ntasks-per-core",   required_argument, 0,        IGNORE   },    // SLURM
        { "ntasks-per-node",   required_argument, 0,        CNMODE   },    // SLURM
        { "ntasks",            required_argument, 0,        NPROCS   },    // SLURM
        { "t",                 required_argument, 0,        NTHRDS   },    // llrun@supermuc
        { "time",              required_argument, 0,        IGNORE   },    // SLURM
        { "C",                 required_argument, 0,        MPICWD   },    // Sun
        { "D",                 required_argument, 0,        MPICWD   },    // SLURM
        { "chdir",             required_argument, 0,        MPICWD   },    // SLURM
        { "wdir",              required_argument, 0,        MPICWD   },
        { "cwd",               required_argument, 0,        MPICWD   },
        { "env",               required_argument, 0,        MPIENV   },
        { "envs",              required_argument, 0,        MPIENV   },    // runjob
        { "configfile",        required_argument, 0,        MPICFG   },
        { "A",                 required_argument, 0,        TARGET   },    // Sun: replaces target
        { "exe",               required_argument, 0,        TARGET   },    // BG: includes full path
        { "host",              required_argument, 0,        IGNORE   },    // IMPI convenience
        { "machinefile",       required_argument, 0,        IGNORE   },    // MPICH convenience
        { "q",                 required_argument, 0,        IGNORE   },    // cobalt/qsub queue convenience
        { "path",              required_argument, 0,        T_PATH   },    // mpiexec
        { "args",              required_argument, 0,        T_ARGS   },    // BG
        { "mode",              required_argument, 0,        CNMODE   },    // BG/Cobalt
        { "ranks-per-node",    required_argument, 0,        CNMODE   },    // runjob
        { "npernode",          required_argument, 0,        CNMODE   },    // PBS mpiexec
        { "pernode",           no_argument,       0,        CNMODE   },    // PBS mpiexec
        { "nnp",               required_argument, 0,        CNMODE   },    // NECSX
        { "N",                 required_argument, 0,        CNMODE   },    // aprun
        { "d",                 no_argument,       0,        SWITCH_D },    // aprun/openmpi/mpt/etc
        { "SN",                no_argument,       &modeopt, MODE_SN  },    // yod
        { "VN",                no_argument,       &modeopt, MODE_VN  },    // yod
        { "nodelist",          required_argument, 0,        IGNORE   },    // SLURM
        { "exclude",           required_argument, 0,        IGNORE   },    // SLURM
        { "c",                 required_argument, 0,        CPUCMD   },    // ibrun.symm
        { "m",                 required_argument, 0,        MICCMD   },    // ibrun.symm
        { "2",                 required_argument, 0,        MICCMD   },    // ibrun.symm
      #if HAVE_PLATFORM_BGL || HAVE_PLATFORM_BGP || HAVE_PLATFORM_BGQ
        { "block",             required_argument, 0,        IGNORE   },    // BG runjob convenience
        { "connect",           required_argument, 0,        IGNORE   },    // BG mpirun convenience
        { "corner",            required_argument, 0,        IGNORE   },    // BG runjob convenience
        { "exp-env",           required_argument, 0,        IGNORE   },    // BG mpirun convenience
        { "mapfile",           required_argument, 0,        IGNORE   },    // BG mpirun convenience
        { "mapping",           required_argument, 0,        IGNORE   },    // BG runjob convenience
        { "partition",         required_argument, 0,        IGNORE   },    // BG mpirun convenience
        { "shape",             required_argument, 0,        IGNORE   },    // BG mpirun convenience
        // most mpirun commands implement -verbose with no argument, but not BlueGene
        { "verbose",           required_argument, 0,        IGNORE   },    // BG mpirun convenience
      #endif
      #ifdef _AIX
        { "cmdfile",           required_argument, 0,        MPICFG   },    // poe convenience
        { "pgmmodel",          required_argument, 0,        IGNORE   },    // poe convenience
      #endif

        { 0,                   0,                 0,        0        }
    };
    // *INDENT-ON*


    int nnodes = 0;    // only for NECSX & Cobalt
    int ch, opti = 0;
    opterr = 0;    // disable automatic report of unrecognized options
    if (verbose > 1)
    {
        cerr << "argv[" << optind << "]=" << argv[optind] << endl;
    }
    while ((ch = getopt_long_only(argc, argv, "-:", unified_options, &opti)) != -1)
    {
        if (  (optind >= argc)
           && (launch_options.target == NULL)
           && (ch != TARGET)
           && measure
           && (launcher != "ibrun.symm"))
        {
            if (verbose > 1)
            {
                cerr << "Got invalid optind=" << optind << endl;
            }
            cerr << SCAN << "Launch command line parsing failed (due to bad version of libiberty?)" << endl
                 << "(Hint: try inserting \"-- \" immediately before target executable as workaround.)" << endl;
            exit(2);
        }
        switch (ch)
        {
            case 0:
                if (verbose > 1)
                {
                    cerr << unified_options[opti].name << ": modeopt=" << modeopt << endl;
                }
                switch (modeopt)
                {
                    case MODE_SN:
                        launch_options.cnmode = modes[MODE_SN];
                        break;

                    case MODE_VN:
                        launch_options.cnmode = modes[MODE_VN];
                        break;

                    default:
                        break;
                }
                break;

            case 1:
                cerr << "Got non-option " << argv[optind - 1] << endl;
                break;

            case SWITCH_D:
                // needs to be handled differently for aprun/mpt/etc
                if (launcher == "mpirun")
                {
                    // MPT -d directory
                    launch_options.mpicwd = argv[optind];
                    if (verbose)
                    {
                        cerr << "mpicwd=" << launch_options.mpicwd << endl;
                    }
                    launch_options.other += spc + argv[optind - 1] + spc + argv[optind];
                    optind++;
                }
                else if (  (launcher == "aprun")
                        || (launcher == "mprun"))
                {
                    if (verbose)
                    {
                        cerr << "Ignoring -d switch with arg \"" << argv[optind] << "\"" << endl;
                    }
                    launch_options.other += spc + argv[optind - 1] + spc + argv[optind];
                    optind++;
                }
                else
                {
                    // e.g., "orterun -d"
                    if (verbose)
                    {
                        cerr << "Ignoring -d switch without arg" << endl;
                    }
                    launch_options.other += spc + argv[optind - 1];
                }
                break;

            case CNMODE:
                if (verbose > 1)
                {
                    cerr << "cnmode=" << (optarg ? optarg : "pernode") << endl;
                }
                if (!optarg)
                {
                    // single process per node
                    launch_options.cnmode = UTILS_CStr_dup("1p");
                    launch_options.other += spc + argv[optind - 1];
                    break;
                }

                // cobalt/qsub script mode?
                if (string(optarg) == "script")
                {
                    cerr << SCAN << "Abort: qsub script mode not supported!" << endl;
                    cerr << "Hint: prefix mpirun/runjob commands within script instead." << endl;
                    exit(1);
                }

                // scan will replace specified mode with "script scan" so drop original here
                if (launcher != "qsub")
                {
                    if (strchr(argv[optind - 1], '=') != NULL)
                    {
                        // "--value=val" option format
                        launch_options.other += spc + argv[optind - 1];
                    }
                    else
                    {
                        launch_options.other += spc + argv[optind - 2] + spc + optarg;
                    }
                }
                if (string(optarg) == "VN")
                {
                    launch_options.cnmode = modes[MODE_VN];
                }
                else if (string(optarg) == "CO")
                {
                    launch_options.cnmode = modes[MODE_CO];
                }
                else if (string(optarg) == "SMP")
                {
                    launch_options.cnmode = modes[MODE_SMP];
                }
                else if (string(optarg) == "DUAL")
                {
                    launch_options.cnmode = modes[MODE_DUAL];
                }
                else if (string(optarg) == "QUAD")
                {
                    launch_options.cnmode = modes[MODE_QUAD];
                }
                else if (atoi(optarg) > 0)
                {
                    // got a numeric value, so add "per"
                    char* mode = (char*)malloc(2 + strlen(optarg));
                    sprintf(mode, "%sp", optarg);
                    launch_options.cnmode = mode;
                }
                else if (  (*optarg == 'c')
                        && (atoi(optarg + 1) > 0))
                {
                    // convert cobalt numeric mode to "per"
                    char* mode = (char*)malloc(2 + strlen(optarg));
                    sprintf(mode, "%sp", optarg + 1);
                    launch_options.cnmode = mode;
                }
                else
                {
                    launch_options.cnmode = optarg;
                }
                break;

            case NNODES:
                nnodes = atoi(optarg);
                if (strchr(argv[optind - 1], '=') != NULL)
                {
                    // "--value=val" option format
                    launch_options.other += spc + argv[optind - 1];
                }
                else
                {
                    launch_options.other += spc + argv[optind - 2] + spc + optarg;
                }
                if (verbose > 1)
                {
                    cerr << "nnodes=" << optarg << endl;
                }
                break;

            case NPROCS:
                if (  (launcher == "qsub")
                   && (string(argv[optind - 2]) == "-n"))
                {
                    // "qsub -n" specifies number of nodes, not processes!
                    nnodes                = atoi(optarg);
                    launch_options.other += spc + argv[optind - 2] + spc + optarg;
                    if (verbose > 1)
                    {
                        cerr << "nnodes=" << optarg << endl;
                    }
                    break;
                }
                launch_options.nprocs = optarg;
                if (launcher != "qsub")
                {
                    if (strchr(argv[optind - 1], '=') != NULL)
                    {
                        // "--value=val" option format
                        launch_options.other += spc + argv[optind - 1];
                    }
                    else
                    {
                        launch_options.other += spc + argv[optind - 2] + spc + optarg;
                    }
                }
                if (verbose > 1)
                {
                    cerr << "nprocs=" << optarg << endl;
                }
                break;

            case NTHRDS:
                launch_options.other += spc + argv[optind - 2] + spc + optarg;
                if (launcher != "llrun")
                {
                    // generally "-t" specifies time-limit
                    break;
                }
                launch_options.nthrds = optarg;
                if (verbose > 1)
                {
                    cerr << "nthrds=" << optarg << endl;
                }
                break;

            case MPICFG:
                launch_options.other += spc + argv[optind - 2] + spc + optarg;
                cerr << SCAN << "Warning: Ignoring MPI configuration in file \""
                     << optarg << "\"" << endl;
                break;

            case MPICWD:
                launch_options.other += spc + argv[optind - 2] + spc + optarg;
                if (launcher == "llrun")
                {
                    // "llrun -C" specifies queue, not workdir!
                    break;
                }
                launch_options.mpicwd = optarg;
                if (verbose > 1)
                {
                    cerr << "mpicwd=" << optarg << endl;
                }
                break;

            case MPIENV:
                // MPI environment variable exports included in "other" launch options
                if (verbose > 1)
                {
                    cerr << "mpienv=\"" << optarg << "\"" << endl;
                }
                if (launcher != "qsub")
                {
                    // qsub --env moved to mpirun --env or runjob --envs
                    launch_options.other += spc + argv[optind - 2] + " \"" + optarg + "\"";
                }
                {
                    // check whether OMP_NUM_THREADS specified
                    bool   bgq = epk_archive_exists("/bgsys/drivers/ppcfloor/hlcs");
                    string envs(optarg);
                    size_t sep, pos = 0;
                    do
                    {
                        sep = envs.find(':', pos);
                        string env(envs.substr(pos, sep - pos));

                        // XXXX check for EPIK/SCOREP configuration variables
                        if (env.substr(0, 16) == "OMP_NUM_THREADS=")
                        {
                            launch_options.nthrds = UTILS_CStr_dup(env.substr(16).c_str());
                            if (verbose > 1)
                            {
                                cerr << "(env)nthrds=" << launch_options.nthrds << endl;
                            }
                        }

                        // cobalt qsub requires "--env VAR1=val1:VAR2=val2" specification
                        // reformat for mpirun as "-env VAR1=val1 -env VAR2=val2"
                        // reformat for runjob as "-envs VAR1=val1 -envs VAR2=val2"
                        if (launcher == "qsub")
                        {
                            launch_options.mpienv += (bgq ? " --envs " : " -env ") + env;
                        }
                        pos = sep + 1;
                    }
                    while (sep != string::npos);
                }
                break;

            case MICCMD:
                if (launcher != "ibrun.symm")
                {
                    // something else to be ignored
                    launch_options.other += spc + argv[optind - 2] + spc + optarg;
                }
                else
                {
                    string cmd(optarg);
                    size_t sep = cmd.find(spc);
                    if (sep != string::npos)
                    {
                        launch_options.target2 = UTILS_CStr_dup(cmd.substr(0, sep).c_str());
                        launch_options.t_args2 = UTILS_CStr_dup(cmd.substr(sep + 1).c_str());
                    }
                    else
                    {
                        launch_options.target2 = optarg;
                    }
                    dual_mic = (strcmp(argv[optind - 2], "-2") == 0);    // dual-MIC config

                    // don't include target and args in launch_options.other
                    if (verbose > 1)
                    {
                        // *INDENT-OFF*    Uncrustify issue #2788
                        cerr << "dual_mic=" << dual_mic
                             << " target2=<" << launch_options.target2
                             << "> t_args2=" << ((sep != string::npos)
                                                 ? launch_options.t_args2
                                                 : "(nil)")
                             << endl;
                        // *INDENT-ON*
                    }
                }
                break;

            case CPUCMD:
                if (launcher != "ibrun.symm")
                {
                    // something else to be ignored
                    launch_options.other += spc + argv[optind - 2] + spc + optarg;
                }
                else
                {
                    string cmd(optarg);
                    size_t sep = cmd.find(spc);
                    if (sep != string::npos)
                    {
                        launch_options.target = UTILS_CStr_dup(cmd.substr(0, sep).c_str());
                        launch_options.t_args = UTILS_CStr_dup(cmd.substr(sep + 1).c_str());
                    }
                    else
                    {
                        launch_options.target = optarg;
                    }

                    // don't include target and args in launch_options.other
                    if (verbose > 1)
                    {
                        // *INDENT-OFF*    Uncrustify issue #2788
                        cerr << "target=<" << launch_options.target
                             << "> t_args=" << ((sep != string::npos)
                                                ? launch_options.t_args
                                                : "(nil)")
                             << endl;
                        // *INDENT-ON*
                    }
                }
                break;

            case TARGET:
                if (launcher == "qsub")
                {
                    // "qsub -A" specifies project not target!
                    launch_options.other += spc + argv[optind - 2] + spc + optarg;
                    break;
                }
                launch_options.target = optarg;

                // don't include target in launch_options.other
                if (verbose > 1)
                {
                    cerr << "target=" << optarg << endl;
                }
                break;

            case T_PATH:
                launch_options.t_path = optarg;
                launch_options.other += spc + argv[optind - 2] + spc + optarg;
                if (verbose > 1)
                {
                    cerr << "t_path=" << optarg << endl;
                }
                break;

            case T_ARGS:
                launch_options.t_args = optarg;

                // don't include t_args in launch_options.other
                if (verbose > 1)
                {
                    cerr << "t_args=\"" << optarg << "\"" << endl;
                }
                break;

            case IGNORE:    // arg with parameter to explicitly ignore
                if (strchr(argv[optind - 1], '=') != NULL)
                {
                    // "--value=val" option format
                    launch_options.other += spc + argv[optind - 1];
                    if (verbose > 1)
                    {
                        cerr << "Ignoring launcher arg \"" << argv[optind - 1] << "\"" << endl;
                    }
                }
                else
                {
                    launch_options.other += spc + argv[optind - 2] + spc + optarg;
                    if (verbose > 1)
                    {
                        cerr << "Ignoring launcher arg \"" << argv[optind - 2]
                             << " " << optarg << "\"" << endl;
                    }
                }
                break;

            case ':':
                cerr << "Missing option parameter" << endl;
                break;

            case '?':    // unrecognized or ambiguous option
                if (verbose > 0)
                {
                    cerr << "Ignoring unrecognized launcher arg \"" << argv[optind - 1] << "\"" << endl;
                }
                launch_options.other += spc + argv[optind - 1];
                break;

            default:
                cerr << "Unhandled launcher arg " << optopt << endl;
        }
        if ((verbose > 1) && argv[optind])
        {
            cerr << "argv[" << optind << "]=" << argv[optind] << endl;
        }
    }

    // NECSX and Cobalt may need nprocs calculated from nnodes and cnmode
    if (  !launch_options.nprocs
       && launch_options.cnmode
       && (nnodes > 0))
    {
        int nprocs = nnodes * atoi(launch_options.cnmode);
        launch_options.nprocs = (char*)calloc(16, 1);
        sprintf(launch_options.nprocs, "%d", nprocs);
        if (verbose)
        {
            cerr << "Set nprocs=" << string(launch_options.nprocs)
                 << " from nnodes=" << nnodes << " and ppn="
                 << string(launch_options.cnmode) << endl;
        }
    }
    if (!launch_options.other.empty() && verbose)
    {
        cerr << "Other launch options:" << launch_options.other << endl;
    }

    return 0;
}


// locate executable on provided path
string
which(string exe,
      string path)
{
    if (exe[0] == '/')
    {
        return exe;    // given with absolute path
    }
    size_t token_start = 0;
    while (token_start < path.length())
    {
        size_t token_length = strcspn(&path.c_str()[token_start], ":");
        string token        = path.substr(token_start, token_length);
        token_start += token_length + 1;
        token       += string("/") + exe;
        if (access(token.c_str(), X_OK) == 0)
        {
            return token;
        }
    }
    if (verbose > 1)
    {
        cerr << "No " << exe << " in (" << path << ")" << endl;
    }

    return "";
}


// check for symbol in executable
bool
symbol_in_executable(const char* executable,
                     const char* symbol)
{
    string nm_command;
    string nm_path = which("nm", getenv("PATH"));
    if (nm_path.empty())
    {
        cerr << SCAN << "Error: \"nm\" not located on PATH!" << endl;
        exit(2);
    }
    #ifdef _AIX
        nm_command += "OBJECT_MODE=32_64 ";
    #endif
    nm_command += nm_path + spc + string(executable) + " 2>&1"
                  + " | grep -c " + string(symbol) + " > /dev/null";
    if (verbose > 1)
    {
        cerr << nm_command << endl;
    }
    int retval = system(nm_command.c_str());
    if (retval == 2)
    {
        cerr << SCAN << nm_command << endl
             << "Warning: " << strerror(errno) << endl;
    }

    return (retval == 0);
}


// check for shared-library linked to executable
bool
shared_library_linked(const char* executable,
                      const char* library)
{
    string ldd_command;
    ldd_command += "ldd " + string(executable) + " 2>&1"
                   + " | grep -c " + string(library) + " > /dev/null";
    if (verbose > 1)
    {
        cerr << ldd_command << endl;
    }
    int retval = system(ldd_command.c_str());
    if (retval == 2)
    {
        cerr << SCAN << ldd_command << endl
             << "Warning: " << strerror(errno) << endl;
    }

    return (retval == 0);
}


bool
includes_instrumentation(const string& target)
{
    return (  symbol_in_executable(target.c_str(), "SCOREP_InitMeasurement")
           || shared_library_linked(target.c_str(), "libscorep_"));
}


bool
includes_mpi_instrumentation(const string& target)
{
    return (  symbol_in_executable(target.c_str(), "scorep_mpi_status_size")
           || shared_library_linked(target.c_str(), "libscorep_mpi_omp.so")
           || shared_library_linked(target.c_str(), "libscorep_mpi.so")
           || shared_library_linked(target.c_str(), "libscorep_adapter_mpi_mgmt.so"));
}


bool
includes_omp_instrumentation(const string& target)
{
    return (  symbol_in_executable(target.c_str(), "POMP2_Parallel_fork")
           || shared_library_linked(target.c_str(), "libscorep_mpi_omp.so")
           || shared_library_linked(target.c_str(), "libscorep_omp.so")
           || shared_library_linked(target.c_str(), "libscorep_adapter_pomp_omp_mgmt.so")
           || shared_library_linked(target.c_str(), "libscorep_adapter_opari2_openmp_mgmt.so"));
}


bool
includes_pthread_instrumentation(const string& target)
{
    return (  symbol_in_executable(target.c_str(), "SCOREP_Subsystem_PthreadAdapter")
           || shared_library_linked(target.c_str(), "libscorep_adapter_pthread_mgmt.so"));
}


string
numberWithLeadingZeroes(int value,
                        int max)
{
    ostringstream maxStr;
    maxStr << max;
    ostringstream valueStr;
    valueStr << setw(maxStr.str().length()) << setfill('0')
             << value;

    return valueStr.str();
}


bool
getenvBool(const char* name,
           bool        defaultValue)
{
    char* env = getenv(name);
    if (env == NULL)
    {
        return defaultValue;
    }

    return epk_str2bool(env);
}


void
testLockFile(const string& fileName)
{
    int waitStep = 60;
    int waitSum  = 0;

    if (!fileName.empty())
    {
        while (access(fileName.c_str(), F_OK) == 0)
        {
            cerr << "Measurement start blocked by '" << fileName
                 << "' ... (retry every " << waitStep << " sec)" << endl;
            sleep(waitStep);
            waitSum += waitStep;
        }
        cerr << "Measurement start after " << waitSum
             << " sec wait time." << endl;
    }
}


void
checkTraceFileSystem(const string& archive)
{
    char* traceFileSystemEnv = getenv("SCAN_TRACE_FILESYS");
    if (!traceFileSystemEnv)
    {
        return;
    }

    stringstream traceFileSystems(traceFileSystemEnv);
    string       traceFileSystem;
    bool         allowedFileSystem = false;

    // verify file system/device for trace collection
    if (verbose)
    {
        cerr << "SCAN_TRACE_FILESYS=" << traceFileSystemEnv << endl;
    }

    // checking all colon separated file systems in SCAN_TRACE_FILESYS
    while (getline(traceFileSystems, traceFileSystem, ':'))
    {
        struct stat statBuffer;
        int         returnValue        = stat(traceFileSystem.c_str(), &statBuffer);
        dev_t       traceFileSystemDev = statBuffer.st_dev;
        if (returnValue != 0)
        {
            cerr << SCAN << "Error: Testing '" << traceFileSystem
                 << "' from SCAN_TRACE_FILESYS: " << strerror(errno) << endl;
            continue;
        }

        returnValue = stat(archive.c_str(), &statBuffer);
        dev_t archiveFileSystemDev = statBuffer.st_dev;
        if (returnValue != 0)
        {
            cerr << "Error: Testing target directory '"
                 << archive << "': " << strerror(errno) << endl;
            continue;
        }

        if (verbose > 1)
        {
            cerr << "Got experiment file system device '" << archiveFileSystemDev
                 << "' for '" <<  archive
                 << "' and trace file system device '" << traceFileSystemDev
                 << "' for '" << traceFileSystem << "'" << endl;
        }

        if (archiveFileSystemDev == traceFileSystemDev)
        {
            if (verbose)
            {
                cerr << "File system '" << traceFileSystemDev
                     << "' for '" << traceFileSystem
                     << "' from SCAN_TRACE_FILESYS is a suitable match for the file system `"
                     << archiveFileSystemDev << "' of the target directory '"
                     << archive << "'." << endl;
            }
            allowedFileSystem = true;
            break;
        }
    }
    if (!allowedFileSystem)
    {
        cerr << SCAN << "Error: The archive directory \"" << archive
             << "\" is not located on a suitable file system\n"
             << "based on the list of allowed file systems defined in:\n"
             << "SCAN_TRACE_FILESYS=" << traceFileSystemEnv << "  " << endl;
        exit(7);
    }
}


int
main(int   argc,
     char* argv[])
{
    string         mic_config;
    int            ch, retval, status;
    int            last_scan_arg   = 0;
    int            runs_per_config = 1;    // default one single run
    time_t         time1, time2;
    double         dtime;
    string         date;
    string         envflag, envassign = spc;    // specify environment variable on launch command
    bool           epk_inst_mpi, epk_inst_omp;
    bool           epk_inst_pthreads = false;
    bool           preview           = false;
    MultiRunConfig multirun_cfg;
    bool           redirect         = false;   // redirect launcher stdout/stderr (for "mpiexec"?)
    bool           olaunch          = false;   // if false, require launch command
    bool           measure          = true;    // if false, analyze only
    bool           summary          = getenvBool("SCOREP_ENABLE_PROFILING", true);
    bool           tracing          = getenvBool("SCOREP_ENABLE_TRACING", false);
    bool           used_enable_opts = false;                           // Note if -t, -s, -q are set as parameters
    char*          env_filter       = getenv("SCOREP_FILTERING_FILE"); // filterfile
    string         filter           = env_filter ? env_filter : "";

    // XXXX resolve whether to use one or both metric specifications
    string       metrics = "";    // SCOREP_METRIC_RUSAGE + SCOREP_METRIC_PAPI
    string       archive;
    string       blocker;   // potential measurement lockfile
    string       escape;    // potential launcher escape
    const string USAGE =
        "Scalasca" + spc + VERSION
        + ": measurement collection & analysis nexus\n"
          "usage: scan {options} [launchcmd [launchargs]] target [targetargs]\n"
          "      where {options} may include:\n"
          "  -h    Help      : show this brief usage message and exit.\n"
          "  -v    Verbose   : increase verbosity.\n"
          "  -n    Preview   : show command(s) to be launched but don't execute.\n"
          "  -q    Quiescent : execution with neither summarization nor tracing.\n"
          "  -s    Summary   : enable runtime summarization. [Default]\n"
          "  -t    Tracing   : enable trace collection and analysis.\n"
          "  -a    Analyze   : skip measurement to (re-)analyze an existing trace.\n"
          "  -e exptdir      : Experiment archive to generate and/or analyze.\n"
          "                    (overrides default experiment archive title)\n"
          "  -f filtfile     : File specifying measurement filter.\n"
          "  -l lockfile     : File that blocks start of measurement.\n"
          "  -R #runs        : Specify the number of measurement runs per config.\n"
          "  -M cfgfile      : Specify a config file for a multi-run measurement.\n"
          "  -P preset       : Specify a preset for a multi-run measurement, e.g., 'pop'.\n"
          "  -L              : List available multi-run presets.\n"
          "  -D cfgfile      : Check a multi-run config file for validity and dump\n"
          "                  : the processed configuration for comparison.\n";

    if (argc == 1)
    {
        cerr << USAGE << endl;
        exit(0);
    }

    while ((ch = getopt(argc, argv, "+hvnqstae:f:l:m:R:OM:P:LD:")) != EOF)
    {
        switch (ch)
        {
            case 'e':
                archive = optarg;
                if (verbose)
                {
                    cerr << "Got experiment archive \"" << archive << "\"" << endl;
                }
                break;

            case 'f':
                filter = optarg;
                if (verbose)
                {
                    cerr << "Got measurement filter \"" << filter << "\"" << endl;
                }
                break;

            case 'l':
                blocker = optarg;
                if (verbose)
                {
                    cerr << "Got measurement lockfile \"" << blocker << "\"" << endl;
                }
                break;

            case 'm':
                metrics = optarg;
                if (verbose)
                {
                    cerr << "Got measurement metrics \"" << metrics << "\"" << endl;
                }
                cerr << "Specify measurement metrics as SCOREP_METRIC_PAPI and SCOREP_METRIC_RUSAGE" << endl;
                exit(1);
                break;

            case 'M':
                multirun_cfg.loadConfig(optarg);
                if (verbose)
                {
                    cerr << "Got multi-run configuration file \"" << optarg << "\"" << endl;
                }
                break;

            case 'P':
                multirun_cfg.usePreset(optarg);
                if (verbose)
                {
                    cerr << "Using multi-run preset \"" << optarg << "\"" << endl;
                }
                break;

            case 'L':
                multirun_cfg.listAvailablePresets();
                exit(0);
                break;

            case 'D':
                cerr << "Parsing multi-run configuration file \"" << optarg << "\"" << endl;
                multirun_cfg.loadConfig(optarg);
                multirun_cfg.saveConfigAs(string(optarg) + ".dump");
                cerr << "Configuration file successfully parsed and dumped to \"" << optarg << ".dump\".\n"
                     << "Check if the output reflects the expected configuration!" << endl;
                exit(0);
                break;

            case 'n':
                preview = true;
                break;

            case 'O':
                olaunch = true;
                break;

            case 'q':    // execution without summarization or tracing
                measure          = true;
                summary          = false;
                tracing          = false;
                used_enable_opts = true;
                break;

            case 'R':
                runs_per_config = epk_str2int(optarg);
                if (runs_per_config < 1)
                {
                    cerr << "Invalid number of measurement runs per config: " << optarg << endl;
                    exit(1);
                }
                if (verbose)
                {
                    cerr << "Number of measurement runs per config to be done: " << runs_per_config << endl;
                }
                break;

            case 's':    // summarization measurement
                measure          = true;
                summary          = true;
                used_enable_opts = true;
                break;

            case 't':    // trace collection & analysis
                measure          = true;
                tracing          = true;
                used_enable_opts = true;
                break;

            case 'a':    // analysis only
                measure = false;
                tracing = true;
                break;

            case 'v':
                if (verbose == 1)
                {
                    // report submitted command-line
                    cerr << argv[0];
                    for (int i = 1; i < argc; i++)
                    {
                        cerr << spc << argv[i];
                    }
                    cerr << endl;
                }
                verbose++;
                break;

            case 'h':
                cerr << USAGE << endl;
                exit(0);
                break;

            case '?':    // unrecognized option
            case ':':    // missing option argument
            default:
                cerr << USAGE << endl;
                exit(1);
        }
    }

    if (used_enable_opts && (multirun_cfg.getNumSets() > 1))
    {
        cerr << SCAN
             << "Warning: Explicit measurement control options ('-s', '-t', or '-q')\n"
             << "are ignored for multi-run measurements.  Corresponding settings are\n"
             << "only taken from the multi-run configuration file!" << endl;
    }

    multirun_cfg.setInfo(runs_per_config, verbose);

    // check multi-run requirements
    if (!multirun_cfg.checkEnvBlacklist())
    {
        // failed the check of the blacklist against the environment
        exit(1);
    }

    // verify filter
    if (  !filter.empty()
       && (access(filter.c_str(), R_OK) != 0))
    {
        cerr << SCAN << "Abort: Bad filter " << filter
             << ": " << strerror(errno) << endl;
        exit(2);
    }

    if (getenv("SCAN_MPI_REDIRECT"))
    {
        redirect = epk_str2bool(getenv("SCAN_MPI_REDIRECT"));
        if (verbose)
        {
            cerr << "Got SCAN_MPI_REDIRECT \"" << redirect << "\"" << endl;
        }
    }
    #ifdef __FUJITSU
        else
        {
            char* jobdir = getenv("PJM_JOBDIR");
            redirect = (strncmp("/work/", jobdir, 6) == 0);
            if (verbose)
            {
                cerr << "Set SCAN_MPI_REDIRECT=" << redirect << endl;
            }
        }
    #endif

    if (getenv("SCAN_ESCAPE"))
    {
        escape = getenv("SCAN_ESCAPE");
        if (verbose)
        {
            cerr << "Got SCAN_ESCAPE \"" << escape << "\"" << endl;
        }
    }

    if (getenv("SCAN_SETENV"))
    {
        envflag = getenv("SCAN_SETENV");
        if (verbose)
        {
            cerr << "Got SCAN_SETENV \"" << envflag << "\"" << endl;
        }
        if (envflag[envflag.length() - 1] == '=')
        {
            envflag.erase(envflag.length() - 1);
            envassign = '=';
        }
    }

    last_scan_arg = optind;

    if (optind == argc)
    {
        cerr << USAGE << "Error: Insufficient arguments." << endl;
        exit(1);
    }

    if (strcmp(argv[optind], escape.c_str()) == 0)
    {
        if (verbose)
        {
            cerr << "Scanned escaped arg \"" << escape << "\"" << endl;
        }
        optind++;
    }

    // next argument is potentially target or launcher
    if (access(argv[optind], X_OK) == 0)
    {
        bool epk_inst = includes_instrumentation(argv[optind]);
        if (verbose)
        {
            cerr << (epk_inst ? "" : "No ")
                 << "SCOREP instrumentation found in " << argv[optind] << endl;
        }
        olaunch |= epk_inst;    // omit launch check for instrumented target
    }
    else if (optind == (argc - 1))
    {
        // no access to target
        cerr << argv[optind] << ": " << strerror(errno) << endl;
        exit(1);
    }

    if (!olaunch && (optind == (argc - 1)))
    {
        // no launcher and no instrumentation
        cerr << "No SCOREP instrumentation found in " << argv[optind] << endl;
        exit(1);
    }

    string launcher, launcher_path;
    if (!olaunch)
    {
        // start parsing launch options
        unsigned launch_endind, launch_begind = optind++;

        launcher_path = which(argv[launch_begind], getenv("PATH"));
        if (launcher_path.empty())
        {
            cerr << SCAN << "Error: " << argv[launch_begind] << " launcher not located!" << endl;
            exit(2);
        }

        // get basename of launcher
        size_t sep = launcher_path.rfind('/');
        if (sep == string::npos)
        {
            sep = 0;
        }
        else
        {
            sep++;
        }
        launcher = launcher_path.substr(sep);

        if (  (launcher == "mpiexec")
           || (launcher == "mpiexec.py")
           || (launcher == "mpiexec.hydra")
           || (launcher == "mpiexec_mpt")
           || (launcher == "yod")           // Catamount
           || (launcher == "mpirun.lsf")    // LSF
           || (launcher == "cobalt-mpirun") // ALCF
           || (launcher == "orterun")       // OpenMPI
           || (launcher == "aprun")         // CrayXT
           || (launcher == "ibrun")         // TACC
           || (launcher == "ibrun.symm")    // TACC
           || (launcher == "llrun")         // JSC
           || (launcher == "poe")           // AIX POE
           || (launcher == "srun")          // SLURM
           || (launcher == "yhrun")         // SLURM-derived YHRMS@TH-1A
           || (launcher == "ccc_mprun")     // Curie
           || (launcher == "mprun")         // Sun HPC Clustertools
           || (launcher == "mpirun"))
        {
            if (verbose)
            {
                cerr << "Got launcher \"" << launcher << "\"" << endl;
            }
        }
        else if (  getenv("SCAN_MPI_LAUNCHER")
                && (launcher == getenv("SCAN_MPI_LAUNCHER")))
        {
            if (verbose)
            {
                cerr << "Got custom SCAN_MPI_LAUNCHER \"" << launcher << "\"" << endl;
            }
        }
        else if (launcher == "runjob")
        {
            if (verbose)
            {
                cerr << SCAN << "Got BG/Q launcher runjob!" << endl;
            }
            escape = ":";

            // set RUNJOB configuration from COBALT
            if (getenv("RUNJOB_BLOCK"))
            {
                cerr << "RUNJOB_BLOCK=" << getenv("RUNJOB_BLOCK") << endl;
            }
            else if (getenv("COBALT_PARTNAME"))
            {
                char* block = getenv("COBALT_PARTNAME");
                cerr << "COBALT_PARTNAME=" << block << " -> RUNJOB_BLOCK" << endl;
                setenv("RUNJOB_BLOCK", block, 1);
            }
            if (getenv("RUNJOB_CORNER"))
            {
                cerr << "RUNJOB_CORNER=" << getenv("RUNJOB_CORNER") << endl;
            }
            else if (getenv("COBALT_CORNER"))
            {
                char* corner = getenv("COBALT_CORNER");
                cerr << "COBALT_CORNER=" << corner << " -> RUNJOB_CORNER" << endl;
                setenv("RUNJOB_CORNER", corner, 1);
            }
            if (getenv("RUNJOB_SHAPE"))
            {
                cerr << "RUNJOB_SHAPE=" << getenv("RUNJOB_SHAPE") << endl;
            }
            else if (getenv("COBALT_SHAPE"))
            {
                char* shape = getenv("COBALT_SHAPE");
                cerr << "COBALT_SHAPE=" << shape << " -> RUNJOB_SHAPE" << endl;
                setenv("RUNJOB_SHAPE", shape, 1);
            }

            // check environment for RUNJOB configuration
            if (getenv("RUNJOB_EXE"))
            {
                launch_options.target = getenv("RUNJOB_EXE");
                if (verbose)
                {
                    cerr << "RUNJOB_EXE target=" << launch_options.target << endl;
                }
                unsetenv("RUNJOB_EXE");
            }
            if (getenv("RUNJOB_ARGS"))
            {
                launch_options.t_args = getenv("RUNJOB_ARGS");
                if (verbose)
                {
                    cerr << "RUNJOB_ARGS t_args=" << launch_options.t_args << endl;
                }
                unsetenv("RUNJOB_ARGS");
            }
            if (getenv("RUNJOB_NP"))
            {
                launch_options.nprocs = getenv("RUNJOB_NP");
                if (verbose)
                {
                    cerr << "RUNJOB_NP nprocs=" << launch_options.nprocs << endl;
                }
                unsetenv("RUNJOB_NP");
            }
            if (getenv("RUNJOB_RANKS_PER_NODE"))
            {
                char* ppn  = getenv("RUNJOB_RANKS_PER_NODE");
                char* mode = (char*)malloc(8);
                sprintf(mode, "%sp", ppn);
                launch_options.cnmode = mode;
                if (verbose)
                {
                    cerr << "RUNJOB_RANKS_PER_NODE cnmode=" << launch_options.cnmode << endl;
                }
                unsetenv("RUNJOB_RANKS_PER_NODE");
            }

            // XXXX RUNJOB_ENVS might contain settings
            if (getenv("RUNJOB_ENVS"))
            {
                cerr << "Warning: ignored RUNJOB_ENVS=\"" << getenv("RUNJOB_ENVS") << "\"" << endl;
            }
        }
        else if (launcher == "qsub")
        {
            if (verbose)
            {
                cerr << SCAN << "Got Cobalt launcher qsub!" << endl;
            }
            cobalt = true;
            if (getenv("COBALT_PARTNAME"))
            {
                char* block = getenv("COBALT_PARTNAME");
                if (verbose)
                {
                    cerr << "qsub COBALT_PARTNAME=" << block << endl;
                }
                launch_options.other += " --block " + string(block);
            }
            if (getenv("COBALT_CORNER"))
            {
                char* corner = getenv("COBALT_CORNER");
                if (verbose)
                {
                    cerr << "qsub COBALT_CORNER=" << corner << endl;
                }
                launch_options.other += " --corner " + string(corner);
            }
            if (getenv("COBALT_SHAPE"))
            {
                char* shape = getenv("COBALT_SHAPE");
                if (verbose)
                {
                    cerr << "qsub COBALT_SHAPE=" << shape << endl;
                }
                launch_options.other += " --shape " + string(shape);
            }
        }
        else
        {
            cerr << SCAN << "Error: unrecognized launcher \"" << launcher_path << "\"" << endl;
            cerr << "(Hint: if this is actually a launcher for MPI applications," << endl
                 << " try setting SCAN_MPI_LAUNCHER=" << launcher << ")" << endl;
            exit(2);
        }

        parse_launch(launcher, measure, argc, argv);

        launch_endind = optind - 1;
        if (verbose > 1)
        {
            cerr << "launch endind=" << launch_endind << endl;
        }
    }
    else if (verbose)
    {
        cerr << "Omitting launch command" << endl;
    }

    if (  getenv("SCAN_TARGET")
       && strlen(getenv("SCAN_TARGET")))
    {
        // use explicitly specified target
        if (launch_options.target)
        {
            cerr << "Target \"" << launch_options.target << "\" being overridden!" << endl;
        }
        launch_options.target = getenv("SCAN_TARGET");
        if (verbose)
        {
            cerr << "Got SCAN_TARGET \"" << launch_options.target << "\"" << endl;
        }

        int argi = optind;
        while (argi < argc)
        {
            if (strcmp(argv[argi], launch_options.target) == 0)
            {
                // add intervening args to launch_options
                while (optind < argi)
                {
                    launch_options.other += spc + string(argv[optind++]);
                }
                if (verbose)
                {
                    cerr << "Other launch options:" << launch_options.other << endl;
                }
                break;
            }
            argi++;
        }
        if (argi == argc)
        {
            cerr << "Failed to find SCAN_TARGET \"" << launch_options.target
                 << "\" on command-line" << endl;
            exit(2);
        }
    }

    string t_args = launch_options.t_args ? launch_options.t_args : "";

    // retrieve target and args from remainder of commandline
    if (optind < argc)
    {
        if (strcmp(argv[optind], escape.c_str()) == 0)
        {
            if (verbose)
            {
                cerr << "Scanned escaped arg \"" << escape << "\"" << endl;
            }
            optind++;
        }
        launch_options.target = argv[optind++];
        if (verbose)
        {
            cerr << "Target:" << launch_options.target << endl;
        }
        while (optind < argc)
        {
            t_args += string(argv[optind++]) + spc;
        }
        if (!t_args.empty())
        {
            t_args[t_args.length() - 1] = '\0';    // trailing space
        }
        launch_options.t_args = (char*)t_args.c_str();
        if (verbose)
        {
            cerr << "T_args:\"" << launch_options.t_args << "\"" << endl;
        }
    }

    // target2 examined to determine measurement & analysis configurations
    if (  (launcher == "ibrun.symm")
       && launch_options.target2)
    {
        string target;    // target executable, with path if specified
        if (launch_options.t_path)
        {
            target = string(launch_options.t_path) + "/";
        }
        target += launch_options.target2;

        // expand leading tilde with home directory
        if (  (target[0] == '~')
           && getenv("HOME"))
        {
            target.replace(0, 1, getenv("HOME"));
        }

        if (access(target.c_str(), X_OK) == -1)
        {
            cerr << SCAN << "Abort: Target executable " << target
                 << ": " << strerror(errno) << endl;
            if (!launcher.empty() && !launch_options.other.empty())
            {
                cerr << "(Hint: if `" << target
                     << "' is a parameter of an (ignored) " << launcher
                     << " launch argument, rather than the intended target executable,"
                     << " then try quoting \""
                     << launch_options.other.substr(launch_options.other.rfind(" -") + 1)
                     << spc << launch_options.target << "\".)" << endl;
            }
            exit(1);
        }
        if (epk_archive_exists(target.c_str()))
        {
            cerr << SCAN << "Abort: Target executable `" << target
                 << "' is a directory!" << endl;
            exit(1);
        }

        // check target for SCOREP instrumentation
        bool epk_inst = includes_instrumentation(target);
        if (!epk_inst)
        {
            cerr << SCAN << "Abort: No "
                 << "SCOREP instrumentation found in target " << target << endl;
            exit(1);
        }
        else if (verbose)
        {
            cerr << "SCOREP instrumentation found in target " << target << endl;
        }

        // examine target for MPI instrumentation
        epk_inst_mpi = includes_mpi_instrumentation(target);
        if (verbose)
        {
            cerr << (epk_inst_mpi ? "" : "No ")
                 << "SCOREP MPI instrumentation found in target " << target << endl;
        }

        if (  epk_inst_mpi
           && !olaunch
           && launcher.empty())
        {
            cerr << SCAN << "Abort: No launcher for MPI target " << target << endl;
            exit(1);
        }

        // omit launcher requirement when not MPI
        if (  !epk_inst_mpi
           && !olaunch
           && launcher.empty())
        {
            if (verbose)
            {
                cerr << "Omitting MPI launch" << endl;
            }
            olaunch = true;
        }

        // examine target for OpenMP instrumentation
        epk_inst_omp = includes_omp_instrumentation(target);
        if (verbose)
        {
            cerr << (epk_inst_omp ? "" : "No ")
                 << "SCOREP OMP instrumentation found in target " << target << endl;
        }

        // examine target for Pthreads instrumentation
        epk_inst_pthreads = includes_pthread_instrumentation(target);
        if (verbose)
        {
            cerr << (epk_inst_pthreads ? "" : "No ")
                 << "SCOREP Pthreads instrumentation found in target " << target << endl;
        }

        if (  epk_inst_omp
           && !launch_options.nthrds)
        {
            launch_options.nthrds = getenv("OMP_NUM_THREADS");
            if (launch_options.nthrds && verbose)
            {
                cerr << "Got " << launch_options.nthrds
                     << " threads from OMP_NUM_THREADS" << endl;
            }
            else if (verbose)
            {
                cerr << "OMP_NUM_THREADS not set" << endl;
            }
        }
    }

    // target examined to determine measurement & analysis configurations
    else
    {
        // check target set, exists & executable
        if (  !launch_options.target
           && !launch_options.target2)
        {
            cerr << SCAN << "Abort: Target executable not known!" << endl;
            exit(1);
        }

        string target;    // target executable, with path if specified
        if (launch_options.t_path)
        {
            target = string(launch_options.t_path) + "/";
        }
        target += launch_options.target;

        // expand leading tilde with home directory
        if (  (target[0] == '~')
           && getenv("HOME"))
        {
            target.replace(0, 1, getenv("HOME"));
        }

        if (access(target.c_str(), X_OK) == -1)
        {
            cerr << SCAN << "Abort: Target executable " << target
                 << ": " << strerror(errno) << endl;
            if (  !launcher.empty()
               && !launch_options.other.empty())
            {
                cerr << "(Hint: if `" << target
                     << "' is a parameter of an (ignored) " << launcher
                     << " launch argument, rather than the intended target executable,"
                     << " then try quoting \""
                     << launch_options.other.substr(launch_options.other.rfind(" -") + 1)
                     << spc << launch_options.target << "\".)" << endl;
            }
            exit(1);
        }
        if (epk_archive_exists(target.c_str()))
        {
            cerr << SCAN << "Abort: Target executable `" << target
                 << "' is a directory!" << endl;
            exit(1);
        }

        // check target for SCOREP instrumentation
        bool epk_inst = includes_instrumentation(target);
        if (!epk_inst)
        {
            cerr << SCAN << "Abort: No "
                 << "SCOREP instrumentation found in target " << target << endl;
            exit(1);
        }
        else if (verbose)
        {
            cerr << "SCOREP instrumentation found in target " << target << endl;
        }

        // examine target for MPI instrumentation
        epk_inst_mpi = includes_mpi_instrumentation(target);
        if (verbose)
        {
            cerr << (epk_inst_mpi ? "" : "No ")
                 << "SCOREP MPI instrumentation found in target " << target << endl;
        }

        if (  epk_inst_mpi
           && !olaunch
           && launcher.empty())
        {
            cerr << SCAN << "Abort: No launcher for MPI target " << target << endl;
            exit(1);
        }

        // omit launcher requirement when not MPI
        if (  !epk_inst_mpi
           && !olaunch
           && launcher.empty())
        {
            if (verbose)
            {
                cerr << "Omitting MPI launch" << endl;
            }
            olaunch = true;
        }

        // examine target for OpenMP instrumentation
        epk_inst_omp = includes_omp_instrumentation(target);
        if (verbose)
        {
            cerr << (epk_inst_omp ? "" : "No ")
                 << "SCOREP OMP instrumentation found in target " << target << endl;
        }

        // examine target for Pthreads instrumentation
        epk_inst_pthreads = includes_pthread_instrumentation(target);
        if (verbose)
        {
            cerr << (epk_inst_pthreads ? "" : "No ")
                 << "SCOREP Pthreads instrumentation found in target " << target << endl;
        }

        if (  epk_inst_omp
           && !launch_options.nthrds)
        {
            launch_options.nthrds = getenv("OMP_NUM_THREADS");
            if (launch_options.nthrds && verbose)
            {
                cerr << "Got " << launch_options.nthrds
                     << " threads from OMP_NUM_THREADS" << endl;
            }
            else if (verbose)
            {
                cerr << "OMP_NUM_THREADS not set" << endl;
            }
        }
    }

    // determine number of processes (if necessary)

    if (  (epk_archive_exists("/bgl/BlueLight") || epk_archive_exists("/bgsys"))
       && (cobalt || !launch_options.nprocs))
    {
        if (!launch_options.cnmode)
        {
            // set default mode according to system
            if (launcher == "runjob")
            {
                launch_options.cnmode = UTILS_CStr_dup("1p");
            }
            else
            {
                // mpirun
                launch_options.cnmode = modes[MODE_SMP];
            }
        }

        if (!launch_options.nprocs)
        {
            // determine nprocs from mode and number of nodes
            int nnodes = 0, nprocs = 0;
            if (getenv("LOADL_BG_SIZE"))
            {
                nnodes = atoi(getenv("LOADL_BG_SIZE"));
                if (verbose)
                {
                    cerr << "Got " << nnodes
                         << " nodes from LOADL_BG_SIZE" << endl;
                }
            }
            if (getenv("COBALT_PARTSIZE"))
            {
                nnodes = atoi(getenv("COBALT_PARTSIZE"));
                if (verbose)
                {
                    cerr << "Got " << nnodes
                         << " nodes from COBALT_PARTSIZE" << endl;
                }
            }
            if (atoi(launch_options.cnmode))
            {
                nprocs = nnodes * atoi(launch_options.cnmode);
            }
            else if (string(launch_options.cnmode) == string(modes[MODE_VN]))
            {
                nprocs = nnodes * 4;
            }
            else if (string(launch_options.cnmode) == string(modes[MODE_QUAD]))
            {
                nprocs = nnodes * 4;
            }
            else if (string(launch_options.cnmode) == string(modes[MODE_DUAL]))
            {
                nprocs = nnodes * 2;
            }
            else    /* MODE_SMP */
            {
                nprocs = nnodes;
            }

            launch_options.nprocs = (char*)calloc(16, 1);
            sprintf(launch_options.nprocs, "%d", nprocs);
        }
        if (verbose)
        {
            cerr << "Set BlueGene mode=" << string(launch_options.cnmode)
                 << " nprocs=" << launch_options.nprocs << endl;
        }
    }

    if (  epk_inst_mpi
       && (launcher == "ibrun.symm"))
    {
        char* tasks = getenv("SLURM_TASKS_PER_NODE");
        if (!tasks)
        {
            cerr << "SLURM_TASKS_PER_NODE not set!" << endl;
        }
        else
        {
            int   nodes  = 1, ppn;
            char* cnmode = (char*)calloc(8, 1);
            sscanf(tasks, "%d(x%d)", &ppn, &nodes);
            launch_options.nprocs = (char*)calloc(16, 1);
            sprintf(launch_options.nprocs, "%d", nodes * ppn);
            sprintf(cnmode, "%dp", ppn);
            launch_options.cnmode = cnmode;    // used to specify cpu configuration

            if (verbose)
            {
                cerr << "Got " << launch_options.nprocs << " processes with "
                     << ppn << " per node from SLURM_TASKS_PER_NODE=" << tasks << endl;
            }

            if (launch_options.target2)
            {
                const char* mic_ppn   = (getenv("MIC_PPN") ? getenv("MIC_PPN") : "4"); // default:4
                const char* mic_thrds = getenv("MIC_OMP_NUM_THREADS");                 // default:30
                int         ranks     = nodes * atoi(mic_ppn);
                if (dual_mic)
                {
                    ranks *= 2;
                }
                if (ranks)
                {
                    char* mic_ranks = (char*)calloc(8, 1);
                    sprintf(mic_ranks, "%d", ranks);
                    mic_config  = (dual_mic ? "2" : "") + string("mic");
                    mic_config += string(mic_ppn) + "p" + string(mic_ranks);
                    if (mic_thrds)
                    {
                        mic_config += "x" + string(mic_thrds);
                    }
                    if (verbose)
                    {
                        cerr << "Got MIC config " << mic_config << endl;
                    }
                }
            }
            else if (verbose)
            {
                cerr << "No MIC config!" << endl;
            }
        }
    }

    if (  epk_inst_mpi
       && !launch_options.nprocs)
    {
        // used by SLURM srun and various other MPI launchers
        char* env = getenv("SLURM_NPROCS");
        if (env)
        {
            launch_options.nprocs = env;
            if (verbose)
            {
                cerr << "Got " << launch_options.nprocs
                     << " processes from SLURM_NPROCS" << endl;
            }
        }
        else if (launcher == "srun")
        {
            cerr << "Warning: SLURM_NPROCS not set!" << endl;
        }
    }

    if (  epk_inst_mpi
       && (launcher == "mpiexec")
       && !launch_options.nprocs)
    {
        // check LSF environment
        char* env = getenv("LSB_DJOB_NUMPROC");
        if (env)
        {
            launch_options.nprocs = env;
            if (verbose)
            {
                cerr << "Got " << launch_options.nprocs
                     << " processes from LSB_DJOB_NUMPROC" << endl;
            }
        }
    }

    if (  epk_inst_mpi
       && (launcher == "mpirun.lsf")
       && !launch_options.nprocs)
    {
        char* env = getenv("LSB_MCPU_HOSTS");
        if (!env)
        {
            cerr << "Warning: LSB_MCPU_HOSTS not set!" << endl;
        }
        else
        {
            env = UTILS_CStr_dup(env);
            if (verbose > 1)
            {
                cerr << "LSB_MCPU_HOSTS = " << env << endl;
            }

            // parse alternating pairs of hostnames and process counts
            unsigned nprocs = 0;
            char*    token  = strtok(env, " ");
            do
            {
                nprocs += atoi(strtok(NULL, " "));
            }
            while ((token = strtok(NULL, " ")) != NULL);

            launch_options.nprocs = (char*)calloc(16, 1);
            sprintf(launch_options.nprocs, "%u", nprocs);
            if (verbose)
            {
                cerr << "Got " << launch_options.nprocs
                     << " processes from LSB_MCPU_HOSTS" << endl;
            }
            free(env);
        }
    }

    if (  epk_inst_mpi
       && (launcher == "ibrun")
       && !launch_options.nprocs)
    {
        // determine SGE configuration
        const char* peway     = getenv("PE");
        const char* nslots    = getenv("NSLOTS");
        const char* my_nslots = getenv("MY_NSLOTS");
        if (my_nslots)
        {
            launch_options.nprocs = getenv("MY_NSLOTS");
            if (verbose)
            {
                cerr << "Got " << launch_options.nprocs
                     << " processes from MY_NSLOTS" << endl;
            }
        }
        else if (nslots && peway)
        {
            int slots  = atoi(nslots);
            int way    = atoi(peway);
            int nprocs = slots * way / 16;
            if (verbose)
            {
                cerr << "Got " << nprocs << " processes"
                     << " from NSLOTS(" << nslots
                     << ")*PE(" << peway << ")/16" << endl;
            }
            if (nprocs > 0)
            {
                launch_options.nprocs = (char*)calloc(16, 1);
                sprintf(launch_options.nprocs, "%d", nprocs);
            }
        }
    }

    if (  epk_inst_mpi
       && (launcher == "poe"))
    {
        unsigned nprocs = 0;
        if (getenv("LOADL_STEP_ID"))
        {
            // LoadLeveler batch job
            // newer versions of LoadLeveler have number of task instances (for non-master task)
            string llquery = "llq -l $LOADL_STEP_ID | grep -c \"Task Instance:\"";
            int    tokens;
            FILE*  stream = popen(llquery.c_str(), "r");
            if (!stream)
            {
                cerr << SCAN << "Error: " << llquery << endl << strerror(errno) << endl;
            }
            else if ((tokens = fscanf(stream, "%u", &nprocs)) != 1)
            {
                cerr << SCAN << "Error: llquery parsed " << tokens << " tokens" << endl << strerror(errno) << endl;
            }
            else if (verbose)
            {
                cerr << "Got " << nprocs << " processes from llquery" << endl;
            }
            if (stream)
            {
                pclose(stream);
            }
            if (nprocs > 0)
            {
                // don't count master task
                nprocs--;
            }

            if (nprocs <= 0)
            {
                // try again with former keyword specifying number of CPUs for step
                string llquery = "llq -l $LOADL_STEP_ID | grep \"Step Cpus:\" | cut -d: -f 2";
                stream = popen(llquery.c_str(), "r");
                if (!stream)
                {
                    cerr << SCAN << "Error: " << llquery << endl << strerror(errno) << endl;
                }
                else if ((tokens = fscanf(stream, "%u", &nprocs)) != 1)
                {
                    cerr << SCAN << "Error: llquery parsed " << tokens << " tokens" << endl << strerror(errno) << endl;
                }
                else if (verbose)
                {
                    cerr << "Got " << nprocs << " processes from llquery" << endl;
                }
                if (stream)
                {
                    pclose(stream);
                }
            }
        }
        else if (getenv("MP_PROCS"))
        {
            // interactive job
            nprocs = atoi(getenv("MP_PROCS"));
            if (verbose)
            {
                cerr << "Got " << nprocs << " processes from MP_PROCS" << endl;
            }
        }

        launch_options.nprocs = (char*)calloc(16, 1);
        sprintf(launch_options.nprocs, "%u", nprocs);
    }

    if (  epk_inst_mpi
       && (launcher == "mpiexec")
       && !launch_options.nprocs)
    {
        // PBS_NODEFILE lists compute nodes one per line
        if (getenv("PBS_NODEFILE"))
        {
            unsigned nnodes = 0;
            int      tokens;
            string   wcquery = (launch_options.cnmode ? "sort -u" : "cat")
                               + string(" $PBS_NODEFILE | wc -l");
            FILE* stream = popen(wcquery.c_str(), "r");
            if (!stream)
            {
                cerr << SCAN << "Error: " << wcquery << endl << strerror(errno) << endl;
            }
            else if ((tokens = fscanf(stream, "%u", &nnodes)) != 1)
            {
                cerr << SCAN << "Error: wcquery parsed " << tokens << " tokens" << endl << strerror(errno) << endl;
            }
            else if (verbose > 1)
            {
                cerr << "Got " << nnodes << " nodes from PBS_NODEFILE" << endl;
            }
            if (nnodes > 0)
            {
                int ppn    = launch_options.cnmode ? atoi(launch_options.cnmode) : 1;
                int nprocs = nnodes * ppn;
                launch_options.nprocs = (char*)calloc(16, 1);
                sprintf(launch_options.nprocs, "%d", nprocs);
                if (verbose)
                {
                    cerr << "Got " << nprocs << " processes from PBS_NODEFILE" << endl;
                }
            }
            if (stream)
            {
                pclose(stream);
            }
        }
        #ifdef __FUJITSU
            else
            {
                int    nprocs = 0;
                string query  = string("mpiexec --stdout /tmp/nprocs -np 1 printenv OMPI_UNIVERSE_SIZE 2>/dev/null");
                if (verbose > 1)
                {
                    cerr << query << endl;
                }
                int retval = system(query.c_str());
                if (retval == 2)
                {
                    cerr << SCAN << query << endl
                         << "Warning: " << strerror(errno) << endl;
                }

                FILE* stream = fopen("/tmp/nprocs", "r");
                if (!stream)
                {
                    cerr << SCAN << "Error: fopen(/tmp/nprocs)" << endl << strerror(errno) << endl;
                }
                else
                {
                    fscanf(stream, "%d", &nprocs);
                    fclose(stream);
                    if (nprocs > 0)
                    {
                        launch_options.nprocs = (char*)calloc(16, 1);
                        sprintf(launch_options.nprocs, "%d", nprocs);
                    }
                    if (verbose)
                    {
                        cerr << "Got " << nprocs << " processes from OMPI_UNIVERSE_SIZE" << endl;
                    }
                }
                unlink("/tmp/nprocs");    // ignore failure to remove temporary file
            }
        #endif
    }

    if (  epk_inst_mpi
       && (launcher == "yod"))
    {
        envassign = "=";

        // PBS_NNODES is the number of available compute nodes
        unsigned    nnodes = 0;
        const char* env    = getenv("PBS_NNODES");
        if (env)
        {
            nnodes = atoi(env);
            if (verbose)
            {
                cerr << "PBS_NNODES=" << nnodes << endl;
            }
            if (nnodes <= 0)
            {
                cerr << SCAN << "Abort: invalid PBS_NNODES=" << env << " for yod launch" << endl;
                exit(1);
            }
        }

        // nprocs=all is the default, indicating all available processors
        unsigned nprocs = nnodes;
        if (  (nnodes && !launch_options.nprocs)
           || (launch_options.nprocs && (string(launch_options.nprocs) == "all")))
        {
            launch_options.nprocs = (char*)calloc(8, 1);
            if (  !launch_options.cnmode
               || (string(launch_options.cnmode) == string(modes[MODE_VN])))
            {
                nprocs *= 2;    // XXXX dualcore may not always be the default
            }
            sprintf(launch_options.nprocs, "%u", nprocs);
        }
        if (verbose)
        {
            cerr << "Got " << nprocs << " processes on " << nnodes << " compute nodes" << endl;
        }
    }

    if (epk_inst_mpi)
    {
        if (getenv("SCAN_MPI_RANKS"))
        {
            // explicit specification
            int ranks = epk_str2int(getenv("SCAN_MPI_RANKS"));
            cerr << "Using SCAN_MPI_RANKS=" << ranks << " processes!" << endl;
            launch_options.nprocs = getenv("SCAN_MPI_RANKS");
        }
        if (olaunch && !launch_options.nprocs)
        {
            // mark as omitted
            launch_options.nprocs = UTILS_CStr_dup("O");
        }
        else if (!launch_options.nprocs)
        {
            // check nprocs set and valid
            cerr << SCAN << "Warning: Number of processes not known!" << endl;
        }
        else if (atoi(launch_options.nprocs) <= 0)
        {
            // verify number
            cerr << SCAN << "Warning: Invalid number of processes: " << launch_options.nprocs << endl;
            unsetenv("SCAN_MPI_RANKS");
            launch_options.nprocs = UTILS_CStr_dup("X");
        }

        // check mpicwd exists, if specified
        if (  launch_options.mpicwd
           && !epk_archive_exists(launch_options.mpicwd))
        {
            cerr << SCAN << "Abort: Non-existant MPICWD \"" << launch_options.mpicwd << "\"" << endl;
            exit(1);
        }
    }

    // determine (provisional) archive location
    string epk_archive = ".";

    string epk_title, epk_title_base;
    char*  env_title = getenv("SCOREP_EXPERIMENT_DIRECTORY");
    if (env_title && archive.empty())
    {
        archive = env_title;
    }
    if (!archive.empty())
    {
        // override title with specified archive
        if (archive[archive.length() - 1] == '/')
        {
            archive[archive.length() - 1] = '\0';
        }
        if (verbose)
        {
            cerr << "archive: " << archive << endl;
        }
        string::size_type prefix_start = archive.rfind(expt_prefix);
        string::size_type path_split   = archive.rfind("/");
        if (  (prefix_start == string::npos)
           || ((prefix_start > 0) && (path_split != prefix_start - 1)))
        {
            cerr << SCAN << "Warning: no \"" << expt_prefix
                 << "\" prefix for experiment archive name \"" << archive << "\"" << endl;
            expt_prefix.clear();
            prefix_start = (path_split == string::npos) ? 0 : path_split + 1;
        }
        epk_title = archive.substr(prefix_start + expt_prefix.length());    // title excluding prefix
        if (path_split != string::npos)
        {
            // archive includes path specification
            epk_archive = archive.substr(0, prefix_start - 1);
        }
        else
        {
            epk_archive = ".";    // unspecified path
        }
    }
    if (epk_title.empty())
    {
        // default to override
        // determine title from basename(target) + cnmode + nprocs x nthrds
        if (launch_options.target)
        {
            string target(launch_options.target);
            size_t sep = target.rfind('/');
            if (sep == string::npos)
            {
                sep = 0;
            }
            else
            {
                sep++;
            }
            size_t dot = target.find('.', sep);
            if (dot == string::npos)
            {
                dot = target.length();
            }
            epk_title = target.substr(sep, dot - sep) + "_";
            if (launch_options.cnmode)
            {
                epk_title += launch_options.cnmode;
            }
            if (launch_options.nprocs)
            {
                epk_title += launch_options.nprocs;
            }
            else
            {
                epk_title += "O";
            }
            if (launch_options.nthrds)
            {
                epk_title += string("x") + launch_options.nthrds;
            }
            else if (epk_inst_omp)
            {
                epk_title += "xO";
            }
            else if (epk_inst_pthreads)
            {
                epk_title += "xP";
            }
            if (!mic_config.empty())
            {
                epk_title += "+";
            }
        }
        else if (launch_options.target2)
        {
            // ibrun.symm -m target2
            string target(launch_options.target2);
            size_t sep = target.rfind('/');
            if (sep == string::npos)
            {
                sep = 0;
            }
            else
            {
                sep++;
            }
            size_t dot = target.find('.', sep);
            if (dot == string::npos)
            {
                dot = target.length();
            }
            epk_title = target.substr(sep, dot - sep) + "_";
        }

        if (!mic_config.empty())
        {
            epk_title += mic_config;
        }

        epk_title_base = epk_title;

        // append suffix indicating type of measurement
        if (tracing)
        {
            epk_title += "_trace";    // may include summary
        }
        else if (summary)
        {
            epk_title += "_sum";
        }
    }

    if (launch_options.mpicwd)
    {
        // update relative archive paths
        if (epk_archive == ".")
        {
            epk_archive = string(launch_options.mpicwd);
        }
        else if (epk_archive[0] != '/')
        {
            epk_archive = string(launch_options.mpicwd) + "/" + epk_archive;
        }
    }

    if (measure)
    {
        // verify accessiblity of directory to contain new archive
        if (access(epk_archive.c_str(), W_OK | X_OK) == -1)
        {
            cerr << SCAN << "Abort: unwritable directory " << epk_archive << endl;
            exit(1);
        }

        if (tracing)
        {
            // check file system of experiment directory against SCAN_TRACE_FILESYS
            checkTraceFileSystem(epk_archive);
        }
    }

    if (tracing)
    {
        // ensure an initial "-t" parameter is mirrored in the env variable
        setenv("SCOREP_ENABLE_TRACING", "true", 1);
    }
    if (!summary)
    {
        // ensure an initial "-q" parameter is mirrored in the env variable
        setenv("SCOREP_ENABLE_PROFILING", "false", 1);
    }

    // append archive name to archive path
    string epk_archive_base = epk_archive + string("/") + expt_prefix + epk_title;
    string multi_run_suffix = "";
    if (  (runs_per_config > 1)
       || (multirun_cfg.getNumSets() > 1))
    {
        // create a subdirectory for the run set, because of multiple run configurations
        // and/or additional runs per config
        multi_run_suffix = "_multi-run";
        if (!multirun_cfg.getPresetName().empty())
        {
            // We are in preset mode, use its name for the base directory
            // Name is a valid file name, therefore usable for the directory
            multi_run_suffix = "_preset_" + multirun_cfg.getPresetName();
        }

        if (epk_title_base.empty())
        {
            epk_title_base = epk_title;
        }
        epk_archive_base = epk_archive + string("/") + expt_prefix + epk_title_base;

        if (multirun_cfg.getNumSets() > 1)
        {
            ostringstream stream;
            stream << multirun_cfg.getNumSets();
            multi_run_suffix += "_c" + stream.str();
        }
        if (runs_per_config > 1)
        {
            ostringstream stream;
            stream << runs_per_config;
            multi_run_suffix += "_r" + stream.str();
        }

        if (measure && !preview)
        {
            // Do not create the base directory before a potential block is lifted
            testLockFile(blocker);

            string dirname = epk_archive_base + multi_run_suffix;
            retval = mkdir(dirname.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            if (retval != 0)
            {
                cerr << "Error: " << dirname << ": " << strerror(errno) << endl;
                exit(1);
            }
            multirun_cfg.saveConfigAs(epk_archive_base + multi_run_suffix + "/scalasca_run.cfg");
        }
    }


    // running multiple configurations specified via config file, or at least the default config
    for (int i = 0; i < multirun_cfg.getNumSets(); i++)
    {
        string multi_run_prefix = "";
        string run_count_string = expt_prefix + epk_title_base;

        if (multirun_cfg.getNumSets() > 1)
        {
            run_count_string += string("_c") + numberWithLeadingZeroes(i + 1, multirun_cfg.getNumSets());
        }

        multirun_cfg.setRunVariables(i);
        summary = getenvBool("SCOREP_ENABLE_PROFILING", true);
        tracing = getenvBool("SCOREP_ENABLE_TRACING", false);

        // repeat the current configuration as specified by the -R parameter
        for (int j = 0; j < runs_per_config; j++)
        {
            // Print a separating newline between runs except for the very first run
            if ((i + j) > 0)
            {
                cerr << endl;
            }

            string repeat_count_string = run_count_string;
            string multi_repeat_prefix = "";
            if (runs_per_config > 1)
            {
                repeat_count_string += string("_r") + numberWithLeadingZeroes(j + 1, runs_per_config);
            }

            epk_archive = epk_archive_base;
            if (  (runs_per_config > 1)
               || (multirun_cfg.getNumSets() > 1))
            {
                epk_archive += multi_run_suffix + string("/") + repeat_count_string;
                cerr << SCAN << "RUN: " << i << " REPETITION: " << j << " Archive=" << epk_archive << endl;
                if (!multirun_cfg.getRunConfigTitle(i).empty())
                {
                    cerr << SCAN << "RUN: " << multirun_cfg.getRunConfigTitle(i) << endl;
                }
            }
            cerr << SCAN << "Scalasca " << VERSION << spc;
            if (!measure)
            {
                cerr << "trace analysis";
            }
            else if (tracing)
            {
                cerr << "trace collection and analysis";
            }
            else if (summary)
            {
                cerr << "runtime summarization";
            }
            else
            {
                cerr << "quiescent measurement execution";
            }

            if (preview)
            {
                cerr << " preview";
            }
            cerr << endl;

            // (re-)set environment variable to experiment title ?
            if (verbose)
            {
                cerr << "SCOREP_EXPERIMENT_DIRECTORY=" << epk_archive.c_str() << endl;
            }
            retval = setenv("SCOREP_EXPERIMENT_DIRECTORY", epk_archive.c_str(), 1);
            if (retval)
            {
                cerr << "setenv: " << strerror(errno) << endl;
            }

            // verify archive location usable for new measurement and/or analysis
            if (measure)
            {
                if (epk_archive_exists(epk_archive.c_str()))
                {
                    char* env_overwrite = getenv("SCOREP_OVERWRITE_EXPERIMENT_DIRECTORY");
                    bool  overwrite     = (env_overwrite && (strcmp(env_overwrite, "true") == 0));

                    // remove existing archive if overwrite is set
                    if (getenvBool("SCAN_OVERWRITE", false) || overwrite)
                    {
                        int pause = epk_str2int(getenv("SCAN_OVERWRITE"));
                        cerr << SCAN << "Warning: overwriting " << epk_archive << endl;
                        sleep(pause);
                        string remove = "/bin/rm -rf " + epk_archive;
                        cerr << "[" << remove << "]" << endl;
                        retval = system(remove.c_str());
                        if (retval == -1)
                        {
                            cerr << SCAN << "Error: remove " << strerror(errno) << endl;
                        }
                        status = WEXITSTATUS(retval);
                        if (status)
                        {
                            cerr << SCAN << remove << " returned " << status << endl;
                        }

                        // verify old archive removed
                        if (epk_archive_exists(epk_archive.c_str()))
                        {
                            cerr << SCAN << "Abort: failed to remove existing archive " << epk_archive << endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        cerr << SCAN << "Abort: measurement blocked by existing archive " << epk_archive << endl;
                        exit(1);
                    }
                }
            }
            else
            {
                if (!epk_archive_exists(epk_archive.c_str()))
                {
                    cerr << SCAN << "Abort: archive to analyze not found " << epk_archive << endl;
                    exit(1);
                }

                // verify accessiblity of existing archive for analysis
                if (access(epk_archive.c_str(), R_OK | W_OK | X_OK) == -1)
                {
                    cerr << SCAN << "Abort: inaccessible archive " << epk_archive << endl;
                    exit(1);
                }
            }

            cerr << SCAN << epk_archive << " experiment archive" << endl;

            string analcmd;
            string analyzer;

            if (!measure || tracing)
            {
                // locate trace analyzer
                string path;
                string scan = string(argv[0]);
                size_t sep  = scan.rfind('/');
                #if HAVE(PLATFORM_MIC) || HAVE(MIC_SUPPORT)
                    path += PKGLIBEXECDIR ":";
                #endif    /* HAVE(PLATFORM_MIC) || HAVE(MIC_SUPPORT) */
                if (getenv("SCALASCA_RTS"))
                {
                    path += string(getenv("SCALASCA_RTS")) + ":";
                }
                if (getenv("SCALASCA_DIR"))
                {
                    path += string(getenv("SCALASCA_DIR")) + "/bin:";
                }
                if (getenv("SCALASCA_DIR"))
                {
                    path += string(getenv("SCALASCA_DIR")) + "/bin/backend:";
                }

                // add path to SCAN executable itself (if specified)
                if (sep != string::npos)
                {
                    path += scan.substr(0, sep) + ":";
                }
                path += getenv("PATH");

                if (getenv("SCAN_TRACE_ANALYZER"))
                {
                    analcmd = getenv("SCAN_TRACE_ANALYZER");
                    if (verbose)
                    {
                        cerr << "Got SCAN_TRACE_ANALYZER \"" << analcmd << "\"" << endl;
                    }
                    if (analcmd == "none")
                    {
                        analyzer = "none";
                        cerr << "Info: Automatic trace analysis will be skipped!" << endl;
                    }
                    else
                    {
                        analyzer = which(analcmd, path);
                        if (analyzer.empty())
                        {
                            cerr << SCAN << "Abort: specified trace analyzer (" << analcmd << ") not found!" << endl;
                            exit(1);
                        }

                        // strip dirname and extension from analcmd
                        if ((sep = analcmd.rfind('/')) != string::npos)
                        {
                            analcmd.erase(0, sep);
                        }
                        if ((sep = analcmd.find('.')) != string::npos)
                        {
                            analcmd.erase(sep);
                        }
                    }
                }

                if (  analyzer.empty()
                   && (epk_inst_omp || epk_inst_pthreads)
                   && epk_inst_mpi)
                {
                    analcmd = "scout";
                    string analexe = analcmd + ".hyb";
                    analyzer = which(analexe, path);
                    if (analyzer.empty())
                    {
                        cerr << SCAN << "Warning: hybrid trace analyzer (" << analexe << ") not found!" << endl;
                    }
                }

                if (  analyzer.empty()
                   && epk_inst_mpi)
                {
                    analcmd = "scout";
                    string analexe = analcmd + ".mpi";
                    analyzer = which(analexe, path);
                    if (analyzer.empty())
                    {
                        cerr << SCAN << "Warning: MPI trace analyzer (" << analexe << ") not found!" << endl;
                    }
                    else if (epk_inst_omp)
                    {
                        cerr << SCAN << "Warning: MPI trace analyzer will ignore OpenMP events in hybrid trace!" << endl;
                    }
                }

                if (  analyzer.empty()
                   && (epk_inst_omp || epk_inst_pthreads)
                   && !epk_inst_mpi)
                {
                    analcmd = "scout";
                    string analexe = analcmd + ".omp";
                    analyzer = which(analexe, path);
                    if (analyzer.empty())
                    {
                        cerr << SCAN << "Warning: OMP trace analyzer (" << analexe << ") not found!" << endl;
                    }
                }

                if (analyzer.empty())
                {
                    analcmd = "scout";
                    string analexe = analcmd + ".ser";
                    analyzer = which(analexe, path);
                    if (analyzer.empty())
                    {
                        cerr << SCAN << "Warning: serial trace analyzer (" << analexe << ") not found!" << endl;
                    }
                }

                if (analyzer.empty())
                {
                    cerr << SCAN << "Abort: no suitable trace analyzer found!" << endl;
                    exit(1);
                }
                if (verbose)
                {
                    cerr << "Got trace analyzer \"" << analyzer << "\"" << endl;
                }
            }

            if (measure)
            {
                // update defaults by setting environment variables
                retval = setenv("SCOREP_ENABLE_PROFILING", (summary ? "true" : "false"), 1);
                if (retval)
                {
                    cerr << "setenv(SCOREP_ENABLE_PROFILING): " << strerror(errno) << endl;
                }
                retval = setenv("SCOREP_ENABLE_TRACING", (tracing ? "true" : "false"), 1);
                if (retval)
                {
                    cerr << "setenv(SCOREP_ENABLE_TRACING): " << strerror(errno) << endl;
                }

                if (!filter.empty())
                {
                    if (verbose)
                    {
                        cerr << "SCOREP_FILTERING_FILE=" << filter.c_str() << endl;
                    }
                    retval = setenv("SCOREP_FILTERING_FILE", filter.c_str(), 1);
                    if (retval)
                    {
                        cerr << "setenv: " << strerror(errno) << endl;
                    }
                }
            }

            string launch = launcher_path;
            launch += launch_options.other;

            // formulate revised launch command
            string mpietc;
            if (  epk_archive_exists("/bgl/BlueLight")
               || epk_archive_exists("/bgsys"))
            {
                bool bgq = epk_archive_exists("/bgsys/drivers/ppcfloor/hlcs");
                status = 0;

                if (launcher != "srun")
                {
                    // configure for mpirun or runjob
                    envflag   = "-env";
                    envassign = "=";
                    if (bgq)
                    {
                        envflag = "--envs";
                    }

                    // unset envflag if using it results in conflicts
                    if (launch_options.other.find("--env-all") != string::npos)
                    {
                        envflag.erase();
                    }
                }

                if (verbose)
                {
                    cerr << "Got BlueGene with launcher " << launcher << endl;
                }
                if (cobalt)
                {
                    string cobalt_launcher = (bgq ? "runjob" : "cobalt-mpirun");
                    cerr << SCAN << "Got Cobalt BlueGene launcher " << launcher
                         << " for " << cobalt_launcher << endl;

                    char    buf[256];
                    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
                    if (len < 0)
                    {
                        cerr << SCAN << "Abort; failed readlink: " << strerror(errno) << endl;
                        exit(5);
                    }
                    else if (len == sizeof(buf))
                    {
                        cerr << SCAN << "Abort: readlink buf[" << sizeof(buf) << "] too small" << endl;
                        exit(6);
                    }
                    else
                    {
                        buf[len] = '\0';    // terminate name
                    }

                    // SCAN needs to be specified with full path (where scout is also found)!
                    string            scan_path(buf);
                    string::size_type loc = scan_path.rfind("/fe/", len);
                    if (loc != string::npos)
                    {
                        scan_path.erase(loc, 3);
                    }

                    string script = launch + mpietc + spc + "--mode script ";
                    script += scan_path + spc;
                    for (int iarg = 1; iarg < last_scan_arg; iarg++)
                    {
                        script += argv[iarg] + spc;
                    }
                    script += cobalt_launcher + launch_options.mpienv + spc;

                    if (bgq)
                    {
                        if (launch_options.cnmode)
                        {
                            string ppn = launch_options.cnmode;
                            if (ppn[ppn.length() - 1] == 'p')
                            {
                                ppn.erase(ppn.length() - 1, 1);
                            }
                            script += "--ranks-per-node " + ppn + spc;
                        }
                        script += "--np " + string(launch_options.nprocs) + spc;
                        #ifdef BG_RUNJOB_ALT
                            script += ": ";
                            script += string(launch_options.target);
                            if (launch_options.t_args && strlen(launch_options.t_args))
                            {
                                script += spc + launch_options.t_args;
                            }
                        #else
                            script += "--exe " + string(launch_options.target);
                            if (launch_options.t_args && strlen(launch_options.t_args))
                            {
                                script += spc + "--args " + launch_options.t_args;
                            }
                        #endif
                    }
                    else
                    {
                        if (!launch_options.mpicwd)
                        {
                            script += "-cwd " + string(getenv("PWD")) + spc;
                        }
                        if (launch_options.cnmode)
                        {
                            script += "-mode " + string(launch_options.cnmode) + spc;
                        }
                        script += "-np " + string(launch_options.nprocs) + spc;
                        script += launch_options.target;
                        if (launch_options.t_args && strlen(launch_options.t_args))
                        {
                            script += spc + launch_options.t_args;
                        }
                    }
                    cerr << script << endl;
                    if (!preview)
                    {
                        // submit script
                        retval = system(script.c_str());
                        if (retval == -1)
                        {
                            cerr << SCAN << "Error: qsub " << strerror(errno) << endl;
                        }
                        status = WEXITSTATUS(retval);
                        if (status)
                        {
                            cerr << SCAN << "qsub returned status " << status << endl;
                        }
                    }
                    cerr << SCAN << epk_archive << " job submission"
                         << (preview ? " preview" : "") << spc
                         << (status ? "probably failed!" : "complete.") << endl;
                    exit(0);    // all done for now
                }
            }

            if (envflag.empty())
            {
                // ParaStation MPI needs EPIK configuration explicitly passed
                if (launcher_path.find("/parastation/") != string::npos)
                {
                    #ifdef PARASTATION_ALT
                        // exporting all environment variables to all processes may lead to timeouts
                        mpietc += " --envall";
                    #else
                        // the documented "-E ENV val" may be unreliable for more than one envvar
                        envflag = "-E";
                    #endif
                }

                // OpenMPI (mostly) needs EPIK configuration explicitly passed
                if (launcher == "orterun")
                {
                    envflag   = "-x";
                    envassign = "=";
                }
            }

            // incorporate updated environment in launch command
            if (!envflag.empty())
            {
                // copy all (set?) SCOREP configuration variables from environment
                string cmd  = "printenv | grep '^SCOREP_' 2>/dev/null";
                FILE*  pipe = popen(cmd.c_str(), "r");
                if (!pipe)
                {
                    cerr << SCAN << "Error: " << cmd << endl << strerror(errno) << endl;
                }
                else
                {
                    char confvar[1024];
                    while (fgets(confvar, 1024, pipe))
                    {
                        char* val = strchr(confvar, '=');
                        confvar[strlen(confvar) - 1] = '\0';
                        *val                         = '\0';
                        val++;
                        if (verbose > 1)
                        {
                            cerr << "Got config-var: " << confvar << envassign << val << endl;
                        }
                        mpietc += spc + envflag + spc + confvar + envassign + val;
                    }
                    pclose(pipe);
                }
            }

            #ifdef _SX
                // On NEC-SX, environment variables that should be passed by mpirun to
                // the MPI tasks need to be specified in MPIEXPORT
                {
                    vector< string > exp_vars;

                    // Tokenize current value of MPIEXPORT
                    const char* env_value = getenv("MPIEXPORT");
                    if (env_value)
                    {
                        char* tmp = UTILS_CStr_dup(env_value);
                        char* pos = strtok(tmp, " ");
                        while (pos)
                        {
                            exp_vars.push_back(string(pos));
                            pos = strtok(NULL, " ");
                        }
                        free(tmp);
                    }

                    // Add all (set?) SCOREP configuration variables from environment
                    string cmd  = "printenv | grep '^SCOREP_' 2>/dev/null";
                    FILE*  pipe = popen(cmd.c_str(), "r");
                    if (!pipe)
                    {
                        cerr << SCAN << "Error: " << cmd << endl << strerror(errno) << endl;
                    }
                    else
                    {
                        char confvar[1024];
                        while (fgets(confvar, 1024, pipe))
                        {
                            char* val = strchr(confvar, '=');
                            confvar[strlen(confvar) - 1] = '\0';
                            *val                         = '\0';
                            val++;
                            if (verbose > 1)
                            {
                                cerr << "Got config-var: " << confvar << envassign << val << endl;
                            }
                            if (find(exp_vars.begin(), exp_vars.end(), string(confvar)) == exp_vars.end())
                            {
                                exp_vars.push_back(string(confvar));
                            }
                        }
                        pclose(pipe);
                    }

                    // Set/overwrite MPIEXPORT
                    string                     mpiexport;
                    vector< string >::iterator it = exp_vars.begin();
                    while (it != exp_vars.end())
                    {
                        mpiexport += *it + " ";
                        ++it;
                    }
                    if (verbose)
                    {
                        cerr << "Setting MPIEXPORT=" << mpiexport << endl;
                    }
                    retval = setenv("MPIEXPORT", mpiexport.c_str(), 1);
                    if (retval)
                    {
                        cerr << "setenv: " << strerror(errno) << endl;
                    }
                }
            #endif

            if (verbose && !olaunch)
            {
                cerr << "Launch \"" << launch << "\"" << endl;
            }
            if (verbose && measure)
            {
                if (launch_options.target)
                {
                    cerr << "Target \"" << launch_options.target;
                    if (launch_options.t_args && strlen(launch_options.t_args))
                    {
                        cerr << spc << launch_options.t_args;
                    }
                    cerr << "\"" << endl;
                }
                if (launch_options.target2)
                {
                    cerr << "MIC Target \"" << launch_options.target2;
                    if (launch_options.t_args2 && strlen(launch_options.t_args2))
                    {
                        cerr << spc << launch_options.t_args2;
                    }
                    cerr << "\"" << endl;
                }
                if (!mpietc.empty())
                {
                    cerr << "MPIetc \"" << mpietc << "\"" << endl;
                }
            }

            if (measure)
            {
                string collect = olaunch ? "" : (launch + mpietc);
                if (!escape.empty())
                {
                    collect += spc + escape;
                }

                if (launcher != "ibrun.symm")
                {
                    collect += spc + launch_options.target;
                    if (launch_options.t_args && strlen(launch_options.t_args))
                    {
                        collect += spc + launch_options.t_args;
                    }
                }
                else    // ibrun.symm
                {
                    if (launch_options.target)
                    {
                        collect += spc + string("-c \"") + launch_options.target;
                        if (launch_options.t_args && strlen(launch_options.t_args))
                        {
                            collect += spc + launch_options.t_args;
                        }
                        collect += "\"";
                    }
                    if (launch_options.target2)
                    {
                        collect += spc + (dual_mic ? "-2" : "-m") + string(" \"") + launch_options.target2;
                        if (launch_options.t_args2 && strlen(launch_options.t_args2))
                        {
                            collect += spc + launch_options.t_args2;
                        }
                        collect += "\"";
                    }
                }

                // Test for lock file before starting measurement
                testLockFile(blocker);

                if (preview)
                {
                    cerr << SCAN << "Collect preview" << endl << collect << endl;
                }
                else
                {
                    // actual experiment collection
                    ofstream elog;
                    string   elogfile = epk_title + ".log";    // should be a "unique" name
                    string   eoutfile = epk_title + ".out";    // only used with redirect

                    if (launcher != "cobalt-mpirun")
                    {
                        elog.open(elogfile.c_str(), ios::out | ios::trunc);
                        if (!elog.good())
                        {
                            cerr << SCAN << "Abort: failed to open log file " << elogfile << endl;
                            exit(2);
                        }
                    }

                    time(&time1);
                    date = ctime(&time1);
                    date.erase(date.length() - 1);
                    cerr << SCAN << date << ": Collect start" << endl << collect << endl;
                    if (launcher != "cobalt-mpirun")
                    {
                        elog << SCAN << date << ": Collect start" << endl << collect << endl;
                        elog.close();

                        if (!redirect)
                        {
                            collect += " 2>&1 | tee -a " + elogfile;    // append to collect log
                        }
                        else
                        {
                            #ifdef __FUJITSU
                                string mpiexec     = "mpiexec";
                                size_t mpiexec_pos = collect.find(mpiexec);
                                string redirection = string(" --std ") + eoutfile;
                                collect.insert(mpiexec_pos + mpiexec.length(), redirection);
                                if (verbose)
                                {
                                    cerr << "[" << collect << "]" << endl;
                                }
                            #else
                                collect += string(" 1>") + eoutfile + string(" 2>>") + elogfile;
                            #endif
                            cerr << "[launch output redirected to " + epk_archive + "]" << endl;
                        }
                    }

                    retval = system(collect.c_str());
                    if (retval == -1)
                    {
                        cerr << SCAN << "Error: collect " << strerror(errno) << endl;
                    }
                    status = WEXITSTATUS(retval);

                    time(&time2);
                    date = ctime(&time2);
                    date.erase(date.length() - 1);
                    dtime = difftime(time2, time1);
                    cerr << SCAN << date << ": Collect done (status=" << status
                         << ") " << dtime << "s" << endl;
                    if (launcher != "cobalt-mpirun")
                    {
                        elog.open(elogfile.c_str(), ios::out | ios::app);
                        if (!elog.good())
                        {
                            cerr << SCAN << "Error: failed to reopen log file " << elogfile << endl;
                        }
                        else
                        {
                            elog << SCAN << date << ": Collect done (status=" << status
                                 << ") " << dtime << "s" << endl;
                            elog.close();
                        }
                    }

                    // wait for filesystem sync?
                    if (getenvBool("SCAN_WAIT", false))
                    {
                        int waits = epk_str2int(getenv("SCAN_WAIT"));
                        if (verbose)
                        {
                            cerr << "Got SCAN_WAIT=" << waits << endl;
                        }
                        while (!epk_archive_exists(epk_archive.c_str()) && waits)
                        {
                            cerr << SCAN << "Info: SCAN_WAIT for " << epk_archive << endl;
                            sleep(1);
                            waits--;
                        }
                    }

                    // verify experiment archive created
                    if (!epk_archive_exists(epk_archive.c_str()))
                    {
                        cerr << "Abort: missing experiment archive " << epk_archive << endl;
                        exit(2);
                    }

                    if (launcher != "cobalt-mpirun")
                    {
                        // move log(s) into experiment archive
                        // note: rename(2) doesn't work between filesystems
                        string mv_command = "mv " + elogfile + " " + epk_archive + "/scorep.log";
                        retval = system(mv_command.c_str());
                        if (retval)
                        {
                            cerr << SCAN << mv_command << endl
                                 << "Warning: " << strerror(errno) << endl;
                        }
                        else if (verbose)
                        {
                            cerr << "Archived " << elogfile << endl;
                        }
                        if (redirect)
                        {
                            mv_command = "mv " + eoutfile + " " + epk_archive + "/scorep.out";
                            retval     = system(mv_command.c_str());
                            if (retval)
                            {
                                cerr << SCAN << mv_command << endl
                                     << "Warning: " << strerror(errno) << endl;
                            }
                            else if (verbose)
                            {
                                cerr << "Archived " << eoutfile << endl;
                            }
                        }
                    }
                    else
                    {
                        // copy cobalt stderr/stdout logs into experiment archive
                        string cjob       = string(launch_options.mpicwd) + "/" + getenv("COBALT_JOBID");
                        string cp_command = "cp -p " + cjob + ".error " + epk_archive + "/scorep.log";
                        retval = system(cp_command.c_str());
                        if (retval)
                        {
                            cerr << SCAN << cp_command << endl
                                 << "Warning: " << strerror(errno) << endl;
                        }
                        else if (verbose)
                        {
                            cerr << "Archived " << cjob + ".error" << endl;
                        }
                        cp_command = "cp -p " + cjob + ".output " + epk_archive + "/scorep.out";
                        retval     = system(cp_command.c_str());
                        if (retval)
                        {
                            cerr << SCAN << cp_command << endl
                                 << "Warning: " << strerror(errno) << endl;
                        }
                        else if (verbose)
                        {
                            cerr << "Archived " << cjob + ".output" << endl;
                        }
                    }

                    // archive filter
                    string archived_filter = epk_archive + "/scorep.filter";
                    if (  !filter.empty()
                       && (access(archived_filter.c_str(), F_OK) != 0)
                       && (access(filter.c_str(), R_OK) == 0))
                    {
                        string cp_command = "cp -p " + filter + spc + archived_filter;
                        retval = system(cp_command.c_str());
                        if (retval)
                        {
                            cerr << SCAN << cp_command << endl
                                 << "Warning: " << strerror(errno) << endl;
                        }
                        else if (verbose)
                        {
                            cerr << "Archived " << filter << endl;
                        }
                    }

                    // additional archivable info
                    if (  (launcher == "yod")
                       || (launcher == "aprun"))
                    {
                        string arc_command;
                        arc_command = which("xtprocadmin", getenv("PATH"));
                        if (!arc_command.empty())
                        {
                            arc_command += " -A > " + epk_archive + "/xtnids";
                            arc_command += " 2>/dev/null";
                            retval       = system(arc_command.c_str());
                            if (retval)
                            {
                                // retry with old command format
                                arc_command  = "xtprocadmin > " + epk_archive + "/xtnids";
                                arc_command += " 2>/dev/null";
                                retval       = system(arc_command.c_str());
                            }
                            if (  WIFEXITED(retval)
                               && (WEXITSTATUS(retval) != EXIT_SUCCESS))
                            {
                                cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                            }
                            else if (verbose)
                            {
                                cerr << "Archived xtnids" << endl;
                            }
                        }

                        arc_command = which("xtnodestat", getenv("PATH"));
                        if (!arc_command.empty())
                        {
                            arc_command += " > " + epk_archive + "/xtstat";
                            arc_command += " 2>/dev/null";
                            retval       = system(arc_command.c_str());
                            if (  WIFEXITED(retval)
                               && (WEXITSTATUS(retval) != EXIT_SUCCESS))
                            {
                                cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                            }
                            else if (verbose)
                            {
                                cerr << "Archived xtstat" << endl;
                            }
                        }

                        arc_command = which("xtshowmesh", getenv("PATH"));
                        if (!arc_command.empty())
                        {
                            arc_command += " > " + epk_archive + "/xtmesh";
                            arc_command += " 2>/dev/null";
                            retval       = system(arc_command.c_str());
                            if (  WIFEXITED(retval)
                               && (WEXITSTATUS(retval) != EXIT_SUCCESS))
                            {
                                cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                            }
                            else if (verbose)
                            {
                                cerr << "Archived xtmesh" << endl;
                            }
                        }

                        arc_command = which("xtshowcabs", getenv("PATH"));
                        if (!arc_command.empty())
                        {
                            arc_command += " > " + epk_archive + "/xtcabs";
                            arc_command += " 2>/dev/null";
                            retval       = system(arc_command.c_str());
                            if (  WIFEXITED(retval)
                               && (WEXITSTATUS(retval) != EXIT_SUCCESS))
                            {
                                cerr << SCAN << arc_command << " => " << WEXITSTATUS(retval) << endl;
                            }
                            else if (verbose)
                            {
                                cerr << "Archived xtcabs" << endl;
                            }
                        }
                    }

                    // move lockfile into experiment archive (when it still exists)
                    if (  !blocker.empty()
                       && (access(blocker.c_str(), F_OK) == 0))
                    {
                        string mv_command = "mv " + blocker + " " + epk_archive;
                        size_t sep        = blocker.rfind('/');
                        if (sep != string::npos)
                        {
                            mv_command += blocker.substr(sep);
                        }
                        retval = system(mv_command.c_str());
                        if (retval)
                        {
                            cerr << SCAN << mv_command << endl
                                 << "Warning: " << strerror(errno) << endl;
                        }
                        else if (verbose)
                        {
                            cerr << "Archived " << blocker << endl;
                        }
                    }
                }
            }

            if (  !measure
               || (!preview && measure && tracing))
            {
                // verify that the experiment archive contains (analyzable) trace data
                bool   verified = true;
                string datadir  = epk_archive + expt_tracedir;
                if (!epk_archive_exists(datadir.c_str()))
                {
                    cerr << "Error: missing trace data " << datadir << endl;
                    verified = false;
                }
                else if (verbose)
                {
                    cerr << "Got trace data " << datadir << endl;
                }

                // verify auxilliary files also exist
                struct stat sbuf;
                string      edeffile = epk_archive + "/traces.otf2";
                retval = stat(edeffile.c_str(), &sbuf);
                if (retval != 0)
                {
                    cerr << "Error: " << edeffile << ": " << strerror(errno) << endl;
                    verified = false;
                }
                else if (!(sbuf.st_mode & (S_IRUSR | S_IRGRP)))
                {
                    // readable
                    cerr << "Error: unreadable " << edeffile << endl;
                    verified = false;
                }
                else if (sbuf.st_size == 0)
                {
                    cerr << "Error: empty " << edeffile << endl;
                    verified = false;
                }
                else if (verbose)
                {
                    cerr << "Got trace defs " << edeffile << endl;
                }

                if (  verified
                   && epk_inst_mpi
                   && (launcher != "ibrun.symm"))
                {
                    // check number of process ranks
                    int ranks = 0;

                    // XXXX NEC SX requires be/elg_print therefore BINDIR prepended to PATH.
                    // XXXX Alternatively, could explicitly open epik.esd and parse/count records.
                    const string printer    = "otf2-print";
                    string       scan       = string(argv[0]);
                    string       print_path = which(printer,
                                                    string(BINDIR) + ":"
                                                    + scan.substr(0, scan.rfind('/')) + ":"
                                                    + getenv("PATH"));
                    if (print_path.empty())
                    {
                        cerr << SCAN << "Warning: executable \"" << printer << "\" not found on PATH!" << endl;
                    }
                    else
                    {
                        string opt  = " --show-global-defs ";
                        string cmd  = print_path + opt + edeffile + " 2>/dev/null | grep -c ^LOCATION_GROUP.*PROCESS";
                        FILE*  pipe = popen(cmd.c_str(), "r");
                        if (!pipe)
                        {
                            cerr << SCAN << "Error: " << cmd << endl << strerror(errno) << endl;
                        }
                        else
                        {
                            if (fscanf(pipe, "%d", &ranks) != 1)
                            {
                                cerr << SCAN << "Error: " << cmd << endl << "Failed to parse result value" << endl;
                            }
                            pclose(pipe);
                        }

                        // check for OpenMP
                        int openmp = 0;
                        cmd  = print_path + opt + edeffile + " 2>/dev/null | grep -c 'Paradigm: OPENMP'";
                        pipe = popen(cmd.c_str(), "r");
                        if (!pipe)
                        {
                            cerr << SCAN << "Error: " << cmd << endl << strerror(errno) << endl;
                        }
                        else
                        {
                            if (fscanf(pipe, "%d", &openmp) != 1)
                            {
                                cerr << SCAN << "Error: " << cmd << endl << "Failed to parse result value" << endl;
                            }
                            if (verbose)
                            {
                                cerr << "Got paradigm OpenMP=" << openmp << " from " << edeffile << endl;
                            }
                            pclose(pipe);
                        }
                    }
                    if (ranks == 0)
                    {
                        cerr << "Error: counting MPI ranks in " << edeffile << endl;
                    }
                    else if (verbose)
                    {
                        cerr << "Got " << ranks << " MPI ranks in " << edeffile << endl;
                    }
                    if (  ranks
                       && launch_options.nprocs
                       && (atoi(launch_options.nprocs) != ranks))
                    {
                        cerr << "Warning: " << edeffile << " MPI rank count mis-match: "
                             << ranks << "<>" << launch_options.nprocs << endl;
                        if (!getenv("SCAN_MPI_RANKS"))
                        {
                            // don't override!
                            launch_options.nprocs = (char*)calloc(16, 1);
                            sprintf(launch_options.nprocs, "%d", ranks);
                            cerr << "Warning: setting SCAN_MPI_RANKS=" << ranks
                                 << " for trace analysis!" << endl;
                            retval = setenv("SCAN_MPI_RANKS", launch_options.nprocs, 0);
                            if (retval)
                            {
                                cerr << "setenv: " << strerror(errno) << endl;
                            }
                        }
                    }
                    if (getenv("SCAN_MPI_RANKS"))
                    {
                        // override launch specification
                        // XXXX flag might need to depend on MPI launcher
                        launch += string(" -n ") + launch_options.nprocs;
                    }
                }

                // XXXX check readability of datadir
                // XXXX check existance/readability/size of required rank data
                // XXXX check writability of scout.cubex
                // XXXX warn if existing scout.cubex will be overwritten

                if (!verified)
                {
                    if (  (runs_per_config > 1)
                       || (multirun_cfg.getNumSets() > 1))
                    {
                        cerr << "Warning: " << epk_archive << " archive unsuitable for trace analysis." << endl;
                        continue;
                    }
                    else
                    {
                        cerr << "Abort: " << epk_archive << " archive unsuitable for trace analysis." << endl;
                        exit(4);
                    }
                }
                else if (verbose)
                {
                    cerr << "Verified integrity of " << epk_archive << endl;
                }
            }

            if (  (!measure || (measure && tracing))
               && (analcmd != "none"))
            {
                if (analyzer.find("scout.ser") != string::npos)
                {
                    olaunch = true;
                }
                string analyze = (olaunch ? "" : launch);
                if (!escape.empty())
                {
                    analyze += spc + escape;
                }

                string analyzer_cmd = analyzer;
                if (getenv("SCAN_ANALYZE_OPTS"))
                {
                    // include additional analyzer options
                    analyzer_cmd += spc + string(getenv("SCAN_ANALYZE_OPTS"));
                }
                analyzer_cmd += spc + epk_archive;

                // explicitly specify OTF2 handle
                analyzer_cmd += "/traces.otf2";

                if (launcher != "ibrun.symm")
                {
                    analyze += spc + analyzer_cmd;
                }
                else    // ibrun.symm
                {
                    if (launch_options.target)
                    {
                        analyze += spc + string("-c \"") + analyzer_cmd + string("\"");
                    }
                    if (launch_options.target2)
                    {
                        analyze += spc + (dual_mic ? "-2" : "-m") + string(" \"") + analyzer_cmd + string("\"");
                    }
                }

                if (launcher == "runjob")
                {
                    // remove "--mapping <file|spec>"
                    string flag  = "--mapping ";
                    size_t start = analyze.find(flag);
                    if (start != string::npos)
                    {
                        size_t end = analyze.find_first_of(' ', start + flag.length() + 1);
                        if (end != string::npos)
                        {
                            if (verbose)
                            {
                                cerr << "Eliminating incompatible runjob option: "
                                     << analyze.substr(start, end - start) << endl;
                            }
                            analyze.erase(start, end - start);
                        }
                        else
                        {
                            cerr << "Warning: trace analyzer " << analcmd
                                 << " likely incompatible with runjob " << flag << " option!" << endl;
                        }
                    }
                }

                if (launcher == "poe")
                {
                    // remove "-pgmmodel mpmd" and "-cmdfile <file>"
                    string poe_flag = "-pgmmodel ";
                    size_t start    = analyze.find(poe_flag);
                    size_t end      = analyze.find("pmd ", start);
                    if (start != string::npos)
                    {
                        if (end != string::npos)
                        {
                            if (verbose)
                            {
                                cerr << "Eliminating incompatible POE option: "
                                     << analyze.substr(start, end + 4 - start) << endl;
                            }
                            analyze.erase(start, end + 4 - start);
                        }
                        else
                        {
                            cerr << "Warning: trace analyzer " << analcmd
                                 << " likely incompatible with POE " << poe_flag << " option!" << endl;
                        }
                    }

                    poe_flag = "-cmdfile ";
                    start    = analyze.find(poe_flag);
                    if (start != string::npos)
                    {
                        end = analyze.find_first_of(' ', start + poe_flag.length() + 1);
                        if (end != string::npos)
                        {
                            if (verbose)
                            {
                                cerr << "Eliminating incompatible POE option: "
                                     << analyze.substr(start, end - start) << endl;
                            }
                            analyze.erase(start, end - start);
                        }
                        else
                        {
                            cerr << "Warning: trace analyzer " << analcmd
                                 << " likely incompatible with POE " << poe_flag << " option!" << endl;
                        }
                    }
                }

                if (preview)
                {
                    cerr << SCAN << "Analyze preview" << endl << analyze << endl;
                }
                else
                {
                    // actual experiment analysis
                    ofstream slog;
                    string   slogfile = epk_archive + "/" + analcmd + ".log";
                    string   serrfile = epk_archive + "/" + analcmd + ".err";
                    slog.open(slogfile.c_str(), ios::out | ios::app);
                    if (!slog.good())
                    {
                        cerr << SCAN << "Abort: failed to open log file " << slogfile << endl;
                        exit(2);
                    }
                    time(&time1);
                    date = ctime(&time1);
                    date.erase(date.length() - 1);
                    cerr << SCAN << date << ": Analyze start" << endl << analyze << endl;
                    slog << SCAN << date << ": Analyze start" << endl << analyze << endl;
                    slog.close();

                    string cobalt_mpirun = "cobalt-mpirun";
                    size_t cobalt_pos    = analyze.find(cobalt_mpirun);
                    string redirection   = string(" --stderr ") + serrfile
                                           + string(" --stdout ") + slogfile;
                    if (cobalt_pos != string::npos)
                    {
                        analyze.insert(cobalt_pos + cobalt_mpirun.length(), redirection);
                        cerr << "[" << analyze << "]" << endl;
                    }
                    else
                    {
                        analyze += string(" 2>>") + serrfile;
                        if (!redirect)
                        {
                            analyze += string(" | tee -a ") + slogfile;
                        }
                        else
                        {
                            #ifdef __FUJITSU
                                string mpiexec     = "mpiexec";
                                size_t mpiexec_pos = analyze.find("mpiexec");
                                analyze.insert(mpiexec_pos + mpiexec.length(), redirection);
                                if (verbose)
                                {
                                    cerr << "[" << analyze << "]" << endl;
                                }
                            #else
                                analyze += string(" 1>>") + slogfile;
                            #endif
                            cerr << "[launch output redirected to " + slogfile + "]" << endl;
                        }
                    }
                    #ifdef _AIX
                        // multithreaded analysis can be very slow with default malloc
                        if (  (analyzer.find("scout.omp") != string::npos)
                           || (analyzer.find("scout.hyb") != string::npos))
                        {
                            if (verbose)
                            {
                                cerr << "Setting MALLOCOPTIONS=multiheap" << endl;
                            }
                            retval = setenv("MALLOCOPTIONS", "multiheap", 1);
                            if (retval)
                            {
                                cerr << "setenv: " << strerror(errno) << endl;
                            }
                        }
                    #endif
                    retval = system(analyze.c_str());
                    if (retval == -1)
                    {
                        cerr << SCAN << "Error: analyze " << strerror(errno) << endl;
                    }
                    status = WEXITSTATUS(retval);

                    time(&time2);
                    date = ctime(&time2);
                    date.erase(date.length() - 1);
                    dtime = difftime(time2, time1);
                    cerr << SCAN << date << ": Analyze done (status=" << status
                         << ") " << dtime << "s" << endl;
                    slog.open(slogfile.c_str(), ios::out | ios::app);
                    if (!slog.good())
                    {
                        cerr << SCAN << "Error: failed to reopen log file " << slogfile << endl;
                    }
                    else
                    {
                        slog << SCAN << date << ": Analyze done (status=" << status
                             << ") " << dtime << "s" << endl;
                        slog.close();
                    }

                    // remove analysis errorfile if empty
                    struct stat sbuf;
                    stat(serrfile.c_str(), &sbuf);    // ignore failure
                    if (sbuf.st_size == 0)
                    {
                        unlink(serrfile.c_str());    // ignore failure
                    }

                    // verify trace analysis report created
                    bool   verified = true;
                    string analfile = epk_archive + "/" + analcmd + ".cubex";
                    retval = stat(analfile.c_str(), &sbuf);
                    if (retval != 0)
                    {
                        cerr << "Error: " << analfile << ": " << strerror(errno) << endl;
                        verified = false;
                    }
                    else if (!(sbuf.st_mode & (S_IRUSR | S_IRGRP)))
                    {
                        // readable
                        cerr << "Error: unreadable " << analfile << endl;
                        verified = false;
                    }
                    else if (sbuf.st_size == 0)
                    {
                        cerr << "Error: empty " << analfile << endl;
                        verified = false;
                    }
                    else if (verbose)
                    {
                        cerr << "Got trace analysis report " << analfile << endl;
                    }

                    if (!verified)
                    {
                        cerr << "Abort: " << epk_archive << " trace analysis failed!" << endl;
                        cerr << serrfile << ":" << endl;
                        string report = "cat " + serrfile;
                        retval = system(report.c_str());
                        status = WEXITSTATUS(retval);
                        if (status)
                        {
                            cerr << report << ": " << strerror(status) << endl;
                        }
                        exit(4);
                    }
                    else
                    {
                        off_t  kbytes = 0;
                        string cmd    = "du -sk " + epk_archive + expt_tracedir;
                        if (verbose)
                        {
                            cerr << "[" << cmd << "]" << endl;
                        }
                        FILE* pipe = popen(cmd.c_str(), "r");
                        if (fscanf(pipe, "%lu", &kbytes) != 1)
                        {
                            cerr << SCAN << "Error: " << cmd << endl << "Failed to parse result value" << endl;
                        }
                        pclose(pipe);

                        ostringstream bytes("");
                        bytes << fixed << setprecision(3);
                        if (kbytes > (1 << 30))
                        {
                            bytes << (float)kbytes / (1 << 30) << "TB";
                        }
                        else if (kbytes > (1 << 20))
                        {
                            bytes << (float)kbytes / (1 << 20) << "GB";
                        }
                        else if (kbytes > 0)
                        {
                            bytes << (float)kbytes / (1 << 10) << "MB";
                        }

                        // if "du" failed or returned zero, ignore size report
                        if (kbytes)
                        {
                            bytes << " of ";
                        }

                        char* clean = getenv("SCAN_CLEAN");
                        if (clean && epk_str2bool(clean))
                        {
                            if (verbose)
                            {
                                cerr << "Got SCAN_CLEAN=" << clean << endl;
                            }
                            cmd = "/bin/rm -rf " + epk_archive + expt_tracedir;
                            if (verbose)
                            {
                                cerr << "[" << cmd << "]" << endl;
                            }
                            retval = system(cmd.c_str());
                            status = WEXITSTATUS(retval);
                            if (status)
                            {
                                cerr << cmd << ": " << strerror(status) << endl;
                            }
                            else
                            {
                                cerr << "Info: " << bytes.str() << "analyzed trace data cleaned from "
                                     << epk_archive << "." << endl;
                            }
                        }
                        else
                        {
                            cerr << "Warning: " << bytes.str() << "analyzed trace data retained in "
                                 << epk_archive << expt_tracedir << "!" << endl;
                        }
                    }
                }
            }

            cerr << SCAN << epk_archive << (preview ? " preview" : "") << " complete." << endl;
        }
    }

    return 0;
}
