#pragma once

enum ErrorCode {
    ErrorCode_NullSubject = 1,
    ErrorCode_NullSecondarySubject = 2,
    ErrorCode_NullTertiarySubject = 3,
    ErrorCode_NullArgument = 4,
    ErrorCode_NullOutBuffer = 5,
    ErrorCode_InvalidPath = 6,
    ErrorCode_InvalidName = 7,
    ErrorCode_InvalidEntity = 8,
    ErrorCode_AlreadyExists = 9,
    ErrorCode_DatabaseError = 10,
    ErrorCode_InvalidDatabase = 11,
    ErrorCode_InvalidDatabaseVersion = 12,
    ErrorCode_MigrationFailed = 13,
    ErrorCode_FilesystemError = 14
};
