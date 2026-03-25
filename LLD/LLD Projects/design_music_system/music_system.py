import collections
import threading
import time
from threading import Thread

event=threading.Event()
class MusicThread(Thread):
    def __init__(self, frames):
        super().__init__()
        self.frames=frames[:]

    def run(self):
        for frame in self.frames:
            event.wait()
            print(f"Playing the frame {frame}")
            time.sleep(1)

class Song:
    def __init__(self, name, frames):
        self.name=name
        self.frames=frames[:]
        self.musicThread=MusicThread(self.frames)
    def play(self):
        self.musicThread.start()
        event.set()

class PlayList:
    def __init__(self, play_list_name):
        self.songList=[]

    def addSong(self, song):
        self.songList.append(song)
    def play(self):
        for song in self.songList:
            song.play()


class MusicPlayerSystem:
    def __init__(self):
        self.songQueue=collections.deque()

    def addSong(self, song):
        self.songQueue.append(song)

    def playSong(self):
        song=self.songQueue.popleft()
        song.play()

    def playPlayList(self, playList):
        playList.play()
    def pause(self):
        event.clear()

    def resume(self):
        event.set()

if __name__ == '__main__':
    player=MusicPlayerSystem()
    song1=Song("a1",[1,2,3,4,5,6,7,8,9,10])
    # song2=Song("a2",[1,2,3,4,5,6,7])
    # song3=Song("a3",[1,2,3,4,5,6,7])
    # song4=Song("a4",[1,2,3,4,5,6,7])

    playList=PlayList("my play list")
    playList.addSong(song1)
    # playList.addSong(song2)
    # playList.addSong(song3)
    # playList.addSong(song4)
    print("start")
    player.playPlayList(playList)
    print("sleeping")
    time.sleep(4)

    player.pause()
    print("paused")
    time.sleep(4)
    player.resume()
    print('Resumed')