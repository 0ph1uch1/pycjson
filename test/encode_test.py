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
            {"啊啊啊": "ß"},
            321321432.231543245,  # large float # TODO assert fail in decimal part length
        ]

        for case in test_cases:
            re_json = json.dumps(case, indent=None, separators=(",", ":"),ensure_ascii=False)
            re_cjson = cjson.dumps(case)
            self.assertEqual(re_cjson, re_json)


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
