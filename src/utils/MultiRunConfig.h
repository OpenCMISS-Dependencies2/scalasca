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
 *  @brief Declaration of the class MultiRunConfig.
 *
 *  This header file provides the declaration of the class MultiRunConfig and
 *  related functions.
 **/
/*-------------------------------------------------------------------------*/


#ifndef SCAN_MULTIRUNCONFIG_H
#define SCAN_MULTIRUNCONFIG_H


#include <map>
#include <string>
#include <utility>
#include <vector>


/*-------------------------------------------------------------------------*/
/**
 *  @class MultiRunConfig
 *  @brief Managing sets of variables for different run configurations.
 *
 **/
/*-------------------------------------------------------------------------*/

class MultiRunConfig
{
    public:
        MultiRunConfig();

        /// @brief Set provided parameter data.
        ///
        /// Sets the number of runs per configuration and verbosity level of
        /// the scan run.
        ///
        /// @param runsPerCfg
        ///     Number of repetitions per configuration
        /// @param verbose
        ///     Chosen Scalasca verbosity level
        ///
        void
        setInfo(unsigned int runsPerCfg,
                unsigned int verbose)
        {
            mVerbose    = verbose;
            mRunsPerCfg = runsPerCfg;
        }

        /// @brief Check environment against a blacklist of variables.
        ///
        /// Checks the global environment against a list of Score-P and Scalasca
        /// environment variables that should not be set in case of a multi-run
        /// experiment with configuartion file or preset.
        ///
        /// @returns
        ///     False if offending variables were found in the environment.
        ///
        bool
        checkEnvBlacklist();

        /// @brief Set run variables per runId.
        ///
        /// Set the variables stored for the runId and unset all other run
        /// variables to avoid interference.
        ///
        /// @param runId
        ///     Id of a stored configuration
        ///
        void
        setRunVariables(int runId);

        /// @brief Load input config file.
        ///
        /// Load a file for the configuration file based multi-run mode
        ///
        /// @param fileName
        ///     Full file name of a configuration file
        ///
        void
        loadConfig(const std::string& fileName);

        /// @brief Choose a multi-run preset.
        ///
        /// Specifies a preset name for the preset based multi-run mode.
        /// It allows using an existing preset configuration in conjunction with
        /// global variables.
        /// In case multiple files of the same name exist in different paths,
        /// the first match is chosen.
        ///
        /// @param preset
        ///     Name of the chosen preset
        ///
        void
        usePreset(const std::string& preset);

        /// @brief List available presets.
        ///
        /// List all available presets found in SCAN_MULTIRUN_PRESET_PATH and
        /// the current Scalasca installation.
        /// Readability and syntax is checked for the found presets.
        ///
        void
        listAvailablePresets();

        /// @brief Save used configuration.
        ///
        /// Save the used configuration to the specified file name.
        /// This is used to store the parsed configuration of a multi-run
        /// measurement inside the archive for reference.
        /// It also generates a configuration file for repeat-only measurements
        /// containing Score-P/Scalasca variables.
        ///
        /// @param fileName Target file name for the configuration
        ///
        void
        saveConfigAs(const std::string& fileName);

        /// @brief Returns the number of configuration sets.
        ///
        /// @returns
        ///     Count of stored configurations
        ///
        int
        getNumSets();

        /// @brief Get title of a configuration.
        ///
        /// Returns the name of a run configuration. Everything after the '-' in
        /// the configuration line is interpreted as a title. Using a title is
        /// optional, resulting in an empty string.
        ///
        /// @param configNum
        ///     Index of the respective configuration
        /// @returns
        ///     Stored title for index i
        ///
        std::string
        getRunConfigTitle(int configNum);

        /// @brief Get the name of the loaded preset.
        ///
        /// Returns the name of the loaded preset. The name is equivalent to the
        /// file name without path or extentsion.
        ///
        /// @returns
        ///     Preset name string
        ///
        std::string
        getPresetName(void);


    private:
        /// The verbosity level of the scan run
        unsigned int mVerbose;

        /// Number of runs for each run configuration
        unsigned int mRunsPerCfg;

        /// Value of the SCAN_MULTIRUN_DEFAULT_CFG environment variable
        std::string mDefaultCfg;
        bool        mDefaultCfgLoaded;

        /// Multi-run via explicit configuration files
        bool mConfigMode;

        /// @name Preset mode data
        /// @{

