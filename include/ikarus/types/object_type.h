#pragma once

#include <ikarus/macros.h>

IKARUS_BEGIN_HEADER

/// \defgroup object_types ObjectTypes
/// \brief ObjectTypes are used to identify the type of objects.
/// @{

/// \brief The type of a folder.
/// \remark folders have the first bit set and then mirror the object type of the underlying object
enum IkarusFolderType {
    /// \brief Not a folder or no folder.
    IkarusFolderType_None = 0,
    /// \brief An IkarusBlueprintFolder
    IkarusFolderType_BlueprintFolder = 0b1000'0001,
    /// \brief An IkarusPropertyFolder
    IkarusFolderType_PropertyFolder = 0b1000'0010,
    /// \brief An IkarusEntityFolder
    IkarusFolderType_EntityFolder = 0b1000'0011,
};

/// \brief The type of an object.
/// \remark folders have the first bit set and then mirror the object type of the underlying object
enum IkarusObjectType {
    /// \brief Not an object or no object.
    ObjectType_None = 0,
    /// \brief An IkarusBlueprint.
    ObjectType_Blueprint = 0b0000'0001,
    /// \brief An IkarusProperty.
    ObjectType_Property = 0b0000'0010,
    /// \brief An IkarusEntity.
    ObjectType_Entity = 0b0000'0011,
    /// \brief An IkarusBlueprintFolder
    ObjectType_BlueprintFolder = 0b1000'0001,
    /// \brief An IkarusPropertyFolder
    ObjectType_PropertyFolder = 0b1000'0010,
    /// \brief An IkarusEntityFolder
    ObjectType_EntityFolder = 0b1000'0011,
};

/// \brief A bitset of IkarusObjectType%s.
enum ObjectTypes {
    /// \brief No object type.
    ObjectTypes_None = 0,
    /// \brief An IkarusBlueprint.
    ObjectTypes_Blueprint = 1 << ObjectType_Blueprint,
    /// \brief An IkarusProperty.
    ObjectTypes_Property = 1 << ObjectType_Property,
    /// \brief An IkarusEntity.
    ObjectTypes_Entity = 1 << ObjectType_Entity,
    /// \brief An IkarusBlueprintFolder
    ObjectTypes_BlueprintFolder = 1 << ObjectType_BlueprintFolder,
    /// \brief An IkarusPropertyFolder
    ObjectTypes_PropertyFolder = 1 << ObjectType_PropertyFolder,
    /// \brief An IkarusEntityFolder
    ObjectTypes_EntityFolder = 1 << ObjectType_EntityFolder,
};

// @}

IKARUS_END_HEADER