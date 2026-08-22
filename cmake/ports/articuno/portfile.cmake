vcpkg_from_gitlab(
  GITLAB_URL https://gitlab.com
  OUT_SOURCE_PATH SOURCE_PATH
  REPO colorglass/articuno
  REF 136aad1df42926370353f77c7fafd5626a382f97
  SHA512 26d514f8144edd70004809becf1920c829607ebfb86889dc1697d5d4ebf58ac634b9d80bea8561d512d1500ad117b36fcfb158debb5e368cb51f20b85f4e3003
  HEAD_REF main
  PATCHES
    fix-ryml-parse-rename.patch
    fix-ryml-error-callback-signature.patch
    fix-anchor-format-arg.patch
)

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
        FEATURES
        "icywind-archive"       BUILD_ICYWIND_ARCHIVE
        "jsoncons-archive"      BUILD_JSONCONS_ARCHIVE
        "rapidyaml-archive"     BUILD_RAPIDYAML_ARCHIVE
        "toml11-archive"        BUILD_TOML11_ARCHIVE
        "tomlplusplus-archive"  BUILD_TOMLPP_ARCHIVE
        "pugixml-archive"       BUILD_PUGIXML_ARCHIVE
        "tests"                 BUILD_TESTS
        )

vcpkg_configure_cmake(
  SOURCE_PATH "${SOURCE_PATH}"
  PREFER_NINJA
  OPTIONS ${FEATURE_OPTIONS}
)
vcpkg_install_cmake()

file(
  INSTALL "${SOURCE_PATH}/LICENSE"
  DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
  RENAME copyright)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")
