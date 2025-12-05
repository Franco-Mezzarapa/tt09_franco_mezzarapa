#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Use all available processor cores for parallel jobs, or default to 1 if nproc isn't available.
JOBS=$(nproc 2>/dev/null || echo 1)

# --- Help/Usage Function ---
usage() {
    echo "Verification Workflow Manager"
    echo ""
    echo "Usage: $0 {build|test|coverage|clean} {module|all}"
    echo ""
    echo "Actions:"
    echo "  build       - Compiles the testbench for a specific module or all modules."
    echo "  test        - Runs the tests for a specific module or all modules."
    echo "  coverage    - Runs tests and generates a coverage report for a specific module or all modules."
    echo "  clean       - Removes generated files for a specific module or all modules."
    echo ""
    echo "Modules:"
    echo "  deserializer"
    echo "  serializer"
    echo "  xor_encrypt"
    echo "  integration"
    echo "  all         - Performs the action on all modules."
    echo ""
    echo "Examples:"
    echo "  ./run.sh test serializer"
    echo "  ./run.sh coverage all"
    echo "  ./run.sh clean integration"
    exit 1
}

# --- Argument Parsing ---
if [ "$#" -ne 2 ]; then
    usage
fi

ACTION=$1
MODULE=$2

# Normalize module names (e.g., 'integration' -> 'integration_test')
if [ "$MODULE" == "integration" ]; then
    TARGET_DIR="integration_test"
else
    TARGET_DIR=$MODULE
fi


# --- Main Logic ---
case $ACTION in
    build)
        echo "--- Building module: $MODULE ---"
        if [ "$MODULE" == "all" ]; then
            make -j$JOBS all JOBS=$JOBS
        else
            make -j$JOBS -C src/$TARGET_DIR build JOBS=$JOBS
        fi
        ;;

    test)
        echo "--- Testing module: $MODULE ---"
        if [ "$MODULE" == "all" ]; then
            make all_tests JOBS=$JOBS
        else
            make -j$JOBS -C src/$TARGET_DIR run JOBS=$JOBS
        fi
        ;;

    coverage)
        echo "--- Generating coverage for module: $MODULE ---"
        if [ "$MODULE" == "all" ]; then
            make all_coverage JOBS=$JOBS
        else
            make -j$JOBS -C src/$TARGET_DIR coverage JOBS=$JOBS
        fi
        ;;

    clean)
        echo "--- Cleaning module: $MODULE ---"
        if [ "$MODULE" == "all" ]; then
            make clean_all
        else
            make -C src/$TARGET_DIR clean
        fi
        ;;

    *)
        usage
        ;;
esac

echo -e "\nAction '$ACTION' for module '$MODULE' completed successfully."
