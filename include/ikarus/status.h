#pragma once

enum StatusCode {
    StatusCode_Ok = 0,
    StatusCode_InvalidArgument = 100,
    StatusCode_InvalidVersion = 101,
    StatusCode_NotFound = 102,
    StatusCode_Duplicate = 103,
    StatusCode_InternalError = 200,
    StatusCode_Corrupted = 201,
};
