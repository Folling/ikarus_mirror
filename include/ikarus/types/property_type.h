#pragma once

// IMPLEMENTATION_DETAIL_PROPERTY_TYPES

/// \file id.h
/// \author Folling <mail@folling.io>

#include <ikarus/macros.h>

IKARUS_BEGIN_HEADER

/// \defgroup property_types Property Types
/// \brief Property Types delineate the type of data stored by a property.
/// @{

/// \brief The type of a property.
/// \details Designates the type of data stored by the property as well as which settings are
/// available.
/// \see IkarusPropertySettings
enum IkarusPropertyType {
    /// \brief A true/false boolean-like value.
    IkarusPropertyType_Toggle,
    /// \brief An arbitrary numeric value.
    IkarusPropertyType_Number,
    /// \brief An arbitrary textual value.
    IkarusPropertyType_Text,
};

/// @}

IKARUS_END_HEADER
