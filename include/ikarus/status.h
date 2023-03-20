#pragma once

enum StatusCode {
    StatusCode_Ok = 0,
    StatusCode_InvalidArgument = 100,
    StatusCode_NotFound = 101,
    StatusCode_Duplicate = 102,
    StatusCode_InternalError = 200,
    StatusCode_Corrupted = 201,
};
