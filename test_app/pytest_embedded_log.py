#!/usr/bin/env python3
"""
Helper script for running tests with pytest-embedded and QEMU
Can be used for local testing and CI integration
"""

import sys
import re
import pytest

def test_esp_raii_lib(dut):
    """Test ESP RAII Library with Unity framework"""
    
    # Wait for test start
    dut.expect("Starting ESP RAII Library Unity Tests", timeout=10)
    
    # Collect test results
    test_results = []
    unity_output = []
    
    # Wait for GPIO tests
    dut.expect("Running GPIO tests...", timeout=5)
    
    # Wait for Timer tests  
    dut.expect("Running Timer tests...", timeout=10)
    
    # Wait for ETL integration tests
    dut.expect("Running ETL integration tests...", timeout=10)
    
    # Check for test completion
    try:
        result = dut.expect(r"TESTS_COMPLETED: (PASS|FAIL)", timeout=20)
        
        if "PASS" in result.group(0).decode():
            print("✅ All tests passed!")
        else:
            print("❌ Some tests failed!")
            # Get failure details
            dut.expect(r"(\d+) test\(s\) failed", timeout=2)
            pytest.fail("Tests failed")
            
    except Exception as e:
        print(f"Test execution error: {e}")
        pytest.fail(f"Test execution failed: {e}")

if __name__ == "__main__":
    # This allows running the script directly for debugging
    print("This script should be run with pytest-embedded")
    print("Example: pytest --embedded-services esp,idf --target esp32")