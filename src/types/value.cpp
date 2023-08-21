#include "ikarus/types/value.h"

#include <cmath>
#include <limits>

#include <catch2/catch_test_macros.hpp>

/// \brief Creates an indeterminate entity value of a given type.
/// \param type The type of the value.
/// \return The entity value.
IKA_API IkarusEntityValue value_create_invalid(IkarusPropertyType type) {
    return IkarusEntityValue{
        ._type = type,
        ._data = IkarusEntityValueData{},
        ._state = IkarusEntityValueState_Invalid,
    };
}

IkarusEntityValue ikarus_value_create_toggle(bool value) {
    return IkarusEntityValue{
        ._type = IkarusPropertyType_Toggle,
        ._data = IkarusEntityValueData{.toggle = IkarusToggleValue{._value = value}},
        ._state = IkarusEntityValueState_Normal,
    };
}

IkarusEntityValue ikarus_value_create_number(long double value) {
    if (auto fp_class = std::fpclassify(value); fp_class != FP_NORMAL && fp_class != FP_ZERO) {
        return value_create_invalid(IkarusPropertyType_Number);
    }

    return IkarusEntityValue{
        ._type = IkarusPropertyType_Number,
        ._data = IkarusEntityValueData{.number = IkarusNumberValue{._value = value}},
        ._state = IkarusEntityValueState_Normal,
    };
}

IkarusEntityValue ikarus_value_create_text(char const * value) {
    if (value == nullptr) {
        return value_create_invalid(IkarusPropertyType_Text);
    };

    return IkarusEntityValue{
        ._type = IkarusPropertyType_Text,
        ._data = IkarusEntityValueData{.text = IkarusTextValue{._value = value}},
        ._state = IkarusEntityValueState_Normal,
    };
}

IkarusEntityValue ikarus_value_create_indeterminate(IkarusPropertyType type) {
    IkarusEntityValueData data{};

    switch (type) {
    case IkarusPropertyType_Toggle: {
        data.toggle = IkarusToggleValue{._value = false};
        break;
    }
    case IkarusPropertyType_Number: {
        data.number = IkarusNumberValue{._value = 0.0};
        break;
    }
    case IkarusPropertyType_Text: {
        data.text = IkarusTextValue{._value = ""};
        break;
    }
    default: return value_create_invalid(type);
    }

    return IkarusEntityValue{
        ._type = type,
        ._data = data,
        ._state = IkarusEntityValueState_Indeterminate,
    };
}

IkarusEntityValue ikarus_value_get_default(IkarusPropertyType type) {
    switch (type) {
    case IkarusPropertyType_Toggle: return ikarus_value_create_toggle(false);
    case IkarusPropertyType_Number: return ikarus_value_create_number(0.0);
    case IkarusPropertyType_Text: return ikarus_value_create_text("");
    default: return value_create_invalid(type);
    }
}

bool ikarus_toggle_value_get_underlying(IkarusToggleValue const * value) {
    return value->_value;
}

long double ikarus_number_value_get_underlying(IkarusNumberValue const * value) {
    return value->_value;
}

char const * ikarus_text_value_get_underlying(IkarusTextValue const * value) {
    return value->_value;
}

// no need to check for validity here, since these concrete types are only created by the library
bool ikarus_toggle_value_is_equal(IkarusToggleValue const * value, bool check) {
    return value != nullptr && value->_value == check;
}

bool ikarus_number_value_is_equal(IkarusNumberValue const * value, long double check) {
    return value != nullptr && value->_value == check;
}

bool ikarus_text_value_is_equal(IkarusTextValue const * value, char const * check) {
    return value != nullptr && check != nullptr && std::strcmp(value->_value, check) == 0;
}

bool ikarus_value_is_equal(IkarusEntityValue const * left, IkarusEntityValue const * right) {
    if (left == nullptr || right == nullptr) {
        return false;
    }

    if (left->_state == IkarusEntityValueState_Invalid || right->_state == IkarusEntityValueState_Invalid) {
        return false;
    }

    if (left->_type != right->_type) {
        return false;
    }

    // indeterminate values are only equal if they have the same type
    if (left->_state == IkarusEntityValueState_Indeterminate && right->_state == IkarusEntityValueState_Indeterminate) {
        return true;
    }

    switch (left->_type) {
    case IkarusPropertyType_Toggle: return left->_data.toggle._value == right->_data.toggle._value;
    case IkarusPropertyType_Number: return left->_data.number._value == right->_data.number._value;
    case IkarusPropertyType_Text: return std::strcmp(left->_data.text._value, right->_data.text._value) == 0;
    default: return false;
    }
}

