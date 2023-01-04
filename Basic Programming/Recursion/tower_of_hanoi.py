def towerOfHanoi(disk, start, ending, temp):
    if disk == 1:
        print(f"Moving a single disk from {start}, {ending} ")
        return
    towerOfHanoi(disk - 1, start, temp, ending)
    print(f"Moving a single disk from {start}, {ending} ")
    towerOfHanoi(disk - 1, temp, ending, start)


if __name__ == "__main__":
    disk = ["third", "second", "first"]
    pole = ["A", "B", "C"]
    towerOfHanoi(8, 0, 2, 1)
