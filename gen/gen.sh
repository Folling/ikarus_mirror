#!/usr/bin/env bash

# shellcheck disable=SC2001

input_path=$1
output_path=$2


find "$input_path/sources" -name "*.json" | while read -r file; do
    relative=$(realpath --relative-to './sources' "$file" | sed 's/json/h/')
    out="$output_path/$relative"

    output=$(jq -c <"$file")
    type="$(echo "$output" | jq -r '.type_name')"
    type_cap="$(echo "$type" | sed 's/[^_]\+/\L\u&/g')"

    if [[ -f "$out" ]]; then
        rm "$out";
    fi

    printf "#pragma once\n\n" >> "$out"

    echo "$output" | jq -cr '.depends_on[]' | while read -r dependency; do
        printf "\n#include <%s.h>" "$dependency" >> "$out"
    done

printf "\n\n#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct %s;

" "$type_cap" >> "$out"

    echo "$output" | jq -c '.functions[]' | while read -r func; do
        name=$(echo "$func" | jq -r '.name')
        name_cap="$(echo "$name" | sed 's/[^_]\+/\L\u&/g' | sed 's/_//g')"

        version=1
        echo "$func" | jq -c '.versions[]' | while read -r func_version; do
            ret_type=$(echo "$func_version" | jq -r '.return_type')
            args=$(echo "$func_version" | jq -r '[.parameters[] | [.type, .name] | join(" ")] | join(", ")')

            enum_name="Ikarus${type_cap}${name_cap}V${version}Flags"

            echo -n "enum $enum_name {" >> "$out"

            flag_count="$(echo "$func_version" | jq -r '.flags | length')"

            if [[ "$flag_count" -gt 64 ]]; then
                echo "Only up to 64 flags are allowed, got $flag_count" 1>&2
                exit 1
            fi

            flag_i=0
            echo "$func_version" | jq -r '.flags[]' | while read -r flag; do
                flag_cap="$(echo "$flag" | sed 's/[^_]\+/\L\u&/g')"

                if [[ flag_i -eq 0 ]]; then
                    printf "\n    %s_%s = 1 << %i" "$enum_name" "$flag_cap" "$flag_i" >> "$out"
                else
                    printf ",\n    %s_%s = 1 << %i" "$enum_name" "$flag_cap" "$flag_i" >> "$out"
                fi

                flag_i+=1
            done

            if [[ flag_count -eq "0" ]]; then
                printf "};\n\n" >> "$out"
            else
                printf "\n};\n\n" >> "$out"
            fi

            printf "%s ikarus_%s_%s_v%i(%s, %s flags, ErrorCode * err_out);\n\n" "$ret_type" "$type" "$name" "$version" "$args" "$enum_name" >> "$out"
            version+=1
        done
    done
done
