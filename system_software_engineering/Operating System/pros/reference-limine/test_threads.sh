#!/usr/bin/env bash

set -u

# ============================================================
# PrOS Thread Scheduler Test
# ============================================================

PROJECT_ROOT="$HOME/Algorithms_MIT/Operating System/pros/reference-limine"
LOG_FILE="$PROJECT_ROOT/thread_test.log"
ISO_FILE="$PROJECT_ROOT/template-x86_64.iso"

echo "========================================"
echo "       PrOS Thread Scheduler Test"
echo "========================================"
echo

# ------------------------------------------------------------
# 1. Enter project root
# ------------------------------------------------------------

cd "$PROJECT_ROOT" || {
    echo "FAIL: Cannot enter project root"
    exit 1
}

echo "[1/5] Project root:"
pwd
echo

# ------------------------------------------------------------
# 2. Clean
# ------------------------------------------------------------

echo "[2/5] Cleaning..."

if ! make -C kernel clean; then
    echo "FAIL: kernel clean failed"
    exit 1
fi

# ------------------------------------------------------------
# 3. Build
# ------------------------------------------------------------

echo
echo "[3/5] Building PrOS..."

if ! make -j"$(nproc)"; then
    echo
    echo "FAIL: PrOS build failed"
    exit 1
fi

echo
echo "BUILD: PASSED"
echo

# ------------------------------------------------------------
# 4. Run QEMU directly
# ------------------------------------------------------------

echo "[4/5] Running PrOS in QEMU..."
echo

rm -f "$LOG_FILE"

echo "Serial output:"
echo "$LOG_FILE"
echo

timeout 10s qemu-system-x86_64 \
    -M q35 \
    -cdrom "$ISO_FILE" \
    -boot d \
    -m 2G \
    -serial "file:$LOG_FILE" \
    -display none

QEMU_STATUS=$?

echo
echo "QEMU exit status: $QEMU_STATUS"
echo

# ------------------------------------------------------------
# Check that serial output exists
# ------------------------------------------------------------

if [[ ! -f "$LOG_FILE" ]]; then
    echo "FAIL: Serial log was not created"
    exit 1
fi

if [[ ! -s "$LOG_FILE" ]]; then
    echo "FAIL: Serial log is empty"
    exit 1
fi

# ------------------------------------------------------------
# 5. Display serial output
# ------------------------------------------------------------

echo "========================================"
echo "           PRoS SERIAL OUTPUT"
echo "========================================"

cat "$LOG_FILE"

echo
echo "========================================"
echo "             TEST RESULTS"
echo "========================================"

FAIL=0

check_output()
{
    local expected="$1"

    if grep -Fq "$expected" "$LOG_FILE"; then
        echo "PASS: $expected"
    else
        echo "FAIL: Missing:"
        echo "      $expected"
        FAIL=1
    fi
}

# ------------------------------------------------------------
# Thread initialization
# ------------------------------------------------------------

echo
echo "--- Thread initialization ---"

check_output "PrOS: Thread subsystem initialized"
check_output "PrOS: Thread A created"
check_output "PrOS: Thread B created"
check_output "PrOS: Scheduler initialized"
check_output "PrOS: Starting kernel thread"

# ------------------------------------------------------------
# Thread execution order
# ------------------------------------------------------------

echo
echo "--- Thread execution order ---"

EXPECTED_SEQUENCE=(
    "THREAD A: A1"
    "THREAD B: B1"
    "THREAD A: A2"
    "THREAD B: B2"
    "THREAD A: A3"
    "THREAD B: B3"
)

PREVIOUS_LINE=0

for expected in "${EXPECTED_SEQUENCE[@]}"; do

    LINE=$(grep -n -m1 -F "$expected" "$LOG_FILE" | cut -d: -f1)

    if [[ -z "$LINE" ]]; then
        echo "FAIL: Missing: $expected"
        FAIL=1
        continue
    fi

    if (( LINE <= PREVIOUS_LINE )); then
        echo "FAIL: Wrong order: $expected"
        FAIL=1
    else
        echo "PASS: $expected"
    fi

    PREVIOUS_LINE=$LINE
done

# ------------------------------------------------------------
# Final result
# ------------------------------------------------------------

echo
echo "========================================"

if (( FAIL == 0 )); then

    echo "      THREAD TEST: PASSED"
    echo "========================================"
    echo
    echo "Verified:"
    echo
    echo "    A1 -> B1 -> A2 -> B2 -> A3 -> B3"
    echo
    exit 0

else

    echo "      THREAD TEST: FAILED"
    echo "========================================"
    echo
    echo "Serial log:"
    echo "$LOG_FILE"
    echo
    exit 1

fi