        /// Multi-run via predefined presets
        bool mPresetMode;

        /// Base name of the preset
        std::string mPreset;

        /// Standard preset file extension
        const std::string mPresetExtension;

        /// Lookup paths for detecting presets
        std::vector< std::string > mPresetPaths;

        /// Found presets
        std::vector< std::string > mAvailablePresets;

        /// Environment variables used relating to Score-P/Scalasca
        std::map< std::string, std::string > mPresetEnvironmentVariables;

        /// @}


        /// Used configuration files
        std::vector< std::string > mInputCfgFiles;

        /// Tracking file origins for documentation purposes
        std::vector< unsigned int > mCfgFileStartCfg;

        /// Variables of the global section
        std::map< std::string, std::string > mGlobalVariables;

        /// Optional title for the global section
        std::string mGlobalSectionTitle;

        /// Variables and titles of each run section
        std::vector< std::map< std::string, std::string > > mPerRunVariables;
        std::vector< std::string >                          mRunConfigTitles;

        /// Variables to be cleared before setting the next run configuration.
        /// Cumulative list of Score-P and Scalasca variables, as well as
        /// variables set in the configuration file.
        std::vector< std::string > mClearList;


        /// @name Data of last parsed configuration file
        /// @{

        unsigned int                                        mLastCfgStartCfg;
        std::string                                         mLastCfgInputCfgFile;
        std::string                                         mLastCfgGlobalSectionTitle;
        std::vector< std::string >                          mLastCfgRunCfgTitles;
        std::map< std::string, std::string >                mLastCfgGlobalVariables;
        std::vector< std::map< std::string, std::string > > mLastCfgPerRunVariables;
        std::vector< std::string >                          mLastCfgGlobalClearList;
        std::vector< std::string >                          mLastCfgRunClearList;

        /// @}


        /// @brief Load a default configuration file.
        ///
        /// SCAN_MULTIRUN_DEFAULT_CFG can be used to specify a default
        /// configuration file. Only the global section of such a file is used.
        ///
        void
        loadDefaultConfig();

        /// @brief Parse a configuration file.
        ///
        /// Parse a provided configuration file and report any errors.
        ///
        /// @param fileName
        ///     File name of the configuration to be checked
        /// @returns
        ///     False if any errors occurred
        ///
        bool
        parseConfig(const std::string& fileName);

        /// @brief Load a specific configuration file.
        ///
        /// Internal method that loads the parsed configuration for
        /// configuration file and preset mode.
        ///
        /// @param fileName
        ///     File name of the configuration to be loaded
        ///
        void
        load(const std::string& fileName);


        /// @name String manipulation and parsing
        /// @{

        /// @brief Clean up a given line.
        ///
        /// This includes removal of leading and trailing spaces.
        ///
        /// @param line
        ///     Input string
        /// @returns
        ///     Cleaned string
        ///
        std::string
        cleanLine(const std::string& line);

        /// @brief Get a key value pair
        ///
        /// Turn a string line into a '=' separated key value pair.
        /// The first occurence of a '=' is taken as separator, the remaining
        /// line is taken as value. The key side must not contain spaces.
        ///
        /// @param line
        ///     Input string
        /// @returns
        ///     Pair of key and value strings
        ///
        std::pair< std::string, std::string >
        getValuePair(const std::string& line);

        /// @brief Recognize a global section of a configuration file
        ///
        /// @param line
        ///     Input string
        /// @returns
        ///     True if section is identified
        ///
        bool
        isGlobalSeparator(const std::string& line);

        /// @brief Recognize a run section of a configuration file
        ///
        /// @param line
        ///     Input string
        /// @returns
        ///     True if section is identified
        ///
        bool
        isRunSeparator(const std::string& line);

        // SCAN output based on verbosity
        /// @brief Error messages
        ///
        /// Generate error/debug output depending on the Scalasca verbosity
        /// level. Allows to force an output anyway.
        ///
        /// @param msg
        ///     Message string
        /// @param force
        ///     Force output, default false
        ///
        void
        message(const std::string& msg,
                bool               force = false);

        /// Error checking during config file parsing
        int mParsingLineNumber;
        int mParsingErrorCount;


        /// @brief Formated parser error output
        ///
        /// @param msg
        ///     Message string
        ///
        void
        parserError(const std::string& msg);

        /// @}
};


#endif    // !SCAN_MULTIRUNCONFIG_H
