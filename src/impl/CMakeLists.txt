file(
    GLOB_RECURSE
    FILES
    "*.hpp"
    "*.cpp"
    "*.ipp"
    "*.tpp"
)

set(SOURCE_FILES ${FILES} PARENT_SCOPE)