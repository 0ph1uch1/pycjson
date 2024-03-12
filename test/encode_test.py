import os
import sys
import unittest


class TestEncode(unittest.TestCase):
    def test_encode(self):
        import json
        import math

        import cjson

        test_cases = [
            [1, 2, 3, 4],  # simple list
            [1, 2.3, "a", None, True, False, [], {}],  # list
            ("a", 1, 2.3, 2.3, None, True, False, [], {}),  # tuple
            tuple(range(100)),  # large tuple
            "a",  # simple string
            1,  # simple int
            2.3,  # simple float
            math.inf, math.nan, math.pi,
            None,
            True,
            False,
            [],
            {},
            dict({a: b for a in range(10) for b in range(10)}),  # set
            # 321321432.231543245,  # large float # TODO assert fail in decimal part length
            "锟斤拷"
        ]

        for case in test_cases:
            re_json = json.dumps(case, indent=None, separators=(",", ":"), ensure_ascii=False)
            re_cjson = cjson.dumps(case)  # json formatted in default
            self.assertEqual(re_cjson, re_json)

    def _check(self, a, b):
        if isinstance(a, (list, tuple)):
            if not isinstance(b, (list, tuple)):
                self.fail("type mismatch")
            if len(a) != len(b):
                self.fail("list/tuple length mismatch")
            for va, vb in zip(a, b):
                self._check(va, vb)
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
                self._check(va, vb)
            return
        if isinstance(a, (int, float)):
            self.assertAlmostEqual(a, b, 14, "number mismatch")
            return
        self.assertEqual(a, b, "mismatch")


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
