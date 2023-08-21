// IMPLEMENTATION_DETAIL_OBJECT_SCOPES, IMPLEMENTATION_DETAIL_TREE_LAYOUT

/// \file object_scope.h
/// \author Folling <mail@folling.io>

/// \defgroup object_scopes Object Scopes
/// \brief Scopes define where objects belong to.
/// \details They are required to differentiate between different types of objects with NULL as their parent.
/// @{

#pragma once

#include <ikarus/macros.h>
#include <ikarus/types/object.h>

IKARUS_BEGIN_HEADER

/// \brief The global scope of all blueprints.
struct IkarusBlueprintScope {
    /// \private \brief Empty structs aren't allowed in C, so we need a dummy field.
    short _dummy;
};

/// \brief Data for a property scope. This can either be a blueprint or an entity.
union IkarusPropertyScopeData {
    /// \private \brief The blueprint the property is scoped to.
    IkarusBlueprint _blueprint;
    /// \private \brief The entity the property is scoped to.
    IkarusEntity _entity;
};

/// \brief The type of a property scope. This can either be a blueprint or an entity.
enum IkarusPropertyScopeType {
    /// \brief The property is scoped to a blueprint.
    IkarusPropertyScopeType_Blueprint,
    /// \brief The property is scoped to an entity.
    IkarusPropertyScopeType_Entity
};

/// \brief The scope of a property
struct IkarusPropertyScope {
    /// \private \brief Represents the type of the scope.
    IkarusPropertyScopeType _type;
    /// \private \brief Represents the data of the scope.
    IkarusPropertyScopeData _data;
};

/// The global scope of all entities.
struct IkarusEntityScope {
    /// \private \brief Empty structs aren't allowed in C, so we need a dummy field.
    short _dummy;
};

/// \private \brief The data for an object scope.
union IkarusObjectScopeData {
    /// \private \brief The blueprint data of the scope.
    IkarusBlueprintScope _blueprint;
    /// \private \brief The property data of the scope.
    IkarusPropertyScope _property;
    /// \private \brief The entity data of the scope.
    IkarusEntityScope _entity;
};

/// The type of an object scope.
enum IkarusObjectScopeType {
    /// \brief The scope is a blueprint scope.
    IkarusObjectScopeType_Blueprint,
    /// \brief The scope is a property scope.
    IkarusObjectScopeType_Property,
    /// \brief The scope is an entity scope.
    IkarusObjectScopeType_Entity
};

/// \brief The scope of an object.
struct IkarusObjectScope {
    /// \private \brief Represents the type of the scope.
    IkarusObjectScopeType _type;
    /// \private \brief Represents the data of the scope.
    IkarusObjectScopeData _data;
};

/// \brief Creates a blueprint scope.
/// \return The created blueprint scope.
IKA_API IkarusBlueprintScope ikarus_blueprint_scope_create();
/// \brief Converts a blueprint scope to an object scope.
/// \param scope The scope to convert.
/// \return The converted scope.
IKA_API IkarusObjectScope ikarus_blueprint_scope_to_object_scope(IkarusBlueprintScope const * scope);

/// \brief Creates a property scope from a blueprint.
/// \param blueprint The blueprint the property is scoped to.
/// \return The created property scope.
IKA_API IkarusPropertyScope ikarus_property_scope_create_blueprint(IkarusBlueprint const * blueprint);
/// \brief Creates a property scope from a entity.
/// \param entity The entity the property is scoped to.
/// \return The created property scope.
IKA_API IkarusPropertyScope ikarus_property_scope_create_entity(IkarusEntity const * entity);
/// \brief Converts a property scope to an object scope.
/// \param scope The scope to convert.
/// \return The converted scope.
IKA_API IkarusObjectScope ikarus_property_scope_to_object_scope(IkarusPropertyScope const * scope);

/// \brief Fetches the type of an property scope.
/// \param scope The scope to fetch the type of.
/// \return The type of the scope.
IKA_API IkarusPropertyScopeType ikarus_property_scope_get_type(IkarusPropertyScope const * scope);

/// \brief Visits a property scope, calling the appropriate function.
/// \param scope The scope to to visit
/// \param blueprint The function to call if the property is scoped to a blueprint.
/// \param entity The function to call if the property is scoped to an entity.
/// \param data Optional data to pass to the functions.
void ikarus_property_scope_visit(
    IkarusPropertyScope const * scope,
    void (*blueprint)(IkarusBlueprint const *, void *),
    void (*entity)(IkarusEntity const *, void *),
    void * data
);

/// \brief Creates an entity scope.
/// \return The created entity scope.
IKA_API IkarusEntityScope ikarus_entity_scope_create();
/// Converts an entity scope to an object scope.
/// \param scope The scope to convert.
/// \return The converted scope.
IKA_API IkarusObjectScope ikarus_entity_scope_to_object_scope(IkarusEntityScope const * scope);

/// \brief Fetches the type of an object scope.
/// \param scope The scope to fetch the type of.
/// \return The type of the scope.
IKA_API IkarusObjectScopeType ikarus_object_scope_get_type(IkarusObjectScope const * scope);

/// \brief Visits an object scope, calling the appropriate function.
/// \param scope The scope to visit.
/// \param blueprint The function to call if the scope is an #IkarusBlueprintScope.
/// \param property The function to call if the scope is an #IkarusPropertyScope.
/// \param entity The function to call if the scope is an #IkarusEntityScope.
/// \remark function pointers may be null in which case they are not called.
IKA_API void ikarus_object_scope_visit(
    IkarusObjectScope const * scope,
    void (*blueprint)(IkarusBlueprintScope const *, void *),
    void (*property)(IkarusPropertyScope const *, void *),
    void (*entity)(IkarusEntityScope const *, void *),
    void * data
);

/// @}

IKARUS_END_HEADER
