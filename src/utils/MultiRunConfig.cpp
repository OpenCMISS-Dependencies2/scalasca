/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2018-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


/*-------------------------------------------------------------------------*/
/**
 *  @file
 *  @brief Implementation of the class MultiRunConfig.
 *
 *  This file provides the implementation of the class MultiRunConfig and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "MultiRunConfig.h"

#include <dirent.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

using namespace std;


// A configuration file has two valid states: inside a singular GLOBAL variable
// section or in a RUN variable section. All other entries are ignored.
enum CfgStates
{
    UNDEFINED,
    GLOBAL,
    RUN
};


bool
hasSuffix(const string& value,
          const string& suffix)
{
    if (value.length() < suffix.length())
    {
        return false;
    }

    const string::size_type startPos = value.length() - suffix.length();

    return (value.compare(startPos, suffix.length(), suffix) == 0);
}


MultiRunConfig::MultiRunConfig()
    : mPresetExtension(".preset")
{
    // Separation between multi-run measurements with configuration file
    // and those based on a preset.
    mConfigMode = false;
    mPresetMode = false;

    mPreset = "";

    // Get directories containing presets from SCAN_MULTIRUN_PRESET_PATH
    char* envPresetPath = getenv("SCAN_MULTIRUN_PRESET_PATH");

    istringstream presetPaths(envPresetPath ? envPresetPath : "");
    string        path;
    while (getline(presetPaths, path, ':'))
    {
        DIR* dir;
        if ((dir = opendir(path.c_str())) != NULL)
        {
            mPresetPaths.push_back(path);
            closedir(dir);
        }
    }

    // add the share/scalasca path of the used installation to the path list
    mPresetPaths.push_back(string(PRESETDIR));

    // Get potential default config
    char* envDefaultCfg = getenv("SCAN_MULTIRUN_DEFAULT_CFG");
    mDefaultCfg       = envDefaultCfg ? envDefaultCfg : "";
    mDefaultCfgLoaded = false;

    mRunsPerCfg = 1;
}


bool
MultiRunConfig::checkEnvBlacklist()
{
    string blacklist = "";

    if (!mConfigMode && !mPresetMode && (mRunsPerCfg <= 1))
    {
        // No need to check if there is no multi-run configuration, preset or additional runs
        return "";
    }

    vector< std::string > referenceVarList;
    if (mConfigMode || mPresetMode)
    {
        string cmd = "scorep-info --help >/dev/null 2>&1";
        int    ret = system(cmd.c_str());
        if (ret != 0)
        {
            message("Error: 'scorep-info' not available!\n"
                    "This is required for checking the environment in multi-run measurements with config files!",
                    true);
            exit(1);
        }

        // Check list of variables that should not be set in the global environment.
        // Instead use global section of run configuration file for consistency.

        // All scorep variables
        cmd = "scorep-info config-vars --values | sed 's/=.*$//g'";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe)
        {
            message("Error: Could not run:" + cmd, true);
            exit(1);
        }

        char confVar[1024];
        while (fgets(confVar, 1024, pipe))
        {
            confVar[strlen(confVar) - 1] = '\0';
            referenceVarList.push_back(string(confVar));
        }
        pclose(pipe);

        // Scalasca variables from a hard-coded list:
        string scalList[9] = {
            "SCAN_ANALYZE_OPTS",
            "SCAN_CLEAN",
            "SCAN_MPI_LAUNCHER",
            "SCAN_MPI_RANKS",
            "SCAN_OVERWRITE",
            "SCAN_SETENV",
            "SCAN_TARGET",
            "SCAN_TRACE_ANALYZER",
            "SCAN_WAIT"
        };

        referenceVarList.insert(referenceVarList.end(), scalList, scalList + 9);
    }

    // Config mode uses all Score-P and Scalasca variables.
    if (mConfigMode)
    {
        for (int i = 0; i < referenceVarList.size(); i++)
        {
            mClearList.push_back(referenceVarList[i]);
            char* envVal = getenv(referenceVarList[i].c_str());
            if (envVal && (string(envVal).length() > 0))
            {
                blacklist += "\n    " + referenceVarList[i] + "=" + string(envVal);
            }
        }

        if (!blacklist.empty())
        {
            message("Score-P or Scalasca variables are set in the global environment for a\n"
                    "multi-run experiment with configuration file! Please use the global\n"
                    "variables section of the configuration file for these variables!\n"
                    "Offending variables:" + blacklist,
                    true);

            return false;
        }
    }

    // Preset mode blacklists only variables from the used preset;
    // other variables from the reference list will be kept as reference for the
    // generated configuration file.
    if (mPresetMode)
    {
        for (int i = 0; i < mClearList.size(); i++)
        {
            char* envVal = getenv(mClearList[i].c_str());
            if (envVal && (string(envVal).length() > 0))
            {
                blacklist += "\n    " + mClearList[i] + "=" + string(envVal);
            }
        }

        for (int i = 0; i < referenceVarList.size(); i++)
        {
            char* envVal = getenv(referenceVarList[i].c_str());
            if (envVal && (string(envVal).length() > 0))
            {
                mPresetEnvironmentVariables[referenceVarList[i]] = string(envVal);
            }
        }

        if (!blacklist.empty())
        {
            message("Score-P or Scalasca variables are set in the global environment for a multi run\n"
                    "experiment with the preset '"
                    + mPreset
                    + "'.\n"
                      "Variables used in the preset config cannot be used in the global environment!\n"
                      "Offending variables:"
                    + blacklist,
                    true);

            return false;
        }
    }

    // Repeats only and pass in config and preset mode.
    return true;
}


void
MultiRunConfig::setRunVariables(int runId)
{
    if (!mConfigMode && !mPresetMode)
    {
        return;
    }

    if (mPerRunVariables.size() > 0)
    {
        // Unset all variables used in run configurations.
        // The clean list contains Score-P and Scalasca related variables as well as all
        // variables from all run configurations.
        // If either of these are intended to be used, they have to be set explicitly
        // either in the global section or in respective run configurations
        // Only reset environment with config file, pure repetitions keep their environment.
        for (int i = 0; i < mClearList.size(); i++)
        {
            unsetenv(mClearList[i].c_str());
        }

        // Set variables from the global section, can be overwritten in the per run settings except for special cases.
        for (map< string, string >::iterator it = mGlobalVariables.begin();
             it != mGlobalVariables.end();
             ++it)
        {
            message("Set global variable: " + it->first + " to '" + it->second + "'");
            setenv(it->first.c_str(), it->second.c_str(), 1);
        }

        // Set variables for this run.
        if (runId < mPerRunVariables.size())
        {
            stringstream sstm;
            sstm << runId;
            message("Run no: " + sstm.str());
            for (map< string, string >::iterator it =
                     mPerRunVariables[runId].begin();
                 it != mPerRunVariables[runId].end();
                 ++it)
            {
                message("Set var: " + it->first + " to " + it->second);
                setenv(it->first.c_str(), it->second.c_str(), 1);
            }
        }
    }
}


bool
MultiRunConfig::parseConfig(const string& fileName)
{
    string   line;
    ifstream inputFile(fileName.c_str());

    if (!inputFile)
    {
        message("Error: Opening file " + fileName, true);
        exit(1);
    }

    // Clear previous data from last parsed config.
    mLastCfgGlobalSectionTitle = "";
    mLastCfgRunCfgTitles.clear();
    mLastCfgGlobalVariables.clear();
    mLastCfgPerRunVariables.clear();
    mLastCfgGlobalClearList.clear();
    mLastCfgRunClearList.clear();

    // Remember the start config position for documentation purposes.
    mLastCfgStartCfg = mPerRunVariables.size();

    // Store the source file name for later documentation.
    mLastCfgInputCfgFile = fileName;

    string    globalSectionTitle  = "";
    CfgStates cfgState            = UNDEFINED;
    bool      beenInRunSection    = false;
    bool      beenInGlobalSection = false;
    mParsingLineNumber = 0;
    mParsingErrorCount = 0;
    while (getline(inputFile, line))
    {
        mParsingLineNumber++;

        // Remove unwanted clutter from line.
        string cleanedLine = cleanLine(line);

        // Skip if is empty or comment.
        if ((cleanedLine.size() == 0) || (line[0] == '#'))
        {
            continue;
        }

        // Order of separator checks relevant as they may contain similar checks
        if (isGlobalSeparator(cleanedLine))
        {
            if (  (beenInRunSection && !beenInGlobalSection)
               || beenInGlobalSection)
            {
                // Global has to be the first section if it exists at all
                // and only one global section for consistency.
                parserError(
                    "Either multiple global sections or run sections before the global section!");
            }
            beenInGlobalSection = true;
            cfgState            = GLOBAL;

            mLastCfgGlobalSectionTitle = cleanedLine.substr(2, cleanedLine.length() - 2);
            continue;
        }

        if (isRunSeparator(cleanedLine))
        {
            beenInRunSection = true;
            cfgState         = RUN;
            mLastCfgRunCfgTitles.push_back(cleanedLine.substr(1, cleanedLine.length() - 1));
            mLastCfgPerRunVariables.resize(mLastCfgPerRunVariables.size() + 1);
            continue;
        }

        // Deal with lines containing envrionment variables.
        if (cfgState != UNDEFINED)
        {
            pair< string, string > valuepair = getValuePair(cleanedLine);
            if (cfgState == GLOBAL)
            {
                mLastCfgGlobalClearList.push_back(valuepair.first);
                mLastCfgGlobalVariables[valuepair.first] = valuepair.second;
            }
            else if (cfgState == RUN)
            {
                mLastCfgRunClearList.push_back(valuepair.first);
                mLastCfgPerRunVariables[mLastCfgPerRunVariables.size() - 1][valuepair.first] =
                    valuepair.second;
            }
        }
        else
        {
            // UNDEFINED state
            parserError(
                "Variable statements are not allowed outside global or run sections, '--' or '-' have to come first!");
        }
    }

    if (!beenInRunSection && !beenInGlobalSection)
    {
        parserError(
            "Neither run nor global section defined, there has to be at least one valid section!");
    }

    // Check if the parsing resulted in any errors.
    if (mParsingErrorCount)
    {
        cerr << endl << "The configuration file parsing of '" << fileName
             << "' encountered " << mParsingErrorCount << " errors." << endl
             << endl;

        return false;
    }

    return true;
}


void
MultiRunConfig::load(const string& fileName)
{
    if (!parseConfig(fileName))
    {
        message("Error: Parsing file " + fileName, true);
        exit(1);
    }

    // Add last successfully parsed configuration file.
    mInputCfgFiles.push_back(mLastCfgInputCfgFile);
    mCfgFileStartCfg.push_back(mLastCfgStartCfg);

    if (mLastCfgGlobalSectionTitle.length() >= 2)
    {
        mGlobalSectionTitle += (mGlobalSectionTitle.empty() ? "" : "; ")
                               + mLastCfgGlobalSectionTitle;
    }
    mRunConfigTitles.insert(mRunConfigTitles.end(),
                            mLastCfgRunCfgTitles.begin(),
                            mLastCfgRunCfgTitles.end());

    // Map global variables (insert doesn't work because duplicates aren't overwritten).
    for (map< string, string >::iterator it = mLastCfgGlobalVariables.begin();
         it != mLastCfgGlobalVariables.end();
         ++it)
    {
        mGlobalVariables[it->first] = it->second;
    }

    // Insert vector of run variable maps.
    mPerRunVariables.insert(mPerRunVariables.end(),
                            mLastCfgPerRunVariables.begin(),
                            mLastCfgPerRunVariables.end());
    mClearList.insert(mClearList.end(),
                      mLastCfgGlobalClearList.begin(),
                      mLastCfgGlobalClearList.end());
    mClearList.insert(mClearList.end(),
                      mLastCfgRunClearList.begin(),
                      mLastCfgRunClearList.end());
}


void
MultiRunConfig::loadDefaultConfig()
{
    if ((mDefaultCfg == "") || mDefaultCfgLoaded)
    {
        // Either none specified or already loaded
        return;
    }
    mDefaultCfgLoaded = true;

    if (!parseConfig(mDefaultCfg))
    {
        message("Error: Parsing default config file " + mDefaultCfg, true);
        exit(1);
    }

    // Add global elements from last successfully parsed configuration.
    // We don't want to add run configurations from a default file.
    mInputCfgFiles.push_back(mLastCfgInputCfgFile);
    mCfgFileStartCfg.push_back(mLastCfgStartCfg);

    if (mLastCfgGlobalSectionTitle.length() >= 2)
    {
        mGlobalSectionTitle += (mGlobalSectionTitle.empty() ? "" : "; ")
                               + mLastCfgGlobalSectionTitle;
    }

    // Map global variables (insert doesn't work because duplicates aren't overwritten).
    for (map< string, string >::iterator it = mLastCfgGlobalVariables.begin();
         it != mLastCfgGlobalVariables.end();
         ++it)
    {
        mGlobalVariables[it->first] = it->second;
    }
    mClearList.insert(mClearList.end(),
                      mLastCfgGlobalClearList.begin(),
                      mLastCfgGlobalClearList.end());
}


void
MultiRunConfig::loadConfig(const string& fileName)
{
    if (mPresetMode)
    {
        message("Error: Already using preset mode with '" + mPreset
                + "'.\nThe options '-P' and '-M' cannot be used together!",
                true);
        exit(1);
    }

    // If configurations are loaded (mPresetMode or mConfigMode) and a default configuration
    // is given, try loading this configuration (once) before proceeding.
    // This way, the defaults can be overwritten by any following configurations.

    loadDefaultConfig();

    mConfigMode = true;
    load(fileName);
}


void
MultiRunConfig::usePreset(const std::string& preset)
{
    if (mConfigMode)
    {
        message("Error: Already using config mode with '"
                + std::accumulate(mInputCfgFiles.begin(), mInputCfgFiles.end(),
                                  std::string(", "))
                + "'.\nThe options '-P' and '-M' cannot be used together!",
                true);
        exit(1);
    }


    // Iterate over the preset paths and find the first good file name.
    string presetFileName = "";
    for (int i = 0; i < mPresetPaths.size(); i++)
    {
        string tempFileName = mPresetPaths[i] + "/" + preset + mPresetExtension;
        cerr << "path:>" << tempFileName << "< \n";

        ifstream tempFile(tempFileName.c_str());
        if (tempFile.good())
        {
            // First existing file
            presetFileName = tempFileName;
            break;
        }
    }

    if (presetFileName.empty())
    {
        message("Abort: No preset file '"
                + preset + mPresetExtension
                + "' found!",
                true);
        exit(1);
    }

    // If configurations are loaded (mPresetMode or mConfigMode) and a default configuration
    // is given, try loading this configuration (once) before proceeding.
    // This way, the defaults can be overwritten by any following configurations.

    loadDefaultConfig();

    mPreset     = preset;
    mPresetMode = true;
    load(presetFileName);
}


void
MultiRunConfig::listAvailablePresets(void)
{
    cerr << "Available presets for multi-run measurements:" << endl;

    for (int i = 0; i < mPresetPaths.size(); i++)
    {
        DIR*           dir;
        struct dirent* ent;
        if ((dir = opendir(mPresetPaths[i].c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                string tempFileName = ent->d_name;
                if (hasSuffix(tempFileName, mPresetExtension))
                {
                    // Matches the naming scheme/suffix
                    string tempFullPath = mPresetPaths[i] + "/"
                                          + tempFileName;
                    ifstream tempFile(tempFullPath.c_str());
                    if (tempFile.good())
                    {
                        // Is readable
                        string line;
                        if (getline(tempFile, line))
                        {
                            // Is not empty
                            const string tempFileBasename = tempFileName.substr(0, tempFileName.length() - mPresetExtension.length());
                            cerr << "  " << line << endl
                                 << " '" << tempFileBasename
                                 << "' " << tempFullPath << "\n" << endl;

                            // Test validity without adding the parsed data and
                            // without program exit in case of errors.
                            parseConfig(tempFullPath);
                        }
                    }
                }
            }
            closedir(dir);
        }
    }
}


void
MultiRunConfig::saveConfigAs(const string& fileName)
{
    if (!mConfigMode && !mPresetMode && (mRunsPerCfg <= 1))
    {
        return;
    }

    ofstream outputFile(fileName.c_str());

    if (!outputFile)
    {
        cerr << "Error opening file " << fileName << endl;
        exit(2);
    }


    if (!mConfigMode && !mPresetMode && (mRunsPerCfg > 1))
    {
        // Just repetitions of the default environment;
        // Dump known and set Score-P and Scalasca variables as global section
        // of a configuration file.
        outputFile
            <<
            "# Generated multiple run configuration for a case with repeated runs only"
            << endl;
        outputFile
            <<
            "# Contains only known variables, e.g., Score-P/Scalasca variables."
            << endl;
        outputFile << "-- Global Variable Section" << endl;

        for (int i = 0; i < mClearList.size(); i++)
        {
            if (mClearList[i] != "SCOREP_EXPERIMENT_DIRECTORY")
            {
                // Directory will be set automatically and differs from run to run.
                if (getenv(mClearList[i].c_str()))
                {
                    outputFile << mClearList[i] << "=" << getenv(
                        mClearList[i].c_str()) << endl;
                }
            }
        }

        outputFile.close();

        return;
    }

    // If a configuration file was used.

    outputFile << "# Stored multiple run configuration" << endl;
    if (mPresetMode)
    {
        outputFile << "# Configuration for a '" << mPreset << "' preset run"
                   << endl;
    }

    outputFile << "# Used input config files:" << endl;
    for (int i = 0; i < mInputCfgFiles.size(); i++)
    {
        outputFile << "#   " << mInputCfgFiles[i] << endl;
    }

    outputFile << endl << "# Global Variables:\n--"
               << (mGlobalSectionTitle.empty() ? ":" : mGlobalSectionTitle)
               << endl;
    if (mCfgFileStartCfg.size() > 1)
    {
        outputFile
            <<
            "# If combined from multiple files, the last setting of a variable is valid."
            << endl;
    }

    if (mPresetMode && !mPresetEnvironmentVariables.empty())
    {
        outputFile
            << "# Score-P/Scalasca variables set in the global environment:"
            << endl;
        for (map< string, string >::iterator it =
                 mPresetEnvironmentVariables.begin();
             it != mPresetEnvironmentVariables.end();
             ++it)
        {
            outputFile << it->first.c_str() << "=" << it->second.c_str()
                       << endl;
        }
        outputFile << "\n# Variables from the preset '" << mPreset << "':"
                   << endl;
    }

    for (map< string, string >::iterator it = mGlobalVariables.begin();
         it != mGlobalVariables.end();
         ++it)
    {
        outputFile << it->first.c_str() << "=" << it->second.c_str() << endl;
    }

    int fileCounter = 0;
    for (int i = 0; i < mPerRunVariables.size(); i++)
    {
        if (  (mCfgFileStartCfg.size() > 1)
           && (mCfgFileStartCfg[fileCounter] == i))
        {
            outputFile << endl << "# Imported from config file: "
                       << mInputCfgFiles[fileCounter] << endl;
            fileCounter++;
        }
        outputFile << endl << "# Run Config " << i + 1 << "\n-"
                   << mRunConfigTitles[i] << endl;
        for (map< string, string >::iterator it =
                 mPerRunVariables[i].begin();
             it != mPerRunVariables[i].end();
             ++it)
        {
            outputFile << it->first.c_str() << "=" << it->second.c_str()
                       << endl;
        }
    }
    outputFile.close();
}


std::string
MultiRunConfig::getRunConfigTitle(int i)
{
    return mRunConfigTitles[i];
}


std::string
MultiRunConfig::getPresetName(void)
{
    return mPreset;
}


int
MultiRunConfig::getNumSets()
{
    if (mPerRunVariables.size() == 0)
    {
        // If no set is explicitly stored, there still will be an implicit default run.
        return 1;
    }

    return mPerRunVariables.size();
}


// String manipulation and parsing methods

string
MultiRunConfig::cleanLine(const string& line)
{
    string clean  = line;
    string remove = " \t";


    clean.erase(0, clean.find_first_not_of(remove.c_str()));
    clean.erase(clean.find_last_not_of(remove.c_str()) + 1);

    return clean;
}


pair< string, string >
MultiRunConfig::getValuePair(const string& line)
{
    pair< string, string > keyValue;
    size_t                 separatorPos = line.find('=');

    if (separatorPos == string::npos)
    {
        parserError("Key value pair '" + line + "' missing '=' separator!");

        return keyValue;
    }
    keyValue.first = line.substr(0, separatorPos);

    if (keyValue.first.find(' ') != string::npos)
    {
        parserError(
            "Key '" + line
            + "' contains spaces, invalid environment variable!");

        return keyValue;
    }
    if (separatorPos == line.length() - 1)
    {
        // Empty value, no need to substr the value.
        return keyValue;
    }

    string value = line.substr(separatorPos + 1, line.length() - (separatorPos + 1));

    // Remove potential enclosing quotes, as the used 'setenv' does not remove them
    // like an 'export' in the shell.
    if (  (value.length() >= 2)
       && (  ((value[0] == '\'') && (value[value.length() - 1] == '\''))
          || ((value[0] == '"') && (value[value.length() - 1] == '"'))))
    {
        value = value.substr(1, value.length() - 2);
    }
    keyValue.second = value;

    return keyValue;
}


bool
MultiRunConfig::isGlobalSeparator(const string& line)
{
    if (line.size() > 1)
    {
        if ((line[0] == '-') && (line[1] == '-'))
        {
            return true;
        }
    }

    return false;
}


bool
MultiRunConfig::isRunSeparator(const string& line)
{
    if (line.size() > 0)
    {
        if (line[0] == '-')
        {
            return true;
        }
    }

    return false;
}


void
MultiRunConfig::message(const string& msg,
                        bool          force)
{
    if (mVerbose || force)
    {
        cerr << msg << endl;
    }
}


void
MultiRunConfig::parserError(const string& msg)
{
    if (mParsingErrorCount == 0)
    {
        cerr << endl << "Parsing configuration file '" << mLastCfgInputCfgFile
             << "':" << endl;
    }
    cerr << "Error line " << setw(3) << mParsingLineNumber << ": " << msg
         << endl;
    mParsingErrorCount++;
}
