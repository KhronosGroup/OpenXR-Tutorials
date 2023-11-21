# SPDX-FileCopyrightText: 2021-2023, Collabora, Ltd.
# SPDX-License-Identifier: CC0-1.0

# To install cmake-format:
#   pipx install cmakelang

with section("format"):
    # line_width = 100
    tab_size = 4
    use_tabchars = False
    fractional_tab_policy = "use-space"

    max_prefix_chars = 4

    dangle_parens = True
    dangle_align = "prefix"
    max_pargs_hwrap = 4
    max_rows_cmdline = 1
    always_wrap = ["set_source_files_properties", "install"]

    keyword_case = "upper"


# Do not reflow comments

with section("markup"):
    enable_markup = False
