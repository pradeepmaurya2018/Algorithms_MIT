# core/verdict.py
def verdict(test, passed, reason):
    status = "PASS" if passed else "FAIL"
    print(f"[{status}] {test.name} → {reason}")
