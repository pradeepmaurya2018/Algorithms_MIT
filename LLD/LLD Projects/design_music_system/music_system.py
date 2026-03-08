import collections
import time

class Song:
    def __init__(self, name, duration):
        self.name=name
        self.duration=duration
    def play(self):
        time_=self.duration
        while time_:
            print(f"I am playing the song {self.name}")
            time_-=1
            time.sleep(1)

class PlayList:
    def __init__(self, play_list_name):
        self.songList=[]

    def addSong(self, song):
        self.songList.append(song)
    def play(self):
        for song in self.songList:
            song.play()

class AudioEngin:
    def __init__(self):
         pass
    def play(self, s):
        print(f"Playing song {s}")
    def pause(self):
        print(f"Paused")
    def stop(self):
        print("Stopped")



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


if __name__ == '__main__':
    player=MusicPlayerSystem()
    song1=Song("a1",5)
    song2=Song("a2",6)
    song3=Song("a3",10)
    song4=Song("a4",3)

    playList=PlayList("my play list")
    playList.addSong(song1)
    playList.addSong(song2)
    playList.addSong(song3)
    playList.addSong(song4)

    player.playPlayList(playList)