bool ikarus_value_is_invalid(IkarusEntityValue const * value) {
    return value == nullptr || value->_state == IkarusEntityValueState_Invalid;
}

IkarusPropertyType ikarus_value_get_type(IkarusEntityValue const * value) {
    return value->_type;
}

void ikarus_value_visit(
    IkarusEntityValue const * value,
    void (*toggle)(IkarusToggleValue const * value, void * data),
    void (*number)(IkarusNumberValue const * value, void * data),
    void (*text)(IkarusTextValue const * value, void * data),
    void * data
) {
    if (value == nullptr) {
        return;
    }

    switch (value->_type) {
    case IkarusPropertyType_Toggle: {
        if (toggle != nullptr) {
            toggle(&value->_data.toggle, data);
        }
        break;
    }
    case IkarusPropertyType_Number: {
        if (number != nullptr) {
            number(&value->_data.number, data);
        }
        break;
    }
    case IkarusPropertyType_Text: {
        if (text != nullptr) {
            text(&value->_data.text, data);
        }
        break;
    }
    default: break;
    }
}

TEST_CASE("toggle_value_creation", "[value]") {
    auto toggle_value = ikarus_value_create_toggle(true);

    REQUIRE(ikarus_value_get_type(&toggle_value) == IkarusPropertyType_Toggle);
    REQUIRE(ikarus_toggle_value_is_equal(&toggle_value._data.toggle, true));
}

TEST_CASE("number_value_creation", "[value]") {
    auto number_value = ikarus_value_create_number(1.0);

    REQUIRE(ikarus_value_get_type(&number_value) == IkarusPropertyType_Number);
    REQUIRE(ikarus_number_value_is_equal(&number_value._data.number, 1.0));

    auto nan_value = ikarus_value_create_number(std::numeric_limits<long double>::quiet_NaN());
    REQUIRE(ikarus_value_is_invalid(&nan_value));
    auto signaling_non_value = ikarus_value_create_number(std::numeric_limits<long double>::signaling_NaN());
    REQUIRE(ikarus_value_is_invalid(&signaling_non_value));
    auto inf_value = ikarus_value_create_number(std::numeric_limits<long double>::infinity());
    REQUIRE(ikarus_value_is_invalid(&inf_value));
    auto neg_inf_value = ikarus_value_create_number(-std::numeric_limits<long double>::infinity());
    REQUIRE(ikarus_value_is_invalid(&neg_inf_value));
}

TEST_CASE("text_value_creation", "[value]") {
    auto text_value = ikarus_value_create_text("test");

    REQUIRE(ikarus_value_get_type(&text_value) == IkarusPropertyType_Text);
    REQUIRE(ikarus_text_value_is_equal(&text_value._data.text, "test"));

    auto null_value = ikarus_value_create_text(nullptr);
    REQUIRE(ikarus_value_is_invalid(&null_value));
}

TEST_CASE("default_value_creation", "[value]") {
    auto types = {
        IkarusPropertyType_Toggle,
        IkarusPropertyType_Number,
        IkarusPropertyType_Text,
    };

    for (auto type : types) {
        auto value = ikarus_value_get_default(type);
        REQUIRE(ikarus_value_get_type(&value) == type);
    }
}

TEST_CASE("toggle_value_underlying", "[value]") {
    auto true_toggle_value = ikarus_value_create_toggle(true);
    auto false_toggle_value = ikarus_value_create_toggle(false);

    REQUIRE(ikarus_toggle_value_get_underlying(&true_toggle_value._data.toggle) == true);
    REQUIRE(ikarus_toggle_value_get_underlying(&false_toggle_value._data.toggle) == false);
}

TEST_CASE("number_value_underlying", "[value]") {
    auto zero_number_value = ikarus_value_create_number(0.0);
    auto third_number_value = ikarus_value_create_number(1.0 / 3.0);
    auto large_number_value = ikarus_value_create_number(1.2345678910e123);

    REQUIRE(ikarus_number_value_get_underlying(&zero_number_value._data.number) == 0.0);
    REQUIRE(ikarus_number_value_get_underlying(&third_number_value._data.number) == 1.0 / 3.0);
    REQUIRE(ikarus_number_value_get_underlying(&large_number_value._data.number) == 1.2345678910e123);
}

