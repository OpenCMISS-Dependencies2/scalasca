#!/usr/bin/env python

import unittest
import subprocess
import _otf2

exceptions_full = [
    "AnchorFile_Load",
    "AnchorFile_Save",
    "Buffer_ClearRewindPoint",
    "Buffer_Delete",
    "Buffer_FlushBuffer",
    "Buffer_GetBeginOfChunk",
    "Buffer_GetNumberEvents",
    "Buffer_GetPosition",
    "Buffer_GetPositionTimeStamp",
    "Buffer_Guarantee",
    "Buffer_GuaranteeCompressed",
    "Buffer_GuaranteeRecord",
    "Buffer_New",
    "Buffer_ReadDouble",
    "Buffer_ReadFloat",
    "Buffer_ReadGetNextChunk",
    "Buffer_ReadGetPreviousChunk",
    "Buffer_ReadInt16",
    "Buffer_ReadInt32",
    "Buffer_ReadInt32Full",
    "Buffer_ReadInt64",
    "Buffer_ReadInt64Full",
    "Buffer_ReadInt8",
    "Buffer_ReadMetricValue",
    "Buffer_ReadSeekChunk",
    "Buffer_ReadSeekChunkTime",
    "Buffer_ReadString",
    "Buffer_ReadTimeStamp",
    "Buffer_ReadUint16",
    "Buffer_ReadUint32",
    "Buffer_ReadUint32Full",
    "Buffer_ReadUint64",
    "Buffer_ReadUint64Full",
    "Buffer_ReadUint8",
    "Buffer_RequestNewChunk",
    "Buffer_Rewind",
    "Buffer_RewriteTimeStamp",
    "Buffer_SetLocationID",
    "Buffer_SetPosition",
    "Buffer_SetPositionTimeStamp",
    "Buffer_Skip",
    "Buffer_SkipCompressed",
    "Buffer_StoreRewindPoint",
    "Buffer_SwitchMode",
    "Compression_Zlib_Available",
    "Compression_Zlib_Compress",
    "Compression_Zlib_Decompress",
    "Error_RegisterCallback",
    "File_CreateDirectory",
    "File_GetSizeUnchunked",
    "File_Read",
    "File_Reset",
    "File_SeekChunk",
    "File_SeekPrevChunk",
    "File_Write",
    "UTILS_CStr_dup",
    "UTILS_CStr_find",
    "UTILS_CStr_npos",
    "UTILS_Debug_Prefix",
    "UTILS_Debug_Printf",
    "UTILS_Debug_RawPrintf",
    "UTILS_DoesFileExist",
    "UTILS_Error_Abort",
    "UTILS_Error_FromPosix",
    "UTILS_Error_Handler",
    "UTILS_GetExecutablePath",
    "UTILS_IO_GetCwd",
    "UTILS_IO_GetHostname",
    "UTILS_IO_GetLine",
    "UTILS_IO_GetWithoutPath",
    "UTILS_IO_HasPath",
    "UTILS_IO_JoinPath",
    "UTILS_IO_SimplifyPath"
]

exceptions_prefix = [
    "AnchorFile_",
    "Buffer_",
    "Compression_Zlib_",
    "Error_RegisterCallback",
    "File_",
    "UTILS_",
]


class TestSymbolsComplete(unittest.TestCase):
    def setUp(self):
        libpath = _otf2.Config.lib_path()
        r = subprocess.check_output(['nm', '-g', '--defined-only', libpath])
        out = r.decode("ASCII")
        lines = out.split('\n')

        self.lib_names = set()
        for line in lines:
            if not line:
                continue
            symbol = line.split(' ')[2]
            if not symbol.startswith("OTF2_"):
                continue
            name = symbol[5:]
            self.lib_names.add(name)

        self.module_names = dir(_otf2)

    def test_module_complete(self):
        found = 0
        for name in self.lib_names:
            if name in self.module_names:
                # Success
                continue

            if any(name.startswith(ex_pre) for ex_pre in exceptions_prefix):
                # Exception
                continue

            found += 1
            print("Symbol not defined in python _otf module: {}".format(name))

        self.assertFalse(found, msg="At least one missing python symbol.")

    # This doesn't work because of defines
    # def test_module_bounded(self):
    #     found = 0
    #     for name in self.module_names:
    #         if name in self.lib_names:
    #             # Success
    #             continue
    #
    #         found += 1
    #         print("Python symbol not from lib: {}".format(name))
    #
    #     self.assertFalse(found, msg="At least one python module not from libotf.so.")

if __name__ == '__main__':
    unittest.main()
