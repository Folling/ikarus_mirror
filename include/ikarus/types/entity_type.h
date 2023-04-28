#pragma once

enum EntityType {
    EntityType_None = 0,
    EntityType_First = 1,

    EntityType_Folder = 1,
    EntityType_Template = 2,
    EntityType_Property = 3,
    EntityType_Page = 4,

    EntityType_Max = 5
};

enum EntityTypes {
    EntityTypes_None = 0,
    EntityTypes_Folder = 1 << EntityType_Folder,
    EntityTypes_Template = 1 << EntityType_Template,
    EntityTypes_Property = 1 << EntityType_Property,
    EntityTypes_Page = 1 << EntityType_Page,
};
