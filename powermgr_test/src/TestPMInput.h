/********************************************************************
 * Copyright (c) 2017 LG Electronics Inc. All Rights Reserved
 *
 * Project Name   : webOS-MTP
 * Group          : CSP2
 * Security       : Confidential
 ********************************************************************/
/********************************************************************
 * @File
 * Filename       : TestMTPInput.h
 * Purpose        : MTP  Test Input Const values
 * Platform       : webOS
 * Author(s)      : CSP2(MTP)
 * Creation Date  :
 * Modifications  :
 * Sl No  Modified by  Date  Version Description
 *
 *
 ********************************************************************/

#ifndef TestMTPInput_H
#define TestMTPInput_H

#define VALID_TRANSACTION_ID  50
#define INVALID_TRANSACTION_ID  -50
#define VALID_CONTAINER_CODE  0x1001
#define VALID_CONTAINER_CODE_OPEN_SESSION  0x1002
#define VALID_CONTAINER_CODE_handleMtpGetObjectHandles  0x1007
#define INVALID_CONTAINER_CODE  -0x1001
#define VALID_STRING  "webOSTestString"
#define NULL_STRING  ""
#define INVALID_CONTAINER_CODE_HEX  0xEFFF
#define INVALID_readParamFromRecivePacket_1  -100
#define INVALID_readParamFromRecivePacket_2  10000
#define Error_Return_Value_From_readParamFromRecivePacket  0
#define MTP_File_Path   "/var/mtp/mtp_test.txt"
#define WRITE_PARAMETER_DATA   12345
#define VALID_HANDLE 0x00000001
#define INVALID_OBJECT_HANDLE 0x2009
#define VALID_ACTION_ADD "add"
#define VALID_ACTION_REMOVE "remove"
#define VALID_ACTION_RENAME "rename"
#define INVALID_ACTION "basha"
#define INVALID_ACTION_EMPTY_STRING ""
#define OBJ_HANDLE_INTERNAL        0x00000001
#define GTEST_HAS_TR1_TUPLE

#endif // TestMTPInput_H