TEST_CASE("text_value_underlying", "[value]") {
    auto test_value = ikarus_value_create_text("test");
    auto empty_value = ikarus_value_create_text("");

    REQUIRE(std::strcmp(ikarus_text_value_get_underlying(&test_value._data.text), "test") == 0);
    REQUIRE(std::strcmp(ikarus_text_value_get_underlying(&empty_value._data.text), "") == 0);
}

TEST_CASE("toggle_comparison", "[value]") {
    auto true_toggle_value = ikarus_value_create_toggle(true);
    auto false_toggle_value = ikarus_value_create_toggle(false);

    REQUIRE(ikarus_toggle_value_is_equal(&true_toggle_value._data.toggle, true));
    REQUIRE(ikarus_toggle_value_is_equal(&false_toggle_value._data.toggle, false));
}

TEST_CASE("number_comparison", "[value]") {
    auto zero_number_value = ikarus_value_create_number(0.0);
    auto third_number_value = ikarus_value_create_number(1.0 / 3.0);
    auto large_number_value = ikarus_value_create_number(1.2345678910e123);

    REQUIRE(ikarus_number_value_is_equal(&zero_number_value._data.number, 0.0));
    REQUIRE(ikarus_number_value_is_equal(&third_number_value._data.number, 1.0 / 6.0 + 1.0 / 6.0));
    REQUIRE(ikarus_number_value_is_equal(&large_number_value._data.number, 1.2345678910e123));
}

TEST_CASE("text_comparison", "[value]") {
    auto test_value = ikarus_value_create_text("test");
    auto empty_value = ikarus_value_create_text("");

    REQUIRE(ikarus_text_value_is_equal(&test_value._data.text, "test"));
    REQUIRE(ikarus_text_value_is_equal(&empty_value._data.text, ""));
}

TEST_CASE("value_comparison", "[value]") {
    auto true_toggle_value = ikarus_value_create_toggle(true);
    auto false_toggle_value = ikarus_value_create_toggle(false);
    auto number_value1 = ikarus_value_create_number(0.0);
    auto number_value2 = ikarus_value_create_number(0.0);
    auto invalid_value = ikarus_value_create_text(nullptr);

    auto indeterminate_toggle = ikarus_value_create_indeterminate(IkarusPropertyType_Toggle);
    auto indeterminate_number1 = ikarus_value_create_indeterminate(IkarusPropertyType_Number);
    auto indeterminate_number2 = ikarus_value_create_indeterminate(IkarusPropertyType_Number);

    REQUIRE(!ikarus_value_is_equal(nullptr, nullptr));
    REQUIRE(!ikarus_value_is_equal(&true_toggle_value, nullptr));
    REQUIRE(!ikarus_value_is_equal(nullptr, &true_toggle_value));

    REQUIRE(!ikarus_value_is_equal(&invalid_value, &invalid_value));
    REQUIRE(!ikarus_value_is_equal(&true_toggle_value, &invalid_value));
    REQUIRE(!ikarus_value_is_equal(&invalid_value, &true_toggle_value));

    REQUIRE(ikarus_value_is_equal(&true_toggle_value, &true_toggle_value));
    REQUIRE(!ikarus_value_is_equal(&true_toggle_value, &false_toggle_value));
    REQUIRE(!ikarus_value_is_equal(&true_toggle_value, &number_value1));
    REQUIRE(!ikarus_value_is_equal(&number_value1, &true_toggle_value));
    REQUIRE(ikarus_value_is_equal(&number_value1, &number_value2));

    REQUIRE(!ikarus_value_is_equal(&indeterminate_toggle, &indeterminate_number1));
    REQUIRE(ikarus_value_is_equal(&indeterminate_number1, &indeterminate_number2));
}

TEST_CASE("invalid_value", "[value]") {
    auto invalid_value = ikarus_value_create_toggle(false);
    invalid_value._state = IkarusEntityValueState_Invalid;

    REQUIRE(ikarus_value_is_invalid(&invalid_value));
}

TEST_CASE("visit_value", "[value]") {
    auto toggle_value = ikarus_value_create_toggle(true);
    auto number_value = ikarus_value_create_number(0.0);
    auto text_value = ikarus_value_create_text("test");

    auto values = {
        std::make_pair(toggle_value, 1),
        std::make_pair(number_value, 2),
        std::make_pair(text_value, 3),
    };

    for (auto [value, expected] : values) {
        int test = 0;

        ikarus_value_visit(
            &value,
            [](IkarusToggleValue const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 1; },
            [](IkarusNumberValue const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 2; },
            [](IkarusTextValue const * _, void * data) { *reinterpret_cast<decltype(test) *>(data) = 3; },
            &test
        );

        REQUIRE(test == expected);
    }
}
