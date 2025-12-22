COVER_WRAPPER_PATH="$1"
shift
MUST_WRAPPER_PATH="$1"
shift
INTERN_COMPILER="$1"
shift

# First, compile normally using CoVer
${COVER_WRAPPER_PATH} --verbose --wrap-target "${INTERN_COMPILER}" --predefined-contracts --generate-json-report "$@"
# Save each compilation step args to disk
echo "$@" >> compile_cmds

# Once CoVer is done (finished linking), replay compilation using MUST now that contract_messages.json exists
if [[ "$@" != *" -c "* ]] && [[ "$@" != *" -E "* ]]; then
    while IFS= read -r line; do
        ${MUST_WRAPPER_PATH} --verbose --wrap-target "${INTERN_COMPILER}" --tsan-json contract_messages.json ${line}
    done < compile_cmds
fi

# Cleanup
rm compile_cmds
