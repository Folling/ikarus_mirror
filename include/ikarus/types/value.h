#pragma once

// IMPLEMENTATION_DETAIL_PROPERTY_TYPES

#include <ikarus/macros.h>
#include <ikarus/stdtypes.h>
#include <ikarus/types/property_type.h>

IKARUS_BEGIN_HEADER

/// \defgroup entity_value Entity Values
/// \brief The values stored in entities.
/// \details Each entity has a value for each property it is associated with.
/// The value is of the type specified by the property and constrained by the property's settings.
/// \see PropertyType PropertySettings
/// @{

/// \brief A true/false boolean-like value. For example "IsDead".
struct IkarusToggleValue {
    /// \private \brief The value of the property.
    bool _value;
};

/// \brief An arbitrary numeric value. For example "Age".
struct IkarusNumberValue {
    /// \private \brief The value of the property.
    long double _value;
};

/// \brief An arbitrary textual value. For example "First Name".
struct IkarusTextValue {
    /// \private \brief The value of the property.
    char const * _value;
};

/// \private \brief The data for a value.
union IkarusEntityValueData {
    /// \private \brief The value as a toggle.
    IkarusToggleValue toggle;
    /// \private \brief The value as a number.
    IkarusNumberValue number;
    /// \private \brief The value as text.
    IkarusTextValue text;
};

/// \brief The state of an entity value.
/// \details States provide insight into the nature of a value.
enum IkarusEntityValueState {
    /// \brief The value is invalid.
    IkarusEntityValueState_Invalid,
    /// \brief The value is normal and can be used as-is.
    IkarusEntityValueState_Normal,
    /// \brief The value is unknown.
    IkarusEntityValueState_Indeterminate,
};

/// \brief The value of an entity associated with a property.
struct IkarusEntityValue {
    /// \private \brief The type of the value.
    IkarusPropertyType _type;
    /// \private \brief The data for the value.p
    IkarusEntityValueData _data;
    /// \private \brief The state of the value.
    IkarusEntityValueState _state;
};

/// \brief Creates an entity value from a toggle value.
/// \param value The toggle value.
/// \return The entity value.
IKA_API IkarusEntityValue ikarus_value_create_toggle(bool value);
/// \brief Creates an entity value from a number value.
/// \param value The number value.
/// \return The entity value.
/// \remark If the value is NaN or infinity an InvalidEntityValue is returned.
IKA_API IkarusEntityValue ikarus_value_create_number(long double value);
/// \brief Creates an entity value from a text value.
/// \param value The text value.
/// \return The entity value.
/// \remark If the value is null an InvalidEntityValue is returned.
IKA_API IkarusEntityValue ikarus_value_create_text(char const * value);

/// \brief Creates an indeterminate entity value of a given type.
/// \param type The type of the value.
/// \return The entity value.
IKA_API IkarusEntityValue ikarus_value_create_indeterminate(IkarusPropertyType type);

/// \brief Fetches the default value for a property type.
/// \remark Not to be confused with the default value of a property. See ikarus_property_get_default_value
/// \param type The property type.
/// \return The default value for the property type.
IKA_API IkarusEntityValue ikarus_value_get_default(IkarusPropertyType type);

/// \brief Fetches the underlying value of a toggle value.
/// \param value The toggle value.
/// \return The underlying value.
IKA_API bool ikarus_toggle_value_get_underlying(IkarusToggleValue const * value);

/// \brief Fetches the underlying value of a number value.
/// \param value The number value.
/// \return The underlying value.
IKA_API long double ikarus_number_value_get_underlying(IkarusNumberValue const * value);

/// \brief Fetches the underlying value of a text value.
/// \param value The text value.
/// \return The underlying value.
IKA_API char const * ikarus_text_value_get_underlying(IkarusTextValue const * value);

/// \brief Checks if a toggle value is equal to a boolean.
/// \param value The toggle value.
/// \param check The boolean value.
/// \return False if value is null. True if it is equal to check, false otherwise.
IKA_API bool ikarus_toggle_value_is_equal(IkarusToggleValue const * value, bool check);

/// \brief Checks if a number value is equal to a number.
/// \param value The number value.
/// \param check The number value.
/// \return False if value is null. True if it is equal to check, false otherwise.
IKA_API bool ikarus_number_value_is_equal(IkarusNumberValue const * value, long double check);

/// \brief Checks if a text value is equal to a string.
/// \param value The text value.
/// \param check The string value.
/// \return False if value or check are null. True if it is equal to check, false otherwise.
IKA_API bool ikarus_text_value_is_equal(IkarusTextValue const * value, char const * check);

/// \brief Checks if two entity values are equal.
/// \details Two entity values are equal if they are of the same type and their value is considered equal.
/// Note that floating point values can only be checked for approximate equality.
/// \param left The left-hand entity value.
/// \param right The right-hand entity value.
/// \return True if the values are considered equal, false otherwise.
/// \remark Null values compare false to all other values. As do invalid values. Indeterminate values however, compare true to
/// other indeterminate values of the same type.
IKA_API bool ikarus_value_is_equal(IkarusEntityValue const * left, IkarusEntityValue const * right);

/// \brief Checks if an entity value is invalid.
/// \param value The entity value.
/// \return True if the value is invalid or null, false otherwise.
IKA_API bool ikarus_value_is_invalid(IkarusEntityValue const * value);

/// \brief Fetches the type of an entity value.
/// \param value The entity value.
/// \return The type of the entity value.
IKA_API IkarusPropertyType ikarus_value_get_type(IkarusEntityValue const * value);

/// \brief Visits an entity value, calling the appropriate function for the value's type.
/// \param value The entity value to visit.
/// \param toggle The function to call if the value is a toggle value.
/// \param number The function to call if the value is a number value.
/// \param text The function to call if the value is a text value.
/// \param data The data to pass to the functions.
/// \remark function pointers may be null in which case they are not called.
IKA_API void ikarus_value_visit(
    IkarusEntityValue const * value,
    void (*toggle)(IkarusToggleValue const *, void *),
    void (*number)(IkarusNumberValue const *, void *),
    void (*text)(IkarusTextValue const *, void *),
    void * data
);

IKARUS_END_HEADER
