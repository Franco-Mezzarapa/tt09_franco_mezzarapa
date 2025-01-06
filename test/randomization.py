# randomization.py

import random

class Message:
    def __init__(self, size):
        self.size = size
        self.data = self.randomize()
        self.MAX_MSG_VALUE = 2**size - 1

    def randomize(self):
        ranges = [
            (0, (2**self.size) // 4 - 1),
            ((2**self.size) // 4, (2**self.size) // 2 - 1),
            ((2**self.size) // 2, 3 * (2**self.size) // 4 - 1),
            (3 * (2**self.size) // 4, (2**self.size) - 1)
        ]
        weights = [0.25, 0.25, 0.25, 0.25]
        r = random.choices(ranges, weights=weights, k=1)[0]
        message = random.randint(r[0], r[1])
        print(f"Message Randomize - Selected range: {r}, Generated message: {message}")
        return message

class Key:
    def __init__(self, size):
        self.size = size
        self.data = self.randomize()

    def randomize(self):
        ranges = [
            (0, (2**self.size) // 4 - 1),
            ((2**self.size) // 4, (2**self.size) // 2 - 1),
            ((2**self.size) // 2, 3 * (2**self.size) // 4 - 1),
            (3 * (2**self.size) // 4, (2**self.size) - 1)
        ]
        weights = [0.25, 0.25, 0.25, 0.25]
        r = random.choices(ranges, weights=weights, k=1)[0]
        key = random.randint(r[0], r[1])
        print(f"Key Randomize - Selected range: {r}, Generated key: {key}")
        return key
