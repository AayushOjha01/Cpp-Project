#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <ctime>

// Base class
class AudioPlayer {
public:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void next() = 0;
    virtual void previous() = 0;
    virtual void loop(bool loop) = 0;
    virtual void shuffle(bool shuffle) = 0;
    virtual ~AudioPlayer() {}
};

// Derived class
class MusicPlayer : public AudioPlayer {
public:
    MusicPlayer(const std::vector<std::string>& musicFiles)
        : musicFiles(musicFiles), currentIndex(0), isLooping(false), isShuffled(false) {
        if (!musicFiles.empty()) {
            music.openFromFile(musicFiles[currentIndex]);
        }
        std::srand(std::time(nullptr));  // Seed for shuffling
    }

    void play() override {
        if (music.getStatus() != sf::Music::Playing) {
            music.play();
        }
    }

    void pause() override {
        if (music.getStatus() == sf::Music::Playing) {
            music.pause();
        }
    }

    void stop() override {
        music.stop();
    }

    void next() override {
        if (isShuffled && !shuffleIndices.empty()) {
            currentIndex = (currentIndex + 1) % shuffleIndices.size();
            music.openFromFile(musicFiles[shuffleIndices[currentIndex]]);
        }
        else {
            currentIndex = (currentIndex + 1) % musicFiles.size();
            music.openFromFile(musicFiles[currentIndex]);
        }
        play();
    }

    void previous() override {
        if (isShuffled && !shuffleIndices.empty()) {
            currentIndex = (currentIndex == 0) ? shuffleIndices.size() - 1 : currentIndex - 1;
            music.openFromFile(musicFiles[shuffleIndices[currentIndex]]);
        }
        else {
            currentIndex = (currentIndex == 0) ? musicFiles.size() - 1 : currentIndex - 1;
            music.openFromFile(musicFiles[currentIndex]);
        }
        play();
    }

    void loop(bool loop) override {
        isLooping = loop;
        music.setLoop(loop);
    }

    void shuffle(bool shuffle) override {
        isShuffled = shuffle;
        if (shuffle) {
            shuffleIndices = std::vector<int>(musicFiles.size());
            for (int i = 0; i < musicFiles.size(); ++i) {
                shuffleIndices[i] = i;
            }
            std::random_shuffle(shuffleIndices.begin(), shuffleIndices.end());
            currentIndex = 0;
        }
        else {
            shuffleIndices.clear();
            currentIndex = 0;
        }
    }

    // Getters
    bool getIsLooping() const {
        return isLooping;
    }

    bool getIsShuffled() const {
        return isShuffled;
    }

    sf::Music::Status getStatus() const {
        return music.getStatus();
    }

private:
    sf::Music music;
    std::vector<std::string> musicFiles;
    std::vector<int> shuffleIndices;
    int currentIndex;
    bool isLooping;
    bool isShuffled;
};

int main() {
    std::vector<std::string> musicFiles = {
        "Songs/Aparibhasit.wav",
        "Songs/High Hopes.wav",
        "Songs/Otherside.wav"
    };

    MusicPlayer player(musicFiles);
    player.play();

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Music Player");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Space:
                    if (player.getStatus() == sf::Music::Playing) {
                        player.pause();
                    }
                    else {
                        player.play();
                    }
                    break;
                case sf::Keyboard::Right:
                    player.next();
                    break;
                case sf::Keyboard::Left:
                    player.previous();
                    break;
                case sf::Keyboard::L:
                    player.loop(!player.getIsLooping());
                    break;
                case sf::Keyboard::S:
                    player.shuffle(!player.getIsShuffled());
                    break;
                default:
                    break;
                }
            }
        }

        window.clear();
        // Drawing code here if needed
        window.display();
    }

    return 0;
}
