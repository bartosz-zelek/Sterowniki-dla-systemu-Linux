# bash-completion file for Simics targets and scripts.
# Can be source'd into bash if one wants to test completions.

function _simics_completions {
    local word=$2

    # Include directory names with final /
    COMPREPLY=($(compgen -S / -d -- "$word"))

    # Make sure appended space (-S ' ') is not removed
    local IFS=$'\n'

    # Include Simics script files
    local dir="${word%/*}"
    if [ -n "$word" ] && [ -d "$dir" ]; then
        # Ignore stderr warnings (e.g. non-existent files)
        local files=$(ls ${dir}/*.{yml,simics} 2> /dev/null)
        COMPREPLY+=($(compgen -W "$files" -S ' ' -- "$word"))
    fi
    # Root dir script files, in case names match a directory
    local files=$(ls *.{yml,simics} 2> /dev/null)
    COMPREPLY+=($(compgen -W "$files" -S ' ' -- "$word"))

    # Include Simics target list
    # Ignore any stderr warnings
    local targets="$(bin/list-targets 2> /dev/null)"
    COMPREPLY+=($(compgen -W "$targets" -S ' ' -- "$word"))
}

# Use "nospace" to avoid always adding spaces at the end
complete -o nospace -o default -o bashdefault -F _simics_completions simics
