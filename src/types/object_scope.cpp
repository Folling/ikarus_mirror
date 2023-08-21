#include "ikarus/types/object_scope.h"

#include <utility>

#include <catch2/catch_test_macros.hpp>

IkarusBlueprintScope ikarus_blueprint_scope_create() {
    return IkarusBlueprintScope{._dummy = 0};
}

IkarusObjectScope ikarus_blueprint_scope_to_object_scope(IkarusBlueprintScope const * scope) {
    IkarusObjectScopeData data{};
    data._blueprint = *scope;

    return IkarusObjectScope{._type = IkarusObjectScopeType_Blueprint, ._data = data};
}

IkarusPropertyScope ikarus_property_scope_create_blueprint(IkarusBlueprint const * blueprint) {
    IkarusPropertyScopeData data{};
    data._blueprint = *blueprint;
    return IkarusPropertyScope{._type = IkarusPropertyScopeType_Blueprint, ._data = data};
}

IkarusPropertyScope ikarus_property_scope_create_entity(IkarusEntity const * entity) {
    IkarusPropertyScopeData data{};
    data._entity = *entity;
    return IkarusPropertyScope{._type = IkarusPropertyScopeType_Entity, ._data = data};
}

IkarusObjectScope ikarus_property_scope_to_object_scope(IkarusPropertyScope const * scope) {
    IkarusObjectScopeData data{};
    data._property = *scope;

    return IkarusObjectScope{._type = IkarusObjectScopeType_Property, ._data = data};
}

IkarusPropertyScopeType ikarus_property_scope_get_type(IkarusPropertyScope const * scope) {
    return scope->_type;
}

void ikarus_property_scope_visit(
    IkarusPropertyScope const * scope,
    void (*blueprint)(IkarusBlueprint const *, void *),
    void (*entity)(IkarusEntity const *, void *),
    void * data
) {
    switch (scope->_type) {
    case IkarusPropertyScopeType_Blueprint: blueprint(&scope->_data._blueprint, data); break;
    case IkarusPropertyScopeType_Entity: entity(&scope->_data._entity, data); break;
    }
}

IkarusEntityScope ikarus_entity_scope_create() {
    return IkarusEntityScope{._dummy = 0};
}

IkarusObjectScope ikarus_entity_scope_to_object_scope(IkarusEntityScope const * scope) {
    IkarusObjectScopeData data{};
    data._entity = *scope;

    return IkarusObjectScope{._type = IkarusObjectScopeType_Entity, ._data = data};
}

IkarusObjectScopeType ikarus_object_scope_get_type(IkarusObjectScope const * scope) {
    return scope->_type;
}

void ikarus_object_scope_visit(
    IkarusObjectScope const * scope,
    void (*blueprint)(IkarusBlueprintScope const *, void *),
    void (*property)(IkarusPropertyScope const *, void *),
    void (*entity)(IkarusEntityScope const *, void *),
    void * data
) {
    switch (scope->_type) {
    case IkarusObjectScopeType_Blueprint: {
        if (blueprint != nullptr) {
            blueprint(&scope->_data._blueprint, data);
        }
        break;
    }
    case IkarusObjectScopeType_Property: {
        if (property != nullptr) {
            property(&scope->_data._property, data);
        }
        break;
    }
    case IkarusObjectScopeType_Entity: {
        if (entity != nullptr) {
            entity(&scope->_data._entity, data);
        }
        break;
    }
    }
}

TEST_CASE("blueprint_object_scope_conversion", "[object_scope]") {
    auto blueprint_scope = ikarus_blueprint_scope_create();
    auto blueprint_object_scope = ikarus_blueprint_scope_to_object_scope(&blueprint_scope);
    REQUIRE(blueprint_object_scope._type == IkarusObjectScopeType_Blueprint);
}

TEST_CASE("property_scope_type", "[object_scope]") {
    auto blueprint = IkarusBlueprint{};
    auto entity = IkarusEntity{};

    auto property_blueprint_scope = ikarus_property_scope_create_blueprint(&blueprint);
    auto property_entity_scope = ikarus_property_scope_create_entity(&entity);

    REQUIRE(ikarus_property_scope_get_type(&property_blueprint_scope) == IkarusPropertyScopeType_Blueprint);
    REQUIRE(ikarus_property_scope_get_type(&property_entity_scope) == IkarusPropertyScopeType_Entity);
}

