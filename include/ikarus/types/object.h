#pragma once

// IMPLEMENTATION_DETAIL_OBJECT_TYPES
// IMPLEMENTATION_DETAIL_LAZY_VALUE_CREATION
// IMPLEMENTATION_DETAIL_PROPERTY_TYPES

/// \file id.h
/// \author Folling <mail@folling.io>

/// \defgroup object Objects
/// \brief Objects are a compound type of all types of objects in the database.
/// \details The following objects currently exist:
/// - blueprints
/// - properties
/// - entities
/// - blueprint folders
/// - property folders
/// - entity folders
/// @{

#include <ikarus/types/id.h>

IKARUS_BEGIN_HEADER

/// \brief A blueprint object.
/// \details  A blueprint is a collection of properties which can be linked to entities.
/// Each entity the blueprint is linked to will have values for the blueprints properties.
struct IkarusBlueprint {
    IkarusId id;
};

/// \brief Properties are the placeholders of values for entities.
/// \details Each entity can have any number of properties.
/// Every property has a type that identifies the kind of data that can be put in.
///
/// The following types currently exist:
/// - Toggle: A true/false boolean-like value
/// - Number: An arbitrary numeric value
/// - Text: An arbitrary textual value
///
/// Property Examples:
/// - Is Dead (Toggle)
/// - Age (Number)
/// - ISBN (Text)
///
/// Every property has settings which can be used to customise the property further.
/// Two settings that are shared among all properties are the following:
/// - Multiple
/// - Allow undefined
///
/// The former transforms a property into a list. Instead of one number, you could then specify a series of numbers.
/// The latter allows you to specify an "unknown" value for a property.
/// It might not be known if a character is dead or not for example.
///
/// Each entity associated with the property has a value for it.
///
/// Properties can also be added to blueprints in which case they are available for all entities associated with the
/// blueprint.
///
/// We call properties within entities "Entity Properties" and properties within blueprints "Blueprint Properties".
///
/// \remark Values for properties are lazily created as space saving measure.
/// Fetching the value for some property of some entity will return the property's default value if none is specified.
/// This default value is specified when the property is created and can be updated later.
///
/// \remark Properties' tree structures are scoped to the entity or blueprint they are associated with.
struct IkarusProperty {
    /// \private \brief The ID of the property.
    IkarusId id;
};

/// \brief Entities are the core building blocks of Ikarus.
/// \detials Blueprints and Properties define the structure of the data.
/// Entities define the data itself.
///
/// Properties can be associated with Entities in two ways:
/// - Directly: The property is linked to the entity.
/// - Indirectly: The property is linked to a blueprint of the entity.
///
/// For each property an entity is linked to, it has a value. These values depend on the property's type.
/// For more information on the types see the property documentation.
///
/// Values are the core type of data within Ikarus.
/// Each value is associated with one page and one property.
///
/// \remark Values are typed, the type of a value is specified by its associated property.
/// For more information on the types see the property documentation.
///
/// \remark Values are guaranteed to be in valid format for a given type
/// but not guaranteed to be valid under the settings of the property.
/// This is because changing the settings can invalidate existing values without resetting them.
struct IkarusEntity {
    /// \private \brief The ID of the entity.
    IkarusId id;
};

/// \brief A blueprint folder.
/// \see Folder
struct IkarusBlueprintFolder {
    /// \private \brief The ID of the folder.
    IkarusId id;
};

/// \brief A property folder.
/// \remark Property folders are scoped to the blueprint or entity they are associated with.
/// \see Folder
struct IkarusPropertyFolder {
    /// \private \brief The ID of the folder.
    IkarusId id;
};

/// \brief An entity folder.
/// \see Folder
struct IkarusEntityFolder {
    /// \private \brief The ID of the folder.
    IkarusId id;
};

/// \private \brief The data of a folder.
union IkarusFolderData {
    /// \private \brief The blueprint folder data of the folder.
    IkarusBlueprintFolder blueprint_folder;
    /// \private \brief The property folder data of the folder.
    IkarusPropertyFolder property_folder;
    /// \private \brief The entity folder data of the folder.
    IkarusEntityFolder entity_folder;
};

/// \brief A generic folder. Similar to how Objects wrap all types of objects, Folders wrap all types of folders.
struct IkarusFolder {
    /// \private \brief The data of the folder.
    IkarusFolderData data;

    /// \private \brief The type of the folder.
    IkarusFolderType type;
};

/// \private \brief The data of an object.
union IkarusObjectData {
    /// \private \brief The blueprint data of the object.
    IkarusBlueprint blueprint;
    /// \private \brief The property data of the object.
    IkarusProperty property;
    /// \private \brief The entity data of the object.
    IkarusEntity entity;
    /// \private \brief The blueprint folder data of the object.
    IkarusBlueprintFolder blueprint_folder;
    /// \private \brief The property folder data of the object.
    IkarusPropertyFolder property_folder;
    /// \private \brief The entity folder data of the object.
    IkarusEntityFolder entity_folder;
};

/// \brief A generic object. Wraps all types of objects, including folders.
struct IkarusObject {
    /// \private \brief The data of the object.
    IkarusObjectData data;
    /// \private \brief The type of the object.
    IkarusObjectType type;
};

// @}

IKARUS_END_HEADER
