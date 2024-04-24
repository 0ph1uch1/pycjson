import os
from typing import TYPE_CHECKING


if TYPE_CHECKING:
    import unittest


def get_benchfiles_fullpath():
    benchmark_folder = os.path.join(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
        "bench"
    )

    return sorted([os.path.join(benchmark_folder, f) for f in os.listdir(benchmark_folder)])


def _num_check(self: "unittest.TestCase", a, b):
    while a != 0 and abs(a) > 1:
        a /= 10
        b /= 10
    self.assertAlmostEqual(a, b, msg="number mismatch")


def check_obj_same(self: "unittest.TestCase", a, b):
    if isinstance(a, (list, tuple)):
        if not isinstance(b, (list, tuple)):
            self.fail("type mismatch")
        if len(a) != len(b):
            self.fail("list/tuple length mismatch")
        for va, vb in zip(a, b):
            check_obj_same(self, va, vb)
        return
    if isinstance(a, dict):
        if not isinstance(b, dict):
            self.fail("type mismatch")
        if len(a) != len(b):
            self.fail("dict length mismatch")
        for ka, va in a.items():
            vb = b.get(ka)
            if vb is None and va is not None:
                self.fail("key mismatch")
            check_obj_same(self, va, vb)
        return
    if isinstance(a, (int, float)):
        import math
        if math.isnan(a):
            self.assertTrue(math.isnan(b), "nan mismatch")
            return
        if math.isinf(a):
            self.assertTrue(math.isinf(b), "inf mismatch")
            self.assertEqual(a > 0, b > 0, "inf sign mismatch")
            return
        _num_check(self, a, b)
        return
    if isinstance(a, str):
        self.assertEqual(a.encode(), b.encode(), "str mismatch")
        return
    self.assertEqual(a, b, "mismatch")
