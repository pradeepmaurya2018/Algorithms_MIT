def towerOfHanoi(disk, start, ending, temp):
    print(tower, disk, start, temp, ending)
    if disk == 0:
        return
    towerOfHanoi(disk - 1, start, temp, ending)
    ele = tower[start].pop()
    print(f"Moving {ele} from {start}, {ending} ")
    tower[ending].append(ele)

    towerOfHanoi(disk - 1, temp, ending, start)


if __name__ == "__main__":
    N = 3
    tower = {
        0: [chr(i + ord("A")) for i in range(N - 1, -1, -1)],
        1: [],
        2: [],
    }
    # print(tower)
    # towerOfHanoi(N, 0, 2, 1)
    # print(tower)


