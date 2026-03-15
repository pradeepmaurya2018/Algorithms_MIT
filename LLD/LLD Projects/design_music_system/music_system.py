import collections
from threading import Thread
import threading
import time
import pyaudio

class AudioEngine:
    def __init__(self, wf):
        self.p = pyaudio.PyAudio()
        self.stream = self.p.open(
            format=self.p.get_format_from_width(wf.getsampwidth()),
            channels=wf.getnchannels(),
            rate=wf.getframerate(),
            output=True
        )

    def play_chunk(self, data):
        self.stream.write(data)

    def close(self):
        self.stream.stop_stream()
        self.stream.close()
        self.p.terminate()

class SongPlayer(Thread):
    def __init__(self) -> None:
        self.running=True
        self.event=threading.Event()

    def run(self) -> None:
        while self.running:
            self.event.wait(timeout=30)
            print("playing the chunk of file")
            time.sleep(0.2)

        print("i am playing the song in a thread")

    def stop(self):
        self.running=False
        self.event.set()

class Song:
    def __init__(self, name, duration):
        self.name=name
        self.duration=duration


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

