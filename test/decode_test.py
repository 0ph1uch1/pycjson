import os
import sys
import unittest


class TestEncode(unittest.TestCase):
    def test_encode(self):
        import json
        import math

        import cjson

        test_cases = [
            True,
            False,
            None,
            1,
            -1,
            2.3,
            -2.3,
            321321432.231543245,
            -321321432.231543245,
            math.inf,
            -math.inf,
            # math.nan, # TODO
            math.pi,
            [], {}, tuple(),
            [1, 2, 3, 4],
            [155, {}, 2.3, "a", None, True, False, [], {}, 11],
            ("a", 1, 2.3, 2.3, None, True, False, [], {}),
            {"啊啊啊": "ß", "ü": ["\uff02", "\u00f8"]},
            dict({a: b for a in range(10) for b in range(10)}),
            [[[[[[[[[[[[[[]]]]]]], [[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]
        ]

        test_cases = [json.dumps(case, ensure_ascii=False) for case in test_cases]

        for case in test_cases:
            re_json = json.loads(case)
            re_cjson = cjson.loads(case)
            with self.subTest(msg=f'decoding_test(case={case})'):
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
