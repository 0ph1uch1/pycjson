import os
import sys
import unittest


class TestBenchmark(unittest.TestCase):
    def test_benchmark_encode(self):
        import json
        import cjson

        benchmark_folder = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            "bench"
        )

        print("test_benchmark_encode start")

        for _file in sorted(os.listdir(benchmark_folder)):
            filename = os.path.join(benchmark_folder, _file)
            with open(filename, "r") as f:
                data = json.load(f)
            time_std = self.time_benchmark(1000, json.dumps, data, indent=None, ensure_ascii=False)
            print(f"file: {_file}, time_std: {time_std}")
            time_cjson = self.time_benchmark(1000, cjson.dumps, data)
            print(f"file: {_file}, time_cjson: {time_cjson}")

    def test_benchmark_decode(self):
        import json
        import cjson

        benchmark_folder = os.path.join(
            os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
            "bench"
        )

        print("test_benchmark_decode start")

        for _file in sorted(os.listdir(benchmark_folder)):
            filename = os.path.join(benchmark_folder, _file)
            with open(filename, "r") as f:
                data = f.read()
            time_std = self.time_benchmark(1000, json.loads, data)
            print(f"file: {_file}, time_std: {time_std}")
            time_cjson = self.time_benchmark(1000, cjson.loads, data)
            print(f"file: {_file}, time_cjson: {time_cjson}")

    def time_benchmark(self, repeat_time, func, *args, **kwargs):
        import time
        time_0 = time.time()
        for _ in range(repeat_time):
            func(*args, **kwargs)
        time_1 = time.time()
        return time_1 - time_0


if __name__ == "__main__":
    sys.path.append(
        os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    )
    unittest.main()
