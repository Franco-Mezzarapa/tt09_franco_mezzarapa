class CoverPoint:
    def __init__(self, name, vname, bins, bins_labels=None, rel=lambda val, bin_: bin_[0] <= val <= bin_[1]):
        """
        Initialize a cover point with bins and labels.

        :param name: Name of the cover point
        :param vname: Variable name being sampled
        :param bins: A list of bins (ranges or values)
        :param bins_labels: Labels for bins, optional
        :param rel: Relation function to match value against bins
        """
        self.name = name
        self.vname = vname
        self.bins = bins
        self.bins_labels = bins_labels if bins_labels else [str(bin_) for bin_ in bins]
        self.rel = rel
        self.hit_count = {label: 0 for label in self.bins_labels}

    def sample(self, value):
        for bin_, label in zip(self.bins, self.bins_labels):
            if self.rel(value, bin_):
                # print(f"Value {value} matched bin {bin_} ({label})")
                self.hit_count[label] += 1
                break
        else:
            print(f"Value {value} did not match any bins")


    def report(self):
        """
        Generate a coverage report for this cover point.
        """
        total_bins = len(self.bins_labels)
        hit_bins = sum(1 for count in self.hit_count.values() if count > 0)
        coverage = (hit_bins / total_bins) * 100
        return {
            "name": self.name,
            "coverage": coverage,
            "hit_bins": hit_bins,
            "total_bins": total_bins,
            "hit_count": self.hit_count,
        }


# Define constants and bins for the message variable
MSG_SIZE = 64  # Example bit width of the message

# Define bins and labels for the message variable
message_bins = [
    (0, (2**MSG_SIZE // 4) - 1),       # Low range
    ((2**MSG_SIZE // 4), (2**MSG_SIZE // 2) - 1),  # Medium range
    ((2**MSG_SIZE // 2), (3 * 2**MSG_SIZE // 4) - 1),  # High range
    ((3 * 2**MSG_SIZE // 4), (2**MSG_SIZE - 1))  # Very high range
]
message_labels = ["low", "medium", "high", "very_high"]

# Create the CoverPoint instance for the message variable
message_coverpoint = CoverPoint(
    name="message.value",
    vname="message",
    bins=message_bins,
    bins_labels=message_labels,
)
        
        
# Define constants and bins for the key variable
KEY_SIZE = 8  #bit width of the key

# Define bins and labels for the key variable
key_bins = [
    (0, (2**KEY_SIZE // 4) - 1),       # Low range
    ((2**KEY_SIZE // 4), (2**KEY_SIZE // 2) - 1),  # Medium range
    ((2**KEY_SIZE // 2), (3 * 2**KEY_SIZE // 4) - 1),  # High range
    ((3 * 2**KEY_SIZE // 4), (2**KEY_SIZE - 1))  # Very high range
]

key_labels = ["low", "medium", "high", "very_high"]

# Create the CoverPoint instance for the key variable
key_coverpoint = CoverPoint(
    name="key.value",
    vname="key",
    bins=key_bins,
    bins_labels=key_labels,
)

# Sampling function
def sample_cp(message, key):
    """
    Sample the key variable.
    """
    message_coverpoint.sample(message)
    key_coverpoint.sample(key)

# Reporting function
def report_coverage():
    """
    Print coverage reports for all defined cover points.
    """
    # List of all coverpoints to report on
    coverpoints = [key_coverpoint, message_coverpoint]  # Add more cover points as needed

    for cp in coverpoints:
        report = cp.report()
        print(f"Coverage Report for {report['name']}:")
        print(f"Coverage: {report['coverage']:.2f}%")
        print(f"Bins Hit: {report['hit_bins']}/{report['total_bins']}")
        print("Hit Count by Bin:")
        for label, count in report["hit_count"].items():
            print(f"  {label}: {count}")
        print()  # Add a newline for better readability between reports

