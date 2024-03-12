import cjson, json, math
from dataclasses import dataclass


test_cases = [
    [1, 2, 3, 4], # simple list
    [1, 2.3, "a", None, True, False, [], {}], # list
    ("a", 1, 2.3, 2.3, None, True, False, [], {}), # tuple
    tuple(range(100)), # large tuple
    "a", # simple string
    1, # simple int
    2.3, # simple float
    math.inf, math.nan, math.pi,
    None,
    True,
    False,
    [],
    {},
    dict({a: b for a in range(10) for b in range(10)}), # set
    321321432.231543245, # large float # TODO assert fail in decimal part length
]
for case in test_cases:
    print("Case:", case, type(case))
    re_json = json.dumps(case, indent=None, separators=(",", ":"))
    re_cjson = cjson.dumps(case) # json formatted in default
    print("cjson:", re_cjson.encode("utf-8"))
    print("json :", re_json.encode("utf-8"))
    assert re_cjson == re_json