TEST_CASE("property_object_scope_conversion", "[object_scope]") {
    auto blueprint = IkarusBlueprint{};
    auto entity = IkarusEntity{};

    auto property_blueprint_scope = ikarus_property_scope_create_blueprint(&blueprint);
    auto property_blueprint_object_scope = ikarus_property_scope_to_object_scope(&property_blueprint_scope);

    REQUIRE(property_blueprint_object_scope._type == IkarusObjectScopeType_Property);

    auto property_entity_scope = ikarus_property_scope_create_entity(&entity);
    auto property_entity_object_scope = ikarus_property_scope_to_object_scope(&property_entity_scope);

    REQUIRE(property_entity_object_scope._type == IkarusObjectScopeType_Property);
}

TEST_CASE("property_scope_visiting", "[object_scope]") {
    auto blueprint = IkarusBlueprint{};
    auto entity = IkarusEntity{};

    auto property_blueprint_scope = ikarus_property_scope_create_blueprint(&blueprint);
    auto property_entity_scope = ikarus_property_scope_create_entity(&entity);

    int test = 0;

    ikarus_property_scope_visit(
        &property_blueprint_scope,
        [](IkarusBlueprint const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 1; },
        [](IkarusEntity const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 2; },
        &test
    );

    REQUIRE(test == 1);

    ikarus_property_scope_visit(
        &property_entity_scope,
        [](IkarusBlueprint const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 1; },
        [](IkarusEntity const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 2; },
        &test
    );

    REQUIRE(test == 2);
}

TEST_CASE("entity_object_scope_conversion", "[object_scope]") {
    auto entity_scope = ikarus_entity_scope_create();
    auto entity_object_scope = ikarus_entity_scope_to_object_scope(&entity_scope);
    REQUIRE(entity_object_scope._type == IkarusObjectScopeType_Entity);
}

TEST_CASE("object_scope_type_fetching", "[object_scope]") {
    auto blueprint = IkarusBlueprint{};

    auto blueprint_scope = ikarus_blueprint_scope_create();
    auto property_scope = ikarus_property_scope_create_blueprint(&blueprint);
    auto entity_scope = ikarus_entity_scope_create();

    auto blueprint_object_scope = ikarus_blueprint_scope_to_object_scope(&blueprint_scope);
    auto property_object_scope = ikarus_property_scope_to_object_scope(&property_scope);
    auto entity_object_scope = ikarus_entity_scope_to_object_scope(&entity_scope);

    REQUIRE(ikarus_object_scope_get_type(&blueprint_object_scope) == IkarusObjectScopeType_Blueprint);
    REQUIRE(ikarus_object_scope_get_type(&property_object_scope) == IkarusObjectScopeType_Property);
    REQUIRE(ikarus_object_scope_get_type(&entity_object_scope) == IkarusObjectScopeType_Entity);
}

TEST_CASE("object_scope_visiting", "[object_scope]") {
    auto blueprint = IkarusBlueprint{};

    auto blueprint_scope = ikarus_blueprint_scope_create();
    auto property_scope = ikarus_property_scope_create_blueprint(&blueprint);
    auto entity_scope = ikarus_entity_scope_create();

    auto blueprint_object_scope = ikarus_blueprint_scope_to_object_scope(&blueprint_scope);
    auto property_object_scope = ikarus_property_scope_to_object_scope(&property_scope);
    auto entity_object_scope = ikarus_entity_scope_to_object_scope(&entity_scope);

    auto scopes = {
        std::make_pair(blueprint_object_scope, 1),
        std::make_pair(property_object_scope, 2),
        std::make_pair(entity_object_scope, 3),
    };

    for (auto [scope, value] : scopes) {
        int test = 0;

        ikarus_object_scope_visit(
            &scope,
            [](IkarusBlueprintScope const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 1; },
            [](IkarusPropertyScope const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 2; },
            [](IkarusEntityScope const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 3; },
            &test
        );

        REQUIRE(test == value);
    }
}
